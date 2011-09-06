/*
 *  Copyright (c) 2008-2010 Travis Desell, Nathan Cole
 *  Copyright (c) 2008-2010 Boleslaw Szymanski, Heidi Newberg
 *  Copyright (c) 2008-2010 Carlos Varela, Malik Magdon-Ismail
 *  Copyright (c) 2008-2011 Rensselaer Polytechnic Institute
 *  Copyright (c) 2010-2011 Matthew Arsenault
 *
 *  This file is part of Milkway@Home.
 *
 *  Milkway@Home is free software: you may copy, redistribute and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation, either version 3 of the License, or (at your
 *  option) any later version.
 *
 *  This file is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SEPARATION_TYPES_H_
#define _SEPARATION_TYPES_H_

#include "separation_config.h"
#include "milkyway_math.h"

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    real l;
    real b;
} LB;

#define LB_L(x) ((x).l)
#define LB_B(x) ((x).b)



typedef struct
{
    real nu;
    real id;
} NuId;


typedef struct
{
    unsigned int number_stars;
    mwvector* stars;
} StarPoints;

#define EMPTY_STAR_POINTS { 0, NULL }


/* Convenience structure for passing mess of LBTrig to CAL kernel in 2 parts */
typedef struct
{
    real lCosBCos, lSinBCos;
} LTrigPair;

typedef struct
{
    real nu;
    real id;
} NuConstants;

typedef struct
{
    real* dx;
    real* qgaus_W;
} StreamGauss;


/* Parameter related types */


typedef struct
{
    real epsilon;    /* Stream weight */
    real mu;         /* mu, angular position */
    real r;          /* r, radial distance from the sun in kpc */
    real theta;      /* theta, zenith angle from Z-axis */
    real phi;        /* phi, azimuthal angle from X-axis */
    real sigma;      /* sigma, standard deviation of the Gaussian defining the stream falloff in kpc */

    real epsilonExp; /* exp(epsilon) */
} StreamParameters;

#define EMPTY_STREAM_PARAMETERS { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }

typedef struct
{
    StreamParameters* parameters;
    real sumExpWeights;
    int number_streams;
} Streams;

#define EMPTY_STREAMS { NULL, 0.0, 0 }

typedef struct
{
    real alpha;
    real r0;
    real q;
    real delta;

    real epsilon;

    real a, b, c;
} BackgroundParameters;

#define EMPTY_BACKGROUND_PARAMETERS { 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0 }

typedef struct
{
    real irv;
    real rPrime;
} RPrime;

typedef struct
{
    real irv_reff_xr_rp3;
    real gPrime;
} RConsts;


typedef struct
{
    real r_point;
    real qw_r3_N;
} RPoints;

typedef struct
{
    real lCosBCos;
    real lSinBCos;
    real bSin;
    real _pad;
} LBTrig;




typedef struct
{
    real likelihood;

    real backgroundIntegral;
    real backgroundLikelihood;

    real* streamIntegrals;

    real* streamLikelihoods;
} SeparationResults;






#ifndef _MSC_VER
  #define SEPARATION_ALIGN(x) __attribute__ ((aligned(x)))
#else
  #define SEPARATION_ALIGN(x) __declspec(align(x))
#endif /* _MSC_VER */


#ifdef __OPENCL_VERSION__


typedef struct
{
    real x, y, z, w;
} mwvector;

#define X(v) ((v).x)
#define Y(v) ((v).y)
#define Z(v) ((v).z)
#define W(v) ((v).w)
#endif /* __OPENCL_VERSION__ */




typedef struct SEPARATION_ALIGN(128)
{
    mwvector a;
    mwvector c;
    real sigma_sq2_inv;
    int large_sigma;          /* abs(stream_sigma) > SIGMA_LIMIT */
} StreamConstants;



/* Parameter related types */

typedef struct SEPARATION_ALIGN(128)
{
    real r_min, r_max, r_step_size;
    real nu_min, nu_max, nu_step_size;
    real mu_min, mu_max, mu_step_size;
    unsigned int r_steps, nu_steps, mu_steps;
} IntegralArea;


/* Kitchen sink of constants, etc. */
typedef struct SEPARATION_ALIGN(128)
{
    /* Constants determined by other parameters */
    real m_sun_r0;
    real q_inv;
    real q_inv_sqr;  /* 1 / q^2 */
    real r0;

    int convolve;
    int number_streams;
    int fast_h_prob;
    int aux_bg_profile;

    real alpha;
    real delta;
    real alpha_delta3;
    real bg_a, bg_b, bg_c;

    int wedge;
    real sun_r0;
    real q;
    real coeff;

    real total_calc_probs;  /* sum of (r_steps * mu_steps * nu_steps) for all integrals */
    int number_integrals;

    real exp_background_weight;
} AstronomyParameters;

typedef struct SEPARATION_ALIGN(16)
{
    real sum;
    real correction;
} Kahan;

#define ZERO_KAHAN { 0.0, 0.0 }

#define CLEAR_KAHAN(k) { (k).sum = 0.0; (k).correction = 0.0; }


#ifdef __cplusplus
}
#endif

#endif /* _SEPARATION_TYPES_H_ */

