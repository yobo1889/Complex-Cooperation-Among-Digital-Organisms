#ifndef WORLD_H
#define WORLD_H

#include "emp/Evolve/World.hpp"
#include "Task.h"
#include "Org.h"
#include <stdlib.h>
#include <cstdlib>
#include "emp/data/DataFile.hpp"

class OrgWorld : public emp::World<Organism>
{
  emp::Random &random;
  std::vector<emp::WorldPosition> reproduce_queue;
  std::vector<Task *> tasks{new Task_1()};
  std::vector<std::string> task_name;
  int leader; // leader holder
  int cycle = 0;
  int highestId = 99; // highest ID in the world, purposefully assigned
  int in[4] = {2,2,2,2}; // fixed inputs for Tasks instructions
  

public:
  float reward;
  int count;
  std::string kill_selfish;
  double highestIdCount = 0; // holds the # of times highest ID in the world has been sent
  double nonHighestIdCount = 1; // holds # of times any other ID has been sent (set to 1 to avoid division of 0)
  int sequentialIDSentCount = 0; /// holds # of times sequential ID has been sent
  int realIDsSent = 0; // holds # of times realIDs have been sent
  int totalMessagesCounter = 0; //holds total # of messages sent
  int orgCount = 0; // Organism counter
  emp::Ptr<emp::DataMonitor<int>> data_node_totalMsgsCount; // data node of total messages sent
  emp::Ptr<emp::DataMonitor<int>> data_node_totalSelfIDsSent; // data node of total sequential IDs sent
  emp::Ptr<emp::DataMonitor<int>> data_node_totalRealIDsSent; // data node of total real IDs sent
  emp::Ptr<emp::DataMonitor<int>> data_node_totalHighestIDsSent; // data node of # of times highest ID in the world (ID 99) has been sent
  OrgWorld(emp::Random &_random) : emp::World<Organism>(_random), random(_random) {}

  ~OrgWorld() { // deletor of data nodes
    if(data_node_totalMsgsCount) data_node_totalMsgsCount.Delete();
    if(data_node_totalSelfIDsSent) data_node_totalSelfIDsSent.Delete();
    if(data_node_totalRealIDsSent) data_node_totalRealIDsSent.Delete();
    if(data_node_totalHighestIDsSent) data_node_totalHighestIDsSent.Delete();
    } 

  const pop_t &GetPopulation() { return pop; }

  /*
    Getter of total messages data node
  */
  emp::DataMonitor<int>& GetTotalMsgsCountDataNode() {
    if(!data_node_totalMsgsCount) {
    data_node_totalMsgsCount.New();
    OnUpdate([this](size_t){
      data_node_totalMsgsCount -> Reset();
      for (size_t i = 0; i < pop.size(); i++) {
        if(IsOccupied(i))
        data_node_totalMsgsCount->AddDatum(totalMessagesCounter); // 1 message sent per organism per update
      }
    });
  }
  return *data_node_totalMsgsCount;
  }

    /*
    Getter of total self IDs sent data node
  */
  emp::DataMonitor<int>& GetTotalSelfIDsSent() {
    if(!data_node_totalSelfIDsSent) {
    data_node_totalSelfIDsSent.New();
    OnUpdate([this](size_t){
      data_node_totalSelfIDsSent -> Reset();
      for (size_t i = 0; i < pop.size(); i++) {
        if(IsOccupied(i))
        data_node_totalSelfIDsSent->AddDatum(sequentialIDSentCount); //
      }
    });
  }
  return *data_node_totalSelfIDsSent;
  }


  /*
    Getter of total real IDs sent data node
  */
  emp::DataMonitor<int>& GetTotalRealIDsSent() {
    if(!data_node_totalRealIDsSent) {
    data_node_totalRealIDsSent.New();
    OnUpdate([this](size_t){
      data_node_totalRealIDsSent -> Reset();
      for (size_t i = 0; i < pop.size(); i++) {
        if(IsOccupied(i))
        data_node_totalRealIDsSent->AddDatum(realIDsSent); //
      }
    });
  }
  return *data_node_totalRealIDsSent;
  }


  /*
    Getter of total real IDs sent data node
  */
  emp::DataMonitor<int>& GetTotalHighestIDsSent() {
    if(!data_node_totalHighestIDsSent) {
    data_node_totalHighestIDsSent.New();
    OnUpdate([this](size_t){
      data_node_totalHighestIDsSent -> Reset();
      for (size_t i = 0; i < pop.size(); i++) {
        if(IsOccupied(i))
        data_node_totalHighestIDsSent->AddDatum(highestIdCount); //
      }
    });
  }
  return *data_node_totalHighestIDsSent;
  }

  /*
    File setter for data nodes
  */
  emp::DataFile & SetupAllSends(const std::string & filename) {
    auto & file = SetupFile(filename);
    auto & node1 = GetTotalMsgsCountDataNode(); // total # of messages sent
    auto & node2 = GetTotalSelfIDsSent(); // total # of self-IDs sent
    auto & node3 = GetTotalRealIDsSent(); // total # of real IDs sent
    auto & node4 = GetTotalHighestIDsSent(); // total # of highest ID having been sent


    file.AddVar(update, "update", "Update");
    file.AddTotal(node1, "Total Number of Messages Count", "Total number of messages");
    file.AddTotal(node2, "Total Number of Self IDs sent Count", "Total number of Self IDs sent");
    file.AddTotal(node3, "Total Number of Real Ids sent Count", "Total number of real IDs sent");
    file.AddTotal(node4, "Total Number of Highest IDs sent Count", "Total number of Highest IDs sent");


    file.PrintHeaderKeys();

    return file;
  }

  void Update(double mutationPercent)
  {

    emp::World<Organism>::Update();

    // Process each organism
    emp::vector<size_t> schedule = emp::GetPermutation(random, GetSize());
    for (int i : schedule)
    {
      if (!IsOccupied(i))
      {
        continue;
      }
      pop[i]->Process(i);
      orgCount++;
    }
    // Time to allow reproduction for any organisms that ran the reproduce instruction
    for (emp::WorldPosition location : reproduce_queue)
    {
      if (!IsOccupied(location))
      {
        return;
      }
      std::optional<Organism> offspring =
          pop[location.GetIndex()]->CheckReproduction(mutationPercent);
      if (offspring.has_value())
      {
        DoBirth(offspring.value(), location.GetIndex());
      }
    }
    reproduce_queue.clear();
  }



  // Check whether the task has been complemted by the organism and assing points based on the result
  void CheckOutput(float output, OrgState &state)
  {
    for (Task *task : tasks)
    {
      emp::Ptr<Organism> currentOrg;
      double out = task->CheckOutput(output, in);
      srand(out);
      int sent = rand() % 100; // modulo created to limit the IDs to a certain treshold
      currentOrg = initiateMsg(state.Seq_ID, sent);
      if (out == 0)
      {
        continue;
      }
      if (!currentOrg)
      {
        continue;
      }
      if (sent != highestId) // if message sent is not the highest ID
      {
        nonHighestIdCount++;
        totalMessagesCounter++;
      }
      else
      {
        highestIdCount++;
        totalMessagesCounter++;
      }

      std::cout << "Percent: " << GetPercent() << " - Messages that are not the highest ID " << nonHighestIdCount << " - Messages that are the highest ID " << highestIdCount << std::endl; // print statement for native.cpp
      if (sent == currentOrg->GetSeqId()) // if message sent is the sequential ID, reward
      {
        state.points += reward;
        sequentialIDSentCount++;
      }
      if (sent == currentOrg->GetHighestID()) // if message sent is the highest ID in the inbox, reward more
      {
        state.points+= reward+1;
      }
      if (sent == currentOrg->GetMaxId()) // if message sent is the set highest ID in the world (99), reward even more
      {
        state.points += reward + 2;
      }
      if (sent > orgCount) { // if it's a nonexistent ID (which could be 2500 given dimensions), punish
        state.points -= reward;
      }
      if (sent < orgCount) { // a real ID has been sent
        realIDsSent++;
      } 
      // if(kill_selfish=="1"){
      //   killSelfish(currentOrg);
      // }
    }
  }

  /*
    Returns percentage of highest ID in organisms' inboxes
  */
  double GetPercent(){
  
     double percent = (highestIdCount / nonHighestIdCount) * 100;
     return percent;

  }

  /*
    Method used by organisms to send messages in the world
  */
  emp::Ptr<Organism> initiateMsg(int org, int msg)
  {

    emp::Ptr<Organism> sender = getOrgByID(org);
    int neighbour = getFacing(sender->GetPos());
    if (IsOccupied(neighbour))
    {
      emp::Ptr<Organism> reciever = &GetOrg(neighbour);
      sender->SendMsg(msg, reciever);
      return sender;
    }
    return NULL;
  }


  /*
    Method that returns a specific organism given a sequential ID
  */
  emp::Ptr<Organism> getOrgByID(int id)
  {
    emp::vector<size_t> schedule = emp::GetPermutation(random, GetSize());
    for (int i : schedule)
    {
      if (!IsOccupied(i))
      {
        continue;
      }
      if (pop[i]->GetSeqId() == id)
      {
        pop[i]->SetPos(i);
        return pop[i];
      }
    }
    return NULL;
  }

  /*
    Method that applies the GetRandomNeighbor of organims and determines the facing of messages on our floor
  */
  int getFacing(int pos)
  {
    emp::WorldPosition new_position = GetRandomNeighborPos(pos);
    return new_position.GetIndex();
  }

  /*
    Method that eliminates organisms that send their own IDs more than 80 times
  */
  void killSelfish(emp::Ptr<Organism> organism){
    std::cout << "selfish-count: " << organism->GetSendSelfId() << std::endl;
    if(organism->GetSendSelfId()>80 && organism->GetSeqId() != 99){ // making sure that we do not eliminate the leader
      organism=nullptr;
    }
  }

  /*
    Method to determine if a leader has been elected
  */
  bool isLeader()
  {
    int percentageMax = highestIdCount / nonHighestIdCount;
    if (percentageMax >= 0.95)
    {
      return true;
    }
    return false;
  }

  void ReproduceOrg(emp::WorldPosition location)
  {
    // Wait until after all organisms have been processed to perform
    // reproduction. If reproduction happened immediately then the child could
    // ovewrite the parent, and then we would be running the code of a deleted
    // organism
    reproduce_queue.push_back(location);
  }
};

#endif