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

void peerAccess             (std::ostream &log); // test that all peers can be accessed
void peerInit               (std::ostream &log); // test that all peers are init correctly
void intersection           (std::ostream &log); // test that peers are formed into quorums correctly
void leaves                 (std::ostream &log); // test the leaves drops peers or does nothing (if all peers are dropped)
void joins                  (std::ostream &log); // test that a peer joins a quorum or if all quorums are filled joins the reserve

#endif //DISTRIBUTED_CONSENSUS_ABSTRACT_SIMULATOR_SMARTSHARDS_TEST_H
