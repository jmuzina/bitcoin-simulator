//
// Created by snwbr on 11/6/2019.
//
#include "quickshards.h"

quickShards::quickShards(int peers, int intersections, double tolerance) {
    _intersections = intersections;
    int quorums = 0.5 * sqrt(8 * peers + _intersections) / sqrt(_intersections) + 1;

    //Check quorum size, decrease size if needed
    while (quorums * (quorums - 1) / 2 * _intersections > peers)
        --quorums;
    _reserve = peers - (quorums * (quorums - 1) / 2 * _intersections);

    // Create shards
    _peersPerShard = (quorums - 1) * _intersections;
    int f = (_peersPerShard-1) * tolerance;
    std::cout << "quorums=" << quorums << std::endl;
    std::cout << "peersPerQuorums=" << _peersPerShard << std::endl;
    std::cout << "f= " << f << std::endl;
    std::cout << "tolerance= " << tolerance << std::endl;
    std::cout << "reserve= " << _reserve << std::endl;
    for (int i = 0; i < quorums; ++i) {
        _shards[i] = quickPBFT(i, _peersPerShard, quorums-1, f);
    }

    // Create intersections
    for (int workQuorum = 0; workQuorum < quorums; ++workQuorum){
        for (int offset = 0; offset < _intersections; ++offset ){
            for (int workPeer=workQuorum;workPeer< (_peersPerShard/_intersections); ++workPeer){
                //std::cout << workQuorum << ", " << workPeer+peersPerQuorum*offset << std::endl;
                quickPeer* tmpA = _shards[workQuorum].getPeer(workPeer+_peersPerShard/_intersections*offset);
                quickPeer* tmpB = _shards[workPeer+1].getPeer(workQuorum+offset*_peersPerShard/_intersections);
                tmpA->setCoPeer(tmpB);
                tmpB->setCoPeer(tmpA);
            }
        }
    }

}

void quickPBFT::printPeers(std::ostream& out) {
    for (auto e: _peers) {
        assert(e.second.getCoPeer() != nullptr);
        out << e.second.getID().first << "." << e.second.getID().second
            << " copeer " ;
        out << e.second.getCoPeer()->getID().first << "." << e.second.getCoPeer()->getID().second << std::endl;
    }
}

int quickShards::getConsensusCount() {
    int count = 0;
    for (auto e: _shards)
        if (e.second.validConsensus() && e.second.validIntersections())
            ++count;
    return count;
}