//
//  ExamplePeer.cpp
//  src
//
//  Created by Kendric Hood on 3/15/19.
//  Copyright © 2019 Kent State University. All rights reserved.
//

#include "ExamplePeer.hpp"
#include "./Common/Peer.hpp"
#include "./Common/Packet.hpp"
#include <iostream>

//
// Example Peer definitions
//
ExamplePeer::~ExamplePeer(){
    
}

ExamplePeer::ExamplePeer(const ExamplePeer &rhs) : Peer<ExampleMessage>(rhs){
    counter = rhs.counter;
}

ExamplePeer::ExamplePeer(std::string id) : Peer(id){
    counter =0;
}

void ExamplePeer::preformComputation(){
    std::cout<< "Peer:"<< _id<< " preforming computation"<<std::endl;
    
    ExampleMessage message;
    message.message = "Message: " + std::to_string(counter)  + " Hello From ";
    message.aPeerId = _id;
    Packet<ExampleMessage> newMessage(std::to_string(counter), _id,_id);
    newMessage.setBody(message);
    _outStream.push_back(newMessage);
    
    for (auto it = _neighbors.begin(); it != _neighbors.end(); it++ )
    {
        ExampleMessage message;
        message.message = "Message: " + std::to_string(counter)  + " Hello From ";
        message.aPeerId = _id;
        Packet<ExampleMessage> newMessage(std::to_string(counter), it->first,_id);
        newMessage.setBody(message);
        _outStream.push_back(newMessage);
    }
    
    for(int i = 0; i < _inStream.size(); i++){
        std::cout << std::endl << _id<< " has receved a message from "<< _inStream[i].sourceId()<< std::endl;
        std::cout << "  MESSAGE "<< _inStream[i].id() <<  ":"<< _inStream[i].getMessage().message<<  _inStream[i].getMessage().aPeerId<< std::endl;
    }
    std::cout << std::endl;
    _inStream.clear();
    counter++;
}

std::ostream& ExamplePeer::printTo(std::ostream &out)const{
    Peer<ExampleMessage>::printTo(out);
    
    out<< _id<< std::endl;
    out<< "counter:"<< counter<< std::endl;
    
    return out;
}

std::ostream& operator<< (std::ostream &out, const ExamplePeer &peer){
    peer.printTo(out);
    return out;
}
