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

// Returns a miner pointer given their ID
BitcoinMiner* BitcoinMiner::findMiner(const std::string id) {
    return dynamic_cast<BitcoinMiner*>(_neighbors.find(id)->second);
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
        std::string challengeBits = hash.substr(0, 1); // Arbitrary first few bits of attempted solution
        // Valid PoW solution - send block to other miners
        if ((challengeBits == "0" || hash == "genesisHash")) { 
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
        const long long solution = (curLength > 1 ? lastNonce : -1);
        const std::string prevHash = (curLength > 1 ? splitHash(curChain->getBlockAt(curLength - 1).getHash()).getHash() : "-1_-1");

        curChain->createBlock(curLength, prevHash, newHash + ":" + std::to_string(solution), {_id}); // add to local blockchain
        //std::cout << "Block " << curLength << " has been mined by " << _id << ":\t" << prevHash << " -> " << newHash << "\n";

        transmitBlock(); // send to other miners
    }
}

void BitcoinMiner::catchUpAndVerify(Blockchain* overtaker) {
    const int OVERTAKER_LENGTH = overtaker->getChainSize();
    int blocksBehind = OVERTAKER_LENGTH - curChain->getChainSize();
    int curPos = OVERTAKER_LENGTH - blocksBehind - 1;
    int prevPos = curPos - 1;

    if (blocksBehind > 0) {
        setBeaten(true);
        setLastNonce(0);
    }

    while (blocksBehind > 0) {
        int curPos = OVERTAKER_LENGTH - blocksBehind - 1;
        if (curPos == 0) ++curPos;
        int prevPos = curPos - 1;
        std::string minerId = *(overtaker->getBlockAt(curPos).getPublishers().begin());

        const splitHash curSplit = (curPos > 0 ? splitHash(overtaker->getBlockAt(curPos).getHash()) : splitHash(-1, "genesisHash"));
        const splitHash prevSplit = (prevPos > 0 ? splitHash(overtaker->getBlockAt(prevPos).getHash()) : splitHash());
        
        std::string curHashCheck;

        if (curPos == 0 || prevSplit.getHash() == "-1_-1") curHashCheck = "genesisHash";
        else picosha2::hash256_hex_string(prevSplit.getHash() + minerId + std::to_string(curSplit.getNonce()), curHashCheck);

        if ((curSplit.getHash() != curHashCheck) || (prevSplit.getHash() != overtaker->getBlockAt(curPos).getPreviousHash().substr(0, 64))) {
            // This miner is on a fork - search from start of local chain for first mismatch and recreate the known good part of the chain.
            int forkPos = 0;
            Blockchain* validated = new Blockchain(false);
            while (forkPos != curChain->getChainSize()) {
                Block longer = overtaker->getBlockAt(forkPos);
                Block local = curChain->getBlockAt(forkPos);
                if ((longer.getHash() == local.getHash()) && longer.getPreviousHash() == local.getPreviousHash()) {
                    std::string curHash, prevHash, miner;
                    if (forkPos == 0) {
                        curHash = "genesisHash";
                        prevHash = "-1_-1";
                        miner = "";
                    }
                    else {
                        Block copyBlock(overtaker->getBlockAt(forkPos));
                        curHash = copyBlock.getHash();
                        prevHash = copyBlock.getPreviousHash();
                        miner = *(overtaker->getBlockAt(forkPos).getPublishers().begin());
                    }
                    validated->createBlock(validated->getChainSize(), prevHash, curHash, {miner});
                    ++forkPos;
                }
                else break;
            }
            setCurChain(*validated);

            // Independently verify all the missed blocks
            for (int verifyPos = forkPos; verifyPos != OVERTAKER_LENGTH; ++verifyPos) {
                int prevVerifyPos = verifyPos - 1;
                std::string verifyId = *(overtaker->getBlockAt(verifyPos).getPublishers().begin());
                
                const splitHash verifySplit = (verifyPos == 0 ? splitHash(-1, "genesisHash") : splitHash(overtaker->getBlockAt(verifyPos).getHash()));
                const std::string prevVerifyHash = (verifyPos < 2 ? "-1_-1" : splitHash(overtaker->getBlockAt(verifyPos).getPreviousHash()).getHash());

                std::string verifyHash;
                
                if (verifyPos == 0 || prevVerifyHash == "-1_-1") verifyHash = "genesisHash";
                else picosha2::hash256_hex_string(prevVerifyHash + verifyId + std::to_string(verifySplit.getNonce()), verifyHash);

                if ((verifySplit.getHash() == verifyHash) && (prevVerifyHash == overtaker->getBlockAt(verifyPos).getPreviousHash().substr(0, 64))) {
                    curChain->createBlock(curChain->getChainSize(), prevVerifyHash, verifySplit.getHash() + ":" + std::to_string(verifySplit.getNonce()), {verifyId});
                    //std::cerr << "\n" << _id << "(A) verified block " << curChain->getChainSize() - 1 << "\t" << prevVerifyHash << " -> " << verifyHash << "\n";
                    --blocksBehind;
                }
                else {
                    std::cerr << "\t\tH(" << prevVerifyHash << ", " << verifyId << ", " << std::to_string(verifySplit.getNonce()) << ") =\t" << verifyHash << "\n";
                    std::cerr << "\t\tCurs\t" << verifySplit.getHash() << "\t" << verifyHash << "\n";
                    std::cerr << "\t\tPrevs\t" << prevVerifyHash << "\t" <<  overtaker->getBlockAt(verifyPos).getPreviousHash().substr(0, 64) << "\n";
                    std::cerr << "FORK RESOLUTION FAILED - EXITING\n";
                    exit(EXIT_FAILURE);
                    break;
                }
            }
            break;
        }
        else if ((curSplit.getHash() == curHashCheck) && (prevSplit.getHash() == overtaker->getBlockAt(curPos).getPreviousHash().substr(0, 64))) {
            curChain->createBlock(curChain->getChainSize(), prevSplit.getHash(), curSplit.getHash() + ":" + std::to_string(curSplit.getNonce()), {minerId});
            //std::cerr << "\n" << _id << "(B) verified block " << curChain->getChainSize() - 1 << "\t" << prevSplit.getHash() << " -> " << curSplit.getHash() << "\n";
            --blocksBehind;
        }
        else {
            std::cerr << "FATAL ERROR - Catch up and verify. No conditions matched. Exiting.\n";
            exit(EXIT_FAILURE);
        }
    }
}

// Checks for solutions from other miners.
void BitcoinMiner::readBlock() {
    receive(); // refresh instream
    int longestChainLength = curChain->getChainSize();
    const int LOCAL_SIZE = curChain->getChainSize();

    // Stores position of message with longest corresponding blockchain
    int longestChainAt = -1;

    // Find longest blockchain
    for (int i = 0; i < _inStream.size(); ++i) {
        const BitcoinMessage RECEIVED_MSG = _inStream[i].getMessage();
        const int RECEIVED_LENGTH = RECEIVED_MSG.length; // blockchain length
        const std::string senderId = _inStream[i].getMessage().peerId;
        if (RECEIVED_LENGTH > LOCAL_SIZE && RECEIVED_LENGTH > longestChainLength) {
            longestChainAt = i;
            longestChainLength = RECEIVED_LENGTH;
        }
        else if (RECEIVED_LENGTH == LOCAL_SIZE && senderId != _id && (findMiner(senderId)->getCurChain()->getBlockAt(findMiner(senderId)->getCurChain()->getChainSize() - 1).getHash() != curChain->getBlockAt(curChain->getChainSize() - 1).getHash())) {
            competitors.push_back(findMiner(senderId));
        }
    }

    // Check if a competitor has overtaken us
    int longestCompetitorSize = -1;
    int longestCompetitorNum = -1;

    for (int competitorNum = 0; competitorNum < competitors.size(); ++competitorNum) {
        int competitorSize = competitors.at(competitorNum)->getCurChain()->getChainSize();
        if (competitorSize > LOCAL_SIZE && competitorSize > longestCompetitorSize) {
            longestCompetitorSize = competitorSize;
            longestCompetitorNum = competitorNum;
        }
    }

    // Node has lost a fork race and will rebuild its chain up to the fork position, then verify the competitor's subsequent block(s)
    if (longestCompetitorSize != -1) {
        //std::cerr << _id << " catching up from " << LOCAL_SIZE - 1 << " to " << longestCompetitorSize - 1<< "(" << competitors.at(longestCompetitorNum)->id() << ")\n";
        Blockchain* longestCompetitorChain = competitors.at(longestCompetitorNum)->getCurChain();
        catchUpAndVerify(longestCompetitorChain);
        competitors.clear();
    }
    // If the longest chain received is longer than the peer's current chain,
    // peer will verify each block that it is missing and add them to its chain.
    else if (longestChainAt != -1) {
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

        // Catch up and verify each block along the way
        if (blocksBehind > 0) catchUpAndVerify(longestChain); 
    }
    _inStream.clear();
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
