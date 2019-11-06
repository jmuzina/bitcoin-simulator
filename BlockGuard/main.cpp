#include <iostream>
#include "QuickShards/quickshards.h"

int main() {
    quickShards testshards(10,1);
    for (int i = 0; i< 1; ++i) {
        testshards.setRandomByzantine();
    }
    std:: cout << "valid consensus count" << testshards.getConsensusCount();
}