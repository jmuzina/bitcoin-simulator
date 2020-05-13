#include "bitcoinPeer.hpp"
#include <iostream>

BitcoinPeer::BitcoinPeer(const std::string id) {
    curChain = new Blockchain(true);
    peerId = id;
}

void BitcoinPeer::makeRequest() {

}
void BitcoinPeer::preformComputation() {

}

void BitcoinPeer::readBlock() {
    int largestSize = 0;
    // Find largest message length the peer knows of
    for (int i = 0; i < _inStream.size; ++i) {
        int msgSize = _inStream[i].getMessage().length;
        if (msgSize > largestSize) largestSize = msgSize;
    }

    BitcoinMessage largestMessage = _inStream[largestSize].getMessage();

    // Find largest message amongst other peers
    // iterator points to maps of peer ids and peer objects
    for (auto it = _neighbors.begin(); it != _neighbors.end(); ++it) {
        std::string neighborId = it->first;
        BitcoinPeer* neighborOb = static_cast<BitcoinPeer*>(it->second);
        // Finds largest known blockchain
        // If the largest known chain came from a valid peer, it is the largest chain.
        if (largestMessage.peerId == neighborOb->id()) {
            setCurChain(*(neighborOb->getCurChain()));
        }
    }
}

void BitcoinPeer::transmitBlock() {

}