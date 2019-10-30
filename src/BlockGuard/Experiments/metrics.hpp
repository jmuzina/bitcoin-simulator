//
//  metrics.hpp
//  src
//
//  Created by Kendric Hood on 7/9/19.
//  Copyright © 2019 Kent State University. All rights reserved.
//

#ifndef metrics_hpp
#define metrics_hpp

#include <stdio.h>
#include "./../Common/DAGBlock.hpp"
#include "BlockGuard/params_Blockguard.hpp"

double  ratioOfSecLvl                   (std::vector<DAGBlock> globalLedger, double secLvl);
double  waitTimeOfSecLvl                (std::vector<DAGBlock> globalLedger, double secLvl);
double  waitTime                        (std::vector<DAGBlock> globalLedger);
double  waitTimeRolling                 (std::vector<DAGBlock> globalLedger, int fromRound);
int     totalNumberOfDefeatedCommittees (std::vector<DAGBlock> globalLedger, double secLvl);
int     defeatedTrnasactions            (std::vector<DAGBlock> globalLedger);
int     totalNumberOfCorrectCommittees  (std::vector<DAGBlock> globalLedger, double secLvl);
int     getRollingAvgWaitTime           (std::vector<DAGBlock> globalLedger, int secLevel, int rounds);

#endif /* metrics_hpp */
