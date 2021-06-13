#include <stdio.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#define PORT 6000

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

int main(int argc, char **argv)
{
    int sock, valread;
    int server_fd;
    struct sockaddr_in serv_addr;
    char hello[1024] = {0};
    char buffer[10000] = {0};
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
    char *beginQuery = "19:;ghitssierul,mmmmmm";
    char *t = (char *)malloc(22);
    int querySize = strlen(beginQuery);
    memset(t, '\0', 22);
    strcpy(t, beginQuery);
    // int fileSize;
    // FILE *f = fopen("data.zip", "r");
    // fseek(f, 0L, SEEK_END);
    // fileSize = ftell(f);
    // rewind(f);
    // char *file = malloc(fileSize);
    // fread(file, 1, fileSize, f);
    // for (int i = 0; i < fileSize; i++)
    // {
    //     t[querySize++] = file[i];
    // }
    // fclose(f);
    // while (1)
    // {
    sleep(10);
    send(sock, t, 22, 0);
    sleep(10);
    recv(sock, buffer, 100, 0);
    //     sleep(20);
    // // }s
    sleep(15);
    // printf("%s\n", buffer);
    // memset(buffer, '\0', 1024);
    // sleep(15);
    // free(t);
    return 0;
}