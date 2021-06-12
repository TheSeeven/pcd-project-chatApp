#include "dbInterface.h"
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

int stringToInt(char string[10])
{
    int result = 0;
    int initialSize = strlen(string) - 1;
    for (int i = 0; string[i] != '\0'; i++)
        result += ((string[i] - 48) * (pow(10, initialSize--)));
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
    while (1)
    {
        buffer = malloc(curentMessageLength);
        valread = read(new_socket, buffer, 1);
        if (valread == ':')
        {
            // curentMessageLength =
        }
        else if ((valread >= '0' && valread <= '9') && gotHeader == false)
        {
            header[headerCounter++] = valread;
        }
        else
        {
            break;
        }
        free(buffer);
    }
    printf("Client Disconected!\n");
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
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

    // int server_fd, new_socket, valread;
    // struct sockaddr_in address;
    // int opt = 1;
    // int addrlen = sizeof(address);

    // if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    // {
    //     perror("socket failed");
    //     exit(EXIT_FAILURE);
    // }

    // if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
    //                &opt, sizeof(opt)))
    // {
    //     perror("setsockopt");
    //     exit(EXIT_FAILURE);
    // }
    // address.sin_family = AF_INET;
    // address.sin_addr.s_addr = INADDR_ANY;
    // address.sin_port = htons(PORT);

    // if (bind(server_fd, (struct sockaddr *)&address,
    //          sizeof(address)) < 0)
    // {
    //     perror("bind failed");
    //     exit(EXIT_FAILURE);
    // }
    // if (listen(server_fd, 2000) < 0)
    // {
    //     perror("listen");
    //     exit(EXIT_FAILURE);
    // }
    // while (1)
    // {
    //     if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
    //                              (socklen_t *)&addrlen)) < 0)
    //     {
    //         perror("Client connection error!");
    //         continue;
    //     }
    //     pthread_t connection;
    //     int *ThreadArgs = (int *)malloc(sizeof(int) * 2);
    //     ThreadArgs[0] = new_socket;
    //     ThreadArgs[1] = server_fd;
    //     pthread_create(&connection, NULL, (void *)handleClient, ThreadArgs);
    // }

    // char s[10] = {0};
    // strcpy(s, "4201");
    // printf("%d\n", stringToInt(s));
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