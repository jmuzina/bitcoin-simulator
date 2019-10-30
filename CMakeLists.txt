cmake_minimum_required(VERSION 3.14)
project(distributed_consensus_abstract_simulator)

set(CMAKE_CXX_STANDARD 14)

#Experiments

include_directories(src)
include_directories(src/bCoin)
include_directories(src/Common)
include_directories(src/PBFT)
include_directories(src/SBFT)
include_directories(src/SmartShards)
include_directories(src/SmartShards/Experiments)
include_directories(src/BlockGuard)
include_directories(src/BlockGuard/Experiments)


add_executable(distributed_consensus_abstract_simulator
        src/bCoin/bCoin_Committee.cpp
        src/bCoin/bCoin_Committee.hpp
        src/bCoin/bCoin_Peer.cpp
        src/bCoin/bCoin_Peer.hpp
        src/bCoin/bCoinReferenceCommittee.cpp
        src/bCoin/bCoinReferenceCommittee.hpp
        src/bCoin/DS_bCoin_Peer.cpp
        src/bCoin/DS_bCoin_Peer.hpp

        src/Common/Block.cpp
        src/Common/Block.hpp
        src/Common/Blockchain.cpp
        src/Common/Blockchain.hpp
        src/Common/ByzantineNetwork.hpp
        src/Common/committee.hpp
        src/Common/DAG.hpp
        src/Common/DAGBlock.cpp
        src/Common/DAGBlock.hpp
        src/Common/hash.hpp
        src/Common/Logger.cpp
        src/Common/Logger.hpp
        src/Common/Network.hpp
        src/Common/Packet.hpp
        src/Common/Peer.hpp

        src/BlockGuard/Experiments/metrics.cpp
        src/BlockGuard/Experiments/metrics.hpp
        src/BlockGuard/Experiments/refComExperiments.cpp
        src/BlockGuard/Experiments/refComExperiments.hpp
        src/BlockGuard/Experiments/Sharded_PBFT_Experiments.cpp
        src/BlockGuard/Experiments/Sharded_PBFT_Experiments.hpp
        src/BlockGuard/Experiments/Sharded_POW_Experiments.cpp
        src/BlockGuard/Experiments/Sharded_POW_Experiments.hpp
        src/BlockGuard/Experiments/Sharded_SBFT_Experiments.cpp
        src/BlockGuard/Experiments/Sharded_SBFT_Experiments.hpp
        src/BlockGuard/params_Blockguard.hpp

        src/SmartShards/Experiments/SmartShards_Experiments.cpp
        src/SmartShards/Experiments/SmartShards_Experiments.h
        src/SmartShards/SmartShard.hpp
        src/SmartShards/SmartShard.cpp

        src/PBFT/DS_PBFT.hpp
        src/PBFT/DS_PBFT_Peer.hpp
        src/PBFT/PBFT_Committee.hpp
        src/PBFT/PBFT_Peer.cpp
        src/PBFT/PBFT_Peer.hpp
        src/PBFT/PBFTPeer_Sharded.cpp
        src/PBFT/PBFTPeer_Sharded.hpp
        src/PBFT/PBFTReferenceCommittee.cpp
        src/PBFT/PBFTReferenceCommittee.hpp
        src/PBFT/SmartShardPBFT_peer.cpp
        src/PBFT/SmartShardPBFT_peer.hpp

        src/SBFT/SBFTReferenceCommittee.cpp
        src/SBFT/SBFTReferenceCommittee.hpp
        src/SBFT/syncBFT_Committee.cpp
        src/SBFT/syncBFT_Committee.hpp
        src/SBFT/syncBFT_Peer.cpp
        src/SBFT/syncBFT_Peer.hpp

        src/ExamplePeer.cpp
        src/ExamplePeer.hpp
        src/main.cpp
        src/SmartShards/params_SmartShards.h
        src/params_common.h)

add_executable(TESTS
        src/bCoin/bCoin_Committee.cpp
        src/bCoin/bCoin_Committee.hpp
        src/bCoin/bCoin_Peer.cpp
        src/bCoin/bCoin_Peer.hpp
        src/bCoin/bCoinReferenceCommittee.cpp
        src/bCoin/bCoinReferenceCommittee.hpp
        src/bCoin/DS_bCoin_Peer.cpp
        src/bCoin/DS_bCoin_Peer.hpp

        src/Common/Block.cpp
        src/Common/Block.hpp
        src/Common/Blockchain.cpp
        src/Common/Blockchain.hpp
        src/Common/ByzantineNetwork.hpp
        src/Common/committee.hpp
        src/Common/DAG.hpp
        src/Common/DAGBlock.cpp
        src/Common/DAGBlock.hpp
        src/Common/hash.hpp
        src/Common/Logger.cpp
        src/Common/Logger.hpp
        src/Common/Network.hpp
        src/Common/Packet.hpp
        src/Common/Peer.hpp

        src/BlockGuard/Experiments/metrics.cpp
        src/BlockGuard/Experiments/metrics.hpp
        src/BlockGuard/Experiments/refComExperiments.cpp
        src/BlockGuard/Experiments/refComExperiments.hpp
        src/BlockGuard/Experiments/Sharded_PBFT_Experiments.cpp
        src/BlockGuard/Experiments/Sharded_PBFT_Experiments.hpp
        src/BlockGuard/Experiments/Sharded_POW_Experiments.cpp
        src/BlockGuard/Experiments/Sharded_POW_Experiments.hpp
        src/BlockGuard/Experiments/Sharded_SBFT_Experiments.cpp
        src/BlockGuard/Experiments/Sharded_SBFT_Experiments.hpp
        src/BlockGuard/params_Blockguard.hpp

        src/SmartShards/Experiments/SmartShards_Experiments.cpp
        src/SmartShards/Experiments/SmartShards_Experiments.h
        src/SmartShards/SmartShard.hpp
        src/SmartShards/SmartShard.cpp

        src/PBFT/DS_PBFT.hpp
        src/PBFT/DS_PBFT_Peer.hpp
        src/PBFT/PBFT_Committee.hpp
        src/PBFT/PBFT_Peer.cpp
        src/PBFT/PBFT_Peer.hpp
        src/PBFT/PBFTPeer_Sharded.cpp
        src/PBFT/PBFTPeer_Sharded.hpp
        src/PBFT/PBFTReferenceCommittee.cpp
        src/PBFT/PBFTReferenceCommittee.hpp
        src/PBFT/SmartShardPBFT_peer.cpp
        src/PBFT/SmartShardPBFT_peer.hpp

        src/SBFT/SBFTReferenceCommittee.cpp
        src/SBFT/SBFTReferenceCommittee.hpp
        src/SBFT/syncBFT_Committee.cpp
        src/SBFT/syncBFT_Committee.hpp
        src/SBFT/syncBFT_Peer.cpp
        src/SBFT/syncBFT_Peer.hpp

        Tests/Common/ByzantineNetwork_Test.cpp
        Tests/Common/ByzantineNetwork_Test.hpp
        Tests/Common/NetworkTests.cpp
        Tests/Common/NetworkTests.hpp

        Tests/PBFT/PBFTPeer_Sharded_Test.cpp
        Tests/PBFT/PBFTPeer_Sharded_Test.hpp
        Tests/PBFT/PBFTPeerTest.cpp
        Tests/PBFT/PBFTPeerTest.hpp
        Tests/PBFT/PBFTReferenceCommittee_Test.cpp
        Tests/PBFT/PBFTReferenceCommittee_Test.hpp

        src/ExamplePeer.cpp
        src/ExamplePeer.hpp
        Tests/main.cpp
        Tests/SmartShards/SmartShards_Test.cpp Tests/SmartShards/SmartShards_Test.h)
