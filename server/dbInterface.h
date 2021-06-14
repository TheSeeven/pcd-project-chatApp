
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

// generates a sesion token for the given address pointer
void generateToken(char *token)
{
    srand(time(NULL));
    for (int i = 0; i < 20; i++)
    {
        token[i] = charset[rand() % 61];
    }
}

//generates a random string for fileLink
void generateRandomString(char *dest, int from, int to)
{
    srand(time(NULL));
    for (int i = from; i < to - 1; i++)
    {
        dest[i] = charset[rand() % 61];
    }
}

void logger(char *logMessage)
{
    char *query = sqlite3_mprintf("INSERT INTO logging(log,date) VALUES('%q',datetime())", logMessage);
    executeQuery(query);
    memset(logMessage, '\0', 200);
    sqlite3_free(query);
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
bool isAuthenticated(char *username, char *authToken)
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
        sqlite3_free(query);
    }

    if (valid == 1)
    {
        query = sqlite3_mprintf("UPDATE auth set date=datetime()  where token = '%q'", authToken);
        executeQuery(query);
        sqlite3_free(query);
        return OK;
    }

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
        sqlite3_free(query);
    }
    return QUERY_SUCCESS;
}

//creates a package of data to be sent
typedef struct dataPack dataPack;
struct dataPack
{
    int msgLength;
    char *data;
};

//seialize frinds data
dataPack serializeDataFriend(sqlite3_stmt *data)
{
    dataPack result;
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

dataPack serializeDataLog(sqlite3_stmt *data)
{
    dataPack result;
    char *log = (char *)sqlite3_column_text(data, 0);
    char *date = (char *)sqlite3_column_text(data, 1);

    int logLength = strlen(log);
    int dateLength = strlen(date);

    char logLengthChar[10] = {0};
    char dateLengthChar[10] = {0};
    intToString(logLength, logLengthChar);
    intToString(dateLength, dateLengthChar);

    char *bytesResult = (char *)malloc(numberOfDigits(logLength) +
                                       numberOfDigits(dateLength) +
                                       logLength +
                                       dateLength + 2);

    int cursorIndex = 0;

    for (int i = 0; logLengthChar[i] != '\0'; i++)
        bytesResult[cursorIndex++] = logLengthChar[i];
    bytesResult[cursorIndex++] = ';';

    for (int i = 0; dateLengthChar[i] != '\0'; i++)
        bytesResult[cursorIndex++] = dateLengthChar[i];
    bytesResult[cursorIndex++] = ';';

    for (int i = 0; i < logLength; i++)
        bytesResult[cursorIndex++] = log[i];
    for (int i = 0; i < dateLength; i++)
        bytesResult[cursorIndex++] = date[i];
    int msgLength = numberOfDigits(logLength) +
                    numberOfDigits(dateLength) +
                    logLength +
                    dateLength + 2;

    result.data = bytesResult;
    result.msgLength = msgLength;
    return result;
}

dataPack serializeDataUsers(sqlite3_stmt *data)
{
    dataPack result;
    char *username = (char *)sqlite3_column_text(data, 0);
    char *email = (char *)sqlite3_column_text(data, 1);
    char *phone = (char *)sqlite3_column_text(data, 2);

    int usernameLength = strlen(username);
    int emailLength = strlen(email);
    int phoneLength = strlen(phone);

    char usernameLengthChar[10] = {0};
    char emailLengthChar[10] = {0};
    char phoneLengthChar[10] = {0};
    intToString(usernameLength, usernameLengthChar);
    intToString(emailLength, emailLengthChar);
    intToString(phoneLength, phoneLengthChar);

    char *bytesResult = (char *)malloc(numberOfDigits(usernameLength) +
                                       numberOfDigits(emailLength) +
                                       numberOfDigits(phoneLength) +
                                       usernameLength +
                                       emailLength +
                                       phoneLength + 3);

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

    for (int i = 0; i < usernameLength; i++)
        bytesResult[cursorIndex++] = username[i];
    for (int i = 0; i < emailLength; i++)
        bytesResult[cursorIndex++] = email[i];
    for (int i = 0; i < phoneLength; i++)
        bytesResult[cursorIndex++] = phone[i];
    int msgLength = numberOfDigits(usernameLength) +
                    numberOfDigits(emailLength) +
                    numberOfDigits(phoneLength) +
                    usernameLength +
                    emailLength +
                    phoneLength + 3;

    result.data = bytesResult;
    result.msgLength = msgLength;
    return result;
}

//
dataPack prepareResult(dataPack data[])
{
    dataPack result;
    int resultCounter = 0;
    int finalLength = 0;
    for (int i = 0; data[i].msgLength != 0; i++)
    {
        finalLength += data[i].msgLength;
    }
    result.data = (char *)malloc(finalLength);
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

dataPack prepareResultAdmin(dataPack data[])
{
    dataPack result;
    int resultCounter = 0;
    int finalLength = 0;
    char header[10] = {0};

    for (int i = 0; data[i].msgLength != 0; i++)
    {
        finalLength += data[i].msgLength;
    }
    intToString(finalLength, header);
    result.data = (char *)malloc(finalLength + strlen(header) + 1);

    for (int i = 0; header[i] != '\0'; i++)
        result.data[resultCounter++] = header[i];
    result.data[resultCounter++] = ':';

    for (int i = 0; data[i].msgLength != 0; i++)
    {
        for (int j = 0; j < data[i].msgLength; j++)
        {
            result.data[resultCounter++] = data[i].data[j];
        }
        free(data[i].data);
    }
    result.msgLength = finalLength + strlen(header) + 1;
    return result;
}

//serialize user data
dataPack serializeDataUsername(sqlite3_stmt *data)
{
    dataPack result;

    char *email = (char *)sqlite3_column_text(data, 0);
    char *phone = (char *)sqlite3_column_text(data, 1);
    char *profilePicture = (char *)sqlite3_column_blob(data, 2);

    int emailLength = strlen(email);
    int phoneLength = strlen(phone);
    int profilePictureLength = sqlite3_column_bytes(data, 2);

    char emailLengthChar[10] = {0};
    char phoneLengthChar[10] = {0};
    char profilePictureLengthChar[10] = {0};

    intToString(emailLength, emailLengthChar);
    intToString(phoneLength, phoneLengthChar);
    intToString(profilePictureLength, profilePictureLengthChar);

    char *bytesResult = (char *)malloc(numberOfDigits(emailLength) +
                                       numberOfDigits(phoneLength) +
                                       numberOfDigits(profilePictureLength) +
                                       emailLength +
                                       phoneLength +
                                       profilePictureLength +
                                       3);

    int cursorIndex = 0;

    for (int i = 0; emailLengthChar[i] != '\0'; i++)
        bytesResult[cursorIndex++] = emailLengthChar[i];
    bytesResult[cursorIndex++] = ';';

    for (int i = 0; phoneLengthChar[i] != '\0'; i++)
        bytesResult[cursorIndex++] = phoneLengthChar[i];
    bytesResult[cursorIndex++] = ';';

    for (int i = 0; profilePictureLengthChar[i] != '\0'; i++)
        bytesResult[cursorIndex++] = profilePictureLengthChar[i];
    bytesResult[cursorIndex++] = ';';

    for (int i = 0; i < emailLength; i++)
        bytesResult[cursorIndex++] = email[i];
    for (int i = 0; i < phoneLength; i++)
        bytesResult[cursorIndex++] = phone[i];
    for (int i = 0; i < profilePictureLength; i++)
        bytesResult[cursorIndex++] = profilePicture[i];
    int msgLength = numberOfDigits(emailLength) +
                    numberOfDigits(phoneLength) +
                    numberOfDigits(profilePictureLength) +
                    emailLength +
                    phoneLength +
                    profilePictureLength + 5;

    result.data = bytesResult;
    result.msgLength = msgLength;
    return result;
}

//serialize the message data
dataPack serializeDataMessage(sqlite3_stmt *data)
{
    dataPack result;
    char *username = (char *)sqlite3_column_text(data, 0);
    char *message = (char *)sqlite3_column_text(data, 1);
    char *date = (char *)sqlite3_column_text(data, 2);

    int usernameLength = strlen(username);
    int messageLength = strlen(message);
    int dateLength = strlen(date);

    char usernameLengthChar[10] = {0};
    char messageLengthChar[10] = {0};
    char dateLengthChar[10] = {0};

    intToString(usernameLength, usernameLengthChar);
    intToString(messageLength, messageLengthChar);
    intToString(dateLength, dateLengthChar);

    char *bytesResult = (char *)malloc(numberOfDigits(usernameLength) +
                                       numberOfDigits(messageLength) +
                                       numberOfDigits(dateLength) +
                                       usernameLength +
                                       messageLength +
                                       dateLength + 3);

    int cursorIndex = 0;

    for (int i = 0; usernameLengthChar[i] != '\0'; i++)
        bytesResult[cursorIndex++] = usernameLengthChar[i];
    bytesResult[cursorIndex++] = ';';
    for (int i = 0; messageLengthChar[i] != '\0'; i++)
        bytesResult[cursorIndex++] = messageLengthChar[i];
    bytesResult[cursorIndex++] = ';';
    for (int i = 0; dateLengthChar[i] != '\0'; i++)
        bytesResult[cursorIndex++] = dateLengthChar[i];
    bytesResult[cursorIndex++] = ';';

    for (int i = 0; i < usernameLength; i++)
        bytesResult[cursorIndex++] = username[i];
    for (int i = 0; i < messageLength; i++)
        bytesResult[cursorIndex++] = message[i];
    for (int i = 0; i < dateLength; i++)
        bytesResult[cursorIndex++] = date[i];
    int msgLength = numberOfDigits(usernameLength) +
                    numberOfDigits(dateLength) +
                    numberOfDigits(messageLength) +
                    usernameLength +
                    dateLength +
                    messageLength + 3;

    result.data = bytesResult;
    result.msgLength = msgLength;
    return result;
}

//generates a string for picturelink
char *generateFileLink(char *filename)
{
    int filenameSize = strlen(filename);
    int finalSize = 12 + filenameSize;
    char *result = malloc(finalSize);
    memset(result, '\0', finalSize);
    strcpy(result, filename);
    result[filenameSize] = '/';
    generateRandomString(result, filenameSize + 1, finalSize);
    return result;
}

//seialize frinds data
dataPack serializeDataNotFriend(sqlite3_stmt *data)
{
    dataPack result;
    char *username = (char *)sqlite3_column_text(data, 0);
    char *email = (char *)sqlite3_column_text(data, 1);
    char *phone = (char *)sqlite3_column_text(data, 2);
    char *profilePicture = (char *)sqlite3_column_blob(data, 3);

    int usernameLength = strlen(username);
    int emailLength = strlen(email);
    int phoneLength = strlen(phone);
    int profilePictureLength = sqlite3_column_bytes(data, 3);

    char usernameLengthChar[10] = {0};
    char emailLengthChar[10] = {0};
    char phoneLengthChar[10] = {0};
    char profilePictureLengthChar[10] = {0};
    intToString(usernameLength, usernameLengthChar);
    intToString(emailLength, emailLengthChar);
    intToString(phoneLength, phoneLengthChar);
    intToString(profilePictureLength, profilePictureLengthChar);

    char *bytesResult = (char *)malloc(numberOfDigits(usernameLength) +
                                       numberOfDigits(emailLength) +
                                       numberOfDigits(phoneLength) +
                                       numberOfDigits(profilePictureLength) +
                                       usernameLength +
                                       emailLength +
                                       phoneLength +
                                       profilePictureLength + 4);

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

    for (int i = 0; i < usernameLength; i++)
        bytesResult[cursorIndex++] = username[i];
    for (int i = 0; i < emailLength; i++)
        bytesResult[cursorIndex++] = email[i];
    for (int i = 0; i < phoneLength; i++)
        bytesResult[cursorIndex++] = phone[i];
    for (int i = 0; i < profilePictureLength; i++)
        bytesResult[cursorIndex++] = profilePicture[i];
    int msgLength = numberOfDigits(usernameLength) +
                    numberOfDigits(emailLength) +
                    numberOfDigits(phoneLength) +
                    numberOfDigits(profilePictureLength) +
                    usernameLength +
                    emailLength +
                    phoneLength +
                    profilePictureLength + 4;

    result.data = bytesResult;
    result.msgLength = msgLength;
    return result;
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
    char *createUser = "CREATE TABLE user (id integer, username TEXT NOT NULL UNIQUE, email TEXT NOT NULL UNIQUE, phone TEXT NOT NULL UNIQUE, password TEXT NOT NULL, profilePicture BLOB DEFAULT null,PRIMARY KEY(id AUTOINCREMENT));";
    char *createAuth = "CREATE TABLE auth (user_id integer NOT NULL,token TEXT UNIQUE NOT NULL, date datetime NOT NULL,FOREIGN KEY(user_id) REFERENCES user(id) ON DELETE CASCADE ON UPDATE CASCADE);";
    char *createFriends = "CREATE TABLE friends (id integer NOT NULL,friend_id integer NOT NULL, date datetime NOT NULL, FOREIGN KEY (id) REFERENCES user(id) ON DELETE CASCADE ON UPDATE CASCADE,FOREIGN KEY (friend_id) REFERENCES user(id) ON DELETE CASCADE ON UPDATE CASCADE);";
    char *createMessage = "CREATE TABLE message (expeditor integer NOT NULL, receiver integer NOT NULL,message TEXT NOT NULL, date datetime NOT NULL, file BLOB DEFAULT null,filename TEXT DEFAULT null,FOREIGN KEY(expeditor) REFERENCES user(id) ON DELETE CASCADE ON UPDATE CASCADE,FOREIGN KEY(receiver) REFERENCES user(id) ON DELETE CASCADE ON UPDATE CASCADE);";
    char *createLogging = "CREATE TABLE 'logging' ('id'	INTEGER NOT NULL,'log' TEXT NOT NULL,'date' DATETIME NOT NULL,PRIMARY KEY('id' AUTOINCREMENT));";
    executeQuery(createUser);
    executeQuery(createAuth);
    executeQuery(createFriends);
    executeQuery(createMessage);
    executeQuery(createLogging);
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
        sqlite3_free(query);
        return QUERY_SUCCESS;
    }
    return QUERY_FAIL;
}

// updates the profile picture of a user return 'true' for success, 'false' for fail
bool changeProfilePicture(char *username, char *token, char *picture, int pictureSize)
{
    if (!isAuthenticated(username, token))
        return QUERY_FAIL;

    sqlite3_stmt *pStmt;
    char *query = "UPDATE user SET profilePicture = ? WHERE username = ?";
    sqlite3 *DATABASE;
    sqlite3_open("CHATAPP.db", &DATABASE);
    sqlite3_prepare(DATABASE, query, -1, &pStmt, 0);
    sqlite3_bind_blob(pStmt, 1, picture, pictureSize, SQLITE_STATIC);
    sqlite3_bind_text(pStmt, 2, username, -1, SQLITE_STATIC);

    sqlite3_step(pStmt);
    sqlite3_finalize(pStmt);
    sqlite3_free(query);
    sqlite3_close(DATABASE);
    return QUERY_SUCCESS;
}

// changes the password of the user
bool changeUserPassword(char *username, char *newPassword, char *token)
{
    if (!isAuthenticated(username, token))
        return QUERY_FAIL;
    if (!isGoodPassword(newPassword))
        return QUERY_FAIL;
    char *query = sqlite3_mprintf("UPDATE user SET password = '%q' WHERE username = '%q';", newPassword, username);
    executeQuery(query);
    sqlite3_free(query);
    return QUERY_SUCCESS;
}

// changes the email of the user
bool changeUserEmail(char *username, char *newEmail, char *token)
{
    if (!isAuthenticated(username, token))
        return QUERY_FAIL;
    if (!isGoodEmail(newEmail))
        return QUERY_FAIL;
    char *query = sqlite3_mprintf("UPDATE user SET email = '%q' WHERE username = '%q';", newEmail, username);
    executeQuery(query);
    sqlite3_free(query);
    return QUERY_SUCCESS;
}

// changes the phone of the user
bool changeUserPhone(char *username, char *newPhone, char *token)
{
    if (!isAuthenticated(username, token))
        return QUERY_FAIL;
    if (!isPhoneNumber(newPhone))
        return QUERY_FAIL;
    char *query = sqlite3_mprintf("UPDATE user SET phone = '%q' WHERE username = '%q';", newPhone, username);
    executeQuery(query);
    sqlite3_free(query);
    return QUERY_SUCCESS;
}

//removes a user from the table
bool removeUser(char *username, char *token)
{
    if (!isAuthenticated(username, token))
        return QUERY_FAIL;
    char *query = sqlite3_mprintf("DELETE FROM user WHERE username = '%q';", username);
    executeQuery(query);
    sqlite3_free(query);
    return QUERY_SUCCESS;
}

//add a friend in friendlist
bool addFriend(char *user1, char *user2, char *token)
{
    if (!isAuthenticated(user1, token))
        return QUERY_FAIL;
    char *query = sqlite3_mprintf("INSERT INTO friends(id, friend_id, date) VALUES((SELECT id FROM user where username = '%q'),(SELECT id FROM user where username = '%q'),(datetime()));", user1, user2);
    executeQuery(query);
    sqlite3_free(query);
    return QUERY_SUCCESS;
}

//remove a friend in friendlist
bool removeFriend(char *user1, char *user2, char *token)
{
    if (!isAuthenticated(user1, token))
        return QUERY_FAIL;
    char *query = sqlite3_mprintf("DELETE FROM friends WHERE id = (SELECT id FROM user WHERE username = '%q') AND friend_id = (SELECT id FROM user where username = '%q');", user1, user2);
    executeQuery(query);
    sqlite3_free(query);
    return QUERY_SUCCESS;
}

//sends message to database
bool sendMessage(char *user1, char *user2, char *token, char *message, char *filename, char *file, int fileSize)
{
    if (!isAuthenticated(user1, token))
        return NOT_OK;
    if (fileSize > 0)
    {
        char *fileLink = generateFileLink(filename);
        char *query = "INSERT INTO message VALUES ((SELECT id FROM user WHERE username = ?),(SELECT id FROM user WHERE username = ?),?,datetime(),?,?) ;";
        sqlite3 *DATABASE;
        sqlite3_stmt *pStmt;
        sqlite3_open("CHATAPP.db", &DATABASE);
        sqlite3_prepare(DATABASE, query, -1, &pStmt, 0);
        sqlite3_bind_text(pStmt, 1, user1, -1, SQLITE_STATIC);
        sqlite3_bind_text(pStmt, 2, user2, -1, SQLITE_STATIC);
        sqlite3_bind_text(pStmt, 3, fileLink, -1, SQLITE_STATIC);
        sqlite3_bind_blob(pStmt, 4, file, fileSize, SQLITE_STATIC);
        sqlite3_bind_text(pStmt, 5, filename, -1, SQLITE_STATIC);
        sqlite3_step(pStmt);
        sqlite3_finalize(pStmt);
        sqlite3_close(DATABASE);
        free(fileLink);
        sqlite3_free(query);
    }
    else
    {
        char *query = sqlite3_mprintf("INSERT INTO message(expeditor,receiver,message, date) VALUES ((SELECT id FROM user WHERE username = '%q'),(SELECT id FROM user WHERE username = '%q'),'%q',datetime()) ;", user1, user2, message);
        executeQuery(query);
        sqlite3_free(query);
    }
    return QUERY_SUCCESS;
}

//inserts a new authentification in Auth table
char *loginUser(char *username, char *password)
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
    sqlite3_free(query);
    char *token;
    if (foundId != -1)
    {
        token = malloc(21);
        memset(token, 0, 21);
        generateAuthToken(token);
        query = sqlite3_mprintf("INSERT INTO auth values(%d,'%q',(datetime()));", foundId, token);
        executeQuery(query);
        sqlite3_free(query);
        return token;
    }
    token = malloc(1);
    token[0] = -2;
    return token;
    ;
}

//deletes a authentification on disconnect
bool logoutUser(char *username, char *token)
{
    char *query = sqlite3_mprintf("DELETE FROM auth WHERE user_id=(SELECT id FROM user where username='%q') AND token='%q';", username, token);
    executeQuery(query);
    sqlite3_free(query);
    return QUERY_SUCCESS;
}

//deletes a authentification that is older than 24 hours
bool deleteOldToken()
{
    char *query = "DELETE FROM 'auth' where 86400 < strftime('%s','now') - strftime('%s',date)";
    executeQuery(query);
    sqlite3_free(query);
    return QUERY_SUCCESS;
}

// gets the list of friends and their data for a specific user
dataPack getFriendList(char *username, char *token)
{
    dataPack result;
    result.msgLength = -1;
    if (!isAuthenticated(username, token))
    {
        return result;
    }
    sqlite3 *DATABASE;
    sqlite3_stmt *dbResult;
    const char *tail;

    dataPack databaseObjectsSerialized[100] = {0};
    int dbObjCounter = 0;

    char *query = sqlite3_mprintf("Select user.username, user.email, user.phone, user.profilePicture, friends.date from friends INNER JOIN user on friends.friend_id= user.id where friends.id=(SELECT user.id FROM user WHERE username = '%q' );", username);
    sqlite3_open("CHATAPP.db", &DATABASE);
    sqlite3_prepare_v2(DATABASE, query, -1, &dbResult, &tail);
    while (sqlite3_step(dbResult) == SQLITE_ROW)
    {
        databaseObjectsSerialized[dbObjCounter] = serializeDataFriend(dbResult);
        dbObjCounter++;
    }
    sqlite3_close(DATABASE);
    result = prepareResult(databaseObjectsSerialized);
    sqlite3_free(query);
    return result;
}

//get user data from table
dataPack getUser(char *username, char *token)
{
    dataPack result;
    result.msgLength = -1;
    if (!isAuthenticated(username, token))
    {
        return result;
    }
    sqlite3 *DATABASE;
    sqlite3_stmt *dbResult;
    const char *tail;

    char *query = sqlite3_mprintf("SELECT email,phone,profilePicture from user where username='%q';", username);
    sqlite3_open("CHATAPP.db", &DATABASE);
    sqlite3_prepare_v2(DATABASE, query, -1, &dbResult, &tail);
    while (sqlite3_step(dbResult) == SQLITE_ROW)
    {
        result = serializeDataUsername(dbResult);
    }
    sqlite3_close(DATABASE);
    sqlite3_free(query);
    return result;
}

//get the list of messages between 2 clients (last 1000)
dataPack getMessagesList(char *username, char *friendUsername, char *token)
{
    dataPack result;
    result.msgLength = -1;
    if (!isAuthenticated(username, token))
        return result;

    sqlite3 *DATABASE;
    sqlite3_stmt *dbResult;
    const char *tail;

    dataPack databaseObjectsSerialized[1000] = {0};
    int dbObjCounter = 0;

    char *query = sqlite3_mprintf("SELECT username, message, date from message inner join user on user.id=expeditor WHERE (expeditor=(SELECT user.id FROM user WHERE username = '%q') AND receiver=(SELECT user.id FROM user WHERE username = '%q' )) OR  (expeditor=(SELECT user.id FROM user WHERE username = '%q' ) AND receiver=(SELECT user.id FROM user WHERE username = '%q' )) LIMIT 1000;", username, friendUsername, friendUsername, username);
    sqlite3_open("CHATAPP.db", &DATABASE);
    sqlite3_prepare_v2(DATABASE, query, -1, &dbResult, &tail);
    while (sqlite3_step(dbResult) == SQLITE_ROW)
        databaseObjectsSerialized[dbObjCounter++] = serializeDataMessage(dbResult);

    sqlite3_close(DATABASE);
    result = prepareResult(databaseObjectsSerialized);
    sqlite3_free(query);
    return result;
}

//get file from message
dataPack getFile(char *username, char *pictureLink, char *token)
{
    dataPack result;
    result.msgLength = -1;
    if (!isAuthenticated(username, token))
        result;
    result.msgLength = 0;
    sqlite3 *DATABASE;
    sqlite3_stmt *dbResult;
    const char *tail;

    char *query = sqlite3_mprintf("SELECT file FROM message WHERE filename ='%q' AND (expeditor = (SELECT user.id FROM user WHERE username = '%q') OR receiver = (SELECT user.id FROM user WHERE username = '%q')) ;", pictureLink, username, username);
    sqlite3_open("CHATAPP.db", &DATABASE);
    sqlite3_prepare_v2(DATABASE, query, -1, &dbResult, &tail);

    while (sqlite3_step(dbResult) == SQLITE_ROW)
    {
        size_t pictureSize = sqlite3_column_bytes(dbResult, 0);
        result.data = (char *)malloc(pictureSize);
        char *pointerToPicture = (char *)sqlite3_column_blob(dbResult, 0);
        for (int i = 0; i < pictureSize; i++)
            result.data[i] = pointerToPicture[i];
        result.msgLength = pictureSize;
    }
    sqlite3_close(DATABASE);
    sqlite3_free(query);
    return result;
}

//get all users that are not friends with a specific user
dataPack getNotFriendList(char *username, char *token)
{
    dataPack result;
    result.msgLength = -1;
    if (!isAuthenticated(username, token))
    {
        return result;
    }
    sqlite3 *DATABASE;
    sqlite3_stmt *dbResult;
    const char *tail;

    dataPack databaseObjectsSerialized[100] = {0};
    int dbObjCounter = 0;

    char *query = sqlite3_mprintf("Select user.username, user.email, user.phone, user.profilePicture from user left join (Select friends.friend_id,user.username, user.email, user.phone, user.profilePicture from user inner join friends on friends.id=user.id where friends.id=(SELECT user.id FROM user WHERE username = '%q')) as x on user.id=x.friend_id where x.friend_id is null and user.username!='%q';", username, username);
    sqlite3_open("CHATAPP.db", &DATABASE);
    sqlite3_prepare_v2(DATABASE, query, -1, &dbResult, &tail);
    while (sqlite3_step(dbResult) == SQLITE_ROW)
    {
        databaseObjectsSerialized[dbObjCounter] = serializeDataNotFriend(dbResult);
        dbObjCounter++;
    }
    sqlite3_close(DATABASE);
    result = prepareResult(databaseObjectsSerialized);
    sqlite3_free(query);
    return result;
}

//gets all the users from database
dataPack getAllUsers()
{
    dataPack result;
    result.msgLength = -1;
    sqlite3 *DATABASE;
    sqlite3_stmt *dbResult;
    const char *tail;

    dataPack databaseObjectsSerialized[100] = {0};
    int dbObjCounter = 0;

    char *query = sqlite3_mprintf("Select username, email, phone from user;");
    sqlite3_open("CHATAPP.db", &DATABASE);
    sqlite3_prepare_v2(DATABASE, query, -1, &dbResult, &tail);
    while (sqlite3_step(dbResult) == SQLITE_ROW)
    {
        databaseObjectsSerialized[dbObjCounter] = serializeDataUsers(dbResult);
        dbObjCounter++;
    }
    sqlite3_close(DATABASE);
    result = prepareResultAdmin(databaseObjectsSerialized);
    sqlite3_free(query);
    return result;
}

//gets the log from database
dataPack getLoggedData()
{
    dataPack result;
    result.msgLength = -1;
    sqlite3 *DATABASE;
    sqlite3_stmt *dbResult;
    const char *tail;

    dataPack databaseObjectsSerialized[100] = {0};
    int dbObjCounter = 0;

    char *query = sqlite3_mprintf("Select log,date from logging;");
    sqlite3_open("CHATAPP.db", &DATABASE);
    sqlite3_prepare_v2(DATABASE, query, -1, &dbResult, &tail);
    while (sqlite3_step(dbResult) == SQLITE_ROW)
    {
        databaseObjectsSerialized[dbObjCounter] = serializeDataLog(dbResult);
        dbObjCounter++;
    }
    sqlite3_close(DATABASE);
    result = prepareResultAdmin(databaseObjectsSerialized);
    sqlite3_free(query);
    return result;
}
// public functions
//
//
//