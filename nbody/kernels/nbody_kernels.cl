/*
 * Copyright (c) 2010 The University of Texas at Austin
 * Copyright (c) 2010 Dr. Martin Burtscher
 * Copyright (c) 2011-2012 Matthew Arsenault
 * Copyright (c) 2011 Rensselaer Polytechnic Institute
 *
 * This file is part of Milkway@Home.
 *
 * Milkyway@Home is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Milkyway@Home is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Milkyway@Home.  If not, see <http://www.gnu.org/licenses/>.
 */

/* In case there isn't a space a space between the -D and the
 * symbol. if the thing begins with D there's an Apple OpenCL compiler
 * bug on 10.6 where the D will be stripped. -DDOUBLEPREC=1 will
 * actually define OUBLEPREC */

#ifdef OUBLEPREC
  #define DOUBLEPREC OUBLEPREC
#endif

#ifdef EBUG
  #define DEBUG EBUG
#endif

#ifdef ISK_MASS
  #define DISK_MASS ISK_MASS
#endif

#ifdef ISK_SCALE_LENGTH
  #define DISK_SCALE_LENGTH ISK_SCALE_LENGTH
#endif

#ifdef ISK_SCALE_HEIGHT
  #define DISK_SCALE_HEIGHT ISK_SCALE_HEIGHT
#endif


#ifndef DOUBLEPREC
  #error Precision not defined
#endif


#if !BH86 && !SW93 && !NEWCRITERION && !EXACT
  #error Opening criterion not set
#endif

#if USE_EXTERNAL_POTENTIAL && ((!MIYAMOTO_NAGAI_DISK && !EXPONENTIAL_DISK) || (!LOG_HALO && !NFW_HALO && !TRIAXIAL_HALO))
  #error Potential defines misspecified
#endif

#if WARPSIZE <= 0
  #error Invalid warp size
#endif

/* These were problems when being lazy and writing it */
#if (THREADS6 / WARPSIZE) <= 0
  #error (THREADS6 / WARPSIZE) must be > 0
#elif (MAXDEPTH * THREADS6 / WARPSIZE) <= 0
  #error (MAXDEPTH * THREADS6 / WARPSIZE) must be > 0
#endif

#if DEBUG && cl_amd_printf
  #pragma OPENCL EXTENSION cl_amd_printf : enable
#endif

#if DOUBLEPREC
  /* double precision is optional core feature in 1.2, not an extension */
  #if __OPENCL_VERSION__ < 120
    #if cl_khr_fp64
      #pragma OPENCL EXTENSION cl_khr_fp64 : enable
    #elif cl_amd_fp64
      #pragma OPENCL EXTENSION cl_amd_fp64 : enable
    #else
      #error Missing double precision extension
    #endif
  #endif
#endif /* DOUBLEPREC */

/* Reserve positive numbers for reporting depth > MAXDEPTH. Should match on host */
typedef enum
{
    NBODY_KERNEL_OK                   = 0,
    NBODY_KERNEL_CELL_OVERFLOW        = -1,
    NBODY_KERNEL_TREE_INCEST          = -2,
    NBODY_KERNEL_TREE_STRUCTURE_ERROR = -3,
    NBODY_KERNEL_ERROR_OTHER          = -4
} NBodyKernelError;

#if DEBUG
/* Want first failed assertion to be where it is marked */
#define cl_assert(treeStatus, x)                        \
    do                                                  \
    {                                                   \
      if (!(x))                                         \
      {                                                 \
          if ((treeStatus)->assertionLine < 0)          \
          {                                             \
              (treeStatus)->assertionLine = __LINE__;   \
          }                                             \
      }                                                 \
    }                                                   \
    while (0)

#define cl_assert_rtn(treeStatus, x)                    \
    do                                                  \
    {                                                   \
      if (!(x))                                         \
      {                                                 \
          if ((treeStatus)->assertionLine < 0)          \
          {                                             \
              (treeStatus)->assertionLine = __LINE__;   \
          }                                             \
          return;                                       \
      }                                                 \
    }                                                   \
    while (0)

#else
#define cl_assert(treeStatus, x)
#define cl_assert_rtn(treeStatus, x)
#endif /* DEBUG */


#if DOUBLEPREC
typedef double real;
typedef double2 real2;
typedef double4 real4;
#else
typedef float real;
typedef float2 real2;
typedef float4 real4;
#endif /* DOUBLEPREC */


#if DOUBLEPREC
  #define REAL_EPSILON DBL_EPSILON
  #define REAL_MAX DBL_MAX
  #define REAL_MIN DBL_MIN
#else
  #define REAL_EPSILON FLT_EPSILON
  #define REAL_MAX FLT_MAX
  #define REAL_MIN FLT_MIN
#endif


#define sqr(x) ((x) * (x))
#define cube(x) ((x) * (x) * (x))

#define NSUB 8

#define isBody(n) ((n) < NBODY)
#define isCell(n) ((n) >= NBODY)

#define NULL_BODY (-1)
#define LOCK (-2)


/* This needs to be the same as on the host */
typedef struct __attribute__((aligned(64)))
{
    real radius;
    int bottom;
    uint maxDepth;
    uint blkCnt;
    int doneCnt;

    int errorCode;
    int assertionLine;

    char _pad[64 - (1 * sizeof(real) + 6 * sizeof(int))];

    struct
    {
        real f[32];
        int i[64];
        int wg1[256];
        int wg2[256];
        int wg3[256];
        int wg4[256];
    } debug;
} TreeStatus;



typedef struct
{
    real xx, xy, xz;
    real yy, yz;
    real zz;
} QuadMatrix;



typedef __global volatile real* restrict RVPtr;
typedef __global volatile int* restrict IVPtr;



inline real4 sphericalAccel(real4 pos, real r)
{
    const real tmp = SPHERICAL_SCALE + r;

    return (-SPHERICAL_MASS / (r * sqr(tmp))) * pos;
}

/* gets negative of the acceleration vector of this disk component */
inline real4 miyamotoNagaiDiskAccel(real4 pos, real r)
{
    real4 acc;
    const real a   = DISK_SCALE_LENGTH;
    const real b   = DISK_SCALE_HEIGHT;
    const real zp  = sqrt(sqr(pos.z) + sqr(b));
    const real azp = a + zp;

    const real rp  = sqr(pos.x) + sqr(pos.y) + sqr(azp);
    const real rth = sqrt(cube(rp));  /* rp ^ (3/2) */

    acc.x = -DISK_MASS * pos.x / rth;
    acc.y = -DISK_MASS * pos.y / rth;
    acc.z = -DISK_MASS * pos.z * azp / (zp * rth);
    acc.w = 0.0;

    return acc;
}

inline real4 exponentialDiskAccel(real4 pos, real r)
{
    const real b = DISK_SCALE_LENGTH;

    const real expPiece = exp(-r / b) * (r + b) / b;
    const real factor   = DISK_MASS * (expPiece - 1.0) / cube(r);

    return factor * pos;
}

inline real4 logHaloAccel(real4 pos, real r)
{
    real4 acc;

    const real tvsqr = -2.0 * sqr(HALO_VHALO);
    const real qsqr  = sqr(HALO_FLATTEN_Z);
    const real d     = HALO_SCALE_LENGTH;
    const real zsqr  = sqr(pos.z);

    const real arst  = sqr(d) + sqr(pos.x) + sqr(pos.y);
    const real denom = (zsqr / qsqr) +  arst;

    acc.x = tvsqr * pos.x / denom;
    acc.y = tvsqr * pos.y / denom;
    acc.z = tvsqr * pos.z / ((qsqr * arst) + zsqr);

    return acc;
}

inline real4 nfwHaloAccel(real4 pos, real r)
{
    const real a  = HALO_SCALE_LENGTH;
    const real ar = a + r;
    const real c  = a * sqr(HALO_VHALO) * (r - ar * log((a + r) / a)) / (0.2162165954 * cube(r) * ar);

    return c * pos;
}

inline real4 triaxialHaloAccel(real4 pos, real r)
{
    real4 acc;

    const real qzs      = sqr(HALO_FLATTEN_Z);
    const real rhalosqr = sqr(HALO_SCALE_LENGTH);
    const real mvsqr    = -sqr(HALO_VHALO);

    const real xsqr = sqr(pos.x);
    const real ysqr = sqr(pos.y);
    const real zsqr = sqr(pos.z);

    const real c1 = HALO_C1;
    const real c2 = HALO_C2;
    const real c3 = HALO_C3;

    const real arst  = rhalosqr + (c1 * xsqr) + (c3 * pos.x * pos.y) + (c2 * ysqr);
    const real arst2 = (zsqr / qzs) + arst;

    acc.x = mvsqr * (((2.0 * c1) * pos.x) + (c3 * pos.y) ) / arst2;

    acc.y = mvsqr * (((2.0 * c2) * pos.y) + (c3 * pos.x) ) / arst2;

    acc.z = (2.0 * mvsqr * pos.z) / ((qzs * arst) + zsqr);

    acc.w = 0.0;

    return acc;
}

inline real4 externalAcceleration(real x, real y, real z)
{
    real4 pos = { x, y, z, 0.0 };
    real r = sqrt(sqr(x) + sqr(y) + sqr(z));
    //real r = length(pos); // crashes AMD compiler
    real4 acc;

    if (MIYAMOTO_NAGAI_DISK)
    {
        acc = miyamotoNagaiDiskAccel(pos, r);
    }
    else if (EXPONENTIAL_DISK)
    {
        acc = exponentialDiskAccel(pos, r);
    }

    if (LOG_HALO)
    {
        acc += logHaloAccel(pos, r);
    }
    else if (NFW_HALO)
    {
        acc += nfwHaloAccel(pos, r);
    }
    else if (TRIAXIAL_HALO)
    {
        acc += triaxialHaloAccel(pos, r);
    }

    acc += sphericalAccel(pos, r);

    return acc;
}




/* All kernels will use the same parameters for now */
#define NBODY_KERNEL(name) name(                        \
    RVPtr _posX, RVPtr _posY, RVPtr _posZ,              \
    RVPtr _velX, RVPtr _velY, RVPtr _velZ,              \
    RVPtr _accX, RVPtr _accY, RVPtr _accZ,              \
    RVPtr _mass,                                        \
                                                        \
    IVPtr _next, IVPtr _more, RVPtr _rcrit2,            \
                                                        \
    RVPtr _quadXX, RVPtr _quadXY, RVPtr _quadXZ,        \
    RVPtr _quadYY, RVPtr _quadYZ,                       \
    RVPtr _quadZZ,                                      \
                                                        \
    __global volatile TreeStatus* _treeStatus,          \
    uint maxNBody,                                      \
    int updateVel                                       \
    )


/* Used by sw93 */
inline real bmax2Inc(real cmPos, real pPos, real psize)
{
    real dmin = cmPos - (pPos - 0.5 * psize);         /* dist from 1st corner */
    real tmp = fmax(dmin, psize - dmin);
    return tmp * tmp;      /* sum max distance^2 */
}

inline void incAddMatrix(QuadMatrix* restrict a, QuadMatrix* restrict b)
{
    a->xx += b->xx;
    a->xy += b->xy;
    a->xz += b->xz;

    a->yy += b->yy;
    a->yz += b->yz;

    a->zz += b->zz;
}

inline void quadCalc(QuadMatrix* quad, real4 chCM, real4 kp)
{
    real4 dr;
    dr.x = chCM.x - kp.x;
    dr.y = chCM.y - kp.y;
    dr.z = chCM.z - kp.z;

    real drSq = mad(dr.z, dr.z, mad(dr.y, dr.y, dr.x * dr.x));

    quad->xx = chCM.w * (3.0 * (dr.x * dr.x) - drSq);
    quad->xy = chCM.w * (3.0 * (dr.x * dr.y));
    quad->xz = chCM.w * (3.0 * (dr.x * dr.z));

    quad->yy = chCM.w * (3.0 * (dr.y * dr.y) - drSq);
    quad->yz = chCM.w * (3.0 * (dr.y * dr.z));

    quad->zz = chCM.w * (3.0 * (dr.z * dr.z) - drSq);
}

__attribute__ ((reqd_work_group_size(THREADS6, 1, 1)))
__kernel void NBODY_KERNEL(forceCalculation)
{
    __local uint maxDepth;
    __local real rootCritRadius;

    __local volatile int ch[THREADS6 / WARPSIZE];
    __local volatile real nx[THREADS6 / WARPSIZE], ny[THREADS6 / WARPSIZE], nz[THREADS6 / WARPSIZE];
    __local volatile real nm[THREADS6 / WARPSIZE];

    /* Stack things */
    __local volatile int pos[MAXDEPTH * THREADS6 / WARPSIZE], node[MAXDEPTH * THREADS6 / WARPSIZE];
    __local volatile real dq[MAXDEPTH * THREADS6 / WARPSIZE];

  #if USE_QUAD
    __local real rootQXX, rootQXY, rootQXZ;
    __local real rootQYY, rootQYZ;
    __local real rootQZZ;

    __local volatile real quadXX[MAXDEPTH * THREADS6 / WARPSIZE];
    __local volatile real quadXY[MAXDEPTH * THREADS6 / WARPSIZE];
    __local volatile real quadXZ[MAXDEPTH * THREADS6 / WARPSIZE];

    __local volatile real quadYY[MAXDEPTH * THREADS6 / WARPSIZE];
    __local volatile real quadYZ[MAXDEPTH * THREADS6 / WARPSIZE];

    __local volatile real quadZZ[MAXDEPTH * THREADS6 / WARPSIZE];
  #endif /* USE_QUAD */


  #if !HAVE_INLINE_PTX
    /* Used by the fake thread voting function.
       We rely on the lockstep behaviour of warps/wavefronts to avoid using a barrier
    */
    __local volatile int allBlock[THREADS6 / WARPSIZE];
  #endif /* !HAVE_INLINE_PTX */

    if (get_local_id(0) == 0)
    {
        maxDepth = _treeStatus->maxDepth;
        real rootSize = _treeStatus->radius;

      #if USE_QUAD
        rootQXX = _quadXX[NNODE];
        rootQXY = _quadXY[NNODE];
        rootQXZ = _quadXZ[NNODE];
        rootQYY = _quadYY[NNODE];
        rootQYZ = _quadYZ[NNODE];
        rootQZZ = _quadZZ[NNODE];
      #endif

        if (SW93 || NEWCRITERION)
        {
            rootCritRadius = _critRadii[NNODE];
        }
        else if (BH86)
        {
            real rc;

            if (THETA == 0.0)
            {
                rc = 2.0 * rootSize;
            }
            else
            {
                rc = rootSize / THETA;
            }

            /* Precompute values that depend only on tree level */
            dq[0] = rc * rc;
            for (uint i = 1; i < maxDepth; ++i)
            {
                dq[i] = 0.25 * dq[i - 1];
            }
        }

      #if !HAVE_INLINE_PTX
        for (uint i = 0; i < THREADS6 / WARPSIZE; ++i)
        {
            allBlock[i] = 0;
        }
      #endif

        if (maxDepth > MAXDEPTH)
        {
            _treeStatus->errorCode = maxDepth;
        }
    }
    barrier(CLK_GLOBAL_MEM_FENCE | CLK_LOCAL_MEM_FENCE);

    if (maxDepth <= MAXDEPTH)
    {
        /* Figure out first thread in each warp */
        uint base = get_local_id(0) / WARPSIZE;
        uint sbase = base * WARPSIZE;
        int j = base * MAXDEPTH;
        int diff = get_local_id(0) - sbase; /* Index in warp */

        if (BH86 || EXACT)
        {
            /* Make multiple copies to avoid index calculations later */
            if (diff < MAXDEPTH)
            {
                dq[diff + j] = dq[diff];
            }
            barrier(CLK_GLOBAL_MEM_FENCE | CLK_LOCAL_MEM_FENCE);
        }

        uint k = get_global_id(0);

      #if !HAVE_INLINE_PTX
        (void) atom_add(&allBlock[base], k >= maxNBody);
      #endif

        /* iterate over all bodies assigned to thread */
        while (k < maxNBody)
        {
            int i = _sort[k];  /* Get permuted index */

            /* Cache position info */
            real px = _posX[i];
            real py = _posY[i];
            real pz = _posZ[i];

            real ax = 0.0;
            real ay = 0.0;
            real az = 0.0;

            /* Initialize iteration stack, i.e., push root node onto stack */
            int depth = j;
            if (get_local_id(0) == sbase)
            {
                node[j] = NNODE;
                pos[j] = 0;

                if (SW93 || NEWCRITERION)
                {
                    dq[j] = rootCritRadius;
                }

                #if USE_QUAD
                {
                    quadXX[j] = rootQXX;
                    quadXY[j] = rootQXY;
                    quadXZ[j] = rootQXZ;
                    quadYY[j] = rootQYY;
                    quadYZ[j] = rootQYZ;
                    quadZZ[j] = rootQZZ;
                }
                #endif
            }
            mem_fence(CLK_GLOBAL_MEM_FENCE | CLK_LOCAL_MEM_FENCE);

            bool skipSelf = false;
            do
            {
                int curPos;

                /* Stack is not empty */
                while ((curPos = pos[depth]) < NSUB)
                {
                    int n;
                    /* Node on top of stack has more children to process */
                    if (get_local_id(0) == sbase)
                    {
                        /* I'm the first thread in the warp */
                        n = _child[NSUB * node[depth] + curPos]; /* Load child pointer */
                        pos[depth] = curPos + 1;
                        ch[base] = n; /* Cache child pointer */
                        if (n >= 0)
                        {
                            /* Cache position and mass */
                            nx[base] = _posX[n];
                            ny[base] = _posY[n];
                            nz[base] = _posZ[n];
                            nm[base] = _mass[n];
                        }
                    }
                    mem_fence(CLK_GLOBAL_MEM_FENCE | CLK_LOCAL_MEM_FENCE);

                    /* All threads retrieve cached data */
                    n = ch[base];
                    if (n >= 0)
                    {
                        real dx = nx[base] - px;
                        real dy = ny[base] - py;
                        real dz = nz[base] - pz;
                        real rSq = mad(dz, dz, mad(dy, dy, dx * dx));  /* Compute distance squared */

                        /* Check if all threads agree that cell is far enough away (or is a body) */
                        if (isBody(n) || warpAcceptsCell(allBlock, base, rSq, dq[depth]))
                        {
                            rSq += EPS2;

                          #ifdef __FAST_RELAXED_MATH__
                            real rInv = rsqrt(rSq);   /* Compute distance with softening */
                            real ai = nm[base] * rInv * rInv * rInv;
                            /* FIXME: If EPS is 0, we can get nan */
                          #else
                            real r = sqrt(rSq);   /* Compute distance with softening */
                            real ai = nm[base] / (rSq * r);
                          #endif

                            ax = mad(ai, dx, ax);
                            ay = mad(ai, dy, ay);
                            az = mad(ai, dz, az);

                          #if USE_QUAD
                            {
                                if (isCell(n))
                                {
                                    real quad_dx, quad_dy, quad_dz;

                                    real dr5inv = 1.0 / (sqr(rSq) * r);

                                    /* Matrix multiply Q . dr */
                                    quad_dx = mad(quadXZ[depth], dz, mad(quadXY[depth], dy, quadXX[depth] * dx));
                                    quad_dy = mad(quadYZ[depth], dz, mad(quadYY[depth], dy, quadXY[depth] * dx));
                                    quad_dz = mad(quadZZ[depth], dz, mad(quadYZ[depth], dy, quadXZ[depth] * dx));

                                    /* dr . Q . dr */
                                    real drQdr = mad(quad_dz, dz, mad(quad_dy, dy, quad_dx * dx));

                                    real phiQuad = 2.5 * (dr5inv * drQdr) / rSq;

                                    ax = mad(phiQuad, dx, ax);
                                    ay = mad(phiQuad, dy, ay);
                                    az = mad(phiQuad, dz, az);

                                    ax = mad(-dr5inv, quad_dx, ax);
                                    ay = mad(-dr5inv, quad_dy, ay);
                                    az = mad(-dr5inv, quad_dz, az);
                                }
                            }
                          #endif /* USE_QUAD */


                            /* Watch for self interaction. It's OK to
                             * not skip because dx, dy, dz will be
                             * 0.0 */
                            if (n == i)
                            {
                                skipSelf = true;
                            }
                        }
                        else
                        {
                            /* Push cell onto stack */
                            ++depth;
                            if (get_local_id(0) == sbase)
                            {
                                node[depth] = n;
                                pos[depth] = 0;

                                if (SW93 || NEWCRITERION)
                                {
                                    dq[depth] = _critRadii[n];
                                }

                                #if USE_QUAD
                                {
                                    quadXX[depth] = _quadXX[n];
                                    quadXY[depth] = _quadXY[n];
                                    quadXZ[depth] = _quadXZ[n];

                                    quadYY[depth] = _quadYY[n];
                                    quadYZ[depth] = _quadYZ[n];

                                    quadZZ[depth] = _quadZZ[n];
                                }
                                #endif /* USE_QUAD */
                            }
                            /* Full barrier not necessary since items only synced on wavefront level */
                            mem_fence(CLK_GLOBAL_MEM_FENCE | CLK_LOCAL_MEM_FENCE);
                        }
                    }
                    else
                    {
                        /* Early out because all remaining children are also zero */
                        depth = max(j, depth - 1);
                    }
                }
                --depth;  /* Done with this level */
            }
            while (depth >= j);

            real accX = _accX[i];
            real accY = _accY[i];
            real accZ = _accZ[i];

            real vx = _velX[i];
            real vy = _velY[i];
            real vz = _velZ[i];


            if (USE_EXTERNAL_POTENTIAL)
            {
                real4 acc = externalAcceleration(px, py, pz);

                ax += acc.x;
                ay += acc.y;
                az += acc.z;
            }

            vx = mad(0.5 * TIMESTEP, ax - accX, vx);
            vy = mad(0.5 * TIMESTEP, ay - accY, vy);
            vz = mad(0.5 * TIMESTEP, az - accZ, vz);

            /* Save computed acceleration */
            _accX[i] = ax;
            _accY[i] = ay;
            _accZ[i] = az;

            if (updateVel)
            {
                _velX[i] = vx;
                _velY[i] = vy;
                _velZ[i] = vz;
            }

            if (!skipSelf)
            {
                _treeStatus->errorCode = NBODY_KERNEL_TREE_INCEST;
            }


            k += get_local_size(0) * get_num_groups(0);

          #if !HAVE_INLINE_PTX
            /* In case this thread is done with bodies and others in the wavefront aren't */
            (void) atom_add(&allBlock[base], k >= maxNBody);
          #endif /* !HAVE_INLINE_PTX */
        }
    }
}

__attribute__ ((reqd_work_group_size(THREADS7, 1, 1)))
__kernel void NBODY_KERNEL(integration)
{
    uint inc = get_local_size(0) * get_num_groups(0);

    /* Iterate over all bodies assigned to thread */
    for (uint i = (uint) get_global_id(0); i < NBODY; i += inc)
    {
        real px = _posX[i];
        real py = _posY[i];
        real pz = _posZ[i];

        real ax = _accX[i];
        real ay = _accY[i];
        real az = _accZ[i];

        real vx = _velX[i];
        real vy = _velY[i];
        real vz = _velZ[i];


        real dvx = (0.5 * TIMESTEP) * ax;
        real dvy = (0.5 * TIMESTEP) * ay;
        real dvz = (0.5 * TIMESTEP) * az;

        vx += dvx;
        vy += dvy;
        vz += dvz;

        px = mad(TIMESTEP, vx, px);
        py = mad(TIMESTEP, vy, py);
        pz = mad(TIMESTEP, vz, pz);

        vx += dvx;
        vy += dvy;
        vz += dvz;


        _posX[i] = px;
        _posY[i] = py;
        _posZ[i] = pz;

        _velX[i] = vx;
        _velY[i] = vy;
        _velZ[i] = vz;
    }
}

/* EFFNBODY must be divisible by workgroup size to prevent conditional barrier */
__attribute__ ((reqd_work_group_size(THREADS8, 1, 1)))
__kernel void NBODY_KERNEL(forceCalculation_Exact)
{
    __local real xs[THREADS8];
    __local real ys[THREADS8];
    __local real zs[THREADS8];
    __local real ms[THREADS8];

    cl_assert(_treeStatus, EFFNBODY % THREADS8 == 0);

    for (uint i = get_global_id(0); i < maxNBody; i += get_local_size(0) * get_num_groups(0))
    {
        real px = _posX[i];
        real py = _posY[i];
        real pz = _posZ[i];

        real dax = _accX[i];
        real day = _accY[i];
        real daz = _accZ[i];

        real dvx = _velX[i];
        real dvy = _velY[i];
        real dvz = _velZ[i];



        real ax = 0.0;
        real ay = 0.0;
        real az = 0.0;

        uint nTile = EFFNBODY / THREADS8;
        for (uint j = 0; j < nTile; ++j)
        {
            uint idx = THREADS8 * j + get_local_id(0);
            xs[get_local_id(0)] = _posX[idx];
            ys[get_local_id(0)] = _posY[idx];
            zs[get_local_id(0)] = _posZ[idx];

            ms[get_local_id(0)] = _mass[idx];

            barrier(CLK_LOCAL_MEM_FENCE);

            /* WTF: This doesn't happen the correct number of times
             * unless unrolling forced with on AMD */
            #pragma unroll 8
            for (int k = 0; k < THREADS8; ++k)
            {
                real dx = xs[k] - px;
                real dy = ys[k] - py;
                real dz = zs[k] - pz;

                real rSq = mad(dz, dz, mad(dy, dy, dx * dx)) + EPS2;
                real r = sqrt(rSq);
                real ai = ms[k] / (r * rSq);

                ax = mad(ai, dx, ax);
                ay = mad(ai, dy, ay);
                az = mad(ai, dz, az);
            }

            barrier(CLK_LOCAL_MEM_FENCE);
        }

        if (USE_EXTERNAL_POTENTIAL)
        {
            real4 acc = externalAcceleration(px, py, pz);

            ax += acc.x;
            ay += acc.y;
            az += acc.z;
        }

        if (updateVel)
        {
            dvx = mad(0.5 * TIMESTEP, ax - dax, dvx);
            dvy = mad(0.5 * TIMESTEP, ay - day, dvy);
            dvz = mad(0.5 * TIMESTEP, az - daz, dvz);

            _velX[i] = dvx;
            _velY[i] = dvy;
            _velZ[i] = dvz;
        }

        _accX[i] = ax;
        _accY[i] = ay;
        _accZ[i] = az;
    }
}

