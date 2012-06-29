#include "milkyway_math.h"
#include "nbody_types.h"

real factorial(int n) {
    if (n <= 1){
        return 0;
    }
    return mw_log(n) + factorial(n-1);
}
    
real choose(int n, int c) {
	assert(n > c);
	real result = 0;
	//calculate n!/(n-c)! through cancellation
	for(int i = n; i > (n-c); i--){
		result += mw_log(i);
	}
    return result - (real)(factorial(c));
}

real propability_match(int n, int k, real pobs){
    //mw_printf("n %e k %e pobs %e", (double)n, (double)k, pobs);
    real result;
    result = choose(n, k);
    //mw_printf("Result, choose %e \n", result);
    result += mw_log(pobs) * (real)k;
    //mw_printf("result, pobs %e \n", result);
    result += mw_log(1-pobs)* (real)(n-k);
    //mw_printf("Result before low %e \n", result);
    return result;
}

real prob_test(int k){
	real result = choose(100000, k);
	result += mw_log(.04) * (real)k;
	result += mw_log(1-.04) * (real)(100000-k);
	mw_printf("ans: %e  :: ", (double) result);
	return result;
}
