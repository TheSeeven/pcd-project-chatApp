#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SERVER_PATH "PATH" // TODO: Add a valid path for both client and server
#define BUFFER_LENGTH 250
#define FALSE 0

int main(int argc, char *argv[]){
    int sockfd, rc, bytesReceived;
    char buffer[BUFFER_LENGTH];
    struct sockaddr_un serveraddr;

    do{
        sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
        if(sockfd < 0){
            perror("socket() failed");
            break;
        }

        bzero(&serveraddr,sizeof(serveraddr));
        serveraddr.sun_family = AF_UNIX;
        if(argc > 1){
            strcpy(serveraddr.sun_path, argv[1]);
        }
        else{
            strcpy(serveraddr.sun_path, SERVER_PATH);
        }
        rc = connect (sockfd, (struct sockaddr*)&serveraddr, SUN_LEN(&serveraddr));
        if(rc < 0){
            perror("connect() failed");
            break;
        }

        bzero(&serveraddr,sizeof(serveraddr));
        rc = send(sockfd, buffer, sizeof(buffer), 0);
        if(rc < 0){
            perror("send() failed");
            break;
        }
        bytesReceived=0;
        while(bytesReceived<BUFFER_LENGTH){
            rc = recv(sockfd, &buffer[bytesReceived], BUFFER_LENGTH - bytesReceived, 0);
            if(rc < 0){
                perror("recv() failed");
                break;
            }
            else if(rc == 0){
                printf("The server closed the connection\n");
                break;
            }
            bytesReceived += rc;
        }
    }while(FALSE);
    if(sockfd != -1){
        close(sockfd);
    }

}