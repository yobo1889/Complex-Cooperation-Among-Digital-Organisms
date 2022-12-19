#ifndef ORG_H
#define ORG_H

#include "CPU.h"
#include "OrgState.h"
#include "emp/Evolve/World_structure.hpp"
#include <iostream>
#include <list>

class Organism {

  CPU cpu;
  std::vector<int> inbox = {}; // inbox where messages will be received and sent from
  int offspring_id = 20; // variable to hold the IDs of the organisms' offsprings
  int org_pos; // position of organisms

public:
  Organism(OrgWorld *world, int _seqID, double points = 0.0) : cpu(world) {
    SetPoints(points);
    SetSeqID(_seqID); // seqID, or sequential ID, is the organism's ID in the world
    

  }
  
  void SetPoints(double _in) { cpu.state.points = _in; }
  void AddPoints(double _in) { cpu.state.points += _in; }
  double GetPoints() {return cpu.state.points;}
  bool GetSolved() {return cpu.state.solved;}

  void Reset() {
    cpu.Reset();
  }

  void Mutate(double mutationPercent) {
    cpu.Mutate(mutationPercent);
  }

  /*
    Method created to check every organism at each update and see if they shall reproduce
  */
  std::optional<Organism> CheckReproduction(double mutationPercent) {
    
    if (GetPoints() > 20) {
      Organism offspring = *this;
      offspring.Reset();
      offspring.SetSeqID(offspring_id);
      offspring_id++;
      offspring.Mutate(mutationPercent);
      AddPoints(-20); // reverting offspring's point back to 0
      return offspring;
    }
    return {};
  }

  void Process(emp::WorldPosition current_location) {
    cpu.state.current_location = current_location;
    cpu.RunCPUStep(30);
  }

  void PrintGenome() {
    std::cout << "program ------------" << std::endl;
    cpu.PrintGenome();
    std::cout << "end ---------------" << std::endl;
  }

  /* 
    Method created to get the maximum ID in organism's inbox
  */
  int GetMaxId(){
    return 99;
  }

  int GetHighestID () {
  int maxId = 0;
    for(int i = 0;i<inbox.size();i++){
      if(inbox[i]>maxId){
        maxId = inbox[i];
      }
    }
  return maxId;
  }

  /*
    Method created to retrieve the sequential ID of an organism
  */
  int GetSeqId(){
    return cpu.state.Seq_ID;
  }

  /*
    Method created to set the sequential ID of an organism
  */
  void SetSeqID(int Id){
    cpu.state.Seq_ID = Id;
  }

  /*
    Method created to get the position of an organism
  */
  int GetPos(){
    return org_pos;
  }

  /*
    Method created to set the position of an organism
  */
  void SetPos(int pos){
    org_pos = pos;
  }

  /*
    Method created to get the number of times an organisms sends their own ID
  */
  int GetSendSelfId(){
    return cpu.state.send_self_id_count;
  }

  /*
    Setter of above method
  */
  void SetSendSelfId(int count){
    count = cpu.state.send_self_id_count;
  }

  /*
    Method for an organims to send a message to another organism's inbox
  */
  void SendMsg(int msg, emp::Ptr<Organism> destination){
    destination->RecMsg(msg);
    
  }

  /*
    Method for an organism to receive a message
  */
  void RecMsg(int msg){
    inbox.push_back(msg);
    
  }

  /*
    Method that gets an organim's inbox
  */
  int GetInbox(int index){
   return inbox[index];
  }
};

#endif