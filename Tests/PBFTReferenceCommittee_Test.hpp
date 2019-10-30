//
//  PBFTReferenceCommittee_Test.cpp
//  src
//
//  Created by Kendric Hood on 5/8/19.
//  Copyright © 2019 Kent State University. All rights reserved.
//

#ifndef PBFTReferenceCommittee_Test_hpp
#define PBFTReferenceCommittee_Test_hpp

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "../BlockGuard/PBFT/PBFTPeer_Sharded.hpp"
#include "../BlockGuard/PBFT/PBFTReferenceCommittee.hpp"

void RunPBFTRefComTest              (std::string filepath); // run all PBFT tests

// test init (system setup)
void testInit                       (std::ostream &log);

// test for group and committee assignment and maintance 
void testRefComGroups               (std::ostream &log); // test that groups are made correclty and groups and peers have a many to one relationship
void testRefComCommittee            (std::ostream &log); // test that one committee is formed for a transaction 
                                                         //      committees are formed correctly and peers are 
                                                         //      released from committees when consusnes if done
// test other things
void testGlobalLedger               (std::ostream &log); // test to make sure global ledger is calculated correctly
void testSimultaneousRequest        (std::ostream &log); // test making more then one request a round

// Byzantine tests
void testByzantineConfirmationRate  (std::ostream &log); // test that committees are set-back (do view changes) correctly
void testShuffle                    (std::ostream &log); // test that shuffling byzantine works and does not change thier state (erase ledger, messages etc)
void testByzantineVsDelay           (std::ostream &log); // test that committees are set-back (do view changes) with var delay
void testDelay                      (std::ostream &log); // test that delay is set correctly

#endif /* PBFTReferenceCommittee_Test_hpp */
