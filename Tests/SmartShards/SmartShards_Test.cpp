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

}
void peerInit(std::ostream &log){

}
void intersection(std::ostream &log){

}
void leaves(std::ostream &log){

}
void joins(std::ostream &log){

}
