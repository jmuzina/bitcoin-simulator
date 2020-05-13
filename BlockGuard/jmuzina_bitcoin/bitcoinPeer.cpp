#include "bitcoinPeer.hpp"
#include <iostream>
#include <stdlib.h>     
#include <time.h>       

BitcoinPeer::BitcoinPeer(const std::string id) {
    curChain = new Blockchain(true);
    peerId = id;
}

void BitcoinPeer::makeRequest() {

}

void BitcoinPeer::preformComputation() {
    readBlock();
    mineNext();
    transmitBlock();
}

void BitcoinPeer::mineNext() {
    std::cerr << "attempting to mine " << curChain->getChainSize() << "\n";
    srand(time(nullptr));
    const int toGuess = rand() % 15 + 1;
    int guess = 0;
    while (guess != toGuess) {
        guess = rand() % 15 + 1;
    }
    const int curLength = curChain->getChainSize();
    const std::string newHash = std::to_string(toGuess * curLength) + peerId;
    curChain->createBlock(curLength, curChain->getBlockAt(curLength - 1).getHash(), newHash, {getId()});
}

void BitcoinPeer::readBlock() {
    //for (auto it = neighbors().begin(); it != neighbors().end(); ++it) std::cerr << *it << "\n";
    int largestSize = 0;
    // Find largest message length the peer knows of
    for (auto it = _inStream.begin(); it != _inStream.end(); ++it) {
        int msgSize = it->getMessage().length;
        if (msgSize > largestSize) largestSize = msgSize;
    }

    BitcoinMessage largestMessage = _inStream[largestSize].getMessage();

    // Find largest message amongst other peers
    // iterator points to maps of peer ids and peer objects
    for (auto it = _neighbors.begin(); it != _neighbors.end(); ++it) {
        std::string neighborId = it->first;
        std::cerr << "checking " << neighborId << "\n";
        BitcoinPeer* neighborOb = static_cast<BitcoinPeer*>(it->second);
        // Finds largest known blockchain
        // If the largest known chain came from a valid peer, it is the largest chain.
        if (largestMessage.peerId == neighborOb->id()) {
            setCurChain(*(neighborOb->getCurChain()));
        }
    }
}

void BitcoinPeer::transmitBlock() {
    Blockchain* curChain = getCurChain();
    int curChainSize = curChain->getChainSize();
    Block nextBlock(curChain->getBlockAt(curChainSize - 1));
    BitcoinMessage toSend(nextBlock, peerId, curChainSize + 1);

    for (auto it = _neighbors.begin(); it != _neighbors.end(); ++it) {
        std::string neighborId = it->first;
        BitcoinPeer* neighborOb = static_cast<BitcoinPeer*>(it->second);
        Packet<BitcoinMessage> msgPacket("", neighborId, _id);
        msgPacket.setBody(toSend);
        neighborOb->send(msgPacket);
        _outStream.push_back(msgPacket);
    }
}