/**
 * @file 	problem.c
 * @brief 	Example problem: MEGNO.
 * @author 	Hanno Rein <hanno@hanno-rein.de>
 * @detail 	This example uses the IAS15 or WHFAST integrator
 * to calculate the MEGNO of a two planet system.
 * 
 * @section 	LICENSE
 * Copyright (c) 2014 Hanno Rein, Shangfei Liu, Dave Spiegel
 *
 * This file is part of rebound.
 *
 * rebound is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * rebound is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with rebound.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "rebound.h"
#include "output.h"
#include "tools.h"
#include "integrator.h"
#include "integrator_whfast.h"
#include "particle.h"

double ss_pos[3][3] = 
{
	{-4.06428567034226e-3,	-6.08813756435987e-3,	-1.66162304225834e-6	}, // Sun
	{+3.40546614227466e+0,	+3.62978190075864e+0,	+3.42386261766577e-2	}, // Jupiter
	{+6.60801554403466e+0,	+6.38084674585064e+0,	-1.36145963724542e-1	}, // Saturn
};
double ss_vel[3][3] = 
{
	{+6.69048890636161e-6,	-6.33922479583593e-6,	-3.13202145590767e-9	}, // Sun
	{-5.59797969310664e-3,	+5.51815399480116e-3,	-2.66711392865591e-6	}, // Jupiter
	{-4.17354020307064e-3,	+3.99723751748116e-3,	+1.67206320571441e-5	}, // Saturn
};

double ss_mass[3] =
{
	1.00000597682, 	// Sun + inner planets
	1./1047000.355,	// Jupiter
	1./3501000.6,	// Saturn
};

double tmax = 1e9;

void heartbeat(struct Rebound* const r);

int main(int argc, char* argv[]) {
	struct Rebound* r = rebound_init();
	// Setup constants
	r->dt 		= 10;			// initial timestep (in days)
	//r->integrator	= IAS15;
	r->integrator	= WHFAST;
	const double k	= 0.01720209895;	// Gaussian constant 
	r->G		= k*k;			// These are the same units that mercury6 uses
	rebound_configure_box(r,100,1,1,1);

	// Initial conditions
	for (int i=0;i<3;i++){
		struct Particle p;
		p.x  = ss_pos[i][0]; 		p.y  = ss_pos[i][1];	 	p.z  = ss_pos[i][2];
		p.vx = ss_vel[i][0]; 		p.vy = ss_vel[i][1];	 	p.vz = ss_vel[i][2];
		p.ax = 0; 			p.ay = 0; 			p.az = 0;
		p.m  = ss_mass[i];
		particles_add(r, p); 
	}
	tools_move_to_center_of_momentum(r);
	// Add megno particles 
	tools_megno_init(r, 1e-16);  // N = 6 after this function call. 
	// The first half of particles are real particles, the second half are particles following the variational equations.
	
	// Set callback for outputs.
	r->heartbeat = heartbeat;

	rebound_integrate(r, tmax);
}

void heartbeat(struct Rebound* const r){
	if (output_check(r, 1000.*r->dt)){
		output_timing(r, tmax);
	}
	if (output_check(r, 362.)){
		// Output the time and the MEGNO to the screen and a file.
		FILE* f = fopen("Y.txt","a+");
		fprintf(f,"        %.20e     %.20e\n",r->t, tools_megno(r));
		//printf("        %.20e     %.20e\n",r->t, tools_megno(r));
		fclose(f);
	}
}

void problem_finish(){
}