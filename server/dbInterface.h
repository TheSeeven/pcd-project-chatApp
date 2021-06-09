
#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <memory.h>

#define QUERY_SUCCESS true
#define QUERY_FAIL false
#define OK true
#define NOT_OK false
const char *INSERT_USER_TEMPLATE = "INSERT INTO user(username,email,phone,password) VALUES(";

//
//
//
// internal functions
int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for (i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}
void executeQuery(char *query)
{
    sqlite3 *DATABASE;
    if (sqlite3_open("CHATAPP.db", &DATABASE) != SQLITE_OK)
    {
        printf("Can't open database! error --> %s\n", sqlite3_errmsg(DATABASE));
        exit(EXIT_FAILURE);
    }
    char *errorMessage = NULL;
    if (sqlite3_exec(DATABASE, query, callback, 0, &errorMessage) != SQLITE_OK)
    {
        printf("query execute error! query: %s\n error: --> %s\n\n", query, errorMessage);
        sqlite3_free(errorMessage);
    }
    sqlite3_close(DATABASE);
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

bool verifyLength(char *data, unsigned int size)
{
    int counter = 0;
    for (int i = 0; data[i] != '\0'; i++)
        counter++;
    if (counter > size)
        return NOT_OK;
    return OK;
}

void beginquery(char *query, const char *template)
{
    for (int i = 0; template[i] != '\0'; i++)
    {
        query[i] = template[i];
    }
}
void insertDataInquery(char *query, const char *data, bool needsQuotes, bool isLast)
{
    int index = 0;
    int lastPosition = 0;
    for (int i = 0; query[i] != '\0'; i++)
    {
        index++;
    }
    if (needsQuotes)
    {
        query[index] = '\'';
        index++;
    }
    for (int i = index; data[i - index] != '\0'; i++)
    {
        query[i] = data[i - index];
        lastPosition = i;
    }
    if (needsQuotes)
        query[lastPosition + 1] = '\'';

    if (!isLast)
        query[lastPosition + 1 + needsQuotes] = ',';
    else
    {
        query[lastPosition + 1 + needsQuotes] = ')';
        query[lastPosition + 2 + needsQuotes] = ';';
    }
}

bool insertUser(char *username, char *email, char *phone, char *password)
{
    if (!verifyLength(username, 50))
    {
        return QUERY_FAIL;
    }
    if (!verifyLength(email, 100))
    {
        return QUERY_FAIL;
    }
    if (!verifyLength(phone, 15))
    {
        return QUERY_FAIL;
    }
    if (!verifyLength(password, 500))
    {
        return QUERY_FAIL;
    }

    char *query = sqlite3_mprintf("INSERT INTO user(username,email,phone,password) VALUES('%q','%q','%q','%q');", username, email, phone, password);
    executeQuery(query);
    return QUERY_SUCCESS;
}

bool updateProfilePicture(char *username, FILE *picture)
{
    int flen;
    int size;
    char *sql;

    sqlite3 *DATABASE;
    if (sqlite3_open("CHATAPP.db", &DATABASE) != SQLITE_OK)
    {
        sqlite3_close(DATABASE);
        printf("Error connecting to db!\n");
        return QUERY_FAIL;
    };
    fseek(picture, 0, SEEK_END);
    flen = ftell(picture);
    fseek(picture, 0, SEEK_SET);
    char data[flen + 1];
    size = fread(data, 1, flen, picture);
    fclose(picture);

    sqlite3_stmt *pStmt;
    sql = "UPDATE user SET profilePicture = ? WHERE username = ?";

    sqlite3_prepare(DATABASE, sql, -1, &pStmt, 0);
    sqlite3_bind_blob(pStmt, 1, data, size, SQLITE_STATIC);
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

bool changeUserPassword(char *username, char *newPassword)
{
    char *query = sqlite3_mprintf("UPDATE user SET password = '%q' WHERE username = '%q';", newPassword, username);
    executeQuery(query);
    return QUERY_SUCCESS;
}

bool changeUserEmail(char *username, char *newEmail)
{
    char *query = sqlite3_mprintf("UPDATE user SET email = '%q' WHERE username = '%q';", newEmail, username);
    executeQuery(query);
    return QUERY_SUCCESS;
}

bool changeUserPhone(char *username, char *newPhone)
{
    char *query = sqlite3_mprintf("UPDATE user SET phone = '%q' WHERE username = '%q';", newPhone, username);
    executeQuery(query);
    return QUERY_SUCCESS;
}

bool removeUser(char *username)
{
    char *query = sqlite3_mprintf("DELETE FROM user WHERE username = '%q';", username);
    executeQuery(query);
    return QUERY_SUCCESS;
}

bool addFriend(char *user1, char *user2)
{

    char *query = sqlite3_mprintf("INSERT INTO friends(id, friend_id, date) VALUES((SELECT id FROM user where username = '%q'),(SELECT id FROM user where username = '%q'),(SELECT date('now')));", user1, user2);
    executeQuery(query);
    return QUERY_SUCCESS;
}
bool removeFriend(char *user1, char *user2)
{
    char *query = sqlite3_mprintf("DELETE FROM friends WHERE id = (SELECT id FROM user WHERE username = '%q') AND friend_id = (SELECT id FROM user where username = '%q');", user1, user2);
    ;
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

// public functions
//
//
//