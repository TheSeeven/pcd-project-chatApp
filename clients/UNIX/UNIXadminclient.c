#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdbool.h>

#define SERVER_PATH "unix_socket.server"

#define GET_CONNECTED_USERS 1
#define GET_USERS 2
#define GET_BLOCKED_USERS 3
#define GET_LOGS 4

int getRequestType(char *request)
{
    if (strcmp(request, "getconnectedusers"))
    {
        return GET_CONNECTED_USERS;
    }
    else if (strcmp(request, "getlogs"))
    {
        return GET_LOGS;
    }
    else if (strcmp(request, "getusers"))
    {
        return GET_USERS;
    }
    else if (strcmp(request, "getblockedusers"))
    {
        return GET_BLOCKED_USERS;
    }
}

void prettyPrint(char *data, int msgLength, char *request)
{
    int requestType = getRequestType(request);
    if (requestType == GET_CONNECTED_USERS)
    {
    }
    else if (requestType == GET_USERS)
    {
    }
    else if (requestType == GET_BLOCKED_USERS)
    {
    }
    else if (requestType == GET_LOGS)
    {
    }
}

int expectsData(char *userInput)
{
    if (strcmp(userInput, "getconnectedusers") == 0)
        return 2;
    else if (strcmp(userInput, "getlogs") == 0)
        return 2;
    else if (strcmp(userInput, "getusers") == 0)
        return 2;
    else if (strcmp(userInput, "getblockedusers") == 0)
        return 2;
    else if (strcmp(userInput, "disconectuser") == 0)
        return 1;
    else if (strcmp(userInput, "blockuser") == 0)
        return 1;
    else if (strcmp(userInput, "unblockuser") == 0)
        return 1;
    return 0;
}

int main(int argc, char *argv[])
{
    int sockfd, rc, bytesReceived;
    struct sockaddr_un serveraddr;

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket() failed");
        return 0;
    }
    // bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sun_family = AF_UNIX;
    strcpy(serveraddr.sun_path, SERVER_PATH);

    rc = connect(sockfd, (struct sockaddr *)&serveraddr, SUN_LEN(&serveraddr));
    if (rc < 0)
    {
        perror("connect() failed");
        return 0;
    }
    char userInput[100] = {0};
    int inputSize = 0;
    int valread;
    char *buffer = malloc(1);
    bool gotHeader = false;
    int receiveSize = 1;
    char header[10] = {0};
    int headerCounter = 0;
    char *packets;
    int downloadedSoFar = 0;
    int toDownload = 0;
    int inputCode;
    while (1)
    {
        scanf("%s", userInput);
        inputCode = expectsData(userInput);
        if (inputCode == 2)
        {
            inputSize = strlen(inputSize);

            rc = send(sockfd, userInput, inputSize, 0);
            while (1)
            {
                valread = recv(sockfd, &buffer[bytesReceived], receiveSize, 0);
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

                                prettyPrint(packets, receiveSize, userInput);

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
        else if (inputCode == 1)
        {
        }
        else
        {
        }
        memset(userInput, '\0', 100);
    }
    close(sockfd);
}