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

#ifndef _NBODY_CONFIG_H_
#define _NBODY_CONFIG_H_

#include "milkyway_config.h"

#define MILKYWAY_NBODY_CLIENT_VERSION_MAJOR 
#define MILKYWAY_NBODY_CLIENT_VERSION_MINOR 
#define MILKYWAY_NBODY_CLIENT_VERSION       0.22

#define NBODY_OPENCL 0
#define NBODY_SHOW 1
#define NBODY_CRLIBM 1

#define PERIODIC_OUTPUT 0

#define ENABLE_CRLIBM NBODY_CRLIBM
#define ENABLE_OPENCL NBODY_OPENCL

#if defined(_OPENMP)
  #define EXTRAVER " OpenMP"
#elif NBODY_OPENCL
  #define EXTRAVER " OpenCL"
#else
  #define EXTRAVER
#endif /* defined(_OPENMP) */


#define BOINC_NBODY_APP_VERSION "milkywayathome nbody 0.22 Linux " ARCH_STRING " " PRECSTRING EXTRAVER

#define DEFAULT_OUTPUT_FILE stderr

#endif /* NBODY_CONFIG_H_ */

