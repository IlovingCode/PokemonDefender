/*----------------------------------------------------------------------------*/  
/**
 *	This confidential and proprietary software may be used only as
 *	authorised by a licensing agreement from ARM Limited
 *	(C) COPYRIGHT 2011-2012 ARM Limited
 *	ALL RIGHTS RESERVED
 *
 *	The entire notice above must be reproduced on all authorised
 *	copies and copies may only be made to the extent permitted
 *	by a licensing agreement from ARM Limited.
 *
 *	@brief	Library of math functions.
 */ 
/*----------------------------------------------------------------------------*/ 

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mathlib.cuh"

#ifdef WIN32
	double cbrt(double n)
	{
		return n < 0 ? -pow(-n, 1.0 / 3.0) : pow(n, 1.0 / 3.0);
	}
#endif

/**************************
  basic opencl functions
**************************/

float inversesqrt(float p)
{
	return 1.0f / sqrt(p);
}
float acospi(float p)
{
	return static_cast < float >(acos(p) * (1.0f / M_PI));
};
/*float sinpi(float p)
{
	return static_cast < float >(sin(p * M_PI));
}
float cospi(float p)
{
	return static_cast < float >(cos(p * M_PI));
}*/


float nan(int p)
{
	union
	{
		int p;
		float q;
	} v;
	v.p = p | 0x7FC00000U;
	return v.q;
}

float fMax(float p, float q)
{
	if (p != p)
		return q;
	if (q != q)
		return p;
	if (p > q)
		return p;
	return q;
}

float fMin(float p, float q)
{
	if (p != p)
		return q;
	if (q != q)
		return p;
	if (p < q)
		return p;
	return q;
}


Float2 fMax(Float2 p, Float2 q)
{
	return Float2(fMax(p.x, q.x), fMax(p.y, q.y));
}

Float3 fMax(Float3 p, Float3 q)
{
	return Float3(fMax(p.x, q.x), fMax(p.y, q.y), fMax(p.z, q.z));
}

Float4 fMax(Float4 p, Float4 q)
{
	return Float4(fMax(p.x, q.x), fMax(p.y, q.y), fMax(p.z, q.z), fMax(p.w, q.w));
}


Float2 fMin(Float2 p, Float2 q)
{
	return Float2(fMin(p.x, q.x), fMin(p.y, q.y));
}

Float3 fMin(Float3 p, Float3 q)
{
	return Float3(fMin(p.x, q.x), fMin(p.y, q.y), fMin(p.z, q.z));
}

Float4 fMin(Float4 p, Float4 q)
{
	return Float4(fMin(p.x, q.x), fMin(p.y, q.y), fMin(p.z, q.z), fMin(p.w, q.w));
}

/* 
   float dot( Float2 p, Float2 q ) { return p.x*q.x + p.y*q.y; } float dot( Float3 p, Float3 q ) { return p.x*q.x + p.y*q.y + p.z*q.z; } float dot( Float4 p, Float4 q ) { return p.x*q.x + p.y*q.y +
   p.z*q.z + p.w*q.w; } */

Float3 cross(Float3 p, Float3 q)
{
	return p.yzx * q.zxy - p.zxy * q.yzx;
}

Float4 cross(Float4 p, Float4 q)
{
	return Float4(p.yzx * q.zxy - p.zxy * q.yzx, 0.0f);
}

float length(Float2 p)
{
	return sqrt(dot(p, p));
}

float length(Float3 p)
{
	return sqrt(dot(p, p));
}

float length(Float4 p)
{
	return sqrt(dot(p, p));
}

float length_sqr(Float2 p)
{
	return dot(p, p);
}

float length_sqr(Float3 p)
{
	return dot(p, p);
}

float length_sqr(Float4 p)
{
	return dot(p, p);
}


float distance(Float2 p, Float2 q)
{
	return length(q - p);
}

float distance(Float3 p, Float3 q)
{
	return length(q - p);
}

float distance(Float4 p, Float4 q)
{
	return length(q - p);
}

float distance_sqr(Float2 p, Float2 q)
{
	return length_sqr(q - p);
}

float distance_sqr(Float3 p, Float3 q)
{
	return length_sqr(q - p);
}

float distance_sqr(Float4 p, Float4 q)
{
	return length_sqr(q - p);
}


Float2 normalize(Float2 p)
{
	return p / length(p);
}

Float3 normalize(Float3 p)
{
	return p / length(p);
}

Float4 normalize(Float4 p)
{
	return p / length(p);
}


/**************************************************
  matrix functions, for 2x2, 3x3 and 4x4 matrices:

   * trace
   * determinant
   * transform
   * inverse
   * adjugate
   * characteristic polynomial
   * eigenvalue
   * eigenvector

  additionally, root solver
  for 2nd, 3rd and 4th degree monic polynomials.

*************************************************/

/* 
	struct mat2 { Float2 v[2]; };
	struct mat3 { Float3 v[3]; };
	struct mat4 { Float4 v[4]; };
*/

float trace(mat2 p)
{
	return p.v[0].x + p.v[1].y;
}

float trace(mat3 p)
{
	return p.v[0].x + p.v[1].y + p.v[2].z;
}

float trace(mat4 p)
{
	return p.v[0].x + p.v[1].y + p.v[2].z + p.v[3].w;
}

float determinant(mat2 p)
{
	Float2 v = p.v[0].xy * p.v[1].yx;
	return v.x - v.y;
}

float determinant(mat3 p)
{
	return dot(p.v[0], cross(p.v[1], p.v[2]));
}

float determinant(mat4 p)
{
	return dot(p.v[0],
			   Float4(dot(p.v[1].yzw, cross(p.v[2].yzw, p.v[3].yzw)),
					  -dot(p.v[1].xzw, cross(p.v[2].xzw, p.v[3].xzw)), dot(p.v[1].xyw, cross(p.v[2].xyw, p.v[3].xyw)), -dot(p.v[1].xyz, cross(p.v[2].xyz, p.v[3].xyz))));
}


/* 
   characteristic polynomials for matrices. These polynomials are monic, meaning that the coefficient of the highest component is 1; this component is omitted. The first component is the constant
   part. */

Float2 characteristic_poly(mat2 p)
{
	return Float2(determinant(p), -trace(p));
}


Float3 characteristic_poly(mat3 p)
{
	Float2 v1 = (p.v[0].xy * p.v[1].yx) + (p.v[0].xz * p.v[2].zx) + (p.v[1].yz * p.v[2].zy);

	return Float3(-determinant(p), v1.x - v1.y, -trace(p));
}


Float4 characteristic_poly(mat4 p)
{
	Float2 v1 = (p.v[0].xy * p.v[1].yx) + (p.v[0].xz * p.v[2].zx) + (p.v[0].xw * p.v[3].wx) + (p.v[1].yz * p.v[2].zy) + (p.v[1].yw * p.v[3].wy) + (p.v[2].zw * p.v[3].wz);

	return Float4(determinant(p),
				  -dot(p.v[1].yzw, cross(p.v[2].yzw, p.v[3].yzw))
				  - dot(p.v[0].xzw, cross(p.v[2].xzw, p.v[3].xzw)) - dot(p.v[0].xyw, cross(p.v[1].xyw, p.v[3].xyw)) - dot(p.v[0].xyz, cross(p.v[1].xyz, p.v[2].xyz)), v1.x - v1.y, -trace(p));
}


/* 
	Root finders for monic polynomials (highest coefficient is equal to 1)

	Returns a vector with length equal to the number of roots that the polynomial has;
	for roots that do not genuinely exist, we return NaN.

	The polynomial is basically

	poly(n) = p.x + p.y*n + p.z*n^2 + p.w*n^3

	(including only the components of the vector that actually exist; the next coefficient
	has the value 1, and the remaining ones have value 0. )
 */


Float2 solve_monic(Float2 p)
{
	float v = sqrt(p.y * p.y - 4 * p.x);
	return (p.yy + Float2(v, -v)) * -0.5f;
}

Float3 solve_monic(Float3 p)
{

	p = p * (1.0f / 3.0f);

	float pz = p.z;

	// compute a normalization value to scale the vector by.
	// The normalization factor is divided by 2^20.
	// This is supposed to make internal calculations unlikely
	// to overflow while also making underflows unlikely.
	float scal = 1.0f;

	float cx = static_cast < float >(cbrt(fabs(p.x)));
	float cy = static_cast < float >(cbrt(fabs(p.y)));
	scal = fMax(fMax(fabsf(p.z), cx), cy * cy) * (1.0f / 1048576.0f);
	float rscal = 1.0f / scal;
	p = p * Float3(rscal * rscal * rscal, rscal * rscal, rscal);

	float bb = p.z * p.z;		// div scal^2

	float nq = bb - p.y;		// div scal^2
	float r = 1.5f * (p.y * p.z - p.x) - p.z * bb;	// div scal^3
	float nq3 = nq * nq * nq;	// div scal^6
	float r2 = r * r;			// div scal^6

	if (nq3 < r2)
	{
		// one root
		float root = sqrt(r2 - nq3);	// div scal^3
		float s = static_cast < float >(cbrt(r + root));	// div scal
		float t = static_cast < float >(cbrt(r - root));	// div scal
		return Float3((s + t) * scal - pz, nan(0), nan(0));
	}
	else
	{
		// three roots
		float phi_r = inversesqrt(nq3);	// div scal ^ -3
		float phi_root = static_cast < float >(cbrt(phi_r * nq3));	// div scal
		float theta = acospi(r * phi_r);
		theta *= 1.0f / 3.0f;
		float ncprod = phi_root * cospi(theta);
		float dev = 1.73205080756887729353f * phi_root * sinpi(theta);
		return Float3(2 * ncprod, -dev - ncprod, dev - ncprod) * scal - pz;
	}
}


/* 
 * This function is not overflow-safe. Use with care.
 */
Float4 solve_monic(Float4 p)
{

	// step 1: depress the input polynomial
	float bias = p.w * 0.25f;
	Float3 qv = Float3((-3.0f / 256.0f) * p.w * p.w, (1.0f / 8.0f) * p.w, (-3.0 / 8.0f));
	Float3 rv = Float3((1.0f / 16.0f) * p.z * p.w - (1.0f / 4.0f) * p.y, (-1.0f / 2.0f) * p.z, 0.0f);
	Float3 qx = Float3(qv * p.w + rv) * p.w + p.xyz;

	// step 2: solve a cubic equation to get hold of a parameter p.
	Float3 monicp = Float3(-qx.y * qx.y, (qx.z * qx.z) - (4.0f * qx.x), 2.0f * qx.z);
	Float4 v = Float4(solve_monic(monicp), 1e-37f);

	// the cubic equation may have multiple solutions; at least one of them
	// is numerically at least nonnegative (but may have become negative as a result of
	// a roundoff error). We use fMax() to extract this value or a very small positive value.
	Float2 v2 = fMax(v.xy, v.zw);
	float p2 = fMax(v2.x, v2.y);	// p^2
	float pr = inversesqrt(p2);	// 1/p
	float pm = p2 * pr;			// p

	// step 3: use the solution for the cubic equation to set up two quadratic equations;
	// these two equations then result in the 4 possible roots.
	float f1 = qx.z + p2;
	float f2 = qx.y * pr;
	float s = 0.5f * (f1 + f2);
	float q = 0.5f * (f1 - f2);

	Float4 res = Float4(solve_monic(Float2(q, pm)),
						solve_monic(Float2(s, -pm)));

	// finally, order the results and apply the bias.
	if (res.x != res.x)
		return res.zwxy - bias;
	else
		return res - bias;
}



Float2 transform(mat2 p, Float2 q)
{
	return Float2(dot(p.v[0], q), dot(p.v[1], q));
}


Float3 transform(mat3 p, Float3 q)
{
	return Float3(dot(p.v[0], q), dot(p.v[1], q), dot(p.v[2], q));
}


Float4 transform(mat4 p, Float4 q)
{
	return Float4(dot(p.v[0], q), dot(p.v[1], q), dot(p.v[2], q), dot(p.v[3], q));
}



mat2 adjugate(mat2 p)
{
	mat2 res;
	res.v[0] = Float2(p.v[1].y, -p.v[0].y);
	res.v[1] = Float2(-p.v[1].x, p.v[0].x);
	return res;
}



mat2 invert(mat2 p)
{
	float rdet = 1.0f / determinant(p);
	mat2 res;
	res.v[0] = Float2(p.v[1].y, -p.v[0].y) * rdet;
	res.v[1] = Float2(-p.v[1].x, p.v[0].x) * rdet;
	return res;
}



mat3 adjugate(mat3 p)
{
	mat3 res;
	Float3 prd0 = cross(p.v[1], p.v[2]);
	Float3 prd1 = cross(p.v[2], p.v[0]);
	Float3 prd2 = cross(p.v[0], p.v[1]);
	res.v[0] = Float3(prd0.x, prd1.x, prd2.x);
	res.v[1] = Float3(prd0.y, prd1.y, prd2.y);
	res.v[2] = Float3(prd0.z, prd1.z, prd2.z);
	return res;
}



mat3 invert(mat3 p)
{
	Float3 cross0 = cross(p.v[1], p.v[2]);
	float det = dot(cross0, p.v[0]);
	float rdet = 1.0f / det;
	mat3 res;
	Float3 prd0 = cross0 * rdet;
	Float3 prd1 = cross(p.v[2], p.v[0]) * rdet;
	Float3 prd2 = cross(p.v[0], p.v[1]) * rdet;
	res.v[0] = Float3(prd0.x, prd1.x, prd2.x);
	res.v[1] = Float3(prd0.y, prd1.y, prd2.y);
	res.v[2] = Float3(prd0.z, prd1.z, prd2.z);
	return res;
}



mat4 adjugate(mat4 p)
{
	mat4 res;

	Float3 bpc0 = cross(p.v[2].yzw, p.v[3].yzw);
	Float3 tpc0 = cross(p.v[0].yzw, p.v[1].yzw);
	res.v[0] = Float4(dot(bpc0, p.v[1].yzw), -dot(bpc0, p.v[0].yzw), dot(tpc0, p.v[3].yzw), -dot(tpc0, p.v[2].yzw));

	Float3 bpc1 = cross(p.v[2].xzw, p.v[3].xzw);
	Float3 tpc1 = cross(p.v[0].xzw, p.v[1].xzw);
	res.v[1] = Float4(-dot(bpc1, p.v[1].xzw), dot(bpc1, p.v[0].xzw), -dot(tpc1, p.v[3].xzw), dot(tpc1, p.v[2].xzw));

	Float3 bpc2 = cross(p.v[2].xyw, p.v[3].xyw);
	Float3 tpc2 = cross(p.v[0].xyw, p.v[1].xyw);
	res.v[2] = Float4(dot(bpc2, p.v[1].xyw), -dot(bpc2, p.v[0].xyw), dot(tpc2, p.v[3].xyw), -dot(tpc2, p.v[2].xyw));

	Float3 bpc3 = cross(p.v[2].xyz, p.v[3].xyz);
	Float3 tpc3 = cross(p.v[0].xyz, p.v[1].xyz);
	res.v[3] = Float4(-dot(bpc3, p.v[1].xyz), dot(bpc3, p.v[0].xyz), -dot(tpc3, p.v[3].xyz), dot(tpc3, p.v[2].xyz));

	return res;
}



mat4 invert(mat4 p)
{
	// cross products between the bottom two rows
	Float3 bpc0 = cross(p.v[2].yzw, p.v[3].yzw);
	Float3 bpc1 = cross(p.v[2].xzw, p.v[3].xzw);
	Float3 bpc2 = cross(p.v[2].xyw, p.v[3].xyw);
	Float3 bpc3 = cross(p.v[2].xyz, p.v[3].xyz);

	// dot-products for the top rows
	Float4 row1 = Float4(dot(bpc0, p.v[1].yzw),
						 -dot(bpc1, p.v[1].xzw),
						 dot(bpc2, p.v[1].xyw),
						 -dot(bpc3, p.v[1].xyz));

	float det = dot(p.v[0], row1);
	float rdet = 1.0f / det;

	mat4 res;

	Float3 tpc0 = cross(p.v[0].yzw, p.v[1].yzw);
	res.v[0] = Float4(row1.x, -dot(bpc0, p.v[0].yzw), dot(tpc0, p.v[3].yzw), -dot(tpc0, p.v[2].yzw)) * rdet;

	Float3 tpc1 = cross(p.v[0].xzw, p.v[1].xzw);
	res.v[1] = Float4(row1.y, dot(bpc1, p.v[0].xzw), -dot(tpc1, p.v[3].xzw), dot(tpc1, p.v[2].xzw)) * rdet;
	Float3 tpc2 = cross(p.v[0].xyw, p.v[1].xyw);

	res.v[2] = Float4(row1.z, -dot(bpc2, p.v[0].xyw), dot(tpc2, p.v[3].xyw), -dot(tpc2, p.v[2].xyw)) * rdet;

	Float3 tpc3 = cross(p.v[0].xyz, p.v[1].xyz);
	res.v[3] = Float4(row1.w, dot(bpc3, p.v[0].xyz), -dot(tpc3, p.v[3].xyz), dot(tpc3, p.v[2].xyz)) * rdet;


	return res;
}



Float2 eigenvalues(mat2 p)
{
	return solve_monic(characteristic_poly(p));
}

Float3 eigenvalues(mat3 p)
{
	return solve_monic(characteristic_poly(p));
}

Float4 eigenvalues(mat4 p)
{
	return solve_monic(characteristic_poly(p));
}

Float2 eigenvector(mat2 p, float eigvl)
{
	// for a mat2, we first reverse-subtract the eigenvalue from the matrix diagonal,
	// then return whichever row had the larger sum-of-absolute-values.
	Float4 v = Float4(p.v[0], p.v[1]);
	v.xw = eigvl - v.xw;
	if (fabs(v.x) + fabs(v.y) > fabs(v.z) + fabs(v.w))
		return v.yx;
	else
		return v.wz;
}


Float3 eigenvector(mat3 p, float eigvl)
{
	// for a mat3, we obtain the eigenvector as follows:
	// step 1: subtract the eigenvalue from the matrix diagonal
	// step 2: take two cross products between rows in the matrix
	// step 3: return whichever of the cross products resulted in a longer vector.

	Float3 r0 = p.v[0];
	Float3 r1 = p.v[1];
	Float3 r2 = p.v[2];

	r0.x = r0.x - eigvl;
	r1.y = r1.y - eigvl;
	r2.z = r2.z - eigvl;

	Float3 v1 = cross(r0, r1);
	Float3 v2 = cross(r1, r2);

	float len1 = dot(v1, v1);
	float len2 = dot(v2, v2);
	return len1 > len2 ? v1 : v2;
}


// generalized cross product: 3 vectors with 4 components each.
// The result is a vector that is perpendicular to all the three specified vectors.

// it works in the sense that it produces a perpendicular-to-everyting vector,
// but it has not been tested whether it points in the "right" direction.
Float4 gcross(Float4 p, Float4 q, Float4 r)
{
	return Float4(dot(p.yzw, cross(q.yzw, r.yzw)), -dot(p.xzw, cross(q.xzw, r.xzw)), dot(p.xyw, cross(q.xyw, r.xyw)), -dot(p.xyz, cross(q.xyz, r.xyz)));
}



Float4 eigenvector(mat4 p, float eigvl)
{
	Float4 r0 = p.v[0];
	Float4 r1 = p.v[1];
	Float4 r2 = p.v[2];
	Float4 r3 = p.v[3];

	r0.x = r0.x - eigvl;
	r1.y = r1.y - eigvl;
	r2.z = r2.z - eigvl;
	r3.w = r3.w - eigvl;

	// generate four candidate vectors using the generalized cross product.
	// These will in general point in the same direction (or 180 degree opposite),
	// however they will have different lengths. Pick the longest one.
	Float3 tpc0 = cross(r0.yzw, r1.yzw);
	Float3 tpc1 = cross(r0.xzw, r1.xzw);
	Float3 tpc2 = cross(r0.xyw, r1.xyw);
	Float3 tpc3 = cross(r0.xyz, r1.xyz);

	Float4 v1 = Float4(dot(r2.yzw, tpc0),
					   -dot(r2.xzw, tpc1),
					   dot(r2.xyw, tpc2),
					   -dot(r2.xyz, tpc3));

	Float4 v2 = Float4(dot(r3.yzw, tpc0),
					   -dot(r3.xzw, tpc1),
					   dot(r3.xyw, tpc2),
					   -dot(r3.xyz, tpc3));

	Float3 bpc0 = cross(r2.yzw, r3.yzw);
	Float3 bpc1 = cross(r2.xzw, r3.xzw);
	Float3 bpc2 = cross(r2.xyw, r3.xyw);
	Float3 bpc3 = cross(r2.xyz, r3.xyz);

	Float4 v3 = Float4(dot(r0.yzw, bpc0),
					   -dot(r0.xzw, bpc1),
					   dot(r0.xyw, bpc2),
					   -dot(r0.xyz, bpc3));

	Float4 v4 = Float4(dot(r1.yzw, bpc0),
					   -dot(r1.xzw, bpc1),
					   dot(r1.xyw, bpc2),
					   -dot(r1.xyz, bpc3));

	float len1 = dot(v1, v1);
	float len2 = dot(v2, v2);
	float len3 = dot(v3, v3);
	float len4 = dot(v4, v4);

	if (fMax(len1, len2) > fMax(len3, len4))
		return len1 > len2 ? v1 : v2;
	else
		return len3 > len4 ? v3 : v4;
}


// matrix multiply

mat2 operator *(mat2 a, mat2 b)
{
	mat2 res;
	res.v[0] = a.v[0].x * b.v[0] + a.v[0].y * b.v[1];
	res.v[1] = a.v[1].x * b.v[0] + a.v[1].y * b.v[1];
	return res;
}

mat3 operator *(mat3 a, mat3 b)
{
	mat3 res;
	res.v[0] = a.v[0].x * b.v[0] + a.v[0].y * b.v[1] + a.v[0].z * b.v[2];
	res.v[1] = a.v[1].x * b.v[0] + a.v[1].y * b.v[1] + a.v[1].z * b.v[2];
	res.v[2] = a.v[2].x * b.v[0] + a.v[2].y * b.v[1] + a.v[2].z * b.v[2];
	return res;
}

mat4 operator *(mat4 a, mat4 b)
{
	mat4 res;
	res.v[0] = a.v[0].x * b.v[0] + a.v[0].y * b.v[1] + a.v[0].z * b.v[2] + a.v[0].w * b.v[3];
	res.v[1] = a.v[1].x * b.v[0] + a.v[1].y * b.v[1] + a.v[1].z * b.v[2] + a.v[1].w * b.v[3];
	res.v[2] = a.v[2].x * b.v[0] + a.v[2].y * b.v[1] + a.v[2].z * b.v[2] + a.v[2].w * b.v[3];
	res.v[3] = a.v[3].x * b.v[0] + a.v[3].y * b.v[1] + a.v[3].z * b.v[2] + a.v[3].w * b.v[3];
	return res;
}



/*************************

simple geometric functions

*************************/


// return parameter value for the point on the line closest to the specified point
float param_nearest_on_line(Float2 point, line2 line)
{
	return dot(point - line.a, line.b) / dot(line.b, line.b);
}

float param_nearest_on_line(Float3 point, line3 line)
{
	return dot(point - line.a, line.b) / dot(line.b, line.b);
}

float param_nearest_on_line(Float4 point, line4 line)
{
	return dot(point - line.a, line.b) / dot(line.b, line.b);
}


// return distance between point and line
float point_line_distance(Float2 point, line2 line)
{
	return distance(point, line.a + line.b * param_nearest_on_line(point, line));
}

float point_line_distance(Float3 point, line3 line)
{
	return distance(point, line.a + line.b * param_nearest_on_line(point, line));
}

float point_line_distance(Float4 point, line4 line)
{
	return distance(point, line.a + line.b * param_nearest_on_line(point, line));
}


float point_line_distance_sqr(Float2 point, line2 line)
{
	return distance_sqr(point, line.a + line.b * param_nearest_on_line(point, line));
}

float point_line_distance_sqr(Float3 point, line3 line)
{
	return distance_sqr(point, line.a + line.b * param_nearest_on_line(point, line));
}

float point_line_distance_sqr(Float4 point, line4 line)
{
	return distance_sqr(point, line.a + line.b * param_nearest_on_line(point, line));
}



// distance between plane/hyperplane in 3D and 4D
float point_plane_3d_distance(Float3 point, plane_3d plane)
{
	return dot(point - plane.root_point, plane.normal);
}


float point_hyperplane_4d_distance(Float4 point, hyperplane_4d plane)
{
	return dot(point - plane.root_point, plane.normal);
}


// helper functions to produce a 3D plane from three points and a 4D hyperplane from four points.
plane_3d generate_plane_from_points(Float3 point0, Float3 point1, Float3 poInt2)
{
	plane_3d res;
	res.root_point = point0;
	res.normal = normalize(cross(point1 - point0, poInt2 - point0));
	return res;
}

hyperplane_4d generate_hyperplane_from_points(Float4 point0, Float4 point1, Float4 poInt2, Float4 poInt3)
{
	hyperplane_4d res;
	res.root_point = point0;
	res.normal = normalize(gcross(point1 - point0, poInt2 - point0, poInt3 - point0));
	return res;
}


