//
// Created by khood on 10/14/19.
//

#ifndef DISTRIBUTED_CONSENSUS_ABSTRACT_SIMULATOR_SMARTSHARDS_EXPERIMENTS_H
#define DISTRIBUTED_CONSENSUS_ABSTRACT_SIMULATOR_SMARTSHARDS_EXPERIMENTS_H

#include <iostream>
#include <fstream>
#include <set>
#include <chrono>
#include <random>
#include <string>
#include <deque>

#include "SmartShardPBFT_peer.hpp"
#include "SmartShards/SmartShard.hpp"
#include "SmartShards/params_SmartShards.h"
#include "BlockGuard/params_Blockguard.hpp"

void                churnRateVsQuorumIntersection       (std::ofstream &csv, std::ofstream &log);
void                churnRateVsQuorumIntersectionQuick  (std::ofstream &csv, std::ofstream &log);

// intersection runs
std::string         intersectionWithHaltsAndJoins       (int numberOfShards, int numberOfJoins, int numberOfHalts, std::ofstream &log);
std::string         quickPBFT                           (int numberOfShards, int numberOfJoins, int numberOfHalts, std::ofstream &log);


// util
// returns a list of rounds for for some event to happen, each event gets it's own round
std::deque<int>     scheduleEvents                      (int numberOfEvents);

#endif //DISTRIBUTED_CONSENSUS_ABSTRACT_SIMULATOR_SMARTSHARDS_EXPERIMENTS_H
