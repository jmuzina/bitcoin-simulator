#ifndef JMUZINA_BCOINMSG
#define JMUZINA_BCOINMSG

#include "../Common/Block.hpp"

struct BitcoinMessage {
    Block block;
    std::string peerId;
    int length;

    BitcoinMessage() {
        block = Block();
        peerId = -1;
        length = 0;
    }

    BitcoinMessage(const Block& newBlock, const std::string id, const int len) {
        block = newBlock;
        peerId = id;
        length = len;
    }

    // Copy constructor
    BitcoinMessage(const BitcoinMessage& copy) { 
        block = copy.block; 
        peerId = copy.peerId;
        length = copy.length;
    }

    // Equal operator overload
    BitcoinMessage& operator=(const BitcoinMessage& rhs) {
        block = rhs.block;
        peerId = rhs.peerId;
        length = rhs.length;
        return *this;
    }
};

#endif 