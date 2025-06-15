#include <cstdio>
#include<vector>
#include <iostream>
#include <nlopt.h>
#include <cmath>
#include <nlopt.hpp>

#include "garch.hpp"
const double pi = 3.14159265358979323846;

struct GarchData {
    std::vector<double> returns;
};

double calculate_mean(const std::vector<double>& returns){

    size_t n = returns.size();
    if (n == 0){
        return 0.0f;
    }
    

    
    double sum = 0.0f;

    for (int i=0;i<int(n);i++){
        sum +=returns[i];
        
    }
    double mean = sum/double(n);

    return mean;
}


double calc_variance(const std::vector<double>& returns){
    double mean = calculate_mean(returns);
    double sum = 0.0f;
    size_t n = returns.size();
    double hold;
    for (unsigned int i = 0;i<n;i++){
        hold = returns[i] - mean;
        sum += (hold*hold);
    }
    double var = sum/(double(n) - 1.0f);
    return var;
}



double log_likelihood(const std::vector<double>& returns, const std::vector<double>  x){

    double w = x[0];
    double alpha = x[1];
    double beta = x[2];

    
    size_t n = returns.size();
    double intitial_sigma2 = calc_variance(returns);

    std::vector<double> sigma2(n);
    sigma2[0] = intitial_sigma2;
    double current_sigma2;
    for (unsigned int i = 1;i<n;i++){
        current_sigma2 = w + alpha*(returns[i-1]*returns[i-1]) + beta*(sigma2[i-1]);
        
        sigma2[i] = current_sigma2;
        if (sigma2[i] <= 1e-8 || std::isnan(sigma2[i]) || std::isinf(sigma2[i])) {
            std::cerr << "Bad sigma² at i = " << i << ", value = " << sigma2[i] << "\n";
            

            return 1e10;
        }
    }
    double likelihood = 0.0;
    for (unsigned int i = 0;i<n;i++){
        likelihood += -0.5*(std::log(2*pi)+std::log(sigma2[i]) + ((returns[i]*returns[i])/sigma2[i]));
    }
    return -likelihood;


}

double stationarity_constraint(unsigned n, const double *x, double *grad, void *data) {
    return x[1] - x[2] - 1.0; // x[1] = alpha, x[2] = beta
}


double garch_wrapper(unsigned int n, const double* x, double* grad, void *data){
    GarchData *dat = static_cast<GarchData*>(data);
    const std::vector<double> params(x, x+n);
    return log_likelihood(dat->returns, params);
}

double optimize_params(const std::vector<double>& returns, std::vector<double>& x) {

    nlopt_opt opt = nlopt_create(NLOPT_LN_COBYLA, 3);
    GarchData ctx;
    ctx.returns = returns;     
    nlopt_set_min_objective(opt, garch_wrapper, &ctx);

    std::vector<double> lower_bounds = {1e-8, 1e-3, 1e-3};
    nlopt_set_lower_bounds(opt, lower_bounds.data());

    //std::vector<double> upper_bounds = {1e-4, 0.95, 0.95};
    double returns_var = calc_variance(returns);
    std::vector<double> upper_bounds = {returns_var, 0.999, 0.999};
    nlopt_set_upper_bounds(opt, upper_bounds.data());
    
    nlopt_add_inequality_constraint(opt, stationarity_constraint, nullptr, 1e-4);


//    nlopt_set_maxeval(opt, 200);
    nlopt_set_xtol_rel(opt, 1e-5);
    nlopt_set_ftol_rel(opt, 1e-5);
    double minf;
    

    nlopt_result result = nlopt_optimize(opt, x.data(), &minf);
    if (result <0){
        std::cerr << "NLOpt failed with code" << result << "\n";
    }

    nlopt_destroy(opt);

    return minf;
}


std::vector<double> calculated_sigma2(std::vector<double>& returns, std::vector<double> x ){
    size_t n = returns.size();
    double w = x[0];
    double alpha = x[1];
    double beta = x[2];


    std::vector<double> sigmas(n);
    double denom = 1.0 - alpha - beta;
    if (denom <= 0.0) {
        std::cerr << "Unstable GARCH params: alpha+beta = " 
                << (alpha+beta) << "\n";
        return {};  // or throw
    }
    sigmas[0] = w / denom;

    for (unsigned int i = 0;i<n-1;i++){
        sigmas[i+1] = w + alpha*(returns[i]*returns[i]) + beta*(sigmas[i]);
    }
    return sigmas;
    
}

double forecast_iv(std::vector<double> sigmas, std::vector<double> x, int h){
    size_t n = sigmas.size();
    double w = x[0];
    double alpha = x[1];
    double beta = x[2];
    double phi = alpha + beta;
    double sigma = sigmas.back();
    
    double var_sum = 0.0;
    double forecast = 0.0;
    
    for (unsigned int j = 1;j<=h;++j){
        sigma = (sigma * phi) + w;
        var_sum += sigma;
 
    }
    double avg_var = var_sum / h;
    double iv = std::sqrt(avg_var);
    iv  *= std::sqrt(252.0);
    return iv;

}



