#ifndef JMUZINA_BCOINPEER
#define JMUZINA_BCOINPEER

#include "./../Common/Peer.hpp"
#include "./../Common/Blockchain.hpp"
#include "./bitcoinMessage.hpp"

class BitcoinPeer : public Peer<BitcoinMessage> { // inherits from peer class
public:
    // Attributes
    Blockchain*                     curChain; // This peer's version of the blockchain

    // Methods
                                    BitcoinPeer             (const std::string);
                                    ~BitcoinPeer            () override                 { delete curChain; };
    void                            makeRequest             () override;
    void                            preformComputation      () override;
    void                            readBlock               ();
    void                            transmitBlock           ();
    void                            setCurChain             (const Blockchain& setFrom) { *curChain = setFrom; };
    Blockchain*                     getCurChain             () const                    { return curChain; };
    std::string                     getId                   () const                    { return peerId; };
private:
    std::string                     peerId;
};

#endif 