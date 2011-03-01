/* Copyright 2010, 2011 Matthew Arsenault, Travis Desell, Dave Przybylo,
Nathan Cole, Boleslaw Szymanski, Heidi Newberg, Carlos Varela, Malik
Magdon-Ismail and Rensselaer Polytechnic Institute.

This file is part of Milkway@Home.

Milkyway@Home is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Milkyway@Home is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Milkyway@Home.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "nbody_priv.h"
#include "milkyway_util.h"
#include "nbody_check_params.h"

static mwbool invalidHaloWarning(halo_t type)
{
    warn("Got non-finite required field for halo type '%s'\n", showHaloT(type));
    return TRUE;
}

/* Check for valid halo values and calculate constants. Return true if error. */
mwbool checkHaloConstants(Halo* h)
{
    real phi, cp, cps, sp, sps;
    real qxs, qys;

    /* Common to all 3 models */
    if (!isfinite(h->vhalo) || !isfinite(h->scale_length))
        return invalidHaloWarning(h->type);

    switch (h->type)
    {
        case LogarithmicHalo:
            if (!isfinite(h->flattenZ))
                return invalidHaloWarning(h->type);
            break;

        case NFWHalo:
            break;

        case TriaxialHalo:
            if (   !isfinite(h->triaxAngle)
                || !isfinite(h->flattenX)
                || !isfinite(h->flattenY)
                || !isfinite(h->flattenZ))
            {
                return invalidHaloWarning(h->type);
            }

            phi = h->triaxAngle;
            cp  = mw_cos(phi);
            cps = sqr(cp);
            sp  = mw_sin(phi);
            sps = sqr(sp);

            qxs = sqr(h->flattenX);
            qys = sqr(h->flattenY);

            h->c1 = (cps / qxs) + (sps / qys);
            h->c2 = (cps / qys) + (sps / qxs);

            /* 2 * sin(x) * cos(x) == sin(2 * x) */
            h->c3 = mw_sin(2.0 * phi) * ((qys - qxs) / (qxs * qys));
            break;

        case InvalidHalo:
        default:
            return warn1("Trying to use invalid halo type\n");
    }

    return FALSE;
}

static real calculateTimestep(real mass, real r0)
{
    return sqr(1/10.0) * mw_sqrt((PI_4_3 * cube(r0)) / mass);
}

static real calculateEps2(real nbody, real r0)
{
    real eps = r0 / (10.0 * mw_sqrt(nbody));
    return sqr(eps);
}

static int hasAcceptableEps2(const NBodyCtx* ctx)
{
    int rc = mwCheckNormalPosNum(ctx->eps2);
    if (rc)
        warn("Got an absurd eps2\n");

    return rc;
}

static int hasAcceptableTimes(const NBodyCtx* ctx)
{
    int rc = mwCheckNormalPosNum(ctx->timeEvolve);
    if (rc)
        warn("Got an unacceptable orbit or evolution time\n");
    return rc;
}

static int hasAcceptableSteps(const NBodyCtx* ctx)
{
    int rc = mwCheckNormalPosNum(ctx->timestep);
    if (rc)
        warn("Got an unacceptable timestep\n");

    return rc;
}

static int hasAcceptableNbody(const NBodyCtx* ctx)
{
    int rc = ctx->nbody < 1;
    if (rc)
        warn("nbody = %d is absurd\n", ctx->nbody);
    return rc;
}

int contextSanityCheck(const NBodyCtx* ctx)
{
    int rc = 0;

    rc |= hasAcceptableNbody(ctx);
    rc |= hasAcceptableTimes(ctx);
    rc |= hasAcceptableSteps(ctx);
    rc |= hasAcceptableEps2(ctx);

    return rc;
}
