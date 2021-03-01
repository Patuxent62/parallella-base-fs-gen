/* nbody.h
 *
 * Copyright (c) 2015, James A. Ross
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* JAR */

#ifndef _nbody_h
#define _nbody_h
#define fma(x,y,z) __builtin_fmaf(x,y,z)
#define ALIGN8 8

typedef struct __attribute__((aligned(ALIGN8)))
{
  uint32_t smem_start;
  uint32_t smem_len;
  uint32_t line_length;
  uint32_t xres;
  uint32_t yres;
  uint32_t xres_virtual;
  uint32_t yres_virtual;
  uint32_t xoffset;
  uint32_t yoffset;
  uint32_t bits_per_pixel;
} fbinfo_t;

typedef struct {
	float x, y, z;
	float mass; // pre-scaled by gravitational constant
} Particle;

typedef struct {
	float vx, vy, vz;
	float ax, ay, az;
} ParticleV;

typedef struct __attribute__((aligned(ALIGN8)))
{
	int n;
	int s;
	int cnt;
	float dt;
	float es;
	Particle* p;
	Particle* pn;
	ParticleV* v;
	fbinfo_t fbinfo;
} my_args_t;


float Q_rsqrt( float number )
{
	long i;
	float x2, y;
	const float threehalfs = 1.5f;

	x2 = number * 0.5f;
	y  = number;
	i  = * ( long * ) &y;
	i  = 0x5f3759df - ( i >> 1 );
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) ); // 1st iteration
	//y  = y * ( threehalfs - ( x2 * y * y ) ); // 2nd iteration, this can be removed

	return y;
}

void ComputeAccel(Particle* particles, Particle* others, ParticleV* pv, int n, float eps)
{
	int i, j;
	for (i=0; i<n; i+=4) {
		float x0 = particles[i+0].x, y0 = particles[i+0].y, z0 = particles[i+0].z;
		float x1 = particles[i+1].x, y1 = particles[i+1].y, z1 = particles[i+1].z;
		float x2 = particles[i+2].x, y2 = particles[i+2].y, z2 = particles[i+2].z;
		float x3 = particles[i+3].x, y3 = particles[i+3].y, z3 = particles[i+3].z;
		float ax0 = 0.0f, ay0 = 0.0f, az0 = 0.0f;
		float ax1 = 0.0f, ay1 = 0.0f, az1 = 0.0f;
		float ax2 = 0.0f, ay2 = 0.0f, az2 = 0.0f;
		float ax3 = 0.0f, ay3 = 0.0f, az3 = 0.0f;
		for (j=0; j<n; j++) {
			float x = others[j].x, y = others[j].y, z = others[j].z;
			float mj = others[j].mass;
			float rx0 = x - x0, ry0 = y - y0, rz0 = z - z0;
			float rx1 = x - x1, ry1 = y - y1, rz1 = z - z1;
			float rx2 = x - x2, ry2 = y - y2, rz2 = z - z2;
			float rx3 = x - x3, ry3 = y - y3, rz3 = z - z3;
			float invr0 = Q_rsqrt(fma(rz0,rz0,fma(ry0,ry0,fma(rx0,rx0,eps))));
			float invr1 = Q_rsqrt(fma(rz1,rz1,fma(ry1,ry1,fma(rx1,rx1,eps))));
			float invr2 = Q_rsqrt(fma(rz2,rz2,fma(ry2,ry2,fma(rx2,rx2,eps))));
			float invr3 = Q_rsqrt(fma(rz3,rz3,fma(ry3,ry3,fma(rx3,rx3,eps))));
			float invr30 = invr0*invr0*invr0;
			float invr31 = invr1*invr1*invr1;
			float invr32 = invr2*invr2*invr2;
			float invr33 = invr3*invr3*invr3;
			ax0 = fma(mj*rx0,invr30,ax0); ay0 = fma(mj*ry0,invr30,ay0); az0 = fma(mj*rz0,invr30,az0);
			ax1 = fma(mj*rx1,invr31,ax1); ay1 = fma(mj*ry1,invr31,ay1); az1 = fma(mj*rz1,invr31,az1);
			ax2 = fma(mj*rx2,invr32,ax2); ay2 = fma(mj*ry2,invr32,ay2); az2 = fma(mj*rz2,invr32,az2);
			ax3 = fma(mj*rx3,invr33,ax3); ay3 = fma(mj*ry3,invr33,ay3); az3 = fma(mj*rz3,invr33,az3);
		}
		pv[i+0].ax += ax0; pv[i+0].ay += ay0; pv[i+0].az += az0;
		pv[i+1].ax += ax1; pv[i+1].ay += ay1; pv[i+1].az += az1;
		pv[i+2].ax += ax2; pv[i+2].ay += ay2; pv[i+2].az += az2;
		pv[i+3].ax += ax3; pv[i+3].ay += ay3; pv[i+3].az += az3;
	}
}

void ComputeNewPos(Particle* particles, ParticleV* pv, int n, float dt)
{
	int i;
	for (i=0; i<n; i++) {
		float vx = pv[i].vx;
		float vy = pv[i].vy;
		float vz = pv[i].vz;
		float ax = pv[i].ax;
		float ay = pv[i].ay;
		float az = pv[i].az;
		float halfdtdt = 0.5f*dt*dt;
		particles[i].x += vx*dt + ax*halfdtdt;
		particles[i].y += vy*dt + ay*halfdtdt;
		particles[i].z += vz*dt + az*halfdtdt;
		pv[i].vx = vx + ax*dt;
		pv[i].vy = vy + ay*dt;
		pv[i].vz = vz + az*dt;
		pv[i].ax = 0.0f;
		pv[i].ay = 0.0f;
		pv[i].az = 0.0f;
	}
}

#endif
