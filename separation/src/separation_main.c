/*
Copyright 2008-2010 Travis Desell, Dave Przybylo, Nathan Cole, Matthew
Arsenault, Boleslaw Szymanski, Heidi Newberg, Carlos Varela, Malik
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


#include <stdio.h>
#include <stdlib.h>
#include <popt.h>
#include "separation.h"

#define DEFAULT_ASTRONOMY_PARAMETERS "astronomy_parameters.txt"
#define DEFAULT_STAR_POINTS "stars.txt"

typedef struct
{
    char* star_points_file;
    char* ap_file;  /* astronomy parameters */
    int cleanup_checkpoint;
} SeparationFlags;

#define EMPTY_SEPARATION_FLAGS { NULL, NULL, 0 }

static void freeSeparationFlags(SeparationFlags* sf)
{
    free(sf->star_points_file);
    free(sf->ap_file);
}

/* Use hardcoded names if files not specified */
static void setDefaultFiles(SeparationFlags* sf)
{
    stringDefault(sf->star_points_file, DEFAULT_STAR_POINTS);
    stringDefault(sf->ap_file, DEFAULT_ASTRONOMY_PARAMETERS);
}

/* Returns the newly allocated array of parameters */
static real* parse_parameters(int argc, const char** argv, unsigned int* paramnOut, SeparationFlags* sf)
{
    poptContext context;
    int o;
    real* parameters = NULL;
    static unsigned int numParams;
    static int server_params = 0;
    static const char** rest;

    const struct poptOption options[] =
    {
        {
            "star-points-file", 's',
            POPT_ARG_STRING, &sf->star_points_file,
            's', "Star points files", NULL
        },

        {
            "astronomy-parameter-file", 'a',
            POPT_ARG_STRING, &sf->ap_file,
            'a', "Astronomy parameter file", NULL
        },

        {
            "cleanup-checkpoint", 'c',
            POPT_ARG_NONE, &sf->cleanup_checkpoint,
            'c', "Delete checkpoint on successful", NULL
        },

        {
            "p", 'p',
            POPT_ARG_NONE, &server_params,
            0, "Unused dummy argument to satisfy primitive arguments the server sends", NULL
        },

        {
            "np", '\0',
            POPT_ARG_INT | POPT_ARGFLAG_ONEDASH, &numParams,
            0, "Unused dummy argument to satisfy primitive arguments the server sends", NULL
        },

        POPT_AUTOHELP

        { NULL, 0, 0, NULL, 0, NULL, NULL }
    };

    context = poptGetContext(argv[0],
                             argc,
                             argv,
                             options,
                             POPT_CONTEXT_POSIXMEHARDER);

    while ( ( o = poptGetNextOpt(context)) >= 0 );

    if ( o < -1 )
    {
        poptPrintHelp(context, stderr, 0);
        freeSeparationFlags(sf);
        mw_finish(EXIT_FAILURE);
    }

    rest = poptGetArgs(context);
    parameters = mwReadRestArgs(rest, numParams, paramnOut);
    if (!parameters)
    {
        warn("Failed to read server arguments\n");
        freeSeparationFlags(sf);
        poptFreeContext(context);
        mw_finish(EXIT_FAILURE);
    }

    poptFreeContext(context);
    setDefaultFiles(sf);
    return parameters;
}

static INTEGRAL_AREA* prepare_parameters(const char* ap_file,
                                         ASTRONOMY_PARAMETERS* ap,
                                         BACKGROUND_PARAMETERS* bgp,
                                         STREAMS* streams,
                                         const real* parameters,
                                         const int number_parameters)
{
    int ap_number_parameters;
    INTEGRAL_AREA* ias;

    ias = read_parameters(ap_file, ap, bgp, streams);
    if (!ias)
    {
        warn("Error reading astronomy parameters from file '%s'\n", ap_file);
        return NULL;
    }

    ap_number_parameters = get_optimized_parameter_count(ap, bgp, streams);
    if (number_parameters < 1 || number_parameters != ap_number_parameters)
    {
        warn("Error reading parameters: number of parameters from the "
             "command line (%d) does not match the number of parameters "
             "to be optimized in %s (%d)\n",
             number_parameters,
             ap_file,
             ap_number_parameters);

        free(ias);
        free_streams(streams);
        free_background_parameters(bgp);
        return NULL;
    }

    set_parameters(ap, bgp, streams, parameters);

    return ias;
}

static int worker(const SeparationFlags* sf, const real* parameters, const int number_parameters)
{
    ASTRONOMY_PARAMETERS ap = EMPTY_ASTRONOMY_PARAMETERS;
    BACKGROUND_PARAMETERS bgp = EMPTY_BACKGROUND_PARAMETERS;
    STREAMS streams = EMPTY_STREAMS;
    INTEGRAL_AREA* ias = NULL;
    STREAM_CONSTANTS* sc = NULL;
    real likelihood_val = NAN;
    int rc;

    ias = prepare_parameters(sf->ap_file, &ap, &bgp, &streams, parameters, number_parameters);
    if (!ias)
    {
        warn("Failed to read parameters\n");
        return 1;
    }

    rc = setAstronomyParameters(&ap, &bgp);
    free_background_parameters(&bgp);
    if (rc)
    {
        warn("Failed to set astronomy parameters\n");
        free(ias);
        free_streams(&streams);
        return 1;
    }

    sc = getStreamConstants(&ap, &streams);
    if (!sc)
    {
        warn("Failed to get stream constants\n");
        free(ias);
        free_streams(&streams);
        return 1;
    }

    likelihood_val = evaluate(&ap, ias, &streams, sc, sf->star_points_file);
    rc = isnan(likelihood_val);

    warn("<search_likelihood> %0.20f </search_likelihood>\n", likelihood_val);

    if (rc)
        warn("Failed to calculate likelihood\n");

    free(ias);
    mwAlignedFree(sc);
    free_streams(&streams);

    return rc;
}

#if BOINC_APPLICATION

static int separation_init(const char* appname)
{
    int rc;

  #if BOINC_DEBUG
    rc = boinc_init_diagnostics(  BOINC_DIAG_DUMPCALLSTACKENABLED
                                | BOINC_DIAG_HEAPCHECKENABLED
                                | BOINC_DIAG_MEMORYLEAKCHECKENABLED);
  #else
    rc = boinc_init();
  #endif /* BOINC_DEBUG */


  #if BOINC_APP_GRAPHICS
    #if defined(_WIN32) || defined(__APPLE__)
    rc = boinc_init_graphics(worker);
    #else
    rc = boinc_init_graphics_lib(worker, appname);
    #endif /*  defined(_WIN32) || defined(__APPLE__) */
  #else
    #pragma unused(appname)
  #endif /* BOINC_APP_GRAPHICS */

  #if defined(_WIN32) && COMPUTE_ON_GPU
    //make the windows GPU app have a higher priority
    BOINC_OPTIONS options;
    boinc_options_defaults(options);
    options.normal_thread_priority = 1; // higher priority (normal instead of idle)
    rc = boinc_init_options(&options);
  #endif /* defined(_WIN32) && COMPUTE_ON_GPU */

    return rc;
}

static void printVersion()
{
    warn("<search_application> " SEPARATION_APP_VERSION " </search_application>\n");
}

#else

static int separation_init(const char* appname)
{
  #pragma unused(argc)
  #pragma unused(argv)

  #if DISABLE_DENORMALS
    mwDisableDenormalsSSE();
  #endif

    return 0;
}

static void printVersion()
{
    warn("<search_application> " SEPARATION_APP_VERSION " </search_application>\n");
}

#endif /* BOINC_APPLICATION */

int main(int argc, const char* argv[])
{
    int rc;
    SeparationFlags sf = EMPTY_SEPARATION_FLAGS;
    real* parameters;
    unsigned int number_parameters;

    rc = separation_init(argv[0]);
    printVersion();
    if (rc)
        exit(rc);

    parameters = parse_parameters(argc, argv, &number_parameters, &sf);
    if (!parameters)
    {
        warn("Could not parse parameters from the command line\n");
        rc = 1;
    }
    else
    {
        rc = worker(&sf, parameters, number_parameters);
        if (rc)
            warn("Worker failed\n");
    }

    freeSeparationFlags(&sf);
    free(parameters);

  #if BOINC_APPLICATION && !SEPARATION_OPENCL
    if (sf.cleanup_checkpoint && rc == 0)
    {
        mw_report("Removing checkpoint file '%s'\n", CHECKPOINT_FILE);
        mw_remove(CHECKPOINT_FILE);
    }
  #endif

    mw_finish(rc);

    return rc;
}
