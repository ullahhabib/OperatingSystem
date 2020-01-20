#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;
int
main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;
    if (argc < 3) {
        std::cerr << "usage hostname port\n";
        exit(0);
    }
    int portno = atoi(argv[2]);
    //basic check of the arguments
    //additional checks can be inserted


    //Create a socket for the client
    //If sockfd<0 there was an error in the creation of the socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Problem in creating the socket");
        exit(2);
    }

    //Creation of the socket
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(portno); //convert to big-endian order

    //Connection of the client to the socket
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("Problem in connecting to the server");
        exit(3);
    }
    int i = 0;

    int send_request = htonl(1);
    send(sockfd, &send_request, sizeof(send_request), 0);

    int no_child = 0;

    if (recv(sockfd, &no_child, sizeof(no_child), 0) > 0) {
        cout << "Received int = "<< ntohl(no_child)<< "\n";
    }
    no_child = ntohl(no_child);
    int pipefd[--no_child + 1][2];
    int pid;
    int token;
    int state = 0;
    for(i = 0; i <= no_child; i++)
    {
        if (pipe(pipefd[i]) < 0)
        {
            printf("Pipe Creation failed\n");
            exit(1);
        }
    }
    for(i = 0; i < no_child; i++)
    {
        if ((pid=fork())==0)
        {
            while(1)
            {
                read(pipefd[i][0], &token, sizeof(token) );
                send_request = htonl(1);
                send(sockfd, &send_request, sizeof(send_request), 0);

                state = 0;

                if (recv(sockfd, &state, sizeof(state), 0) > 0) {

                }
                state = ntohl(state);

                if (token >= 0 && state == 1)
                    printf("Process %d  is using the network\n",i+1);
                if (token == -1 || state==-1)
                {
                    printf("Ending\n");
                    token = -1;
                    write(pipefd[i+1][1],&token,sizeof(token));
                    _exit(0);
                }
                token=token+1;
                write(pipefd[i+1][1],&token,sizeof(token));
            }

        }
        //These ends are no longer needed in parent.
        close (pipefd[i][0]);
        close(pipefd[i + 1][1]);
    }
    while(1)
    {
        send_request = htonl(1);
        send(sockfd, &send_request, sizeof(send_request), 0);
        state = 0;
        recv(sockfd, &state, sizeof(state), 0);
        state = ntohl(state);

        if (state==1)
            printf("Process 0  is using the network\n");
        token=token+1;
        write(pipefd[0][1],&token, sizeof(int));
        //Read from the pipe, which is attached to the last child created
        read(pipefd[no_child][0], &token, sizeof(int));
        if (token == -1 ||state == -1)
        {
            token = -1;
            printf("Parent breaking\n");
            write(pipefd[0][1],&token, sizeof(int));
            break;
        }
    }


    for(i=0;i<no_child;i++){
        wait(NULL);}

    exit(0);
}