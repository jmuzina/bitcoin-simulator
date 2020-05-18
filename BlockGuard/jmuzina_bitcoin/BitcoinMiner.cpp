#include "BitcoinMiner.hpp"
#include <iostream>     

// Returns parsed blockhash and solution nonce
splitHash::splitHash(std::string fullHash) {
    if (fullHash.substr(0, 11) == "genesisHash") {
        hash = "genesisHash";
        nonce = -1;
    }
    else if (fullHash.length() == 0) {
        hash = "";
        nonce = -1;
    }
    else {
        const int fullLen = fullHash.length();
        const int noncePos = fullHash.find_first_of(":") + 1;
        const int nonceLen = fullLen - noncePos;
        const int hashLen = fullLen - nonceLen - 1;

        hash = fullHash.substr(0, hashLen);
        nonce = std::stol(fullHash.substr(noncePos, nonceLen));
    }
}

BitcoinMiner::BitcoinMiner(const std::string id) {
    curChain = new Blockchain(true);
    _id = id;
    lastNonce = 0;
}

// Deterministically returns a randomly outputed string,
// using the previous block's hash, current miner ID,
// and an incremented nonce as input.
std::string BitcoinMiner::getSHA(long nonce) const {
    const int curLength = curChain->getChainSize();
    const std::string prevHash = (curLength > 1 ? splitHash(curChain->getBlockAt(curLength - 1).getHash()).getHash() : "");

    std::string hash_hex_str;
    picosha2::hash256_hex_string(prevHash + std::to_string(nonce), hash_hex_str);
    return hash_hex_str;
}

// Handles main mining logic
void BitcoinMiner::preformComputation() {
    // Check node messages to make sure our chain isn't out of date
    readBlock(false);
    // continues executing until we've mined an arbitrary number of blocks
    if (curChain->getChainSize() != 100) {
        std::string hash = (curChain->getChainSize() > 1 ? getSHA(lastNonce) : "genesisHash"); // Miner's attempted Proof of Work solution
        std::string challengeBits = hash.substr(0, 2); // First four bits of attempted solution
        const bool outdated = readBlock(true); // Checks that nobody has beaten the miner since we last checked
        // Valid PoW solution and no other miner has solved the problem - send block to other miners
        if ((challengeBits == "00" || hash == "genesisHash") && !outdated) { 
            mineNext(hash);
            setLastNonce(0);
        }
        // Node has been beaten. Reset nonce.
        else if (outdated) {
            std::cerr << " Fork averted! Node has been beaten!\n";
            setLastNonce(0);
        }
        // PoW solution was incorrect, increment nonce
        else {
            setLastNonce(lastNonce + 1);
        }
    }
}

// Add solved block blockchain
void BitcoinMiner::mineNext(const std::string newHash) {
    const int curLength = curChain->getChainSize();
    std::cerr << "---------------------------\nBlock " << curLength << " has been mined by " << _id << "\n";

    const std::string prevHash = (curLength > 1 ? splitHash(curChain->getBlockAt(curLength - 1).getHash()).getHash() : "");
    std::cerr << prevHash << " -> " << newHash << "\n";
    curChain->createBlock(curLength, prevHash, newHash + ":" + std::to_string(lastNonce), {_id}); // add to local blockchain
    transmitBlock(); // send to other miners
}

// Checks for solutions from other miners.
// if outdatedCheck flag is true, will stop once 
// Outdated status has been determined.
bool BitcoinMiner::readBlock(const bool outdatedCheck) {
    receive(); // refresh instream
    int longestChainLength = curChain->getChainSize();
    const int BLOCKCHAIN_SIZE = curChain->getChainSize();

    // Stores position of message with longest corresponding blockchain
    int longestChainAt = -1;

    // Find longest blockchain
    for (int i = 0; i < _inStream.size(); ++i) {
        const BitcoinMessage RECEIVED_MSG = _inStream[i].getMessage();
        const int RECEIVED_LENGTH = RECEIVED_MSG.length; // blockchain length
        if (RECEIVED_LENGTH > BLOCKCHAIN_SIZE && RECEIVED_LENGTH > longestChainLength) {
            longestChainAt = i;
            longestChainLength = RECEIVED_LENGTH;
        }
    }

    if (outdatedCheck) return (longestChainLength > BLOCKCHAIN_SIZE);

    // If the longest chain received is longer than the peer's current chain,
    // peer will verify each block that it is missing and add them to its chain.
    if (longestChainAt != -1) {
        const Block topBlock = _inStream[longestChainAt].getMessage().block;
        const BitcoinMiner* topNeighbor;
        std::string lastHash = splitHash(curChain->getBlockAt(curChain->getChainSize() - 1).getHash()).getHash();
        for (std::map<std::string, Peer<BitcoinMessage> *>::iterator it = _neighbors.begin(); it != _neighbors.end(); ++it) {
            BitcoinMiner* neighbor = static_cast<BitcoinMiner*>(it->second);
            std::string peerId = *(topBlock.getPublishers()).begin();

            if (neighbor->id() == peerId) {
                topNeighbor = neighbor;
                break;
            }

        }

        Blockchain* neighborChain = topNeighbor->getCurChain();
        
        int blocksBehind = neighborChain->getChainSize() - curChain->getChainSize();

        // Catch up and verify each block along the way
        while (blocksBehind > 0) {
            int curPos = neighborChain->getChainSize() - blocksBehind;
            int prevPos = curPos - 1;

            std::string neighborId = *(neighborChain->getBlockAt(curPos).getPublishers().begin());

            const splitHash curSplit = (curPos > 0 ? splitHash(neighborChain->getBlockAt(curPos).getHash()) : splitHash(-1, "genesisHash"));
            const splitHash prevSplit = (prevPos > 0 ? splitHash(neighborChain->getBlockAt(prevPos).getHash()) : splitHash());
            
            std::string curHashCheck;

            if (curPos == 0 || prevSplit.getHash() == "") curHashCheck = "genesisHash";
            else picosha2::hash256_hex_string(prevSplit.getHash() + std::to_string(curSplit.getNonce()), curHashCheck);

            if ((curSplit.getHash() != curHashCheck) || (prevSplit.getHash() != splitHash(neighborChain->getBlockAt(curPos).getPreviousHash()).getHash())) {
                std::cerr << _id << " is forked!\n";
                // This miner is on a fork
                int forkPos = curChain->getChainSize() - 1;
                while (forkPos != 0) {
                    Block longer = neighborChain->getBlockAt(forkPos);
                    Block local = curChain->getBlockAt(forkPos);
                    // Search from the end of the blockchains for the shallowest matching block
                    if ((splitHash(longer.getHash()).getHash() != splitHash(local.getHash()).getHash()) || (splitHash(longer.getPreviousHash()).getHash() != splitHash(local.getPreviousHash()).getHash()))
                        --forkPos;
                    else break;
                }

                // Copy longest blockchain up to the position where the two chains agreed
                Blockchain* validated = new Blockchain(true);
        
                for (int i = 1; i <= forkPos; ++i) {
                    Block copyBlock = neighborChain->getBlockAt(i);
                    validated->createBlock(copyBlock.getIndex(), splitHash(copyBlock.getPreviousHash()).getHash(), splitHash(copyBlock.getHash()).getHash(), {*copyBlock.getPublishers().begin()});
                }
                
                // Set local chain to validated chain
                setCurChain(*validated);

                // Independently verify all the missed blocks
                for (int verifyPos = forkPos + 1; curChain->getChainSize() != neighborChain->getChainSize(); ++verifyPos) {
                    int prevVerifyPos = verifyPos - 1;
                    std::string verifyId = *(neighborChain->getBlockAt(verifyPos).getPublishers().begin());
                    const splitHash verifySplit = (verifyPos > 0 ? splitHash(neighborChain->getBlockAt(verifyPos).getHash()) : splitHash(-1, "genesisHash"));
                    const splitHash prevVerifySplit = (prevVerifyPos > 0 ? splitHash(neighborChain->getBlockAt(prevVerifyPos).getHash()) : splitHash());
                    
                    std::string verifyHash;
                    
                    if (verifyPos == 0 || prevVerifySplit.getHash() == "") curHashCheck = "genesisHash";
                    else picosha2::hash256_hex_string(prevVerifySplit.getHash() + std::to_string(verifySplit.getNonce()), verifyHash);

                    if ((verifySplit.getHash() == verifyHash) && (prevVerifySplit.getHash() == splitHash(neighborChain->getBlockAt(verifyPos).getPreviousHash()).getHash())) {
                        const int newIndex = curChain->getBlockAt(curChain->getChainSize() - 1).getIndex() + 1;
                        std::cerr << "top block is " << curChain->getChainSize() - 1 << ", adding new block at " << curChain->getChainSize() << "\n";
                        curChain->createBlock(curChain->getChainSize(), prevVerifySplit.getHash(), verifySplit.getHash() + ":" + std::to_string(curSplit.getNonce()), {verifyId});
                    }
                }
                std::cerr << _id << "'s fork merged and corrected with " << neighborId << "!\n";
                break;
            }
            else {
                curChain->createBlock(curChain->getChainSize(), prevSplit.getHash(), curSplit.getHash() + ":" + std::to_string(curSplit.getNonce()), {neighborId});
                --blocksBehind;
            }
        }
    }
    _inStream.clear();
    return false;
}

// Send block to other miners
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

void BitcoinMiner::makeRequest() {


}
