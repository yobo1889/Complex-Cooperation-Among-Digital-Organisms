
#include <cmath>
#include <string>

class Task {
public:
  virtual double CheckOutput(int output, int inputs[4]) = 0;
  virtual std::string name() = 0;
};


class Task_1 : public Task {

   public:
     double CheckOutput(int output, int inputs[4]) override {  // OrgState edited for organism to always return output, referring to an index in their inbox
          return output;
     }
     public: 
     //returns the name of the task solved
     std::string name() override{
        std::string n = "Id_Check";
        return n;
     }

};


