#include "Arduino.h"
#include "SimpleMesh.h"
// Deps for the radio
#include <RFM69.h>
#include <SPI.h>
#include <SPIFlash.h>
#include <uECC.h>
#include <random.h>

#define FREQUENCY   RF69_433MHZ

SimpleMesh::SimpleMesh(int nodeID, int networkID) {
    Serial.println("Initializing SimpleMesh");
    _nodeID = nodeID;
    _networkID = networkID;
    _radio.initialize(FREQUENCY,nodeID,networkID);
#ifdef IS_RFM69HW_HCW
    radio.setHighPower();
#endif
    _radio.promiscuous(true);
    Serial.println("Finished initializing");
    _sayHello();
}

bool SimpleMesh::receiveDone() {
    Serial.println("check for messages");
    if (_radio.receiveDone()) {
        if (_radio.TARGETID == _nodeID) {
             Serial.println("Received message for this node");
            if (_radio.ACKRequested()) {
                _radio.sendACK();
            }
        } 
        if (_radio.TARGETID == 0) { // Broadcast!
             _handleBroadcast();
            return false;
        }
        if (SimpleMesh::_compare(_radio.DATA, "KEY", 3) && _radio.TARGETID == _nodeID) { // is key exchange 
            _finishHandshake(_radio.SENDERID);
            return false;
        }
        // special cases handles
        _seqNumbers[_radio.DATA[0]] = _radio.DATA[2];
        if (_radio.DATA[1] == _nodeID) {
            return true;
        }
        _relay();
    }
    return false;
}

bool SimpleMesh::send(int to,char data[]) {
     Serial.println("Sending message");
    _sequence++;
    if (_sequence >= 255) {
        _sequence = 0;
    }
    // to do add encryption
    char out[sizeof(data) + 3];
    sprintf(out, "%c%c%s", _nodeID, _sequence, data);
    
    Serial.print("Sending ");
    Serial.println(out);

    return _radio.sendWithRetry(to, out, sizeof(out));
}

MessagePayload SimpleMesh::getMessage() {
    MessagePayload message;
    if (_radio.TARGETID != _nodeID) {
        // no message for you, empty data back
        return message;
    }
    message.senderID = _radio.SENDERID;
    strcpy(message.data, _radio.DATA);

    return message;
}

void SimpleMesh::_handleBroadcast() {
    int senderID = _radio.SENDERID;
    if (_radio.DATA == "HELLO") {
        _doHandShake(senderID);
    }
}

void SimpleMesh::_doHandShake(int senderID) {
    uECC_set_rng(&RNG);
    
    const struct uECC_Curve_t * curve = uECC_secp160r1();
    uint8_t privateKey[21];
    uint8_t publicKey[40];
    uECC_make_key(publicKey, privateKey, curve);
    _privatekeys[senderID] = privateKey;

    char out[43];
    sprintf(out, "%s%s", "KEY", publicKey);

    SimpleMesh::send(senderID, out);
}

void SimpleMesh::_finishHandshake(int senderID) {
    uECC_set_rng(&RNG);
    const struct uECC_Curve_t * curve = uECC_secp160r1();
    if (_privatekeys[senderID] == null) {
        uint8_t privateKey[21];
        uint8_t publicKey[40];
        uECC_make_key(publicKey, privateKey, curve);
        _privatekeys[senderID] = privateKey;
        char out[43];
        sprintf(out, "%s%s", "KEY", publicKey);
        SimpleMesh::send(senderID, out);
    }
    char key[40];
    SimpleMesh::_filter(_radio.DATA, 3, 43, key);

}

bool SimpleMesh::_compare(char data[], char compare[], int len) {
    for (int i = 0; i < len; i++){
        if (data[i] != compare[i]) {
            return false;
        }
    }
    return true;
}


void SimpleMesh::_filter(char data[], int start, int len, char* out) {
    start--;
    for (int i = start; i < (len + start); i++){
        out[(i-start)] = data[i];
    }
}

void SimpleMesh::_sayHello() {
    Serial.println("Sending hello");
    _radio.send(0, "HELLO", 5);
    Serial.println("Waiting for new friends");
}

void SimpleMesh::_relay() {
    // TO DO ADD ENCRYPTION
    Serial.println("Relaying");
    for (int i = 0; i < sizeof(_privatekeys); i++) {
        if (_privatekeys != null) {
            Serial.print("Relaying to ");
            Serial.println(i);
            _radio.sendWithRetry(i, _radio.DATA, sizeof(_radio.DATA));
        }
    }
}