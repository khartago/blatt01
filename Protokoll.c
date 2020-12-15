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




/*int get_operation (char* buffer, Protokoll *protocol){

    int operations=0;
    protocol->ack=0;



    int *buf = (int*)buffer;
    int test;
    // check if the operation is delete
    int delete_op= 1;
    test= delete_op & buf[0] ; // check bitweise if the operation gotten from the request is delete
    if (test==1){
        protocol->del = 1;
        if (protocol->ack ==0){
            protocol->ack= 1 <<3; // shift 1 by 3 positions : 0001 => 1000
        }
        operations = operations | delete_op;
        return operations;
    }
    test=0;

    // check if the operation is set
    int set_op= 2;
    test= set_op & buf[0] ; // check bitweise if the operation gotten from the request is set
    if (test==2){
        protocol->set = 2;
        if (protocol->ack ==0){
            protocol->ack= 1 <<3; // shift 1 by 3 positions : 0001 => 1000
        }
        operations = operations | set_op;
        return operations;
    }
    test=0;

    // check if the operation is get
    int get_op= 4;
    test= get_op & buf[0] ; // check bitweise if the operation gotten from the request is get
    if (test==4){
        protocol->get = 4;
        if (protocol->ack ==0){
            protocol->ack= 1 <<3; // shift 1 by 3 positions : 0001 => 1000
        }
        operations = operations | get_op;
        return operations;
    }
    test=0;

    return operations;
}


void get_key (char *buffer, Protokoll *protocol, int len){


    char* buf[len];
    memset(&buf, 0, len);
    memcpy((void*)buf, buffer, len);

    protocol->key = malloc(sizeof (int));
    memcpy (protocol-> key, buffer, len);
}

void get_value ( Protokoll *protocol, int len) {


    char buffer[len];
    int read_bytes=fread(&buffer,1,len,stdin);

    if (read_bytes !=0){
        perror("stdin error");
    }
    protocol-> value=  calloc(len, sizeof (char));
    protocol-> value= buffer;

}

unsigned int value_len (void* buffer){

    int valuelen =0;


    uint32_t *buf= (uint32_t*)buffer;
    long tmp =0;
    memset(&tmp, 0, sizeof(tmp));
    tmp= ntohl((long) buf[0]);

    for (int i=31; i>=0; i--)
    {
        int a= tmp >> i;
        if ( (a & 1) == 1)
        { int num=1;
            for (int k=0; k<a; k--){num *=4;}
            valuelen += num;}
    }
    return valuelen;
}


unsigned int key_len (void* buffer){

    int keylen =0;


    uint16_t *buf= (uint16_t*)buffer;
    short tmp =0;
    memset(&tmp, 0, sizeof(tmp));
    tmp= ntohs((short) buf[0]);

    for (int i=15; i>=0; i--)
    {
        int a= tmp >> i;
        if  (a & 1)
        {
            keylen += binary_to_int(2,i);}
    }
    return keylen;
}

long binary_to_int (int x, int y){
    int i;
    int nb=1;

    for (i=0; i<y; i++){
        nb *=x;
    }
    return nb;
}

void get_ID (char* buffer, Protokoll *protocol){
    int id=0;


    int *buf = (int*) buffer;
    int k=0;
    for ( int i=7; i>=0; i--){
        k=buf[0] >>i;
        if (k&1){ id += binary_to_int (2,i);}
    }
    protocol->ID = id;
}

*/
int unmarshalling (int sockfd, Protocol *protocol)
{
    char buffer[1500];
    memset(buffer, 0, sizeof(buffer));

    int getop_bytes = 0;
    if ((getop_bytes = recv(sockfd, buffer, sizeof(buffer), 0)) < sizeof(ProtocolHeader)){
        perror("recv operation Error ");
        return -1;
    }
    ProtocolHeader *header = (ProtocolHeader*) buffer;

    protocol->header.key_length = htons(header->key_length);
    protocol->header.value_length = htonl(header->value_length);
    protocol->header.flags = header->flags ;

    //memcpy(&protocol->header, header, sizeof(*header));

    if(protocol->header.key_length > 0) {
        protocol->key = malloc(protocol->header.key_length + 1);
        memcpy(protocol->key, buffer + sizeof(*header), protocol->header.key_length);
        protocol->key[protocol->header.key_length] = 0;
    }
    else {
        protocol->key = NULL;
    }
    if(protocol->header.value_length > 0) {
        protocol->value = malloc(protocol->header.value_length +1);
        memcpy (protocol->value, buffer + sizeof(*header) + protocol->header.key_length, protocol->header.value_length);
        protocol->value[protocol->header.value_length] = 0;
    }
    else {
        protocol->value = NULL;
    }
    return 0;
}

int marshalling (Protocol *protocol, char *marshalled){

    ProtocolHeader *p = (ProtocolHeader*) marshalled;

    p->flags = protocol->header.flags;
    p->key_length = htons(protocol->header.key_length);
    p->value_length = htonl(protocol->header.value_length);
    int size = sizeof(ProtocolHeader);
    if(protocol->key!= NULL) {
        size += protocol->header.key_length;
        memcpy(marshalled + sizeof(ProtocolHeader), protocol->key, protocol->header.key_length);
    }
    if(protocol->value!= NULL) {
        size += protocol->header.value_length;
        memcpy(marshalled + sizeof(ProtocolHeader) + protocol->header.key_length, protocol->value,
               protocol->header.value_length);
    }
    return size;
}