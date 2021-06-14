#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdbool.h>
#include <math.h>

#define SERVER_PATH "../../server/unix_socket.server"

#define GET_CONNECTED_USERS 1
#define GET_USERS 2
#define GET_BLOCKED_USERS 3
#define GET_LOGS 4

int stringToInt(char string[10])
{
    int result = 0;
    if (string == NULL)
        return result;

    int initialSize = strlen(string) - 1;
    for (int i = 0; string[i] != '\0'; i++)
        result += ((string[i] - 48) * (pow(10, initialSize--)));
    return result;
}

int getParameterLength(char *string)
{
    int size = 0;
    for (int i = 0; string[i] != ',' && string[i] != '\0' && string[i] != ';'; i++)
    {
        char c = string[i];
        size++;
    }
    return ++size;
}

int getParameterByIndex(char *source, int index)
{
    char *result;
    int destCounter = 0;
    int argumentLength;
    int separators = 0;
    for (int i = 0; source[i] != '\0'; i++)
    {
        char c = source[i];
        if (source[i] == ';')
        {
            separators++;
        }
        else if ((source[i] == '\0' || source[i] == ';') && separators == index)
        {
            break;
        }
        else if (separators == index)
        {
            argumentLength = getParameterLength(&source[i]);
            result = (char *)malloc(argumentLength + 1);
            memset(result, '\0', argumentLength + 1);
            strncpy(result, &source[i], argumentLength - 1);
            int val = stringToInt(result);
            return val;
        }
    }
}

int getRequestType(char *request)
{
    if (strcmp(request, "getconnectedusers") == 0)
    {
        return GET_CONNECTED_USERS;
    }
    else if (strcmp(request, "getlogs") == 0)
    {
        return GET_LOGS;
    }
    else if (strcmp(request, "getusers") == 0)
    {
        return GET_USERS;
    }
    else if (strcmp(request, "getblockedusers") == 0)
    {
        return GET_BLOCKED_USERS;
    }
}

void prettyPrint(char *data, int msgLength, char *request)
{
    int requestType = getRequestType(request);
    if (requestType == GET_CONNECTED_USERS)
    {
        printf("Connected users: \n\n");
        for (int i = 0; i < msgLength; i++)
        {
            if (data[i] == ',')
                printf("\n");
            printf("%c", data[i]);
            fflush(stdout);
        }
    }
    else if (requestType == GET_USERS)
    {
        int usernameLength;
        int emailLength;
        int phoneLength;
        int counterStep = 0;

        char *username;
        char *email;
        char *phone;

        for (int i = 0; i < msgLength; i++)
        {
            usernameLength = getParameterByIndex(&data[i], 0);
            emailLength = getParameterByIndex(&data[i], 1);
            phoneLength = getParameterByIndex(&data[i], 2);
            while (counterStep != 3)
            {
                if (data[i] == ';')
                {
                    counterStep++;
                }
                i++;
            }
            counterStep = 0;
            username = malloc(usernameLength + 1);
            email = malloc(emailLength + 1);
            phone = malloc(phoneLength + 1);
            memset(username, '\0', usernameLength + 1);
            memset(email, '\0', emailLength + 1);
            memset(phone, '\0', phoneLength + 1);

            for (int a = 0; a < usernameLength; a++)
            {
                username[a] = data[i + a];
            }
            i += usernameLength;
            for (int a = 0; a < emailLength; a++)
            {
                email[a] = data[i + a];
            }
            i += emailLength;
            for (int a = 0; a < phoneLength; a++)
            {
                phone[a] = data[i + a];
            }
            i += phoneLength - 1;
            printf("%s     %s     %s\n", username, email, phone);

            free(username);
            free(email);
            free(phone);
        }
    }
    else if (requestType == GET_BLOCKED_USERS)
    {
        printf("Blocked users: \n\n");
        for (int i = 0; i < msgLength; i++)
        {
            if (data[i] == ',')
                printf("\n");
            printf("%c", data[i]);
        }
    }
    else if (requestType == GET_LOGS)
    {
        int logLength;
        int dateLength;
        int counterStep = 0;

        char *log;
        char *date;

        for (int i = 0; i < msgLength; i++)
        {
            logLength = getParameterByIndex(&data[i], 0);
            dateLength = getParameterByIndex(&data[i], 1);
            while (counterStep != 2)
            {
                if (data[i] == ';')
                    counterStep++;
                i++;
            }
            counterStep = 0;
            log = malloc(logLength + 1);
            date = malloc(dateLength + 1);
            memset(log, '\0', logLength + 1);
            memset(date, '\0', dateLength + 1);
            for (int a = 0; a < logLength; a++)
            {
                log[a] = data[i + a];
            }
            i += logLength;
            for (int a = 0; a < dateLength; a++)
            {
                date[a] = data[i + a];
            }
            i += dateLength - 1;

            printf("%s -->  %s\n", log, date);

            free(log);
            free(date);
        }
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
            inputSize = strlen(userInput);

            rc = send(sockfd, userInput, inputSize, 0);
            while (1)
            {
                valread = recv(sockfd, buffer, receiveSize, 0);
                if (valread == -1)
                {
                    perror("valread error ");
                }
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
                            receiveSize = toDownload;
                            if (toDownload == 0)
                            {
                                prettyPrint(packets, downloadedSoFar, userInput);
                                headerCounter = 0;
                                memset(header, 0, 10);
                                free(packets);
                                gotHeader = false;
                                downloadedSoFar = 0;
                                receiveSize = 1;
                                buffer = malloc(1);
                                break;
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
            printf("command %s execute\n", userInput);
        }
        else
        {
            printf("command %s not found, use 'help' if needed\n", userInput);
        }
        memset(userInput, '\0', 100);
    }
    close(sockfd);
}