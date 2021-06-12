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

dataPack processRequest(char *string)
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
        free(request);
    }
    // else if (strcmp(request, "getnotfriendlist") == 0)
    // {
    //     char *username = getArgumentByIndex(string, 0);
    //     char *token = getArgumentByIndex(string, 1);
    //     result = getNotFriendList(username, token);

    //     free(username);
    //     free(token);
    //     free(request);
    // }
    // else if (strcmp(request, "getfriendlist") == 0)
    // {
    //     char *username = getArgumentByIndex(string, 0);
    //     char *token = getArgumentByIndex(string, 1);
    //     result = getFriendList(username, token);

    //     free(username);
    //     free(token);
    //     free(request);
    // }
    // else if (strcmp(request, "addfriend") == 0)
    // {
    //     char *username = getArgumentByIndex(string, 0);
    //     char *usernameFriend = getArgumentByIndex(string, 1);
    //     char *token = getArgumentByIndex(string, 2);
    //     result = addFriend(username, usernameFriend, token);

    //     free(username);
    //     free(usernameFriend);
    //     free(token);
    //     free(request);
    // }
    // else if (strcmp(request, "removefriend") == 0)
    // {
    //     char *username = getArgumentByIndex(string, 0);
    //     char *usernameFriend = getArgumentByIndex(string, 1);
    //     char *token = getArgumentByIndex(string, 2);
    //     result = removeFriend(username, usernameFriend, token);

    //     free(username);
    //     free(usernameFriend);
    //     free(token);
    //     free(request);
    // }
    // else if (strcmp(request, "changeprofilepicture") == 0)
    // {
    //     char *username = getArgumentByIndex(string, 0);
    //     char *token = getArgumentByIndex(string, 1);

    //     result = changeProfilePicture(username, token);

    //     free(username);
    //     free(usernameFriend);
    //     free(token);
    //     free(request);
    // }
    // else if (strcmp(request, "changeuserpassword") == 0)
    // {
    //     char *username = getArgumentByIndex(string, 0);
    //     char *password = getArgumentByIndex(string, 1);
    //     char *token = getArgumentByIndex(string, 2);
    //     result = changeUserPassword(username, password, token);

    //     free(username);
    //     free(password);
    //     free(token);
    //     free(request);
    // }
    // else if (strcmp(request, "changeuseremail") == 0)
    // {
    //     char *username = getArgumentByIndex(string, 0);
    //     char *email = getArgumentByIndex(string, 1);
    //     char *token = getArgumentByIndex(string, 2);
    //     result = changeUserEmail(username, email, token);

    //     free(username);
    //     free(email);
    //     free(token);
    //     free(request);
    // }
    // else if (strcmp(request, "changeuserphone") == 0)
    // {
    //     char *username = getArgumentByIndex(string, 0);
    //     char *phone = getArgumentByIndex(string, 1);
    //     char *token = getArgumentByIndex(string, 2);
    //     result = changeUserPhone(username, phone, token);

    //     free(username);
    //     free(phone);
    //     free(token);
    //     free(request);
    // }
    return result;
}

void handleClient(int *args)
{
    int new_socket = args[0];
    int server_fd = args[1];
    int valread;
    printf("Client connected!\n");
    size_t curentMessageLength = 1;
    char *buffer;
    char header[10] = {0};
    char headerCounter = 0;

    bool gotHeader = false;
    buffer = malloc(curentMessageLength);
    memset(buffer, '\0', curentMessageLength);
    while (1)
    {
        valread = read(new_socket, buffer, curentMessageLength);
        if (valread != 0)
        {
            if (*buffer == ':' && !gotHeader)
            {
                free(buffer);
                gotHeader = true;
                curentMessageLength = stringToInt(header);
                buffer = malloc(curentMessageLength);
                memset(buffer, '\0', curentMessageLength);
            }
            else if ((*buffer >= '0' && *buffer <= '9') && !gotHeader)
            {
                header[headerCounter++] = *buffer;
                free(buffer);
                buffer = malloc(curentMessageLength);
                memset(buffer, '\0', curentMessageLength);
            }
            else if ((*buffer < '0' || *buffer > '9') && !gotHeader)
            {
                free(buffer);
                break;
            }
            else
            {
                dataPack result = processRequest(buffer);
                if (result.msgLength == -1)
                    break;
                send(new_socket, result.data, result.msgLength, 0);
                free(buffer);
                curentMessageLength = 1;
                buffer = malloc(curentMessageLength);
                memset(buffer, '\0', curentMessageLength);
                gotHeader = false;
                headerCounter = 0;
                memset(header, 0, 10);
            }
        }
        else
        {
            int error = 0;
            socklen_t len = sizeof(error);
            int retval = getsockopt(new_socket, SOL_SOCKET, SO_ERROR, &error, &len);
            if (error != 0)
            {
                printf("Client Disconected!");
                pthread_exit(NULL);
            }
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
    // initializeDatabase();
    // dataPack t = getUser("ghitssierul", "tsk7EHqSs4n3JgYvwEo2");
    // FILE *f = fopen("test", "a+");
    // for (int i = 0; i < t.msgLength; i++)
    // {
    //     fputc(t.data[i], f);
    // }
    // fclose(f);

    // dataPack t = getMessagesList("ghitssierul", "ghaaaaarul", "tsk7EHqSs4n3JgYvwEo2");
    // FILE *f = fopen("test", "a+");
    // for (int i = 0; i < t.msgLength; i++)
    //     fputc(t.data[i], f);
    // fclose(f);
    // char test1[10] = {0};
    // intToString(1254, test1);

    // dataPack t = getPicture("ghitssierul", "zxcvbnm", "tsk7EHqSs4n3JgYvwEo2");
    // FILE *f = fopen("test", "a+");
    // for (int i = 0; i < t.msgLength; i++)
    //     fputc(t.data[i], f);
    // fclose(f);
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
        while (1)
        {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                     (socklen_t *)&addrlen)) < 0)
            {
                perror("Client connection error!");
                continue;
            }
            pthread_t connection;
            int *ThreadArgs = (int *)malloc(sizeof(int) * 2);
            ThreadArgs[0] = new_socket;
            ThreadArgs[1] = server_fd;
            pthread_create(&connection, NULL, (void *)handleClient, ThreadArgs);
        }
        return 0;
    }

    // char *username;
    // // char *usernameF;
    // // char *token;
    // char *request = "43:getmessageslist;ghitssierul,ghaaaaarul,tsk7EHqSs4n3JgYvwEo2";
    // // // getMessageListArguments(request, username, usernameF, token);

    dataPack t = processRequest("43:getmessageslist;ghitssierul,ghaaaaarul,tsk7EHqSs4n3JgYvwEo2");
    // username = getRequestName(request);
    // printf("%s", username);
    FILE *f = fopen("test", "a+");
    for (int i = 0; i < t.msgLength; i++)
        fputc(t.data[i], f);
    fclose(f);
    return 0;
}

// insertUser("ghaassssaaarul", "plmssssssscom", "sasssssssss456", "dsassssssssssa");
// updateProfilePicture("ghitssierul", fopen("poza.jpeg", "rb"));
// changeUserPassword("ghitssierul", "mmmmmm");
// removeUser("ghitierul");
// addFriend("ghitssierul", "ghaaaaarul");
// removeFriend("ghitssierul", "ghaaaaarul");

//loginUser("ghitssierul", "mmmmmm");
// logoutUser("ghitssierul", "OfbKWsu4gRfVMrt1kXjp");
// deleteOldToken();
// printf("%d\n", isGoodPassword("RRRrRRRR"));
// printf("%d\n", isGoodUsername("RRRrRRRR"));
// printf("%d\n", isPhoneNumber("0757330934"));
// printf("%d\n", isPhoneNumber("075733w934"));
// printf("%d\n", isPhoneNumber("757123423"));
// printf("%d\n", verifyAuthToken("ghitssierul", "tsk7EHqSs4n3JgYvwEo2"));
// printf("%d\n", verifyAuthToken("ghitssierul", "tsk7ssss4n3JgYvwEo2"));
// printf("%d\n", verifyAuthToken("PPierul", "tsk7ssss4n3JgYvwEo2"));
// printf("%d\n", verifyAuthToken("Ahitssierul", "tsk7EHqSs4n3JgYvwEo2"));
// printf("%d\n", verifyAuthToken("", "tsk7EHqSs4n3JgYvwEo2"));
// printf("%d\n", verifyAuthToken("", "tsk7EHqSs4n3JgYvwEo2"));
// printf("%d\n", verifyAuthToken("", ""));
// printf("%d\n", verifyAuthToken("ghitssierul", ""));
// printf("%d\n", verifyAuthToken("", "tsk7EHqSs4n3JgYvwEo2"));