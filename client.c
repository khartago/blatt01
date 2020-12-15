//
// Created by tkn on 12/11/20.
//
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Protokoll.h"

static char *
read_stdin (void)
{
  size_t cap = 4096, /* Initial capacity for the char buffer */
         len =    0; /* Current offset of the buffer */
  char *buffer = malloc(cap * sizeof (char));
  int c;

  /* Read char by char, breaking if we reach EOF or a newline */
  while ((c = fgetc(stdin)) != '\n' && !feof(stdin))
    {
      buffer[len] = c;

      /* When cap == len, we need to resize the buffer
       * so that we don't overwrite any bytes
       */
      if (++len == cap)
        /* Make the output buffer twice its current size */
        buffer = realloc(buffer, (cap *= 2) * sizeof (char));
    }

  /* Trim off any unused bytes from the buffer */
  buffer = realloc(buffer, (len + 1) * sizeof (char));

  /* Pad the last byte so we don't overread the buffer in the future */
  buffer[len] = '\0';

  return buffer;
}
int main(int argc, char** argv) {
    if (argc < 5) {
        fprintf(stderr, "%s\n", "No enough args provided!");
        return 1;
    }

    char *host = argv[1];
    char *service = argv[2];
    char *Header = argv[3];


    struct addrinfo hints;
    struct addrinfo *res;
    struct addrinfo *p;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;


    int status = getaddrinfo(host, service, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "%s\n", "getaddrinfo() failed!");
        return 1;
    }

    int s = -1;
    for (p = res; p != NULL; p = p->ai_next) {
        s = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (s == -1) {
            continue;
        }


        status = connect(s, p->ai_addr, p->ai_addrlen);
        if (status != 0) {
            fprintf(stderr, "%s\n", "connect() failed!");
            continue;
        }
    }

    if (s == -1) {
        fprintf(stderr, "%s\n", "socket() failed!");
        return 1;
    }

    if (status != 0) return 1;


    fprintf(stderr, "Connected to %s:%s\n", host, service);
    //break;



// Send the header
    Protocol *protocol = malloc(sizeof(Protocol));
    protocol->header.key_length = strlen(argv[4]);
    protocol->key = argv[4];

    int read;
    char* buff=NULL;

    if (strcmp(Header, "DELETE") == 0) {
        protocol->header.flags = FlagDelete;
    } else if (strcmp(Header, "SET") == 0) {
        protocol->header.flags = FlagSet;
        unsigned int chars_read;
        //fgets(buff,sizeof(buff),stdin);
        buff=read_stdin();
        protocol->header.value_length = strlen(buff);
        buff[protocol->header.value_length] = 0;
        protocol->header.value_length = protocol->header.value_length;
        protocol->value=buff;
    } else if (strcmp(Header, "GET") == 0) {
        protocol->header.flags = FlagGet;
    } else {
        perror("unvalid Operation");
    }
    char marshalled[1500];
    int size = marshalling(protocol, marshalled);
    int nb_bytes_sent = send(s, marshalled, size, 0);
    if (nb_bytes_sent < 0) {
        perror("send error");
        exit(0);
    }

    Protocol *protocol2 = malloc(sizeof(Protocol)); // Memory allocation for HTTP request

    unmarshalling(s, protocol2); // Unzip the information in the client's Socket

    if((protocol2->header.flags & FlagAck)!=0)
        printf("ACK: 1\n");
    else
        printf("ACK: 0\n");
    if((protocol2->header.flags & FlagGet)!=0)
    printf("GET: 1\n");
    else
    printf("GET: 0\n");
    if((protocol2->header.flags & FlagSet)!=0)
    printf("GET: 1\n");
    else
    printf("GET: 0\n");
    if((protocol2->header.flags & FlagDelete)!=0)
    printf("DEL: 1\n");
    else
    printf("DEL: 0\n");
    printf("Key Length: %d\n",protocol2->header.key_length);
    printf("Value Length: %d\n",protocol2->header.value_length);

    if (protocol->header.flags & FlagGet) {
        fwrite(protocol2->value, sizeof(char), protocol2->header.value_length, stdout);
        printf("\n");
    }

    if(buff!=NULL)
        free(buff);
    return 0;
}