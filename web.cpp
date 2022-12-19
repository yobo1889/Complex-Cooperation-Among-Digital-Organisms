
//in web.cpp at the VERY top
#define UIT_VENDORIZE_EMP
#define UIT_SUPPRESS_MACRO_INSEEP_WARNINGS
#include "emp/math/Random.hpp"
#include "emp/web/Animate.hpp"
#include "emp/web/web.hpp"
#include "World.h"
#include "Org.h"
#include "emp/config/ArgManager.hpp"
#include "emp/prefab/ConfigPanel.hpp"
#include "emp/web/UrlParams.hpp"
#include "ConfigPanel.h"
#include "emp/games/Othello8.hpp"
#include <string>



MyConfigType config;
emp::web::Document doc{"target"};
emp::web::Document settings("settings");


class Animator : public emp::web::Animate {
    const double num_h_boxes  = 50;
    const double num_w_boxes = 50;
    const double RECT_SIDE = 10;
    const double width{num_w_boxes * RECT_SIDE};
    const double height{num_h_boxes * RECT_SIDE};
    bool toggleEntered = false; // GUI expansion, lets user know what the red organisms mean
    emp::web::Canvas canvas{width, height, "canvas"};
    emp::Random random{config.SEED()};
    OrgWorld world{random};



public:

//Constructor for the class that sets up the GUI for the animation
Animator() {  

       
        auto specs = emp::ArgManager::make_builtin_specs(&config);
        emp::ArgManager am(emp::web::GetUrlParams(), specs);
        am.UseCallbacks();
        if (am.HasUnused()) std::exit(EXIT_FAILURE);
        emp::prefab::ConfigPanel config_panel(config);
        config_panel.SetRange("Population", "70", "200");
        config_panel.SetRange("MUTATION_RATE_NUM","0.02","1");

        settings << config_panel;

        doc << canvas;
        doc << GetToggleButton("Toggle");
        doc << GetStepButton("Step");
        random.ResetSeed(config.SEED());
       
        world.reward = config.Reward();
        world.SetPopStruct_Grid(num_w_boxes, num_h_boxes);
        for(int i = 0;i<config.Population();i++){
         Organism* new_org1 = new Organism(&world,i, 1);
         world.Inject(*new_org1);
        }
        world.Resize(num_w_boxes, num_h_boxes);        
        emp::prefab::ToggleSwitch kill_selfish{
         [&](std::string val){
            world.kill_selfish = val;
         },
         "Kill Selfish Organisms",
         false
         };
         doc << "<br>"; // line break
         doc << "<br>";
         doc << kill_selfish;
}

void DoFrame() override {
        canvas.Clear();
        world.Update(config.MUTATION_RATE_NUM());
        int org_num = 0;
        for (int x = 0; x < num_h_boxes;x++ ){
              for (int y = 0; y < num_w_boxes;y++){
                if (world.IsOccupied(org_num)) {
                    Organism organism = world.GetOrg(org_num);
                      canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "purple", "black");
                      if(organism.GetSeqId()==organism.GetMaxId()){
                        canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "red", "black");
                        if (!toggleEntered) {
                            doc << "<br>";
                            doc << "The organisms that you now see in red are sending the highest ID in the world, woo!!";
                            toggleEntered = true;
                        }
                      }
                }
                else {
                    canvas.Rect(x * RECT_SIDE, y * RECT_SIDE, RECT_SIDE, RECT_SIDE, "white", "black");
                }
                org_num++;
            }
        }
    }

};

Animator animator;

int main() {
    animator.Step();
}





