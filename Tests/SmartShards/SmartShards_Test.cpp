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
    SmartShard system(5,log,1,4,0,1);

    assert(system.size() == 10);
    for (int i = 0; i < system.size(); i++){
        assert(system[i].size() == 2);
    }

    system = SmartShard(5,log,1,4,0,1);

    assert(system.size() == 10);
    for (int i = 0; i < system.size(); i++){
        assert(system[i].size() == 2);
    }

}
void peerInit(std::ostream &log){

}
void intersection(std::ostream &log){

}
void leaves(std::ostream &log){

}
void joins(std::ostream &log){

}
