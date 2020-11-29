//
// Created by tkn on 11/24/20.
//

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAXDATASIZE 512

#define BACKLOG 5


void *get_in_addr( struct sockaddr* sa){
    if (sa->sa_family == AF_INET){
        return & (((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}


int main (int argc, char **argv[]){

    int sockfd, new_fd;
    struct addrinfo hints, *res, *p;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    struct sigaction sa;
    char s[INET6_ADDRSTRLEN];
    int status;


    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use the IP

    if (status= getaddrinfo(NULL, argv[1], &servaddr, &res)!=0) // argv[1] = PORT
    {
        fprintf(stderr, "getassrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    // Loop through all the results
    for (p = res ; p != NULL; p=p->ai_next){
        // Create the socket
        if ((sockfd= socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1 ){
            perror ("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int) )== -1){
        close(sockfd);
        perror("setsockpot");
        exit(1);}

        if( bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            close(sockfd);
            perror("server:bind");
            continue;
        }
        break;
    }

    freeaddrinfo(res);

    if (p == NULL){
        fprintf(stderr, "server : failed to bind \n");
        exit(1);}

    //Listen
    if (listen(sockfd, BACKLOG) == -1){
        perror("listen");
        exit(1);
    }

    printf("server waiting for connections ...\n");

    // Accept the connection
    while (1){
        sin_size= sizeof(their_addr);
        new_fd= accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

        if(new_fd == -1){
            perror("accept");
            continue;
        }
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr)
                , s , sizeof(s));
        printf("server: got connection from %s\n" );

        // Extract the Quote

       // Send the data in new socket
        send(sockfd,msg, sizeof(char)*MAXDATASIZE, 0);

       // close
       close(sockfd);
    }

    return 0;

    // Create a socket
    if (sockfd= socket (AF_INET,SOCK_STREAM,0) <0){
        err_n_die("Error while creating a socket");
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr= htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);// server port

    //listen and bind
    if ((bind(listenfd, (SA*) &servaddr, sizeof(servaddr) )) < 0)
        err_n_die("bind error");

    if ((listen(listenfd, 10)) < 0)
        err_n_die("listen error");

    //Enter the loop
    for ( ;  ; ) {
        struct sockaddr_in addr;
        socklen_t addr_len;
        //accept blocks an incoming connection arrives
        printf("waiting for a connection on port %d\n", PORT);
        fflush(stdout);
        connfd= accept(listenfd, (SA*) NULL, NULL);

        memset(recvline, 0, MAXDATASIZE);

        // read the client
    }


    // create server socket
    int server_socket;
    server_socket = socket(PF_INET, SOCK_STREAM, 0 );

    // Server adress
    struct sockaddr_in server_adress;
    memset(&server_adress, '\0', sizeof(server_adress));
    server_adress.sin_family= AF_INET;
    server_adress.sin_port= htons(9001);
    server_adress.sin_addr.s_addr= INADDR_ANY ;

    // bind the socket to a specified IP and Port
    bind(network_socket, (struct* sockadrr ) &server_adress, sizeof(server_adress));

    listen(server_socket, 6);
    int client_socket;
    client_socket= accept(server_socket, NULL,NULL);

    //Send the message
    char server_message;
    send(client_socket, server_message, sizeof(server_message));

    //Close the socket
    closesocket(server_socket);
    return 0;
}