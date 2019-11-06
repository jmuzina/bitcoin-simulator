//
// Smart Shard class, wraps markPbft into virtual peers that are part of multiple consensus instances
// Mark Gardner
// 9/6/2019
//
#ifndef SMART_SHARD_HPP
#define SMART_SHARD_HPP

#include <vector>
#include <set>
#include "SmartShardPBFT_peer.hpp"
#include "Common/ByzantineNetwork.hpp"

class SmartShard {
protected:
    // type abbreviations
    typedef ByzantineNetwork<SmartShardPBFT_Message, SmartShardPBFT_peer>* shard;

    // system state
    std::vector<shard>                              _system; // list of shards in the system
    std::map<int, std::set<SmartShardPBFT_peer*> >  _peers; // peer id, to list of real peers that it is in the quorums (vir peers)

    // system params
    int                                             _peersPerShard;
    int                                             _shards;
    int                                             _numberOfPeersInReserve;

    // logging
    std::ostream*                                   _out;

public:
	~SmartShard                                         () { for (auto e : _system) delete e; }
	//const int& shards, std::ostream& out, int delay, int peerspershard = -1, int reserveSize = 0, int quorumIntersection = 1)
	SmartShard                                          (const int& shards, std::ostream& out, int delay, int peerspershard = -1, int reserveSize = 0, int quorumIntersection = 1);
    SmartShard                                          (const SmartShard&);

	// setters
	void                        setFaultTolerance       (double);
    void                        setRequestsPerRound     (int requestPerRound);
    void                        setRoundsToRequest      (int roundstoRequest);
    void                        setMaxWait              ();

    // getters
    int                         size();
    int                         getConfirmationCount    ();
    bool                        isByzantine             (int peer);
    int                         getByzantine            ();
    int                         shardCount              () { return _system.size(); }
    int                         peerCount               () { return _peers.size(); }
    std::vector<shard>          getQuorums              () { return _system; }
    int                         getShardSize            () { return _peersPerShard; }

    // mutators
    void                        setupShardNeighborhood  ();
    void                        makeByzantine           (int peer);
    void                        makeCorrect             (int peer);
    void                        makeRequest             (int forQuorum = -1, int toQuorum = -1, int toPeer = -1);
    void                        joinPeer                ();
    void                        leavePeer               ();

    // logging and operators
    void printPeers();
    std::set<SmartShardPBFT_peer*>&   operator[]              (int i);
    SmartShard&                       operator=               (const SmartShard&);

};

#endif // SMART_SHARD_HPP
