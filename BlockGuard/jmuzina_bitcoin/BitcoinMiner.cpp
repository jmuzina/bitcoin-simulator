#include "BitcoinMiner.hpp"
#include <iostream>     

// Returns parsed blockhash and solution nonce
splitHash::splitHash(std::string fullHash) {
    if (fullHash.substr(0, 11) == "genesisHash") {
        hash = "genesisHash";
        nonce = -1;
    }
    else if (fullHash.length() == 0) {
        hash = "-1_-1";
        nonce = -1;
    }
    else {
        const int fullLen = fullHash.length();
        const int noncePos = fullHash.find_first_of(":") + 1;
        const int nonceLen = fullLen - noncePos;
        const int hashLen = fullLen - nonceLen - 1;
        if (noncePos < 64) {
            hash = fullHash;
            nonce = -1;
        }
        else {
            hash = fullHash.substr(0, hashLen);
            std::string nonceStr = fullHash.substr(noncePos, nonceLen);
            nonce = std::stoll(nonceStr);
        }
    }
}

BitcoinMiner::BitcoinMiner(const std::string id) {
    curChain = new Blockchain(true);
    _id = id;
    lastNonce = 0;
    experimentOver = false;
}

// Deterministically returns a randomly outputed string,
// using the previous block's hash, current miner ID,
// and an incremented nonce as input.
std::string BitcoinMiner::getSHA(long long nonce) const {
    const int curLength = curChain->getChainSize();
    const std::string prevHash = (curLength > 1 ? splitHash(curChain->getBlockAt(curLength - 1).getHash()).getHash() : "");
    std::string hash_hex_str;
    picosha2::hash256_hex_string(prevHash + _id + std::to_string(nonce), hash_hex_str);

    //if (hash_hex_str.substr(0,2) == "00") std::cerr << "HO(" << prevHash << ", " << _id << ", " << std::to_string(nonce) << ") =\t" << hash_hex_str << "\n";

    return hash_hex_str;
}

// Handles main mining logic
void BitcoinMiner::preformComputation() {
    // Check node messages to make sure our chain isn't out of date
    readBlock();
    // continues executing until we've mined an arbitrary number of blocks
    if (curChain->getChainSize() != 100) {
        std::string hash = (curChain->getChainSize() > 1 ? getSHA(lastNonce) : "genesisHash"); // Miner's attempted Proof of Work solution
        std::string challengeBits = hash.substr(0, 2); // Arbitrary first few bits of attempted solution
        // Valid PoW solution - send block to other miners
        if ((challengeBits == "00" || hash == "genesisHash")) { 
            mineNext(hash);
            setLastNonce(0);
        }
        // PoW solution was incorrect, increment nonce
        else {
            setLastNonce(lastNonce + 1);
        }
        if (curChain->getChainSize() == 100) setExperimentOver(true);
    }
    else setExperimentOver(true);
}

// Add solved block blockchain
void BitcoinMiner::mineNext(const std::string newHash) {
    if (getBeaten()) setBeaten(false);
    else {
        const int curLength = curChain->getChainSize();
        //std::cout << "---------------------------\nBlock " << curLength << " has been mined by " << _id << "\n";

        const std::string prevHash = (curLength > 1 ? splitHash(curChain->getBlockAt(curLength - 1).getHash()).getHash() : "-1_-1");
        //std::cout << prevHash << " -> " << newHash << "\n";
        curChain->createBlock(curLength, prevHash, newHash + ":" + std::to_string(lastNonce), {_id}); // add to local blockchain

        //std::cerr << "nonce in mineNext is \t" << std::to_string(lastNonce) << "[" << newHash << "]\n";

        transmitBlock(); // send to other miners
    }
}

// Checks for solutions from other miners.
bool BitcoinMiner::readBlock() {
    receive(); // refresh instream
    int longestChainLength = curChain->getChainSize();
    const int BLOCKCHAIN_SIZE = curChain->getChainSize();

    // Stores position of message with longest corresponding blockchain
    int longestChainAt = -1;


    //std::cerr << "instream:\t" << _inStream.size() << "\n";

    // Find longest blockchain
    for (int i = 0; i < _inStream.size(); ++i) {
        const BitcoinMessage RECEIVED_MSG = _inStream[i].getMessage();
        const int RECEIVED_LENGTH = RECEIVED_MSG.length; // blockchain length
        if (RECEIVED_LENGTH > BLOCKCHAIN_SIZE && RECEIVED_LENGTH > longestChainLength) {
            longestChainAt = i;
            longestChainLength = RECEIVED_LENGTH;
        }
    }

    // If the longest chain received is longer than the peer's current chain,
    // peer will verify each block that it is missing and add them to its chain.
    if (longestChainAt != -1) {
        setLastNonce(0); // reset nonce
        setBeaten(true);
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

        Blockchain* longestChain = topNeighbor->getCurChain();
        
        int blocksBehind = longestChain->getChainSize() - curChain->getChainSize();

        const int longestChainSize = longestChain->getChainSize();

        // Catch up and verify each block along the way

        while (blocksBehind > 0) {
            int curPos = longestChainSize - blocksBehind;
            int prevPos = curPos - 1;

            std::string minerId = *(longestChain->getBlockAt(curPos).getPublishers().begin());

            const splitHash curSplit = (curPos > 0 ? splitHash(longestChain->getBlockAt(curPos).getHash()) : splitHash(-1, "genesisHash"));
            const splitHash prevSplit = (prevPos > 0 ? splitHash(longestChain->getBlockAt(prevPos).getHash()) : splitHash());
            
            std::string curHashCheck;

            if (curPos == 0 || prevSplit.getHash() == "-1_-1") curHashCheck = "genesisHash";
            else picosha2::hash256_hex_string(prevSplit.getHash() + minerId + std::to_string(curSplit.getNonce()), curHashCheck);

            if ((curSplit.getHash() != curHashCheck) || (prevSplit.getHash() != longestChain->getBlockAt(curPos).getPreviousHash().substr(0, 64))) {
                // This miner is on a fork
                int forkPos = curChain->getChainSize() - 1;
                while (forkPos != 0) {
                    Block longer = longestChain->getBlockAt(forkPos);
                    Block local = curChain->getBlockAt(forkPos);
                    // Search from the end of the blockchains for the shallowest matching block
                    if ((longer.getHash() != local.getHash()) || longer.getPreviousHash() != local.getPreviousHash())
                        --forkPos;
                    else break;
                }
                // std::cerr << _id << " is forked!\n";
                // Copy longest blockchain up to the position where the two chains agreed
                Blockchain* validated = new Blockchain(false);
                for (int i = 0; i <= forkPos; ++i) {
                    std::string curHash, prevHash, miner;
                    if (i == 0) {
                        curHash = "genesisHash";
                        prevHash = "-1_-1";
                        miner = "";
                    }
                    else {
                        Block copyBlock(longestChain->getBlockAt(i));
                        curHash = copyBlock.getHash();
                        prevHash = copyBlock.getPreviousHash();
                        miner = *(longestChain->getBlockAt(i).getPublishers().begin());
                    }
                    validated->createBlock(validated->getChainSize(), prevHash, curHash, {miner});
                }
                // Set local chain to validated chain
                setCurChain(*validated);

                // Independently verify all the missed blocks
                for (int verifyPos = forkPos + 1; verifyPos != longestChainSize; ++verifyPos) {
                    int prevVerifyPos = verifyPos - 1;
                    //std::cerr << "verifying at\t" << verifyPos << "\n";
                    std::string verifyId = *(longestChain->getBlockAt(verifyPos).getPublishers().begin());
                    
                    const splitHash verifySplit = (verifyPos > 0 ? splitHash(longestChain->getBlockAt(verifyPos).getHash()) : splitHash(-1, "genesisHash"));
                    const std::string prevVerifyHash = (prevVerifyPos > 0 ? splitHash(longestChain->getBlockAt(verifyPos).getPreviousHash()).getHash() : "-1_-1");

                    std::string verifyHash;
                    
                    if (verifyPos == 0 || prevVerifyHash == "-1_-1") verifyHash = "genesisHash";
                    else picosha2::hash256_hex_string(prevVerifyHash + verifyId + std::to_string(verifySplit.getNonce()), verifyHash);

                    if ((verifySplit.getHash() == verifyHash) && (prevVerifyHash == longestChain->getBlockAt(verifyPos).getPreviousHash().substr(0, 64))) {
                        curChain->createBlock(curChain->getChainSize(), prevVerifyHash, verifySplit.getHash() + ":" + std::to_string(verifySplit.getNonce()), {verifyId});
                        //std::cerr << "nonce in fork resolution is \t" << std::to_string(verifySplit.getNonce()) << "[" << verifySplit.getHash() << "]\n";
                        --blocksBehind;
                    }
                    else {
                        std::cerr << "\t\tH(" << prevVerifyHash << ", " << verifyId << ", " << std::to_string(verifySplit.getNonce()) << ") =\t" << verifyHash << "\n";
                        std::cerr << "\t\tCurs\t" << verifySplit.getHash() << "\t" << verifyHash << "\n";
                        std::cerr << "\t\tPrevs\t" << prevVerifyHash << "\t" <<  longestChain->getBlockAt(verifyPos).getPreviousHash().substr(0, 64) << "\n";
                        std::cerr << "FORK RESOLUTION FAILED - EXITING\n";
                        exit(EXIT_FAILURE);
                        break;
                    }
                }
                break;
            }
            else {
                curChain->createBlock(curChain->getChainSize(), prevSplit.getHash(), curSplit.getHash() + ":" + std::to_string(curSplit.getNonce()), {minerId});
                //std::cerr << "nonce in catchUp is \t" << std::to_string(curSplit.getNonce()) << "[" << curSplit.getHash() << "]\n";
                --blocksBehind;
            }
        }
    }
    _inStream.clear();
    return false;
}

// Send block to other miners
void BitcoinMiner::transmitBlock() {
    const int blockLength = curChain->getChainSize();
    Block newBlock = curChain->getBlockAt(blockLength - 1);

    BitcoinMessage toSend(newBlock, _id, blockLength);

    for (auto it = _neighbors.begin(); it != _neighbors.end(); ++it) {
        std::string neighborId = it->first;
        Packet<BitcoinMessage> msgPacket("", neighborId, _id);
        msgPacket.setBody(toSend);
        _outStream.push_back(msgPacket);
    }
    transmit();
}

void BitcoinMiner::makeRequest() {


}
