#include "dbInterface.h"
#include "requestParser.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/un.h>

#define SOCK_PATH "unix_socket.server"
#define PORT 6000

typedef struct
{
    int fd;
    char *username;
} connectedUser;

typedef struct
{
    int newSocket;
    int serverFd;
    char *ip;
    char *username;
} threadClientArguments;

static const connectedUser resetConnectedUser;

typedef struct timeval timeval;

connectedUser CONNNECTED_USERS[2000];
char *blockedUsers[2000] = {0};
int blockUserCount = 0;
pthread_mutex_t mutexlock;

int getNextFreeIndex()
{
    int result = 0;
    for (int i = 0; i < 2000; i++)
    {
        if (CONNNECTED_USERS[i].fd == 0)
            return i;
    }
    return -1;
}

void forceDisconectClient(char *username)
{
    for (int i = 0; i < 2000; i++)
    {
        if (CONNNECTED_USERS[i].fd != 0)
        {
            if (strcmp(CONNNECTED_USERS[i].username, username) == 0)
            {
                close(CONNNECTED_USERS[i].fd);
                free(CONNNECTED_USERS[i].username);
                CONNNECTED_USERS[i] = resetConnectedUser;
                break;
            }
        }
    }
}

int getNextBlockIndex()
{
    for (int i = 0; i < 2000; i++)
    {
        if (blockedUsers[i] == NULL)
        {
            return i;
        }
    }
    return -1;
}

void blockUser(char *username)
{
    int index = getNextBlockIndex();
    blockedUsers[index] = malloc(strlen(username));
    strcpy(blockedUsers[index], username);
}

bool isBlocked(char *username)
{
    for (int i = 0; i < 2000; i++)
    {
        if (blockedUsers[i] != NULL)
        {
            if (strcmp(username, blockedUsers[i]) == 0)
                return true;
        }
    }
    return false;
}

void unblockUser(char *username)
{
    for (int i = 0; i < 2000; i++)
    {
        if (blockedUsers[i] != NULL)
        {
            if (strcmp(username, blockedUsers[i]) == 0)
                free(blockedUsers[i]);
        }
        break;
    }
}

void connectClient(char *username, int fd)
{
    int index = getNextFreeIndex();
    connectedUser user = {fd, username};
    CONNNECTED_USERS[index] = user;
}

void removeDisconectedClient(char *username)
{
    for (int i = 0; i < 2000; i++)
    {
        if (strcmp(username, CONNNECTED_USERS[i].username) == 0)
        {
            free(CONNNECTED_USERS[i].username);
            CONNNECTED_USERS[i] = resetConnectedUser;
            break;
        }
    }
}

int getBlockedUsersSize()
{
    int result = 0;
    int serparators = 0;
    for (int i = 0; i < 2000; i++)
    {
        if (blockedUsers[i] != 0)
        {
            result += strlen(blockedUsers[i]);
            serparators++;
        }
    }
    return result + serparators;
}

int getConnectedUsersSize()
{
    int result = 0;
    int serparators = 0;
    for (int i = 0; i < 2000; i++)
    {
        if (CONNNECTED_USERS[i].username != 0)
        {
            result += strlen(CONNNECTED_USERS[i].username);
            serparators++;
        }
    }
    return result + serparators;
}

char *getDataBlockedUsers()
{
    char *result;
    int totalSize = getBlockedUsersSize();
    int resultDataCounter = 0;
    int resultCounter = 0;
    char header[10] = {0};
    intToString(totalSize, header);
    result = malloc(totalSize + strlen(header) + 1);

    for (int i = 0; i < strlen(header); i++)
        result[resultDataCounter++] = header[i];
    result[resultDataCounter++] = ':';
    for (int i = 0; i < 2000; i++)
    {
        if (blockedUsers[i] != 0)
        {
            for (int j = 0; j < strlen(blockedUsers[i]); j++)
                result[resultCounter++] = blockedUsers[i][j];
            result[resultCounter++] = ',';
        }
    }
    return result;
}

char *getDataConnectedUsers()
{
    char *result;
    int totalSize = getConnectedUsersSize();
    int resultDataCounter = 0;
    int resultCounter = 0;
    char header[10] = {0};
    intToString(totalSize, header);
    result = malloc(totalSize + strlen(header) + 1);

    for (int i = 0; i < strlen(header); i++)
        result[resultDataCounter++] = header[i];
    result[resultDataCounter++] = ':';
    for (int i = 0; i < 2000; i++)
    {
        if (CONNNECTED_USERS[i].username != 0)
        {
            for (int j = 0; j < strlen(CONNNECTED_USERS[i].username); j++)
                result[resultCounter++] = CONNNECTED_USERS[i].username[j];
            result[resultCounter++] = ',';
        }
    }
    return result;
}

dataPack processAdminRequest(char *string)
{
    dataPack result;
    result.msgLength = -1;
    char *request = getRequestName(string);

    if (strcmp(request, "getconnectedusers") == 0)
    {
        result.msgLength = getConnectedUsersSize();
        result.data = getDataConnectedUsers();
    }
    else if (strcmp(request, "getlogs") == 0)
    {
        result = getLoggedData();
    }
    else if (strcmp(request, "disconectuser") == 0)
    {
        char *username = getArgumentByIndex(string, 0);
        forceDisconectClient(username);
        free(username);
    }
    else if (strcmp(request, "blockuser") == 0)
    {
        char *username = getArgumentByIndex(string, 0);
        blockUser(username);
        free(username);
    }
    else if (strcmp(request, "unblockuser") == 0)
    {
        char *username = getArgumentByIndex(string, 0);
        unblockUser(username);
        free(username);
    }
    else if (strcmp(request, "getusers") == 0)
    {
        result = getAllUsers();
    }
    else if (strcmp(request, "getblockedusers") == 0)
    {
        result.data = getDataBlockedUsers();
        result.msgLength = getBlockedUsersSize();
    }
    free(request);
    return result;
}

void handleAdminConnections(void)
{
    int serverSocket, clientSocket, len, rc;
    int valread = 0;
    struct sockaddr_un serverSocketAddr;
    struct sockaddr_un clientSocketocketAddr;
    int backlog = 10;
    memset(&serverSocketAddr, 0, sizeof(struct sockaddr_un));
    memset(&clientSocketocketAddr, 0, sizeof(struct sockaddr_un));
    serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    serverSocketAddr.sun_family = AF_UNIX;
    strcpy(serverSocketAddr.sun_path, SOCK_PATH);
    len = sizeof(serverSocketAddr);
    unlink(SOCK_PATH);
    rc = bind(serverSocket, (struct sockaddr *)&serverSocketAddr, sizeof(serverSocketAddr));
    rc = listen(serverSocket, 1);

    char *buffer = malloc(1);
    char *packets;
    int toDownload = 0;
    char header[10] = {0};
    int headerCounter = 0;
    bool gotHeader = false;
    int receiveSize = 1;
    int downloadedSoFar = 0;
    while (1)
    {
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientSocketocketAddr, &len);
        len = sizeof(clientSocketocketAddr);
        getpeername(clientSocket, (struct sockaddr *)&clientSocketocketAddr, &len);
        while (1)
        {
            valread = recv(clientSocket, buffer, receiveSize, 0);
            if (valread != 0)
            {
                if (*buffer == ':' && !gotHeader)
                {
                    free(buffer);
                    gotHeader = true;
                    receiveSize = stringToInt(header);
                    packets = malloc(receiveSize);
                    buffer = malloc(receiveSize);
                    memset(buffer, '\0', receiveSize);
                }
                else if ((*buffer >= '0' && *buffer <= '9') && !gotHeader)
                {
                    header[headerCounter++] = *buffer;
                    free(buffer);
                    buffer = malloc(receiveSize);
                    memset(buffer, '\0', receiveSize);
                }
                else if ((*buffer < '0' || *buffer > '9') && !gotHeader)
                {
                    free(buffer);
                    break;
                }
                else
                {
                    if (downloadedSoFar < receiveSize)
                    {
                        for (int i = 0; i < valread; i++)
                            packets[downloadedSoFar++] = buffer[i];
                        free(buffer);
                        toDownload = receiveSize - downloadedSoFar;
                        if (toDownload == 0)
                        {
                            dataPack result = processAdminRequest(packets);
                            if (result.msgLength == -1)
                                break;
                            headerCounter = 0;
                            memset(header, 0, 10);
                            free(packets);
                            gotHeader = false;
                            downloadedSoFar = 0;
                            receiveSize = 1;
                        }
                        else
                        {
                            buffer = malloc(toDownload);
                        }
                    }
                }
            }
            else
            {
                break;
            }
        }
    }
    close(serverSocket);
    close(clientSocket);
    pthread_exit(NULL);
}

dataPack processRequest(char *string)
{
    dataPack result;

    result.msgLength = -1;
    char *request = getRequestName(string);
    char logMessage[200] = {0};
    if (strcmp(request, "getmessageslist") == 0)
    {
        char *username = getArgumentByIndex(string, 0);
        char *usernameFriend = getArgumentByIndex(string, 1);
        char *token = getArgumentByIndex(string, 2);
        result = getMessagesList(username, usernameFriend, token);
        if (result.msgLength == -1)
        {
            sprintf(logMessage, "error sending messages from user %s to user %s", username, usernameFriend);
            logger(logMessage);
        }
        sprintf(logMessage, "retrieved messages from user %s to user %s", username, usernameFriend);
        logger(logMessage);
        free(username);
        free(usernameFriend);
        free(token);
    }
    else if (strcmp(request, "getnotfriendlist") == 0)
    {
        char *username = getArgumentByIndex(string, 0);
        char *token = getArgumentByIndex(string, 1);
        result = getNotFriendList(username, token);
        if (result.msgLength == -1)
        {
            sprintf(logMessage, "error retrieving users that don't befriend user %s", username);
            logger(logMessage);
        }
        sprintf(logMessage, "retrieved users that don't befriend user %s", username);
        logger(logMessage);
        free(username);
        free(token);
    }
    else if (strcmp(request, "getfriendlist") == 0)
    {
        char *username = getArgumentByIndex(string, 0);
        char *token = getArgumentByIndex(string, 1);
        result = getFriendList(username, token);
        if (result.msgLength == -1)
        {
            sprintf(logMessage, "error retrieveing friends of user %s", username);
            logger(logMessage);
        }
        sprintf(logMessage, "retrieved friends of user %s", username);
        logger(logMessage);
        free(username);
        free(token);
    }
    else if (strcmp(request, "addfriend") == 0)
    {
        char *username = getArgumentByIndex(string, 0);
        char *usernameFriend = getArgumentByIndex(string, 1);
        char *token = getArgumentByIndex(string, 2);
        if (addFriend(username, usernameFriend, token) == OK)
        {
            result.msgLength = -2;
            sprintf(logMessage, "user %s added %s as friend", username, usernameFriend);
            logger(logMessage);
        }
        else
        {
            sprintf(logMessage, "error user %s could not add %s as a friend", username, usernameFriend);
            logger(logMessage);
        }
        free(username);
        free(usernameFriend);
        free(token);
    }
    else if (strcmp(request, "removefriend") == 0)
    {
        char *username = getArgumentByIndex(string, 0);
        char *usernameFriend = getArgumentByIndex(string, 1);
        char *token = getArgumentByIndex(string, 2);
        if (removeFriend(username, usernameFriend, token) == OK)
        {
            result.msgLength = -2;
            sprintf(logMessage, "user %s removed %s from friends list", username, usernameFriend);
            logger(logMessage);
        }
        else
        {
            sprintf(logMessage, "error user %s could not remove %s from friends list", username, usernameFriend);
            logger(logMessage);
        }
        free(username);
        free(usernameFriend);
        free(token);
    }
    else if (strcmp(request, "changeprofilepicture") == 0)
    {
        char *username = getArgumentByIndex(string, 0);
        char *token = getArgumentByIndex(string, 1);
        char *pictureSizeString = getArgumentByIndex(string, 2);
        int pictureSize = stringToInt(pictureSizeString);
        char *picture = getArgumentByIndexFile(string, 3, pictureSize);
        if (changeProfilePicture(username, token, picture, pictureSize) == OK)
        {
            result.msgLength = -2;
            sprintf(logMessage, "user %s changed his profile picture", username);
            logger(logMessage);
        }
        else
        {
            sprintf(logMessage, "error user %s could not change his profile picture", username);
            logger(logMessage);
        }
        free(username);
        free(token);
        free(pictureSizeString);
        free(picture);
    }
    else if (strcmp(request, "changeuserpassword") == 0)
    {
        char *username = getArgumentByIndex(string, 0);
        char *password = getArgumentByIndex(string, 1);
        char *token = getArgumentByIndex(string, 2);
        if (changeUserPassword(username, password, token) == OK)
        {
            result.msgLength = -2;
            sprintf(logMessage, "user %s changed his password", username);
            logger(logMessage);
        }
        else
        {
            sprintf(logMessage, "error user %s could not change his password", username);
            logger(logMessage);
        }
        free(username);
        free(password);
        free(token);
    }
    else if (strcmp(request, "changeuseremail") == 0)
    {
        char *username = getArgumentByIndex(string, 0);
        char *email = getArgumentByIndex(string, 1);
        char *token = getArgumentByIndex(string, 2);
        if (changeUserEmail(username, email, token) == OK)
        {
            result.msgLength = -2;
            sprintf(logMessage, "user %s changed his email", username);
            logger(logMessage);
        }
        else
        {
            sprintf(logMessage, "error user %s could not change his email", username);
            logger(logMessage);
        }
        free(username);
        free(email);
        free(token);
    }
    else if (strcmp(request, "changeuserphone") == 0)
    {
        char *username = getArgumentByIndex(string, 0);
        char *phone = getArgumentByIndex(string, 1);
        char *token = getArgumentByIndex(string, 2);
        if (changeUserPhone(username, phone, token) == OK)
        {
            result.msgLength = -2;
            sprintf(logMessage, "user %s changed his phone number", username);
            logger(logMessage);
        }
        else
        {
            sprintf(logMessage, "error user %s could not change his phone number", username);
            logger(logMessage);
        }
        free(username);
        free(phone);
        free(token);
    }
    else if (strcmp(request, "insertuser") == 0)
    {
        char *username = getArgumentByIndex(string, 0);
        char *email = getArgumentByIndex(string, 1);
        char *phone = getArgumentByIndex(string, 1);
        char *password = getArgumentByIndex(string, 2);
        if (insertUser(username, email, phone, password) == OK)
        {
            result.msgLength = -2;
            sprintf(logMessage, "account created with username %s ", username);
            logger(logMessage);
        }
        else
        {
            sprintf(logMessage, "error creating new account! ");
            logger(logMessage);
        }
        free(username);
        free(phone);
        free(email);
        free(password);
    }
    else if (strcmp(request, "removeuser") == 0)
    {
        char *username = getArgumentByIndex(string, 0);
        char *token = getArgumentByIndex(string, 2);
        if (removeUser(username, token) == OK)
        {
            result.msgLength = -2;
            sprintf(logMessage, "account of user %s was deleted", username);
            logger(logMessage);
        }
        else
        {
            sprintf(logMessage, "error could not delete account of user %s", username);
            logger(logMessage);
        }
        free(username);
        free(token);
    }
    else if (strcmp(request, "sendmessage") == 0)
    {
        char *username = getArgumentByIndex(string, 0);
        char *usernameFriend = getArgumentByIndex(string, 1);
        char *token = getArgumentByIndex(string, 2);
        char *message = getArgumentByIndex(string, 3);
        char *filename = getArgumentByIndex(string, 4);
        char *fileSizeString = getArgumentByIndex(string, 5);
        int fileSize = stringToInt(fileSizeString);
        char *file = getArgumentByIndexFile(string, 6, fileSize);
        if (sendMessage(username, usernameFriend, token, message, filename, file, fileSize) == OK)
        {
            result.msgLength = -2;
            sprintf(logMessage, "message sent from user %s to user %s", username, usernameFriend);
            logger(logMessage);
        }
        else
        {
            sprintf(logMessage, "error sending message from user %s to user %s", username, usernameFriend);
            logger(logMessage);
        }
        free(username);
        free(usernameFriend);
        free(token);
        free(message);
        free(filename);
        free(fileSizeString);
        free(file);
    }
    free(request);
    return result;
}

void handleClient(threadClientArguments *args)
{

    char logMessage[200] = {0};
    int new_socket = args->newSocket;
    int server_fd = args->serverFd;
    char *username = args->username;

    pthread_mutex_lock(&mutexlock);
    connectClient(username, new_socket);
    pthread_mutex_unlock(&mutexlock);
    char *clientIp = args->ip;
    sprintf(logMessage, "client %s logged in succesfully", clientIp);
    logger(logMessage);
    int valread;
    printf("Client connected!\n");
    size_t receiveSize = 1;
    char *buffer;
    char header[10] = {0};
    char headerCounter = 0;
    bool gotHeader = false;
    buffer = malloc(receiveSize);
    memset(buffer, '\0', receiveSize);
    char *packets;
    int downloadedSoFar = 0;
    int toDownload = 0;
    while (1)
    {
        valread = read(new_socket, buffer, receiveSize);
        if (valread != 0)
        {
            if (*buffer == ':' && !gotHeader)
            {
                free(buffer);
                gotHeader = true;
                receiveSize = stringToInt(header);
                packets = malloc(receiveSize);
                buffer = malloc(receiveSize);
                memset(buffer, '\0', receiveSize);
                sprintf(logMessage, "got the ammount of bytes to read: %lu ", receiveSize);
                logger(logMessage);
            }
            else if ((*buffer >= '0' && *buffer <= '9') && !gotHeader)
            {
                header[headerCounter++] = *buffer;
                free(buffer);
                buffer = malloc(receiveSize);
                memset(buffer, '\0', receiveSize);
            }
            else if ((*buffer < '0' || *buffer > '9') && !gotHeader)
            {
                sprintf(logMessage, "error geting header from client %s", clientIp);
                logger(logMessage);
                free(buffer);
                break;
            }
            else
            {

                if (downloadedSoFar < receiveSize)
                {
                    for (int i = 0; i < valread; i++)
                        packets[downloadedSoFar++] = buffer[i];
                    free(buffer);
                    toDownload = receiveSize - downloadedSoFar;
                    sprintf(logMessage, "reading data from client (%d bytes)", valread);
                    logger(logMessage);
                    if (toDownload == 0)
                    {
                        sprintf(logMessage, "finished reading data from client (%lu bytes)", receiveSize);
                        logger(logMessage);
                        dataPack result = processRequest(packets);
                        if (result.msgLength == -1)
                            break;
                        headerCounter = 0;
                        memset(header, 0, 10);
                        free(packets);
                        gotHeader = false;
                        downloadedSoFar = 0;
                        receiveSize = 1;
                    }
                    else
                    {
                        buffer = malloc(toDownload);
                    }
                }
            }
        }
        else
        {
            sprintf(logMessage, "connection to client %s was lost", clientIp);
            logger(logMessage);
            break;
        }
    }

    pthread_mutex_lock(&mutexlock);
    removeDisconectedClient(username);
    pthread_mutex_lock(&mutexlock);

    sprintf(logMessage, "client %s has disconected", clientIp);
    free(args->ip);
    free(args);
    close(new_socket);
    logger(logMessage);
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    bool DEBUGING = true;
    if (!DEBUGING)
    {
        pthread_t adminHandler;
        pthread_create(&adminHandler, NULL, (void *)handleAdminConnections, NULL);
        char logMessage[200] = {0};
        pthread_mutex_init(&mutexlock, NULL);
        int server_fd, new_socket, valread;
        struct sockaddr_in address;

        int opt = 1;
        int addrlen = sizeof(address);

        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            sprintf(logMessage, "error, server socket could not be created");
            logger(logMessage);
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                       &opt, sizeof(opt)))
        {
            sprintf(logMessage, "error setting socket options");
            logger(logMessage);
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        if (bind(server_fd, (struct sockaddr *)&address,
                 sizeof(address)) < 0)
        {
            sprintf(logMessage, "error binding the socket");
            logger(logMessage);
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        if (listen(server_fd, 2000) < 0)
        {
            sprintf(logMessage, "error, server socket could not listen");
            logger(logMessage);
            perror("listen");
            exit(EXIT_FAILURE);
        }

        int receiveSize = 1;
        bool gotHeader = false;
        char *buffer = malloc(receiveSize);
        memset(buffer, '\0', receiveSize);

        char *packets;
        int downloadedSoFar = 0;
        int toDownload = 0;

        char header[10] = {0};
        int headerCounter = 0;
        sprintf(logMessage, "server was succesfully initialized");
        logger(logMessage);
        bool procesing = false;
        char clientIp[INET_ADDRSTRLEN];
        while (1)
        {
            if (!procesing)
            {
                new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                int child = fork();
                if (child == 0)
                {
                    inet_ntop(AF_INET, &address.sin_addr, clientIp, INET_ADDRSTRLEN);
                    timeval tv;
                    tv.tv_sec = 0;
                    tv.tv_usec = 500;
                    setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);

                    if (new_socket > -1)
                        procesing = true;
                    else
                    {
                        sprintf(logMessage, "connection could not be accepted %s", clientIp);
                        logger(logMessage);
                        exit(1);
                    }
                }
            }
            if (procesing)
            {

                valread = read(new_socket, buffer, receiveSize);
                if (valread != 0)
                {
                    if (*buffer == ':' && !gotHeader)
                    {
                        free(buffer);
                        gotHeader = true;
                        receiveSize = stringToInt(header);
                        packets = malloc(receiveSize);
                        buffer = malloc(receiveSize);
                        memset(buffer, '\0', receiveSize);
                    }
                    else if ((*buffer >= '0' && *buffer <= '9') && !gotHeader)
                    {
                        header[headerCounter++] = *buffer;
                        free(buffer);
                        buffer = malloc(receiveSize);
                        memset(buffer, '\0', receiveSize);
                    }
                    else if ((*buffer < '0' || *buffer > '9') && !gotHeader)
                    {
                        sprintf(logMessage, "client %s provided the wrong format for login", clientIp);
                        logger(logMessage);
                        free(buffer);
                        break;
                    }
                    else
                    {

                        if (downloadedSoFar < receiveSize)
                        {
                            for (int i = 0; i < valread; i++)
                                packets[downloadedSoFar++] = buffer[i];
                            free(buffer);
                            toDownload = receiveSize - downloadedSoFar;
                            if (toDownload == 0)
                            {
                                char *username = getArgumentByIndex(packets, 0);
                                char *password = getArgumentByIndex(packets, 1);
                                char *result = loginUser(username, password);
                                if (result[0] == -2)
                                {
                                    sprintf(logMessage, "client %s could not be accepted", clientIp);
                                    close(new_socket);
                                    procesing = false;
                                    logger(logMessage);
                                }
                                else
                                {
                                    if (!isBlocked(username))
                                    {
                                        send(new_socket, result, 20, 0);
                                        timeval tv;
                                        tv.tv_sec = 800;
                                        tv.tv_usec = 0;
                                        setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);
                                        pthread_t connection;
                                        threadClientArguments *args = malloc(sizeof(threadClientArguments));
                                        args->newSocket = new_socket;
                                        args->serverFd = server_fd;
                                        args->ip = (char *)malloc(INET_ADDRSTRLEN);
                                        args->username = username;
                                        memset(args->ip, '\0', INET_ADDRSTRLEN);
                                        pthread_create(&connection, NULL, (void *)handleClient, args);
                                        headerCounter = 0;
                                        memset(header, 0, 10);
                                        free(packets);
                                        gotHeader = false;
                                        downloadedSoFar = 0;
                                        receiveSize = 1;
                                        procesing = false;
                                    }
                                    else
                                    {
                                        close(new_socket);
                                        sprintf(logMessage, "client %s could not be accepted", clientIp);
                                        logger(logMessage);
                                    }
                                }
                                free(password);
                                free(result);
                                exit(0);
                            }
                            else
                            {
                                buffer = malloc(toDownload);
                            }
                        }
                    }
                }
                else
                {
                    sprintf(logMessage, "connection to client %s was lost", clientIp);
                    logger(logMessage);

                    procesing = false;
                }
            }
        }
    }

    // // sprintf(logMessage, "server closed");
    // logger(logMessage);
    pthread_mutex_destroy(&mutexlock);

    char *string = malloc(20);
    memset(string, 0, 20);
    strcpy(string, "getconnectedusers;");
    // char *string2 = malloc(10);
    // memset(string2, 0, 10);
    // strcpy(string2, "getlogs;");
    // char *string3 = malloc(11);
    // memset(string3, 0, 11);
    // strcpy(string3, "getusers;");
    // char *string4 = malloc(18);
    // memset(string4, 0, 18);
    // strcpy(string4, "getblockedusers;");

    char *username = malloc(9);
    memset(username, 0, 9);
    strcpy(username, "qwertyui");
    char *username2 = malloc(10);
    memset(username2, 0, 10);
    strcpy(username2, "zaqxswcde");
    // char *username3 = malloc(10);
    // memset(username3, 0, 10);
    // strcpy(username3, "edcrfvtgb");

    connectClient(username, 45);
    connectClient(username2, 50);
    // blockUser(username3);

    dataPack result = processAdminRequest(string);
    // dataPack result2 = processAdminRequest(string2);
    // printf("%s", result2.data);
    // fflush(stdout);
    // dataPack result3 = processAdminRequest(string3);
    // printf("%s", result3.data);
    // fflush(stdout);
    // dataPack result4 = processAdminRequest(string4);
    printf("%s\n", result.data);
    fflush(stdout);
    return 0;
}
