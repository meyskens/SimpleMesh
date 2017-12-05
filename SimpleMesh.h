/*
  SimpleMesh.h - Library for mesh networking on the RFM69.
  Created by Ben De Breuker, Maartje Eyskens and Kristie Lim
  Released under GPLv3
*/
#ifndef SimpleMesh_h
#define SimpleMesh_h
// Deps for the radio
#include <RFM69.h>

#include "Arduino.h"

#define MESH_MAX_DATA_LEN 61 // Max data length to be handled by the RFM69 library
//define the max size of the hashtable
const byte HASH_SIZE = 10;

typedef struct {
  int    senderID;
  char   data[MESH_MAX_DATA_LEN]; 
} MessagePayload;

class SimpleMesh
{
  public:
    SimpleMesh(int nodeID, int networkID);
    bool receiveDone();
    MessagePayload getMessage();
    bool send(int to, char data[]);
  private:
    uint8_t _nodeID;
    uint8_t _networkID;
    RFM69 _radio;
    uint8_t _privatekeys[];
    uint8_t _seqNumbers[99];
    uint8_t _sequence;
    
    void _sayHello();
    void _relay();
    void _handleBroadcast();
    void _doHandShake(int senderID);
    bool _compare(char data[], char compare[], int len);
    void _filter(char data[], int start, int len, char* out);
    void _finishHandshake(int senderID);
};

#endif
