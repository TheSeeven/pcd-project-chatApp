
#include <memory.h>
#include <sqlite3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <regex.h>
#include <math.h>

#define QUERY_SUCCESS true
#define QUERY_FAIL false
#define OK true
#define NOT_OK false

const char *charset = "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm123456789";

//
//
//
// internal functions

int numberOfDigits(int num)
{
    int res = 0;
    if (num < 10)
        return 1;
    else
    {
        num = num / 10;
        res++;
    }
    return res + numberOfDigits(num);
}

void intToString(int num, char *dest)
{
    int destCounter = 0;
    int rounds = numberOfDigits(num);
    for (int i = 0; i < rounds; i++)
    {
        char lastDigit = (num / pow(10, (numberOfDigits(num) - 1))) + 48;
        num = num - (pow(10, (numberOfDigits(num) - 1)) * (lastDigit - 48));
        dest[destCounter] = lastDigit;
        destCounter++;
    }
}
// generates a sesion token for the given address pointer
void generateToken(char *token)
{
    srand(time(NULL));
    for (int i = 0; i < 20; i++)
    {
        token[i] = charset[rand() % 61];
    }
}

// checks if password is adequate for security reasons
bool isGoodPassword(char *password)
{
    bool uppercase = false;
    bool lowercase = false;
    int minimumLength = 8;
    int passLength = 0;
    for (int i = 0; password[i] != '\0'; i++)
    {
        if (!uppercase)
            if (password[i] >= 'A' && password[i] <= 'Z')
                uppercase = true;

        if (!lowercase)
            if (password[i] >= 'a' && password[i] <= 'z')
                lowercase = true;
        passLength++;
    }
    if (passLength < minimumLength)
        return NOT_OK;
    if (uppercase && lowercase)
        return OK;
    return NOT_OK;
}

// returns true if the username only contains lowercase, uppercase, numbers
bool isGoodUsername(char *username)
{
    for (int i = 0; username[i] != '\0'; i++)
        if ((username[i] >= 'a' && username[i] <= 'z') || (username[i] >= 'A' && username[i] <= 'Z') || (username[i] >= '0' && username[i] <= '9'))
            return NOT_OK;
    return OK;
}

// returns true if the phone number respects a pattern
bool isPhoneNumber(char *phone)
{
    int phoneNoLength = 0;
    int length = 14;
    int minLength = 10;
    for (int i = 0; phone[i] != '\0'; i++)
    {
        phoneNoLength++;
        if ((i + 1) > length)
            return NOT_OK;
        if (phone[i] >= '0' && phone[i] <= '9')
            continue;
        return NOT_OK;
    }
    if (phoneNoLength < 10)
        return NOT_OK;
    return OK;
}

// checks if the email adress is acording to pattern
bool isGoodEmail(char *email)
{
    regex_t regex;
    int isRegex = regcomp(&regex, "[A-Za-z0-9.]\\+@[A-Za-z0-9]\\+\\.[A-Za-z]", 0);
    isRegex = regexec(&regex, email, 0, NULL, 0);
    regfree(&regex);
    if (!isRegex)
        return OK;
    return NOT_OK;
}

// executes a given statement without returning a value
void executeQuery(char *query)
{
    sqlite3 *DATABASE;
    sqlite3_open("CHATAPP.db", &DATABASE);
    char *errorMessage = NULL;
    if (sqlite3_exec(DATABASE, query, NULL, 0, &errorMessage) != SQLITE_OK)
    {
        printf("query execute error! query: %s\n error: --> %s\n\n", query, errorMessage);
        sqlite3_free(errorMessage);
    }
    sqlite3_close(DATABASE);
}

// verifies the length of a string
bool verifyLength(char *data, unsigned int fileSize)
{
    int counter = 0;
    for (int i = 0; data[i] != '\0'; i++)
        counter++;
    if (counter > fileSize)
        return NOT_OK;
    return OK;
}

//verify if token belongs to a specific user
bool verifyAuthToken(char *username, char *authToken)
{
    int valid = -1;
    sqlite3 *DATABASE;
    sqlite3_stmt *result;
    const char *tail;
    char *query = sqlite3_mprintf("SELECT count() FROM auth WHERE token='%q' AND user_id=(SELECT id FROM user where username='%q');", authToken, username);
    sqlite3_open("CHATAPP.db", &DATABASE);
    sqlite3_prepare_v2(DATABASE, query, -1, &result, &tail);
    while (sqlite3_step(result) == SQLITE_ROW)
    {
        valid = sqlite3_column_int(result, 0);
        sqlite3_close(DATABASE);
    }

    if (valid == 1)
        return OK;

    return NOT_OK;
}

//generates an authentification token
bool generateAuthToken(char *authToken)
{
    sqlite3 *DATABASE;
    bool found = true;
    sqlite3_stmt *result;
    const char *tail;
    while (found)
    {
        found = false;
        generateToken(authToken);
        char *query = sqlite3_mprintf("SELECT user_id FROM auth WHERE token='%q';", authToken);
        sqlite3_open("CHATAPP.db", &DATABASE);
        sqlite3_prepare_v2(DATABASE, query, -1, &result, &tail);
        while (sqlite3_step(result) == SQLITE_ROW)
        {
            found = true;
        }

        sqlite3_finalize(result);
        sqlite3_close(DATABASE);
    }
    return QUERY_SUCCESS;
}

// internal functions
//
//
//

// ------------------------------------------------------------------------------

//
//
//
// public functions

// creates the database and the tables required
void initializeDatabase()
{
    printf("Creating database...\n");
    char *createUser = "CREATE TABLE user (id integer PRIMARY KEY AUTOINCREMENT,username TEXT UNIQUE NOT NULL, email TEXT UNIQUE NOT NULL,phone TEXT UNIQUE NOT NULL, password TEXT NOT NULL,profilePicture BLOB);";
    char *createAuth = "CREATE TABLE auth (user_id integer NOT NULL,token TEXT UNIQUE NOT NULL, date datetime NOT NULL,FOREIGN KEY(user_id) REFERENCES user(id) ON DELETE CASCADE ON UPDATE CASCADE);";
    char *createFriends = "CREATE TABLE friends (id integer NOT NULL,friend_id integer NOT NULL, date datetime NOT NULL, FOREIGN KEY (id) REFERENCES user(id) ON DELETE CASCADE ON UPDATE CASCADE,FOREIGN KEY (friend_id) REFERENCES user(id) ON DELETE CASCADE ON UPDATE CASCADE);";
    char *createMessage = "CREATE TABLE message (expeditor integer NOT NULL,receiver integer NOT NULL, message TEXT,date datetime NOT NULL, picture BLOB,FOREIGN KEY (expeditor) REFERENCES user(id) ON DELETE CASCADE ON UPDATE CASCADE,FOREIGN KEY (receiver) REFERENCES user(id) ON DELETE CASCADE ON UPDATE CASCADE);";
    executeQuery(createUser);
    executeQuery(createAuth);
    executeQuery(createFriends);
    executeQuery(createMessage);
    printf("Tables created succesfully! \n");
}

// inserts a user into the databse, return 'true' for succes, 'false' for fail
bool insertUser(char *username, char *email, char *phone, char *password)
{
    if (verifyLength(username, 50))
        return QUERY_FAIL;
    else if (verifyLength(email, 100))
        return QUERY_FAIL;
    else if (verifyLength(phone, 15))
        return QUERY_FAIL;
    else if (verifyLength(password, 500))
        return QUERY_FAIL;
    else if (isGoodPassword(password))
        return QUERY_FAIL;
    if (isGoodEmail(email) && isGoodPassword(password) && isGoodUsername(username) && isPhoneNumber(phone))
    {
        char *query = sqlite3_mprintf("INSERT INTO user(username,email,phone,password) VALUES('%q','%q','%q','%q');", username, email, phone, password);
        executeQuery(query);
        return QUERY_SUCCESS;
    }
    return QUERY_FAIL;
}

// updates the profile picture of a user return 'true' for success, 'false' for fail
// NOTE: THE SECOND PARAMETER WILL BE CHANGED LATER TO A DIFERENT TYPE
bool updateProfilePicture(char *username, FILE *picture)
{
    int fileLength;
    int fileSize;
    char *query;

    sqlite3 *DATABASE;
    sqlite3_open("CHATAPP.db", &DATABASE);
    fseek(picture, 0, SEEK_END);
    fileLength = ftell(picture);
    fseek(picture, 0, SEEK_SET);
    char data[fileLength + 1];
    fileSize = fread(data, 1, fileLength, picture);
    fclose(picture);

    sqlite3_stmt *pStmt;
    query = "UPDATE user SET profilePicture = ? WHERE username = ?";

    sqlite3_prepare(DATABASE, query, -1, &pStmt, 0);
    sqlite3_bind_blob(pStmt, 1, data, fileSize, SQLITE_STATIC);
    sqlite3_bind_text(pStmt, 2, username, -1, SQLITE_STATIC);

    if (sqlite3_step(pStmt) != SQLITE_DONE)
    {
        sqlite3_close(DATABASE);
        printf("Execution of statement failed!\n");
        return QUERY_FAIL;
    };
    sqlite3_finalize(pStmt);

    sqlite3_close(DATABASE);
    return QUERY_SUCCESS;
}

// changes the password of the user
bool changeUserPassword(char *username, char *newPassword)
{
    if (!isGoodPassword(newPassword))
        return QUERY_FAIL;
    char *query = sqlite3_mprintf("UPDATE user SET password = '%q' WHERE username = '%q';", newPassword, username);
    executeQuery(query);
    return QUERY_SUCCESS;
}

// changes the email of the user
bool changeUserEmail(char *username, char *newEmail)
{
    if (!isGoodEmail(newEmail))
        return QUERY_FAIL;
    char *query = sqlite3_mprintf("UPDATE user SET email = '%q' WHERE username = '%q';", newEmail, username);
    executeQuery(query);
    return QUERY_SUCCESS;
}

// changes the phone of the user
bool changeUserPhone(char *username, char *newPhone)
{
    if (!isPhoneNumber(newPhone))
        return QUERY_FAIL;
    char *query = sqlite3_mprintf("UPDATE user SET phone = '%q' WHERE username = '%q';", newPhone, username);
    executeQuery(query);
    return QUERY_SUCCESS;
}

//removes a user from the table
bool removeUser(char *username)
{
    char *query = sqlite3_mprintf("DELETE FROM user WHERE username = '%q';", username);
    executeQuery(query);
    return QUERY_SUCCESS;
}

//add a friend in friendlist
bool addFriend(char *user1, char *user2)
{

    char *query = sqlite3_mprintf("INSERT INTO friends(id, friend_id, date) VALUES((SELECT id FROM user where username = '%q'),(SELECT id FROM user where username = '%q'),(datetime()));", user1, user2);
    executeQuery(query);
    return QUERY_SUCCESS;
}

//remove a friend in friendlist
bool removeFriend(char *user1, char *user2)
{
    char *query = sqlite3_mprintf("DELETE FROM friends WHERE id = (SELECT id FROM user WHERE username = '%q') AND friend_id = (SELECT id FROM user where username = '%q');", user1, user2);
    executeQuery(query);
    return QUERY_SUCCESS;
}

bool insertMessage(char *user1, char *user2, char *message, int messageLength, FILE *file)
{
    if (file == NULL)
    {
    }
    else
    {
    }
    return QUERY_SUCCESS;
}

//inserts a new authentification in Auth table
bool loginUser(char *username, char *password)
{
    sqlite3_stmt *result;
    const char *tail;
    sqlite3 *DATABASE;
    int foundId = -1;
    char *query = sqlite3_mprintf("SELECT id FROM user WHERE username='%q' AND password='%q';", username, password);
    sqlite3_open("CHATAPP.db", &DATABASE);
    sqlite3_prepare_v2(DATABASE, query, -1, &result, &tail);
    while (sqlite3_step(result) == SQLITE_ROW)
    {
        foundId = sqlite3_column_int(result, 0);
    }
    sqlite3_finalize(result);
    sqlite3_close(DATABASE);
    if (foundId != -1)
    {
        char token[21] = {0};
        generateAuthToken(token);
        query = sqlite3_mprintf("INSERT INTO auth values(%d,'%q',(datetime()));", foundId, token);
        executeQuery(query);
        return QUERY_SUCCESS;
    }
    return QUERY_FAIL;
}

//deletes a authentification on disconnect
bool logoutUser(char *username, char *token)
{
    char *query = sqlite3_mprintf("DELETE FROM auth WHERE user_id=(SELECT id FROM user where username='%q') AND token='%q';", username, token);
    executeQuery(query);
    return QUERY_SUCCESS;
}

//deletes a authentification that is older than 24 hours
bool deleteOldToken()
{
    char *query = "DELETE FROM 'auth' where 86400 < strftime('%s','now') - strftime('%s',date)";
    executeQuery(query);
    return QUERY_SUCCESS;
}

typedef struct friendPack friendPack;
struct friendPack
{
    int msgLength;
    char *data;
};

friendPack serializeFriend(sqlite3_stmt *data)
{
    friendPack result;
    char *username = (char *)sqlite3_column_text(data, 0);
    char *email = (char *)sqlite3_column_text(data, 1);
    char *phone = (char *)sqlite3_column_text(data, 2);
    char *date = (char *)sqlite3_column_text(data, 4);
    char *profilePicture = (char *)sqlite3_column_blob(data, 3);

    int usernameLength = strlen(username);
    int emailLength = strlen(email);
    int phoneLength = strlen(phone);
    int profilePictureLength = sqlite3_column_bytes(data, 3);
    int dateLength = strlen(date);

    char usernameLengthChar[10] = {0};
    char emailLengthChar[10] = {0};
    char phoneLengthChar[10] = {0};
    char profilePictureLengthChar[10] = {0};
    char dateLengthChar[10] = {0};
    intToString(usernameLength, usernameLengthChar);
    intToString(emailLength, emailLengthChar);
    intToString(phoneLength, phoneLengthChar);
    intToString(profilePictureLength, profilePictureLengthChar);
    intToString(dateLength, dateLengthChar);

    char *bytesResult = (char *)malloc(numberOfDigits(usernameLength) +
                                       numberOfDigits(emailLength) +
                                       numberOfDigits(phoneLength) +
                                       numberOfDigits(profilePictureLength) +
                                       numberOfDigits(dateLength) +
                                       usernameLength +
                                       emailLength +
                                       phoneLength +
                                       profilePictureLength +
                                       dateLength + 5);

    int cursorIndex = 0;

    for (int i = 0; usernameLengthChar[i] != '\0'; i++)
        bytesResult[cursorIndex++] = usernameLengthChar[i];
    bytesResult[cursorIndex++] = ';';

    for (int i = 0; emailLengthChar[i] != '\0'; i++)
        bytesResult[cursorIndex++] = emailLengthChar[i];
    bytesResult[cursorIndex++] = ';';

    for (int i = 0; phoneLengthChar[i] != '\0'; i++)
        bytesResult[cursorIndex++] = phoneLengthChar[i];
    bytesResult[cursorIndex++] = ';';

    for (int i = 0; profilePictureLengthChar[i] != '\0'; i++)
        bytesResult[cursorIndex++] = profilePictureLengthChar[i];
    bytesResult[cursorIndex++] = ';';

    for (int i = 0; dateLengthChar[i] != '\0'; i++)
        bytesResult[cursorIndex++] = dateLengthChar[i];
    bytesResult[cursorIndex++] = ';';

    for (int i = 0; i < usernameLength; i++)
        bytesResult[cursorIndex++] = username[i];
    for (int i = 0; i < emailLength; i++)
        bytesResult[cursorIndex++] = email[i];
    for (int i = 0; i < phoneLength; i++)
        bytesResult[cursorIndex++] = phone[i];
    for (int i = 0; i < profilePictureLength; i++)
        bytesResult[cursorIndex++] = profilePicture[i];
    for (int i = 0; i < dateLength; i++)
        bytesResult[cursorIndex++] = date[i];
    int msgLength = numberOfDigits(usernameLength) +
                    numberOfDigits(emailLength) +
                    numberOfDigits(phoneLength) +
                    numberOfDigits(profilePictureLength) +
                    numberOfDigits(dateLength) +
                    usernameLength +
                    emailLength +
                    phoneLength +
                    profilePictureLength +
                    dateLength + 5;

    result.data = bytesResult;
    result.msgLength = msgLength;
    return result;
}

friendPack prepareResult(friendPack data[])
{
    friendPack result;
    int resultCounter = 0;
    int finalLength = 0;
    for (int i = 0; data[i].msgLength != 0; i++)
    {
        finalLength += data[i].msgLength;
    }
    result.data = malloc(finalLength);
    for (int i = 0; data[i].msgLength != 0; i++)
    {
        for (int j = 0; j < data[i].msgLength; j++)
        {
            result.data[resultCounter++] = data[i].data[j];
        }
        free(data[i].data);
    }
    result.msgLength = finalLength;
    return result;
}

friendPack getFriendList(char *username, char *token)
{
    friendPack result;
    result.msgLength = -1;
    if (!verifyAuthToken(username, token))
    {
        return result;
    }
    sqlite3 *DATABASE;
    sqlite3_stmt *dbResult;
    const char *tail;

    friendPack databaseObjectsSerialized[100] = {0};
    int dbObjCounter = 0;

    char *query = sqlite3_mprintf("Select user.username, user.email, user.phone, user.profilePicture, friends.date from friends INNER JOIN user on friends.friend_id= user.id where friends.id=(SELECT user.id FROM user WHERE username = '%q' );", username);
    sqlite3_open("CHATAPP.db", &DATABASE);
    sqlite3_prepare_v2(DATABASE, query, -1, &dbResult, &tail);
    while (sqlite3_step(dbResult) == SQLITE_ROW)
    {
        databaseObjectsSerialized[dbObjCounter] = serializeFriend(dbResult);
        dbObjCounter++;
    }
    sqlite3_close(DATABASE);
    result = prepareResult(databaseObjectsSerialized);
    return result;
}

// public functions
//
//
//