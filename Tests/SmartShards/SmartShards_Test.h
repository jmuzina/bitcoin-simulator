//
// Created by khood on 10/30/19.
//

#ifndef DISTRIBUTED_CONSENSUS_ABSTRACT_SIMULATOR_SMARTSHARDS_TEST_H
#define DISTRIBUTED_CONSENSUS_ABSTRACT_SIMULATOR_SMARTSHARDS_TEST_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "../../src/SmartShards/SmartShard.hpp"

void runSmartShardsTest     (std::string filepath);

void peerAccess             (std::ostream &log);
void peerInit               (std::ostream &log);
void intersection           (std::ostream &log);
void leaves                 (std::ostream &log);
void joins                  (std::ostream &log);

#endif //DISTRIBUTED_CONSENSUS_ABSTRACT_SIMULATOR_SMARTSHARDS_TEST_H
