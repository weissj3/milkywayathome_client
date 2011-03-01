/* Copyright 2010 Matthew Arsenault, Travis Desell, Dave Przybylo,
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

#include <string.h>
#include "nbody_util.h"
#include "nbody_types.h"
#include "milkyway_util.h"
#include "show.h"

/* A bunch of boilerplate for debug printing */

const char* showBool(const mwbool x)
{
    switch (x)
    {
        case FALSE:
            return "false";
        case TRUE:
            return "true";
        default:
            return "invalid boolean (but true)";
    }
}

const char* showCriterionT(const criterion_t x)
{
    switch (x)
    {
        case Exact:
            return "Exact";
        case NewCriterion:
            return "NewCriterion";
        case BH86:
            return "BH86";
        case SW93:
            return "SW93";
        case InvalidCriterion:
            return "InvalidCriterion";
        default:
            return "Bad criterion_t";
    }
}

const char* showSphericalT(const spherical_t x)
{
    switch (x)
    {
        case SphericalPotential:
            return "SphericalPotential";
        case InvalidSpherical:
            return "InvalidSpherical";
        default:
            return "Bad spherical_t";
    }
}

const char* showDiskT(const disk_t x)
{
    switch (x)
    {
        case MiyamotoNagaiDisk:
            return "MiyamotoNagaiDisk";
        case ExponentialDisk:
            return "ExponentialDisk";
        case InvalidDisk:
            return "InvalidDisk";
        default:
            return "Bad disk_t";
    }
}

const char* showHaloT(const halo_t x)
{
    switch (x)
    {
        case LogarithmicHalo:
            return "LogarithmicHalo";
        case NFWHalo:
            return "NFWHalo";
        case TriaxialHalo:
            return "TriaxialHalo";
        case InvalidHalo:
            return "InvalidHalo";
        default:
            return "Bad halo_t";
    }
}

char* showSpherical(const Spherical* s)
{
    char* buf;

    if (!s)
        return NULL;

    if (0 > asprintf(&buf,
                     "{\n"
                     "      type  = %s\n"
                     "      mass  = %g\n"
                     "      scale = %g\n"
                     "    };\n",
                     showSphericalT(s->type),
                     s->mass,
                     s->scale))
    {
        fail("asprintf() failed\n");
    }

    return buf;
}

char* showHalo(const Halo* h)
{
    char* buf;

    if (!h)
        return NULL;

    if (0 > asprintf(&buf,
                     "{ \n"
                     "      type         = %s\n"
                     "      vhalo        = %g\n"
                     "      scale_length = %g\n"
                     "      flattenX     = %g\n"
                     "      flattenY     = %g\n"
                     "      flattenZ     = %g\n"
                     "      c1           = %g\n"
                     "      c2           = %g\n"
                     "      c3           = %g\n"
                     "      triaxAngle   = %g\n"
                     "    };\n",
                     showHaloT(h->type),
                     h->vhalo,
                     h->scale_length,
                     h->flattenX,
                     h->flattenY,
                     h->flattenZ,
                     h->c1,
                     h->c2,
                     h->c3,
                     h->triaxAngle))
    {
        fail("asprintf() failed\n");
    }

    return buf;
}

char* showDisk(const Disk* d)
{
    char* buf;

    if (!d)
        return NULL;

    if (0 > asprintf(&buf,
                     "{ \n"
                     "      type         = %s\n"
                     "      mass         = %g\n"
                     "      scale_length = %g\n"
                     "      scale_height = %g\n"
                     "    };\n",
                     showDiskT(d->type),
                     d->mass,
                     d->scale_length,
                     d->scale_height))
    {
        fail("asprintf() failed\n");
    }

    return buf;
}

char* showBody(const bodyptr p)
{
    char* buf;
    char* vel;
    char* pos;

    if (!p)
        return NULL;

    vel = showVector(Vel(p));
    pos = showVector(Pos(p));

    if (0 > asprintf(&buf,
                     "body { \n"
                     "      mass   = %g\n"
                     "      pos    = %s\n"
                     "      vel    = %s\n"
                     "      ignore = %s\n"
                     "    };\n",
                     Mass(p),
                     pos,
                     vel,
                     showBool(ignoreBody(p))))

    {
        fail("asprintf() failed\n");
    }

    free(vel);
    free(pos);

    return buf;
}

/* For debugging. Need to make this go away for release since it uses
 * GNU extensions */
char* showPotential(const Potential* p)
{
    int rc;
    char* buf;
    char* sphBuf;
    char* diskBuf;
    char* haloBuf;

    if (!p)
        return NULL;

    sphBuf  = showSpherical(&p->sphere[0]);
    diskBuf = showDisk(&p->disk);
    haloBuf = showHalo(&p->halo);

    rc = asprintf(&buf,
                  "{\n"
                  "    sphere = %s\n"
                  "    disk = %s\n"
                  "    halo = %s\n"
                  "    rings  = { unused pointer %p }\n"
                  "  };\n",
                  sphBuf,
                  diskBuf,
                  haloBuf,
                  p->rings);

    if (rc < 0)
        fail("asprintf() failed\n");

    free(sphBuf);
    free(diskBuf);
    free(haloBuf);

    return buf;
}

char* showInitialConditions(const InitialConditions* ic)
{
    char* buf;

    if (!ic)
        return NULL;

    if (0 > asprintf(&buf,
                     "{ \n"
                     "          position     = { %g, %g, %g }\n"
                     "          velocity     = { %g, %g, %g }\n"
                     "        };\n",
                     X(ic->position),
                     Y(ic->position),
                     Z(ic->position),
                     X(ic->velocity),
                     Y(ic->velocity),
                     Z(ic->velocity)))
    {
        fail("asprintf() failed\n");
    }

    return buf;
}

/* Most efficient function ever */
char* showNBodyCtx(const NBodyCtx* ctx)
{
    char* buf;
    char* potBuf;

    if (!ctx)
        return NULL;

    potBuf = showPotential(&ctx->pot);

    if (0 > asprintf(&buf,
                     "ctx = { \n"
                     "  pot = %s\n"
                     "  timeEvolve      = %g\n"
                     "  timestep        = %g\n"
                     "  outfilename     = %s\n"
                     "  histogram       = %s\n"
                     "  histout         = %s\n"
                     "  outfile         = %p\n"
                     "  sunGCDist       = %g\n"
                     "  criterion       = %s\n"
                     "  useQuad         = %s\n"
                     "  allowIncest     = %s\n"
                     "  outputCartesian = %s\n"
                     "  seed            = %ld\n"
                     "  treeRSize       = %g\n"
                     "  theta           = %g\n"
                     "  eps2            = %g\n"
                     "  freqOut         = %u\n"
                     "  nbody           = %d\n"
                     "};\n",
                     potBuf,
                     ctx->timeEvolve,
                     ctx->timestep,
                     ctx->outfilename,
                     ctx->histogram,
                     ctx->histout,
                     ctx->outfile,
                     ctx->sunGCDist,
                     showCriterionT(ctx->criterion),
                     showBool(ctx->useQuad),
                     showBool(ctx->allowIncest),
                     showBool(ctx->outputCartesian),
                     ctx->seed,
                     ctx->treeRSize,
                     ctx->theta,
                     ctx->eps2,
                     ctx->freqOut,
                     ctx->nbody))
    {
        fail("asprintf() failed\n");
    }

    free(potBuf);

    return buf;
}

void printNBodyCtx(const NBodyCtx* ctx)
{
    char* buf = showNBodyCtx(ctx);
    puts(buf);
    free(buf);
}

void printInitialConditions(const InitialConditions* ic)
{
    char* buf = showInitialConditions(ic);
    puts(buf);
    free(buf);
}

char* showVector(const mwvector v)
{
    char* buf;

    if (asprintf(&buf, "{ %g, %g, %g }", X(v), Y(v), Z(v)) < 0)
        fail("asprintf() failed\n");

    return buf;
}

void printVector(const mwvector v)
{
    char* buf = showVector(v);
    puts(buf);
    free(buf);
}

char* showHistogramParams(const HistogramParams* hp)
{
    char* buf;
    if (0 > asprintf(&buf,
                     "histogram-params = { \n"
                     "  phi      = %g\n"
                     "  theta    = %g\n"
                     "  psi      = %g\n"
                     "  startRaw = %g\n"
                     "  endRaw   = %g\n"
                     "  binSize  = %g\n"
                     "  center   = %g\n"
                     "};\n",
                     hp->phi,
                     hp->theta,
                     hp->psi,
                     hp->startRaw,
                     hp->endRaw,
                     hp->binSize,
                     hp->center))

    {
        fail("asprintf() failed\n");
    }

    return buf;
}

void printHistogramParams(const HistogramParams* hp)
{
    char* buf = showHistogramParams(hp);
    puts(buf);
    free(buf);
}

void printBody(const bodyptr p)
{
    char* buf = showBody(p);
    puts(buf);
    free(buf);
}

void printHalo(const Halo* h)
{
    char* buf = showHalo(h);
    puts(buf);
    free(buf);
}

void printDisk(const Disk* d)
{
    char* buf = showDisk(d);
    puts(buf);
    free(buf);
}

void printPotential(const Potential* p)
{
    char* buf = showPotential(p);
    puts(buf);
    free(buf);
}

