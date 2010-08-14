/* Copyright 2010 Matthew Arsenault, Travis Desell, Boleslaw
Szymanski, Heidi Newberg, Carlos Varela, Malik Magdon-Ismail and
Rensselaer Polytechnic Institute.

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

#ifndef _INTEGRALS_LIKELIHOOD_H_
#define _INTEGRALS_LIKELIHOOD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "separation.h"
#include "parameters.h"
#include "evaluation_state.h"
#include "integral_constants.h"


void calculate_integrals(const ASTRONOMY_PARAMETERS* ap,
                         const STREAM_CONSTANTS* sc,
                         const STREAM_GAUSS* sg,
                         EVALUATION_STATE* es,
                         vector* xyz);

double likelihood(const ASTRONOMY_PARAMETERS* ap,
                  const STREAM_CONSTANTS* sc,
                  const STREAMS* streams,
                  EVALUATION_STATE* es,
                  STREAM_GAUSS* sg,
                  vector* xyz,
                  const STAR_POINTS* sp);

#ifdef __cplusplus
}
#endif

#endif /* _INTEGRALS_LIKELIHOOD_H_ */

