#include "BitcoinMiner.hpp"
#include <iostream>
#include <stdlib.h>     
#include <time.h>       

BitcoinMiner::BitcoinMiner(const std::string id) {
    curChain = new Blockchain(true);
    _id = id;
}

void BitcoinMiner::makeRequest() {

}

void BitcoinMiner::preformComputation() {
    readBlock();
}

void BitcoinMiner::mineNext() {
    const int toGuess = rand() % 150 + 1;
    const int curLength = curChain->getChainSize();
    std::cerr << "---------------------------\nBlock " << curLength << " has been mined by " << _id << "\n";

    const std::string prevHash = curChain->getBlockAt(curLength - 1).getHash();
    const std::string newHash = std::to_string(toGuess * curLength) + _id;
    std::cerr << prevHash << " -> " << newHash << "\n";
    curChain->createBlock(curLength, prevHash, newHash, {getId()});
    transmitBlock();
}

void BitcoinMiner::readBlock() {
    receive();
    int longestChainLength = curChain->getChainSize();
    const int BLOCKCHAIN_SIZE = curChain->getChainSize();
    std::deque<Packet<BitcoinMessage>>::iterator longestChain;
    std::string correctPeerId;
    std::string topHash;

    int longestChainAt = -1;

    for (int i = 0; i < _inStream.size(); ++i) {
        const BitcoinMessage RECEIVED_MSG = _inStream[i].getMessage();
        const int RECEIVED_LENGTH = RECEIVED_MSG.length;
        if (RECEIVED_LENGTH > BLOCKCHAIN_SIZE && RECEIVED_LENGTH > longestChainLength) {
            longestChainAt = i;
            longestChainLength = RECEIVED_LENGTH;
        }
    }

    if (longestChainAt != -1) {
        for (auto it = _neighbors.begin(); it != _neighbors.end(); ++it) {
            BitcoinMiner* neighbor = static_cast<BitcoinMiner*>(it->second);
            Blockchain* neighborChain = neighbor->getCurChain();
            if (_inStream[longestChainAt].getMessage().block.getHash() == neighborChain->getBlockAt(neighborChain->getChainSize() - 1).getHash()){
                setCurChain(*neighborChain);
                break;
            }
        }
    }
    _inStream.clear();
}

void BitcoinMiner::transmitBlock() {
    Blockchain* curChain = getCurChain();
    int curChainSize = curChain->getChainSize();
    Block nextBlock(curChain->getBlockAt(curChainSize - 1));
    BitcoinMessage toSend(nextBlock, _id, curChainSize + 1);

    for (auto it = _neighbors.begin(); it != _neighbors.end(); ++it) {
        std::string neighborId = it->first;
        BitcoinMiner* neighborOb = static_cast<BitcoinMiner*>(it->second);
        Packet<BitcoinMessage> msgPacket("", neighborId, _id);
        msgPacket.setBody(toSend);
        neighborOb->send(msgPacket);
        _outStream.push_back(msgPacket);
    }
}