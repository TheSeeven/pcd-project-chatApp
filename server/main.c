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

#define PORT 6000

typedef struct timeval timeval;

dataPack
processRequest(char *string)
{
    dataPack result;

    result.msgLength = -1;

    char *request = getRequestName(string);

    if (strcmp(request, "getmessageslist") == 0)
    {
        char *username = getArgumentByIndex(string, 0);
        char *usernameFriend = getArgumentByIndex(string, 1);
        char *token = getArgumentByIndex(string, 2);
        result = getMessagesList(username, usernameFriend, token);

        free(username);
        free(usernameFriend);
        free(token);
    }
    else if (strcmp(request, "getnotfriendlist") == 0)
    {
        char *username = getArgumentByIndex(string, 0);
        char *token = getArgumentByIndex(string, 1);
        result = getNotFriendList(username, token);

        free(username);
        free(token);
    }
    else if (strcmp(request, "getfriendlist") == 0)
    {
        char *username = getArgumentByIndex(string, 0);
        char *token = getArgumentByIndex(string, 1);
        result = getFriendList(username, token);

        free(username);
        free(token);
    }
    else if (strcmp(request, "addfriend") == 0)
    {
        char *username = getArgumentByIndex(string, 0);
        char *usernameFriend = getArgumentByIndex(string, 1);
        char *token = getArgumentByIndex(string, 2);
        if (addFriend(username, usernameFriend, token) == OK)
            result.msgLength = -2;

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
            result.msgLength = -2;

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
            result.msgLength = -2;

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
            result.msgLength = -2;

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
            result.msgLength = -2;

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
            result.msgLength = -2;

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
            result.msgLength = -2;

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
            result.msgLength = -2;

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
            result.msgLength = -2;

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

void handleClient(int *args)
{
    int new_socket = args[0];
    int server_fd = args[1];
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
            break;
        }
    }
    free(args);
    close(new_socket);
    printf("Client Disconected!\n");
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    bool DEBUGING = false;
    if (!DEBUGING)
    {
        int server_fd, new_socket, valread;
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);

        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                       &opt, sizeof(opt)))
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        if (bind(server_fd, (struct sockaddr *)&address,
                 sizeof(address)) < 0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        if (listen(server_fd, 2000) < 0)
        {
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

        bool procesing = false;
        while (1)
        {
            if (!procesing)
            {
                new_socket = accept(server_fd, (struct sockaddr *)&address,
                                    (socklen_t *)&addrlen);
                int child = fork();
                if (child == 0)
                {
                    timeval tv;
                    tv.tv_sec = 0;
                    tv.tv_usec = 500;
                    setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);

                    if (new_socket > -1)
                        procesing = true;
                    else
                        exit(1);
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
                                    close(new_socket);
                                    procesing = false;
                                    printf("Unacepted client!\n");
                                }
                                else
                                {
                                    send(new_socket, result, 20, 0);
                                    timeval tv;
                                    tv.tv_sec = 800;
                                    tv.tv_usec = 0;
                                    setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);
                                    pthread_t connection;
                                    int *ThreadArgs = (int *)malloc(sizeof(int) * 2);
                                    ThreadArgs[0] = new_socket;
                                    ThreadArgs[1] = server_fd;
                                    pthread_create(&connection, NULL, (void *)handleClient, ThreadArgs);

                                    headerCounter = 0;
                                    memset(header, 0, 10);
                                    free(packets);
                                    gotHeader = false;
                                    downloadedSoFar = 0;
                                    receiveSize = 1;
                                    procesing = false;
                                }
                                free(username);
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
                    printf("Lost connection to client!\n");

                    procesing = false;
                }
            }

            //////////////////////////////////////////
        }
        return 0;
    }

    char *request = "sendMessage;ghitssierul,ghaaaaarul,tsk7EHqSs4n3JgYvwEo2,plm,firier.txt,";
    char *username = getArgumentByIndex(request, 0);
    char *usernameFriend = getArgumentByIndex(request, 1);
    char *token = getArgumentByIndex(request, 2);
    char *message = getArgumentByIndex(request, 3);
    char *filenamme = getArgumentByIndex(request, 4);
    char *file = getArgumentByIndexFile(request, 5, 40);
    if (message == NULL)
    {
        printf("file!");
    }
    if (file == NULL)
    {
        printf("message!");
    }
    return 0;
}
