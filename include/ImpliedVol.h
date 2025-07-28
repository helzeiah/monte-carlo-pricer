#ifndef IMPLIEDVOL_H
#define IMPLIEDVOL_H

#include "Option.h"

double impliedVol(const Option& option, double targetPrice,
  double tol = 1e-8, int maxIter = 50);

#endif //IMPLIEDVOL_H
