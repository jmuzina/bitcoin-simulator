//
// Created by khood on 10/30/19.
//

#include "SmartShards_Test.h"

void runSmartShardsTest(std::string filepath){
    std::ofstream log;
    log.open(filepath + "/SmartShards.log");
    if (log.fail() ){
        std::cerr << "Error: could not open file at: "<< filepath << std::endl;
    }

    peerAccess(log);
    peerInit(log);
    intersection(log);
    leaves(log);
    joins(log);
}

void peerAccess(std::ostream &log){
    log<< std::endl<< "###############################"<< std::setw(LOG_WIDTH)<< std::left<<"!!!"<<"peerAccess"<< std::setw(LOG_WIDTH)<< std::right<<"!!!"<<"###############################"<< std::endl;
    SmartShard system(5,log,1,4,0,1);

    assert(system.size() == 10);
    assert(system.getReserve() == 0);
    for (int i = 0; i < system.size(); i++){
        assert(system[i].size() == 2);
    }

    log<< std::endl<< "###############################"<< std::setw(LOG_WIDTH)<< std::left<<"!!!"<<"peerAccess Complete"<< std::setw(LOG_WIDTH)<< std::right<<"!!!"<<"###############################"<< std::endl;
}
void peerInit(std::ostream &log){
    log<< std::endl<< "###############################"<< std::setw(LOG_WIDTH)<< std::left<<"!!!"<<"peerInit"<< std::setw(LOG_WIDTH)<< std::right<<"!!!"<<"###############################"<< std::endl;



    log<< std::endl<< "###############################"<< std::setw(LOG_WIDTH)<< std::left<<"!!!"<<"peerInit Complete"<< std::setw(LOG_WIDTH)<< std::right<<"!!!"<<"###############################"<< std::endl;
}
void intersection(std::ostream &log){
    log<< std::endl<< "###############################"<< std::setw(LOG_WIDTH)<< std::left<<"!!!"<<"intersection"<< std::setw(LOG_WIDTH)<< std::right<<"!!!"<<"###############################"<< std::endl;

    SmartShard system = SmartShard(5,log,1,4,0,2);

    assert(system.size() == 20);
    assert(system.getReserve() == 0);
    for (int i = 0; i < system.size(); i++){
        assert(system[i].size() == 2);
    }

    int quorumIntersection = 100/((5*(5-1))/2); // = 10
    int peerPerShard = quorumIntersection*(5-1); // = 40
    system = SmartShard(5,log,1,peerPerShard,0,quorumIntersection);

    assert(system.size() == 100);
    assert(system.getReserve() == 0);
    for (int i = 0; i < system.size(); i++){
        assert(system[i].size() == 2);
    }

    quorumIntersection = 100/((6*(6-1))/2); // = 6 remainder 1
    peerPerShard = quorumIntersection*(6-1); // = 30
    system = SmartShard(5,log,1,peerPerShard,0,quorumIntersection);

    assert(system.size() == 90);
    assert(system.getReserve() == 10); // 90 peers used in system 10 left over for reserve
    for (int i = 0; i < system.size(); i++){
        assert(system[i].size() == 2);
    }

    log<< std::endl<< "###############################"<< std::setw(LOG_WIDTH)<< std::left<<"!!!"<<"intersection Complete"<< std::setw(LOG_WIDTH)<< std::right<<"!!!"<<"###############################"<< std::endl;
}
void leaves(std::ostream &log){
    log<< std::endl<< "###############################"<< std::setw(LOG_WIDTH)<< std::left<<"!!!"<<"leaves"<< std::setw(LOG_WIDTH)<< std::right<<"!!!"<<"###############################"<< std::endl;



    log<< std::endl<< "###############################"<< std::setw(LOG_WIDTH)<< std::left<<"!!!"<<"leaves Complete"<< std::setw(LOG_WIDTH)<< std::right<<"!!!"<<"###############################"<< std::endl;
}
void joins(std::ostream &log){
    log<< std::endl<< "###############################"<< std::setw(LOG_WIDTH)<< std::left<<"!!!"<<"joins"<< std::setw(LOG_WIDTH)<< std::right<<"!!!"<<"###############################"<< std::endl;



    log<< std::endl<< "###############################"<< std::setw(LOG_WIDTH)<< std::left<<"!!!"<<"joins Complete"<< std::setw(LOG_WIDTH)<< std::right<<"!!!"<<"###############################"<< std::endl;
}
