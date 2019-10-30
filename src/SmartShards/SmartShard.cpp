#include "SmartShard.hpp"
#include "params_common.h"

SmartShard::SmartShard(const int& shards, std::ostream& out, int delay, int peerspershard = -1, int reserveSize = 0, int quorumIntersection = 1) {
	_out = &out;
	_shards = shards;
	if (peerspershard == -1)
        _peersPerShard = shards - 1;
	else
        _peersPerShard = peerspershard;

	// Initialize Quorums
	for (int i = 0; i < _shards; ++i) {
		_system.push_back(new ByzantineNetwork<markPBFT_message, markPBFT_peer>());
		_system[i]->setLog(*_out);
		_system[i]->setToRandom();
		_system[i]->setMaxDelay(delay);
		_system[i]->initNetwork(_peersPerShard);
		(*_system[i])[rand() % _peersPerShard]->setPrimary(true);
		for (int j = 0; j < _peersPerShard; ++j) {
            (*_system[i])[j]->setShard(i);
            (*_system[i])[j]->setShardCount(_shards);
        }
	}

	// map PBFT instances (called peers) to virtual peers (actual peers)
    int vPeer = 0;
    for(int quorum = 0; quorum < (_system.size()-1); quorum++){
        int nextQuorum = quorum +1 ;
        int offset = quorum*quorumIntersection;
        for(int thisPBFTInstances = offset; thisPBFTInstances < _system[quorum]->size(); ) {
            for (int otherPBFTInstances = 0; otherPBFTInstances < quorumIntersection; otherPBFTInstances++) {
                markPBFT_peer* peerFromThisQuorum = (*_system[quorum])[thisPBFTInstances];
                markPBFT_peer* peerFromOtherQuorum = (*_system[nextQuorum])[quorum + otherPBFTInstances];
                assert(peerFromThisQuorum != NULL);
                assert(peerFromOtherQuorum != NULL);
                _peers[vPeer].insert(peerFromThisQuorum);
                _peers[vPeer].insert(peerFromOtherQuorum);
                vPeer++;
                thisPBFTInstances++;
            }
            nextQuorum++;
        }
    }
    setupShardNeighborhood();

    if(shards*peerspershard < PEER_COUNT){
        _numberOfPeersInReserve = reserveSize + (PEER_COUNT - (shards*peerspershard)); // add left over peers to reserve
    }else {
        _numberOfPeersInReserve = reserveSize;
    }
}

void SmartShard::makeRequest(int forQuorum, int toQuorum, int toPeer) {
    if (forQuorum == -1) {
        forQuorum = rand() % _shards;
    }

    if (toQuorum == -1) {
        toQuorum = rand() % _shards;
    }

    if (toPeer == -1) {
        toPeer = rand() % _peersPerShard;
    }

    markPBFT_message requestMSG;
    requestMSG.creator_id = "MAGIC";
    requestMSG.type = "REQUEST";
    requestMSG.requestGoal = forQuorum;

    (*_system[toQuorum])[toPeer]->makeRequest(requestMSG);

}

int SmartShard::getByzantine(){
    int total = 0;
    for (int peer = 0; peer < _peers.size(); ++peer)
        if (isByzantine(peer))
            ++total;
    return total;
}

bool SmartShard::isByzantine(int peer) {
    auto it = _peers.find(peer);
    if (it == _peers.end())
        std::cerr << "peer does not exist finding byzantine";
    return ((*_peers[peer].begin())->isByzantine());
}

void SmartShard::makeCorrect(int peer) {
    if (_peers.find(peer) == _peers.end())
        std::cerr << "peer does not exist making correct";
    for (auto e : _peers[peer])
        e->makeCorrect();
}

void SmartShard::makeByzantine(int peer) {
    if (_peers.find(peer) == _peers.end())
        std::cerr << "peer does not exist making byzantine";
    for (auto e : _peers[peer]) {
        e->makeByzantine();
        e->clearMessages();
    }
}

int SmartShard::getConfirmationCount() {
    int total = 0;
    for (auto quorum : _system) {
        int max = (*quorum)[0]->ledger().size();
        for (int i = 0; i < _peersPerShard; ++i) {
            if (max < (*quorum)[i]->ledger().size()) {
                max = (*quorum)[i]->ledger().size();
            }
        }
        total += max;
    }
    return total;
}

int SmartShard::size(){
    int total = 0;
    for(auto s : _system){
        total += s->size();
    }
    return total;
}

void SmartShard::dropPeer(){
    assert(_numberOfPeersInReserve > -1);
    if(getByzantine() == _peers.size()){return;}// all peers are dead

    if(_numberOfPeersInReserve == 0){
        int peerToDrop = rand()%_peers.size();
        while (isByzantine(peerToDrop)) {
            peerToDrop = rand() % _peers.size();
        }
        makeByzantine(peerToDrop);
        std::cerr << "no reserve peers, dropping peer\n";
        std::cerr << "reserves: " << _numberOfPeersInReserve << std::endl;
        std::cerr << "num of dropped peers: ";
        int droppedCount = 0;
        for (int peer = 0; peer < _peers.size(); ++peer)
            if (isByzantine(peer))
                ++droppedCount;
        std::cerr << droppedCount << std::endl << std::endl;

    }else{
        _numberOfPeersInReserve--;
        std::cerr << "peers in reverse, removing from reserve\n";
        std::cerr << "reserves: " << _numberOfPeersInReserve << std::endl;
        std::cerr << "num of dropped peers: ";
        int droppedCount = 0;
        for (int peer = 0; peer < _peers.size(); ++peer)
            if (isByzantine(peer))
                ++droppedCount;
        std::cerr << droppedCount << std::endl <<std::endl;
    }
}

void SmartShard::revivePeer(){
    assert(_numberOfPeersInReserve > -1);
    if(_numberOfPeersInReserve == 0){

        for(int peer = 0; peer < _peers.size(); peer++){
            if(isByzantine(peer)){
                makeCorrect(peer);
                std::cerr << "dropped peer exists, adding to dropped\n";
                std::cerr << "reserves: " << _numberOfPeersInReserve << std::endl;
                std::cerr << "num of dropped peers: ";
                int droppedCount = 0;
                for (int peer = 0; peer < _peers.size(); ++peer)
                    if (isByzantine(peer))
                        ++droppedCount;
                std::cerr << droppedCount << std::endl << std::endl;
                return;
            }
        }
    }
    _numberOfPeersInReserve++;
    std::cerr << "no dropped peers, adding to reserve\n";
    std::cerr << "reserves: " << _numberOfPeersInReserve << std::endl;
    std::cerr << "num of dropped peers: ";
    int droppedCount = 0;
    for (int peer = 0; peer < _peers.size(); ++peer)
        if (isByzantine(peer))
            ++droppedCount;
    std::cerr << droppedCount << std::endl<<std::endl;
}

std::set<markPBFT_peer*>& SmartShard::operator[] (int i) {
    if (_peers.find(i) == _peers.end())
        std::cerr << "peer " << i << " does not exist";
    return _peers[i];
}

void SmartShard::setMaxWait() {
    for (auto e : _system)
        for (int i = 0; i < _peersPerShard; ++i) {
            (*e)[i]->setMaxWait();
        }
}

void SmartShard::setRoundsToRequest(int roundstoRequest) {
    for (auto e : _system)
        for (int i = 0; i < _peersPerShard; ++i)
            (*e)[i]->setRoundsToRequest(roundstoRequest);
}

void SmartShard::setRequestsPerRound(int requestPerRound) {
    for (auto e : _system)
        for (int i = 0; i < _peersPerShard; ++i)
            (*e)[i]->setRequestPerRound(requestPerRound);
}

void SmartShard::setupShardNeighborhood() {
    for (auto e : _peers) {
        (*e.second.begin())->setNeighborShard((*(--e.second.end()))->getShard());
        (*(--e.second.end()))->setNeighborShard((*e.second.begin())->getShard());
    }
}

void SmartShard::printPeers() {
	for (auto e : _peers) {
		std::cerr << "Peer " << e.first << ": ";
		for (auto f : e.second)
			std::cerr << f->id() << " quorum " << f->getShard() << ", ";
		std::cerr << std::endl;
	}
}

void SmartShard::setFaultTolerance(double tolerance) {
	for (auto e : _system)
		for (int i = 0; i < _peersPerShard; ++i)
			(*e)[i]->setFaultTolerance(tolerance);

}