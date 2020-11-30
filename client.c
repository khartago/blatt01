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

void *get_in_addr( struct sockaddr* sa){
    if (sa->sa_family == AF_INET){
        return & (((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}


int main (int argc, char *argv[]){

    int sockfd, nbbytes;
    char buf [MAXDATASIZE];
    struct addrinfo hints, *res, *p;
    int status;
    char s [INET6_ADDRSTRLEN];


// Usage check
    if (argc != 3) {
        fprintf(stderr, "usage:Client hostname \n");
        exit(1);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;


// IP Adress save or hostname to IP adress transformation

    for  (p=res; p !=NULL; p= p->ai_next){

        if (p->ai_family == AF_INET){
            void *addr;
            struct addrinfo p = res;

            struct sockaddr_in *ipv4 = (struct sockaddr_in *) p->ai_addr;
            addr = &(ipv4->sin_addr);

        }else if (p->ai_family == AF_INET6){
            struct sockaddr_in *ipv6 = (struct sockaddr_in6 *) p->ai_addr;
            addr = &(ipv6->sin6_addr);

        }
    }
    freeaddrinfo(res);

    argv[1]= (*addr);

    // Get Server info

    if (status= getaddrinfo(argv[1], argv[2], &servaddr, &res)!=0) // argv[2] = PORT
        {
        fprintf(stderr, "getassrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    // Loop through all the results
    for (p = res ; p != NULL; p=p->ai_next){
        // Create the socket
    if ((sockfd= socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1 ){
        perror ("client: socket");
        continue;
    }
    // Connect the socket
    if( connect( sockfd, p->ai_addr, p->ai_addrlen) ==-1 ){
        close(sockfd);
        perror("client:connect");
        continue;
    }
    break;

}
    if (p == NULL){
    fprintf(stderr, "Client : failed to connect \n");
        return 2;}

    // converts ipv4 or ipv6 from binary to a string
    inet_ntop(p->ai_family, getaddrinfo((struct sockaddr *)p->ai_addr), s, sizeof(s));
    printf(" Client connecting to %s\n", s);
    //

    freeaddrinfo(res);

    // recieve the Data
    while (recv(sockfd, buf, MAXDATASIZE-1, 0) != 0) {
        if ((nbbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1) {
            perror("recv");
            exit(1)
        }
        //Print the data
      if(int w= write(buf, sizeof(char), MAXDATASIZE)) == -1)
          printf(" write fail");
        exit(2);
    }
    // close the socket
    close(sockfd);
    return 0;
}



