#include "milkyway_math.h"
#include "nbody_types.h"

real factorial(int n) {
    if (n <= 0){
        return 0;
    }
    return mw_log(n) + factorial(n-1);
}
    
real choose(int n, int c) {
    return (real)factorial(n) - (real)(factorial(c) - (real)factorial(n-c));
}

real propability_match(int n, int k, real pobs){
    mw_printf("n %e k %e pobs %e", (double)n, (double)k, pobs);
    real result;
    result = choose(n, k);
    mw_printf("Result, choose %e \n", result);
    result += mw_log(pobs) * (real)k;
    mw_printf("result, pobs %e \n", result);
    result += mw_log(1-pobs)* (real)(n-k);
    mw_printf("Result before low %e \n", result);
    return -1 * result;
}
