#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

int main(int argc, char *argv[])
{
    int serverfd = 0, clientfd = 0;
    struct sockaddr_in serv_addr;

    char sendBuff[1025];
    time_t ticks;

    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    bind(serverfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(serverfd, 1);

    clientfd = accept(serverfd, (struct sockaddr*)NULL, NULL);

    snprintf(sendBuff, sizeof(sendBuff), "Hello World\n");
    write(clientfd, sendBuff, strlen(sendBuff));

    close(clientfd);
}
