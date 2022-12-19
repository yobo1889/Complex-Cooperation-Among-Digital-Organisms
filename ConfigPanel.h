#include "emp/config/ArgManager.hpp"
EMP_BUILD_CONFIG(MyConfigType,
VALUE(SEED, int, 7, "What value should the random seed be?"), 
VALUE(Population, int, 70, "The number of starting organism in the world"),
VALUE(Reward, float, 0.1, "Points given for solving each task"),
VALUE(MUTATION_RATE_NUM, double, 0.02, "What is the percent probability that each bit in the binary representation of the offprings' genome is going to be flipped?")
)