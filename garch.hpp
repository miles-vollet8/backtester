#ifndef GARCH_HPP
#define GARCH_HPP

#include <vector>

// Public-facing declarations
double garch_var(const std::vector<double>& returns, double omega, double alpha, double beta);
double log_likelihood(const std::vector<double>& returns, const std::vector<double> x);
double optimize_params(const std::vector<double>& returns, std::vector<double>& x);
std::vector<double> calculated_sigma2(std::vector<double>& returns, std::vector<double> x);
double forecast_iv(std::vector<double> sigmas, std::vector<double> x, int h);

#endif