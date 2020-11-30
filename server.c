#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>

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
    char buf;


    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use the IP

    if (status= getaddrinfo(NULL, argv[1], &hints, &res)!=0) // argv[1] = PORT
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

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &res, sizeof(int) )== -1){
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
        FILE *pFile ;
        fopen(argv[2], "r");
        int bytes_read= getline(buf, MAXDATASIZE , pFile);
        if (bytes_read == -1){
            perror("getline");
        }
        fclose(pFile);

        // Send the data in new socket
        if (send(sockfd, buf, sizeof(char)*MAXDATASIZE, 0) == -1 ){
            perror("send");}

        // close
        close(sockfd);
    }

    return 0;

}
