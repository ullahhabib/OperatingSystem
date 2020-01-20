
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include<iostream>
using namespace std;
#define MAXLINE 4096 /*max text line length*/

#define LISTENQ 8 /*maximum number of client connections */

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    int portno = atoi(argv[1]);
    int listenfd, connfd, n;
    socklen_t clilen;
    char buf[MAXLINE];
    struct sockaddr_in cliaddr, servaddr;
    int state = -2, number_of_child = 0, count = 0;
    int *state_array = new int[100];

    if (number_of_child < 1) { std::cout << "Error reading child!\n"; }
    while (cin >> state)
    {
        state_array[count++] = state;
        std::cout << state << " \n";
    }
    //creation of the socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    //preparation of the socket address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(portno);

    bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);

    printf("%s\n", "Server running...waiting for connections.");

    for (; ; ) {

        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);

        count = 0;

        int received_int = 0;

        while ((n = recv(connfd, &received_int, sizeof(received_int), 0)) > 0) {
            int number_to_send = state_array[count++]; // Put your value
            int converted_number = htonl(number_to_send);

            // Write the number to the opened socket
            send(connfd, &converted_number, sizeof(converted_number), 0);
        }

        if (n < 0) {
            perror("Read error");
            exit(1);
        }
        close(connfd);

    }
    //close listening socket
    close(listenfd);
}