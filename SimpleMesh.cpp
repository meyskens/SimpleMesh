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
    _nodeID = nodeID;
    _networkID = networkID;
    _radio.initialize(FREQUENCY,nodeID,networkID);
#ifdef IS_RFM69HW_HCW
    radio.setHighPower();
#endif
    _radio.promiscuous(true);
}

bool SimpleMesh::receiveDone() {
    if (_radio.receiveDone()) {
        if (_radio.TARGETID == _nodeID) {
            if (_radio.ACKRequested()) {
                _radio.sendACK();
            }
            return true;
        } else {
            if (_radio.TARGETID == 0) { // Broadcast!
                _handleBroadcast();
                return false;
            }
            if (SimpleMesh::_compare(_radio.DATA, "KEY", 3)) { // is key exchange 
                // TO DO
                return false;
            }

            // relay everything!
            // TO DO
        }
    }
    return false;
}

bool SimpleMesh::send(int to,char data[]) {
    return _radio.sendWithRetry(to, data, sizeof(data));
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
    sprintf(out, "%s%", "KEY", publicKey);

    SimpleMesh::send(senderID, out);
}

bool SimpleMesh::_compare(char data[], char compare[], int len) {
    for (int i = 0; i < len; i++){
        if (data[i] != compare[i]) {
            return false;
        }
    }
    return true;
}