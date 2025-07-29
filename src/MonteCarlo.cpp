#include "MonteCarlo.h"

#include <algorithm>
#include <cmath>
#include <ranges>
#include <stdexcept>

MonteCarlo::MonteCarlo(const Option& option, unsigned long numSimulations,
                       unsigned int seed)
    : Pricer{option},
      numSimulations{numSimulations},
      randomEngine{seed},
      standardNormal{0.0, 1.0},
      stockPrice{option.getStockPrice()},
      // pre-calc drift: (r - q - 0.5 σ²) * T
      driftPerSim{(option.getRiskFreeRate() - option.getDividendYield() -
                   0.5 * option.getVolatility() * option.getVolatility()) *
                  option.getTimeToMaturity()},
      // pre-calc vol * sqrt(T)
      volTimesSqrtT{option.getVolatility() *
                    std::sqrt(option.getTimeToMaturity())} {
  if (numSimulations == 0) {
    throw std::invalid_argument("Number of simulations must be positive.");
  }
  payoffs.reserve(numSimulations);
  normals.reserve(numSimulations);
}

// delegate ctor
MonteCarlo::MonteCarlo(const Option& option, unsigned long numSimulations)
    : MonteCarlo(option, numSimulations, std::random_device{}()) {}

unsigned long MonteCarlo::getNumSimulations() const { return numSimulations; }

double MonteCarlo::calculatePrice() const {
  if (priceCalculated) {
    return cachedPrice;
  }

  const auto start{std::chrono::high_resolution_clock::now()};

  const double discountFactor{
      std::exp(-option.getRiskFreeRate() * option.getTimeToMaturity())};

  double sumPayoffs{0.0};
  normals.resize(numSimulations);
  payoffs.resize(numSimulations);

  for (unsigned long i{0}; i < numSimulations; ++i) {
    const double z{standardNormal(randomEngine)};
    normals[i] = z;

    const double ST{stockPrice * std::exp(driftPerSim + volTimesSqrtT * z)};
    const double payoff{option.calculatePayoff(ST)};
    payoffs[i] = payoff;
    sumPayoffs += payoff;
  }

  cachedPrice =
      (sumPayoffs / static_cast<double>(numSimulations)) * discountFactor;
  priceCalculated = true;
  lastRunDuration = std::chrono::high_resolution_clock::now() - start;

  return cachedPrice;
}

std::string MonteCarlo::getPricingMethod() const {
  return "Monte Carlo";
}

Greeks MonteCarlo::calculateGreeks() {
  calculatePrice();  // ensure normals/payoffs are filled

  const double S0{option.getStockPrice()};
  const double T{option.getTimeToMaturity()};
  const double r{option.getRiskFreeRate()};
  const double q{option.getDividendYield()};
  const double sig{option.getVolatility()};

  if (T <= 1e-12 || sig <= 1e-12) return Greeks{};

  // bump sizes
  const double epsS{std::max(S0 * 1e-4, 1e-6)};
  const double epsSig{std::max(sig * 1e-3, 1e-4)};
  const double epsR{std::max(std::abs(r) * 1e-3, 1e-5)};
  const double epsT{std::max(T * 1e-3, 1e-5)};

  // times for theta
  const double Tdn{std::max(T - epsT, 1e-8)};
  const double Tup{T + epsT};

  // base stuff
  const double sqrtT{std::sqrt(T)};
  const double drift{(r - q - 0.5 * sig * sig) * T};
  const double volBase{sig * sqrtT};
  const double disc{std::exp(-r * T)};

  auto disc_rT = [](double rr, double TT) { return std::exp(-rr * TT); };

  // bumps (S)
  const double S_up{S0 + epsS};
  const double S_dn{std::max(S0 - epsS, 1e-8)};

  // bumps (σ)
  const double sigUp{sig + epsSig};
  const double sigDn{std::max(sig - epsSig, 1e-12)};
  const double volUpSig{sigUp * sqrtT};
  const double volDnSig{sigDn * sqrtT};
  const double driftUpSig{(r - q - 0.5 * sigUp * sigUp) * T};
  const double driftDnSig{(r - q - 0.5 * sigDn * sigDn) * T};

  // bumps (r)
  const double rUp{r + epsR};
  const double rDn{r - epsR};
  const double driftUpR{(rUp - q - 0.5 * sig * sig) * T};
  const double driftDnR{(rDn - q - 0.5 * sig * sig) * T};

  // bumps (T)
  const double sqrtTdn{std::sqrt(Tdn)};
  const double sqrtTup{std::sqrt(Tup)};
  const double driftTdn{(r - q - 0.5 * sig * sig) * Tdn};
  const double driftTup{(r - q - 0.5 * sig * sig) * Tup};
  const double volTdn{sig * sqrtTdn};
  const double volTup{sig * sqrtTup};

  // accumulators
  double sumBase{0}, sumSup{0}, sumSdn{0};
  double sumSigUp{0}, sumSigDn{0};
  double sumRup{0}, sumRdn{0};
  double sumTdn{0}, sumTup{0};

  for (unsigned long i = 0; i < numSimulations; ++i) {
    const double z = normals[i];

    const double ST = S0 * std::exp(drift + volBase * z);
    const double ST_Su = S_up * std::exp(drift + volBase * z);
    const double ST_Sd = S_dn * std::exp(drift + volBase * z);

    const double ST_sigU = S0 * std::exp(driftUpSig + volUpSig * z);
    const double ST_sigD = S0 * std::exp(driftDnSig + volDnSig * z);

    const double ST_rU = S0 * std::exp(driftUpR + volBase * z);
    const double ST_rD = S0 * std::exp(driftDnR + volBase * z);

    const double ST_Td = S0 * std::exp(driftTdn + volTdn * z);
    const double ST_Tu = S0 * std::exp(driftTup + volTup * z);

    sumBase += option.calculatePayoff(ST);
    sumSup += option.calculatePayoff(ST_Su);
    sumSdn += option.calculatePayoff(ST_Sd);
    sumSigUp += option.calculatePayoff(ST_sigU);
    sumSigDn += option.calculatePayoff(ST_sigD);
    sumRup += option.calculatePayoff(ST_rU);
    sumRdn += option.calculatePayoff(ST_rD);
    sumTdn += option.calculatePayoff(ST_Td);
    sumTup += option.calculatePayoff(ST_Tu);
  }

  const double invN{1.0 / static_cast<double>(numSimulations)};
  const double priceB{sumBase * disc * invN};
  const double priceSu{sumSup * disc * invN};
  const double priceSd{sumSdn * disc * invN};
  const double priceSigU{sumSigUp * disc * invN};
  const double priceSigD{sumSigDn * disc * invN};

  const double discUpR{disc_rT(rUp, T)};
  const double discDnR{disc_rT(rDn, T)};
  const double priceRu{sumRup * discUpR * invN};
  const double priceRd{sumRdn * discDnR * invN};

  const double discTd{disc_rT(r, Tdn)};
  const double discTu{disc_rT(r, Tup)};
  const double priceTd{sumTdn * discTd * invN};
  const double priceTu{sumTup * discTu * invN};

  const double delta = (priceSu - priceSd) / (2.0 * epsS);
  const double gamma = (priceSu - 2.0 * priceB + priceSd) / (epsS * epsS);
  const double vega = (priceSigU - priceSigD) / (2.0 * epsSig);
  const double rho = (priceRu - priceRd) / (2.0 * epsR);
  const double theta = (priceTd - priceTu) / (2.0 * epsT);  // matches BS sign

  return Greeks{delta, gamma, theta, vega, rho};
}

std::pair<double, double> MonteCarlo::getConfidenceInterval(
    double confidenceLevel) {
  validatePriceCalculated();

  if (confidenceLevel <= 0.0 || confidenceLevel >= 1.0) {
    throw std::invalid_argument("Confidence level must be between 0 and 1");
  }

  double zScore{};
  if (confidenceLevel >= 0.99)
    zScore = 2.576;
  else if (confidenceLevel >= 0.95)
    zScore = 1.96;
  else if (confidenceLevel >= 0.90)
    zScore = 1.645;
  else
    zScore = 1.282;

  const double marginOfError{zScore * getStandardError()};
  return {cachedPrice - marginOfError, cachedPrice + marginOfError};
}

double MonteCarlo::getStandardError() {
  validatePriceCalculated();

  double mean{0.0};
  for (double p : payoffs) {
    mean += p;
  }
  mean /= static_cast<double>(payoffs.size());

  double variance{0.0};
  for (double p : payoffs) {
    const double diff{p - mean};
    variance += diff * diff;
  }
  variance /= static_cast<double>(payoffs.size() - 1);

  const double disc{
      std::exp(-option.getRiskFreeRate() * option.getTimeToMaturity())};
  return disc * std::sqrt(variance / static_cast<double>(payoffs.size()));
}

double MonteCarlo::calculateVaR(double confidenceLevel) {
  validatePriceCalculated();
  if (confidenceLevel <= 0.0 || confidenceLevel >= 1.0) {
    throw std::invalid_argument("confidenceLevel must be in (0,1)");
  }

  std::vector<double> sortedPayoffs{payoffs};
  const std::size_t idx{
      static_cast<std::size_t>(confidenceLevel * sortedPayoffs.size())};

  std::ranges::nth_element(sortedPayoffs, sortedPayoffs.begin() + idx);
  return sortedPayoffs[idx];
}

double MonteCarlo::runMoreSimulations(unsigned long additionalSimulations) {
  validatePriceCalculated();

  const unsigned long oldNum{numSimulations};
  const double discountFactor{
      std::exp(-option.getRiskFreeRate() * option.getTimeToMaturity())};

  const double oldSumUndisc{cachedPrice * oldNum / discountFactor};

  numSimulations += additionalSimulations;

  const auto start{std::chrono::high_resolution_clock::now()};

  payoffs.resize(numSimulations);
  normals.resize(numSimulations);

  double newSumUndisc{0.0};
  for (unsigned long i{oldNum}; i < numSimulations; ++i) {
    const double z{standardNormal(randomEngine)};
    normals[i] = z;
    const double ST{stockPrice * std::exp(driftPerSim + volTimesSqrtT * z)};
    const double payoff{option.calculatePayoff(ST)};
    payoffs[i] = payoff;
    newSumUndisc += payoff;
  }

  cachedPrice =
      ((oldSumUndisc + newSumUndisc) / static_cast<double>(numSimulations)) *
      discountFactor;

  lastRunDuration = std::chrono::high_resolution_clock::now() - start;
  return cachedPrice;
}

void MonteCarlo::validatePriceCalculated() const {
  if (!priceCalculated) {
    throw std::runtime_error(
        "Price must be calculated first. Call calculatePrice()");
  }
}