//
//  test.cpp
//  src
//
//  Created by Kendric Hood on 4/15/19.
//  Copyright © 2019 Kent State University. All rights reserved.
//

// run tests over classes

#include "PBFT/PBFTPeerTest.hpp"
#include "PBFT/PBFTPeer_Sharded_Test.hpp"
#include "PBFT/PBFTReferenceCommittee_Test.hpp"
#include "Common/ByzantineNetwork_Test.hpp"
#include "Common/NetworkTests.hpp"
#include "SmartShards/SmartShards_Test.h"

#include <string>

int main(int argc, const char * argv[]){
    if(argc < 2){
        std::cerr << "Error: need test to run and output path" << std::endl;
    }

    std::string testOption = argv[1];
    std::string filePath = argv[2];

    if(testOption == "all"){
        RunPBFT_Tests(filePath);
        RunPBFTPeerShardedTest(filePath);
        RunPBFTRefComTest(filePath);
        RunByzantineNetworkTest(filePath);
        runNetworkTests(filePath);
        runSmartShardsTest(filePath);
    }else if(testOption == "pbft"){
        RunPBFT_Tests(filePath);
    }else if (testOption == "s_pbft"){
        RunPBFTPeerShardedTest(filePath);
    }else if(testOption == "ref_com_pbft"){
        RunPBFTRefComTest(filePath);
    }else if(testOption == "b_network"){
        RunByzantineNetworkTest(filePath);
    }else if(testOption == "network"){
        runNetworkTests(filePath);
    }else if(testOption == "smartshard"){
        runSmartShardsTest(filePath);
    }

    return 0;
}


