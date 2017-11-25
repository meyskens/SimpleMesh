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
    int _nodeID;
    int _networkID;
    RFM69 _radio;
    uint8_t _privatekeys[];
    
    void _handleBroadcast();
    void _doHandShake(int senderID);
    bool _compare(char data[], char compare[], int len);
};

#endif
