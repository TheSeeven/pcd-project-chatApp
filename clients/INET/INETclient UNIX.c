#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <sys/types.h>

#define PORT 6000

#define GET_MESSAGES_LIST 1
#define GET_NOT_FRIEND_LIST 2
#define GET_FRIEND_LIST 3

bool isEnd(char msg[])
{
    char *endmsg = "sfarsit";
    for (int i = 0; i < 7; i++)
    {
        if (msg[i] != endmsg[i])
            return false;
    }
    return true;
}

int putere(int number)
{
    int x=10;
    for(int i=0;i<number;i++)
    {
        x=x*x;
    }
    return x;
}

int stringToInt(char string[10])
{
    int result = 0;
    if (string == NULL)
        return result;

    int initialSize = strlen(string) - 1;
    for (int i = 0; string[i] != '\0'; i++)
        result += ((string[i] - 48) * (putere(initialSize--)));
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
    if (strcmp(request, "getmessageslist"))
    {
        return GET_MESSAGES_LIST;
    }
    else if (strcmp(request, "getnotfriendlist"))
    {
        return GET_NOT_FRIEND_LIST;
    }
    else if (strcmp(request, "getfriendlist"))
    {
        return GET_FRIEND_LIST;
    }
}

void prettyPrint(char *data, int msgLength, char *request)
{
    int requestType = getRequestType(request);

    if (requestType == GET_MESSAGES_LIST)
    {
        int usernameLength;
        int messageLength;
        int dateLength;
        int counterStep = 0;

        char *username;
        char *message;
        char *date;

        for (int i = 0; i < msgLength; i++)
        {
            usernameLength = getParameterByIndex(&data[i], 0);
            messageLength = getParameterByIndex(&data[i],1);
            dateLength = getParameterByIndex(&data[i], 2);
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
            message = malloc(messageLength+1);
            date = malloc(dateLength+1);
            memset(username, '\0', usernameLength + 1);
            memset(message, '\0',messageLength+1);
            memset(date, '\0', dateLength + 1);

            for (int a = 0; a < usernameLength; a++)
            {
                username[a] = data[i + a];
            }
            i += usernameLength;
            for (int a = 0; a < messageLength; a++)
            {
                message[a] = data[i + a];
            }
            i += messageLength;
            for (int a = 0; a < dateLength; a++)
            {
                date[a] = data[i + a];
            }
            i += dateLength - 1;
            printf("%s     %s     %s\n", username,message,date);

            free(username);
            free(message);
            free(date);
        }
    }
    else if (requestType == GET_NOT_FRIEND_LIST)
    {
        int usernameLength;
        int emailLength;
        int phoneLength;
        int profilePictureLength;
        int dateLength;
        int counterStep = 0;

        char *username;
        char *email;
        char *phone;
        char *profilePicture;
        char *date;

        for (int i = 0; i < msgLength; i++)
        {
            usernameLength = getParameterByIndex(&data[i], 0);
            emailLength = getParameterByIndex(&data[i], 1);
            phoneLength = getParameterByIndex(&data[i], 2);
            profilePictureLength = getParameterByIndex(&data[i], 3);
            while (counterStep != 4)
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
            profilePicture = malloc(profilePictureLength+1);
            memset(username, '\0', usernameLength + 1);
            memset(email, '\0', emailLength + 1);
            memset(phone, '\0', phoneLength + 1);
            memset(profilePicture, '\0', profilePictureLength + 1);

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
            i += phoneLength ;
            for (int a = 0; a < profilePictureLength; a++)
            {
                profilePicture[a] = data[i + a];
            }
            i += profilePictureLength -1;
            printf("%s     %s     %s    %s\n", username, email, phone,profilePicture);

            free(username);
            free(email);
            free(phone);
            free(profilePicture);
            free(date);
        }
    }
    else if (requestType == GET_FRIEND_LIST)
    {
        int usernameLength;
        int emailLength;
        int phoneLength;
        int profilePictureLength;
        int dateLength;
        int counterStep = 0;

        char *username;
        char *email;
        char *phone;
        char *profilePicture;
        char *date;

        for (int i = 0; i < msgLength; i++)
        {
            usernameLength = getParameterByIndex(&data[i], 0);
            emailLength = getParameterByIndex(&data[i], 1);
            phoneLength = getParameterByIndex(&data[i], 2);
            profilePictureLength = getParameterByIndex(&data[i], 3);
            dateLength = getParameterByIndex(&data[i], 4);
            while (counterStep != 5)
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
            profilePicture = malloc(profilePictureLength+1);
            date = malloc(dateLength+1);
            memset(username, '\0', usernameLength + 1);
            memset(email, '\0', emailLength + 1);
            memset(phone, '\0', phoneLength + 1);
            memset(profilePicture, '\0', profilePictureLength + 1);
            memset(date, '\0', dateLength + 1);

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
            i += phoneLength ;
            for (int a = 0; a < profilePictureLength; a++)
            {
                profilePicture[a] = data[i + a];
            }
            i += profilePictureLength ;
            for (int a = 0; a < dateLength; a++)
            {
                date[a] = data[i + a];
            }
            i += dateLength - 1;
            printf("%s     %s     %s    %s     %s\n", username, email, phone,profilePicture,date);

            free(username);
            free(email);
            free(phone);
            free(profilePicture);
            free(date);
        }
    }
}

int expectsData(char *userInput)
{
    if (strcmp(userInput, "getmessageslist") == 0)
        return 2;
    else if (strcmp(userInput, "getnotfriendlist") == 0)
        return 2;
    else if (strcmp(userInput, "getfriendlist") == 0)
        return 2;
    else if (strcmp(userInput, "addfriend") == 0)
        return 1;
    else if (strcmp(userInput, "removefriend") == 0)
        return 1;
    else if (strcmp(userInput, "changeprofilepicture") == 0)
        return 1;
    else if (strcmp(userInput, "changeuserpassword") == 0)
        return 1;
    else if (strcmp(userInput, "changeuseremail") == 0)
        return 1;
    else if (strcmp(userInput, "changeuserphone") == 0)
        return 1;
    else if (strcmp(userInput, "sendmessage") == 0)
        return 1;
    return 0;
}

int main(int argc, char **argv)
{
    int sock, valread, rc,bytesReceived;;
    struct sockaddr_in serv_addr;
    int addrlen = sizeof(serv_addr);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    char userInput[100] = {0};
    int inputSize = 0;
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

            rc = send(sock, userInput, inputSize, 0);
            while (1)
            {
                valread = recv(sock, &buffer[bytesReceived], receiveSize, 0);
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
            printf("command %s execute\n", userInput);
        }
        else
        {
            printf("command %s not found, use 'help' if needed\n", userInput);
        }
        memset(userInput, '\0', 100);
    }
    close(sock);
}