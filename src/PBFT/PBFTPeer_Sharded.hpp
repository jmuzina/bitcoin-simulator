//
//  BlockGuardSharded.hpp
//  src
//
//  Created by Kendric Hood on 3/28/19.
//  Copyright © 2019 Kent State University. All rights reserved.
//

#ifndef PBFTPeer_Sharded_hpp
#define PBFTPeer_Sharded_hpp

#include <stdio.h>
#include "PBFT_Peer.hpp"

class PBFTPeer_Sharded : public PBFT_Peer{
protected:
    
    // state vars
    int                                                        _groupId;
    int                                                        _committeeId;
    std::map<std::string, Peer<PBFT_Message>* >                _groupMembers;
    std::map<std::string, Peer<PBFT_Message>* >                _committeeMembers;
    std::vector<int>                                           _committeeSizes;
    
    // logging vars
    bool                                                       _printCommittee;
    bool                                                       _printGroup;

    // methods from PBFT peer that need to be adjusted for sharding
    void                        commitRequest           () override;
    void                        braodcast               (const PBFT_Message&) override; 
    
public:
    PBFTPeer_Sharded                                    (std::string);
    PBFTPeer_Sharded                                    (const PBFTPeer_Sharded&);
    ~PBFTPeer_Sharded                                   ()                                              {};
    
    //setters
    void                        setGroup                (int id)                                        {clearGroup();_groupId = id;};
    void                        setCommittee            (int id)                                        {clearCommittee();_committeeId = id;};
    void                        addGroupMember          (PBFTPeer_Sharded &newMember)                   {_groupMembers[newMember.id()] = &newMember;};
    void                        addCommitteeMember      (PBFTPeer_Sharded &newMember)                   {_committeeMembers[newMember.id()] = &newMember;};
   
    void                        printGroupOn            ()                                              {_printGroup = true;};
    void                        printGroupOff           ()                                              {_printGroup = false;};
    void                        printCommitteeOn        ()                                              {_printCommittee = true;};
    void                        printCommitteeOff       ()                                              {_printCommittee = false;};

    // mutators
    void                        clearCommittee          ()                                              {_committeeMembers.clear(); _committeeId = -1; _currentView = 0;}
    void                        clearGroup              ()                                              {_groupMembers.clear(); _groupId = -1;}
    void                        initPrimary             () override                                     {_primary = findPrimary(_committeeMembers);};
    void                        preformComputation      () override;
    
    // getters
    int                         faultyPeers             ()const override                                {return ceil(double(_committeeMembers.size() + 1) * _faultUpperBound);};
    int                         getGroup                ()const                                         {return _groupId;};
    int                         getCommittee            ()const                                         {return _committeeId;};
    std::vector<std::string>    getGroupMembers         ()const;
    std::vector<std::string>    getCommitteeMembers     ()const;
    std::vector<int>            getCommitteeSizes       ()const                                         {return _committeeSizes;};
    
    std::ostream&               printTo                 (std::ostream&)const;
    void                        log                     ()const                                         {printTo(*_log);};
    
    PBFTPeer_Sharded&           operator=               (const PBFTPeer_Sharded&);
    friend std::ostream&        operator<<              (std::ostream &o, const PBFTPeer_Sharded &p)    {p.printTo(o); return o;};
};

#endif /* PBFTPeer_Sharded_hpp */
