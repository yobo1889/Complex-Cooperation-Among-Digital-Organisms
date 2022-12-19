#include <iostream>
#include <string>
#include <sys/_types/_size_t.h>
#include "Org.h"
#include "emp/base/vector.hpp"
#include "emp/math/random_utils.hpp"
#include "emp/math/Random.hpp"
#include "emp/config/ArgManager.hpp"
#include "emp/prefab/ConfigPanel.hpp"
#include "emp/web/UrlParams.hpp"
#include "ConfigPanel.h"
#include "World.h"

// This is the main function for the NATIVE version of this project.

int main(int argc, char *argv[])
{
  emp::Random random(7); // Random inputted
  OrgWorld world {random};
  world.reward = 1;
  for(int i = 0; i < 70; i++) { // Starting with 70 organisms in the world
    Organism* new_org = new Organism(&world, i, 1);
    world.Inject(*new_org);
  }
  world.Resize(10,10);
  world.kill_selfish = false; //toggled kill_selfish to true for plotted results. 
  world.SetupAllSends("AllMsgsSent_Ext.csv").SetTimingRepeat(100); // creating file for all nodes needed for data plot
  for (int update=0; update<100000; update++) { // 100000 updates being performed for data plot, attempt to replicate paper's results
    world.Update(0.02);
  }
}