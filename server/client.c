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
    char *t = "60:getmessageslist;ghitssierul,ghaaaaarul,tsk7EHqSs4n3JgYvwEo2";

    // fgets(hello, 1024, stdin);
    while (1)
    {
        send(sock, t, 63, 0);
        read(sock, buffer, 10000);
        printf("%s\n", buffer);
        memset(buffer, '\0', 1024);
        sleep(10);
    }

    // break;

    return 0;
}