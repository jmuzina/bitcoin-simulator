#include <iostream>
#include <fstream>
#include "QuickShards/quickshards.h"

int main() {
    auto beginTime = time(nullptr);
    std::ofstream log("d:/test/log.csv");

    std::ostream& out = std::cout;
    const int roundsToConfirmation = 4;
    const int rounds = 10000;
    const int delay = 1;
    const int peers = 1000;
    const int tests = 10;
    int intersectionMax = peers;
    int quorumNum;
    int peersInQuorum;

    log << "interections, numJoins, numHalts, confirmations, numQuorums, peersInQuorum, good, byzantine, reserve" << std::endl;

    for(int intersections = 1; intersections<=intersectionMax; ++intersections) {

        int numberOfHalts = peers;
        for (float numberOfJoins = 1.0; numberOfJoins >= 0.0; numberOfJoins -= .20) {
            if (numberOfJoins < .20)
                numberOfJoins = 0.0;
            int totalConfirmations = 0;
            int totalGood = 0 ;
            int totalByzantine = 0;
            int totalReserve = 0;
            for (int test = 0; test < tests; ++test) {
                quickShards system(peers, intersections);
                while(true) {
                    if (intersections != intersectionMax) {
                        quickShards testSystem(peers, intersections);
                        quickShards nextSystem(peers, intersections + 1);
                        if (nextSystem.getNumQuorums() == testSystem.getNumQuorums()){
                            ++intersections;
                        }
                        else
                            break;
                    }
                    else break;
                }
                int confirmations = 0;
                system.setReserve(0);
                std::deque<int> joinRounds = scheduleEvents(numberOfJoins*peers, rounds);
                std::deque<int> haltRounds = scheduleEvents(numberOfHalts, rounds);
                for (int round = 0; round < rounds; ++round) {
                    if (round == *joinRounds.begin() && !joinRounds.empty()) {
                        system.setRandomByzantineFalse();
                        joinRounds.pop_front();
                    }
                    if (round == *haltRounds.begin() && !haltRounds.empty()) {
                        system.setRandomByzantineTrue();
                        haltRounds.pop_front();
                    }
                    if (round % 4 == 0 && round != 0)
                        confirmations += system.getConsensusCount();

                }
                // End Test
                quorumNum = system.getNumQuorums();
                peersInQuorum = system.getPeersInQuorum();
                totalConfirmations += confirmations;
                totalByzantine +=system.getByzantineNumber();
                totalGood += system.getValidPeers();
                totalReserve += system.getReserve();
            }

            log << intersections << ", " << numberOfJoins << ", " << numberOfHalts << ", "
                << totalConfirmations / tests << ", " << quorumNum << ", " << peersInQuorum
                << ", " << totalGood/tests << ", " << totalByzantine/tests
                << ", " << totalReserve/tests <<  std::endl;

        }
        std::cerr << "intersection: " << intersections << " complete\n";
    }
    log.close();
    out << "End Time: " << time(nullptr)-beginTime << std::endl;
}