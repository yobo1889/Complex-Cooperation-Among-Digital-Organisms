#ifndef ORGSTATE_H
#define ORGSTATE_H

#include "emp/Evolve/World_structure.hpp"
#include <cstddef>

class OrgWorld;

struct OrgState {
  OrgWorld *world;

  bool solved;

  //Array for tracking the four most recent input values
  int last_inputs[4];
  //Index of the most recent input value in array
  size_t last_input_idx;
  //How many points this organism has currently
  double points;
  
  int Seq_ID; // Sequential ID of an organism

  int send_self_id_count = 0; //the number of times an organism send their own id

  //Needs to know current location for possible reproduction
  emp::WorldPosition current_location;

  /**
  * Adds an input number to the tracker
  */
  void add_input(float input) {
    last_inputs[last_input_idx] = input;
    last_input_idx = (last_input_idx + 1) % 4; 
  }
};

#endif