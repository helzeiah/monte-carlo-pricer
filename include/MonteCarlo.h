#ifndef MONTECARLO_H
#define MONTECARLO_H
#include "Option.h"

class MonteCarlo {
  Option option;
  long numSimulations;
  //random generator

public:
  MonteCarlo(Option option, long numSimulations);
  MonteCarlo(Option option, long numSimulations, double seed);

  double getPayoff();


};

#endif //MONTECARLO_H
