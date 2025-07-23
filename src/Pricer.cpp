#include "Pricer.h"

#include <iomanip>

std::ostream& operator<<(std::ostream& os, const Pricer& pricer) {
  os << pricer.getPricingMethod() << ":\n";
  os << pricer.getOption().toString();

  if (pricer.isPriceCalculated()) {
    os << "Results:\n"
       << "Price: $" << std::fixed << std::setprecision(2) << pricer.getPrice()
       << "\n"
       << "Calculation time: " << std::fixed << std::setprecision(4)
       << pricer.getLastCalculationTime() << "s\n";
  } else {
    os << "Status:\n"
       << "Price: Not calculated (call calculatePrice())\n";
  }
  return os;
}