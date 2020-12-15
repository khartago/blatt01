//
// Created by tkn on 12/7/20.
//

#ifndef BLOCK3_PROTOKOLL_H
#define BLOCK3_PROTOKOLL_H
#include "uthash.h"

typedef struct _hashtable{
    unsigned int key_len;
    unsigned int value_len;
    char *key;
    char *value;
    UT_hash_handle hh;
}hashtable;

typedef enum {
    FlagAck = 0x8,
    FlagGet = 0x4,
    FlagSet = 0x2,
    FlagDelete = 0x1,
} Flags;

#pragma pack(push, 1)
typedef struct {
    uint8_t flags;
    uint16_t key_length;
    uint32_t value_length;
} ProtocolHeader;
#pragma pack(pop)

typedef struct {
    ProtocolHeader header;
    char *key;
    char *value;
} Protocol;


/*void initialisation (Protokoll *protocol);

int get_operation (char *buffer, Protokoll *protocol);
unsigned int value_len (void* buffer);

unsigned int key_len (void* buffer);

void get_ID (char *buffer, Protokoll *protocol);
*/
int unmarshalling (int sockfd, Protocol *protocol);

int marshalling (Protocol *protocol, char *buffer);

long binary_to_int (int x, int y);

#endif //BLOCK3_PROTOKOLL_H