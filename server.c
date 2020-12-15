//
// Created by tkn on 12/7/20.
//
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "Protokoll.h"
#include "uthash.h"



int main(int argc, char** argv) {

    struct addrinfo hints;
    struct addrinfo* res;
    struct addrinfo* p;
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    int status=0;
    int sockfd=0;

    if (argc < 2) {
        printf("%s\n", "No enough args provided!");
        return 1;
    }

    char* port = argv[1];
    hashtable *table = NULL;
    //table = malloc(sizeof(table));



    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;


    status = getaddrinfo(NULL, port, &hints, &res);
    if (status != 0){
        fprintf(stderr, "%s\n", "getaddrinfo() failed!");
        return 1;
    }

    int s = -1;
    for(p = res; p != NULL;p = p->ai_next){
        s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (s == -1){
            continue;
        }

        int optval = 1;
        setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));


        status = bind(s, res->ai_addr, res->ai_addrlen);
        if (status != 0){
            close(s);
            continue;
        }

        break;
    }

    if (s == -1){
        fprintf(stderr, "%s\n", "Unable to create socket!");
        return 1;
    }

    if (status != 0){
        fprintf(stderr, "%s\n", "Failed to bind socket!");
        return 1;
    }

    status = listen(s, 1);
    if (status != 0){
        fprintf(stderr, "Listen failed!\n");
        return 1;
    }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while(1) {
        struct sockaddr_storage client_addr;
        socklen_t c_addr_size = sizeof(client_addr);
        int client = accept(s, (struct sockaddr *) &client_addr, &c_addr_size);
        if (client == 1) {
            fprintf(stderr, "connect failed!\n");
            return 1;
        } else {
            fprintf(stderr, "%s\n", "Client accepted!");
        }


        Protocol *protocol = malloc(sizeof(Protocol)); // Memory allocation for HTTP request

        unmarshalling(client, protocol); // Unzip the information in the client's Socket


        // check which operation will be made
        if (protocol->header.flags & FlagSet) {
            hashtable *s;
            protocol->header.flags= FlagSet | FlagAck;
            HASH_FIND(hh,table,protocol->key, protocol->header.key_length, s);
            if (s == NULL){
                s = (struct hashtable *)malloc(sizeof *s);
                s->key = protocol->key;
                s->key_len = protocol->header.key_length;
                s->value_len = protocol->header.value_length;
                s->value = protocol->value;
                HASH_ADD_KEYPTR(hh, table, s->key, s->key_len, s);
                protocol->key=NULL;
                protocol->header.key_length=0;
                protocol->value=NULL;
                protocol->header.value_length=0;
            }
        }
        else if (protocol->header.flags & FlagGet) {
            hashtable *respond;
            HASH_FIND(hh, table, protocol->key, protocol->header.key_length, respond);
            if (respond != NULL) {
                protocol->header.flags= FlagGet | FlagAck;
                protocol->value=calloc(respond->value_len,sizeof(char));
                memcpy(protocol->value, respond->value, respond->value_len);
                protocol->header.value_length = respond->value_len;
                if (respond->value == NULL) {
                    perror("no value found ");
                }
            }else
                protocol->header.flags= FlagGet;
            free(protocol->key);
            protocol->key=NULL;
        }

        else if (protocol->header.flags & FlagDelete) {
            hashtable *s;
            HASH_FIND(hh,table,protocol->key, protocol->header.key_length, s);
            if(s!=NULL) {
                HASH_DELETE (hh, table, s);
                protocol->header.flags= FlagDelete | FlagAck;
            }else
                protocol->header.flags= FlagDelete;
            free(protocol->key);
            protocol->key = NULL;
            protocol->header.key_length = 0;
            protocol->value = NULL;
            protocol->header.value_length = 0;
        }


        char marshalled[1500];
        int size = marshalling(protocol, marshalled);
        //unsigned int respond_length = protocol->header.value_length + protocol->header.key_length;

        int nb_bytes_sent = send(client, marshalled, size, 0);

        if (nb_bytes_sent <0) {
            perror("send error");
            exit(0);
        }



        //}



        //close(sockfd);
        close(client);
        if(protocol->key!=NULL)
            free(protocol->key);
        if(protocol->value!=NULL)
            free(protocol->value);
        free(protocol);
    }
    freeaddrinfo(res);
    return 0;
}

