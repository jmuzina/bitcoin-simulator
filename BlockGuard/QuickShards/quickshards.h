//
// Created by snwbr on 11/6/2019.
//

#ifndef QUICKSMARTSHARD_QUICKSHARDS_H
#define QUICKSMARTSHARD_QUICKSHARDS_H

#include <map>
#include <iostream>
#include <cmath>
#include <cassert>
#include <random>
#include <ctime>
#include <set>
#include <vector>

class quickPeer {
private:
    // ID is made of quorum,number in quorum
    std::pair<int, int> _peerID;
    quickPeer *_coPeer;
    bool _byzantineStatus;

public:
    quickPeer() {};

    quickPeer(const std::pair<int, int> &a) {
        _peerID.first = a.first;
        _peerID.second = a.second;
        _byzantineStatus = false;
        _coPeer = nullptr;
    };

    bool isByzantine() { return _byzantineStatus; }

    void setCoPeer(quickPeer *a) { _coPeer = a; }
    quickPeer* getSelf() {return this;}
    quickPeer* getCoPeer() { return _coPeer; };

    std::pair<int, int> getID() const { return _peerID; }

    void setByzantineStatus(bool status) {
        _byzantineStatus = status;
        if (_coPeer->isByzantine() != status)
            _coPeer->setByzantineStatus(status);
    }
};

class quickPBFT{
private:
    int _f;
    int _quorumID;
    int _otherQuorumCount;
    std::map<int, quickPeer> _peers; // peers in pbft

public:
    quickPBFT(){};
    quickPBFT(int quorum, int numPeers, int otherQuorums, int f){for (int i = 0; i < numPeers; ++i){
        _peers[i] = quickPeer(std::pair<int,int>(quorum,i));}
        _quorumID = quorum;
        _f = f;
        _otherQuorumCount = otherQuorums;
    };
    std::map<int, quickPeer>& getPeerList() {return _peers;}
    bool validConsensus(){int count = 0;
        for(auto e: _peers) if (!e.second.isByzantine()) count++;
        bool result = count > 2*_f;
        return (count > 2 *_f);}
    void printPeers(std::ostream&);
    quickPeer* getPeer(int a) {return &(_peers[a]);}
    int getID() {return _quorumID;}
    bool validIntersections(){
        std::set<int> intersectCount;
        for (auto e: _peers){
            if (!e.second.isByzantine())
                intersectCount.insert(e.second.getCoPeer()->getID().first);
        }
        return (intersectCount.size() == _otherQuorumCount);
    }
};

class quickShards {
private:
    std::map<int, quickPBFT> _shards;
    int _reserve;
    int _peersPerShard;
    int _intersections;

public:
    quickShards() {}
    quickShards(int, int, double tolerance = double(1.0) / double(3.0));
    int getConsensusCount();
    void setRandomByzantineTrue();
    void setRandomByzantineFalse();


};
#endif //QUICKSMARTSHARD_QUICKSHARDS_H
