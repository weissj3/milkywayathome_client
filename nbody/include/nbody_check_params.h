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

#ifndef _NBODY_CHECK_PARAMS_H_
#define _NBODY_CHECK_PARAMS_H_

#include "nbody_types.h"

#ifdef __cplusplus
extern "C"
#endif

mwbool checkHaloConstants(Halo* h);
mwbool checkSphericalConstants(Spherical* s);
mwbool checkDiskConstants(Disk* d);
mwbool checkPotentialConstants(Potential* p);

mwbool checkNBodyCtxConstants(const NBodyCtx* ctx);

#ifdef __cplusplus
}
#endif

#endif /* _NBODY_CHECK_PARAMS_H_ */

