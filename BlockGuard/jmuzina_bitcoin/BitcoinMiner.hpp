#ifndef JMUZINA_BCOINPEER
#define JMUZINA_BCOINPEER

#include "./../Common/Peer.hpp"
#include "./../Common/Blockchain.hpp"
#include <stdlib.h>
#include <time.h>
#include "./BitcoinMessage.hpp"
#include "picosha2.h"

class BitcoinMiner : public Peer<BitcoinMessage> { // inherits from peer class
public:
    // Attributes
    Blockchain*                     curChain; // This peer's version of the blockchain

    // Methods
                                    BitcoinMiner            (const std::string);
                                    ~BitcoinMiner           () override                 { delete curChain; };
    void                            mineNext                (const std::string);
    void                            makeRequest             () override;
    void                            preformComputation      () override;
    bool                            readBlock               ();
    void                            transmitBlock           ();
    void                            setCurChain             (const Blockchain& setFrom) { *curChain = setFrom; };
    void                            setLastNonce            (long long nonce)                { lastNonce = nonce; };
    long                            getLastNonce            () const                    { return lastNonce; };
    Blockchain*                     getCurChain             () const                    { return curChain; };
    std::string                     getId                   () const                    { return peerId; };
    std::string                     getSHA                  (long long) const;

private:
    std::string                     peerId;
    std::string                     foundHash;
    long long                       lastNonce;
};

#endif 