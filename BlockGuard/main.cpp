#include <iostream>
#include "QuickShards/quickshards.h"

int main() {
    auto beginTime = time(nullptr);
    std::ostream& out = std::cout;
    const int roundsToConfirmation = 4;
    const int rounds = 1000;
    const int delay = 1;
    const int peers = 10;
    const int intersections = 1;

    quickShards testshards(peers, intersections);
    testshards.setRandomByzantineFalse();

    long totalConfirmmations = 0;
    for (int round = 0; round < rounds; ++round){
        if(round % (roundsToConfirmation*delay) == 0 && round !=0 ) {
            totalConfirmmations += testshards.getConsensusCount();
        }
    }
    /*  random joins/leaves
    std::deque<int> joinRounds = scheduleEvents(numberOfJoins);
    std::deque<int> haltRounds = scheduleEvents(numberOfHalts);*/
    out << "Total Confirmations: " << totalConfirmmations << std::endl;
    out << "End Time: " << time(nullptr)-beginTime << std::endl;
}