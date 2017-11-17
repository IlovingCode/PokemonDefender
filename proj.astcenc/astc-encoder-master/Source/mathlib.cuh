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
 *	@brief	Internal math library declarations for ASTC codec.
 */ 
/*----------------------------------------------------------------------------*/ 

#ifndef MATHLIB_H_INCLUDED

#define MATHLIB_H_INCLUDED

#include "vectypes.cuh"

// basic OpenCL functions
float inversesqrt(float p);
float acospi(float p);
float sinpi(float p);
float cospi(float p);

float nan(int p);

float fMax(float p, float q);
float fMin(float p, float q);

Float2 fMax(Float2 p, Float2 q);

Float3 fMax(Float3 p, Float3 q);

Float4 fMax(Float4 p, Float4 q);
Float2 fMin(Float2 p, Float2 q);
Float3 fMin(Float3 p, Float3 q);
Float4 fMin(Float4 p, Float4 q);

/* 
	float dot( Float2 p, Float2 q );
	float dot( Float3 p, Float3 q );
	float dot( Float4 p, Float4 q );
*/

static inline float dot(Float2 p, Float2 q)
{
	return p.x * q.x + p.y * q.y;
}
static inline float dot(Float3 p, Float3 q)
{
	return p.x * q.x + p.y * q.y + p.z * q.z;
}
static inline float dot(Float4 p, Float4 q)
{
	return p.x * q.x + p.y * q.y + p.z * q.z + p.w * q.w;
}


Float3 cross(Float3 p, Float3 q);
Float4 cross(Float4 p, Float4 q);

float length(Float2 p);
float length(Float3 p);
float length(Float4 p);

float length_sqr(Float2 p);
float length_sqr(Float3 p);
float length_sqr(Float4 p);

float distance(Float2 p, Float2 q);
float distance(Float3 p, Float3 q);
float distance(Float4 p, Float4 q);

float distance_sqr(Float2 p, Float2 q);
float distance_sqr(Float3 p, Float3 q);
float distance_sqr(Float4 p, Float4 q);

Float2 normalize(Float2 p);
Float3 normalize(Float3 p);
Float4 normalize(Float4 p);



// functions other than just basic opencl functions

Float4 gcross(Float4 p, Float4 q, Float4 r);

struct mat2
{
	Float2 v[2];
};
struct mat3
{
	Float3 v[3];
};
struct mat4
{
	Float4 v[4];
};

float trace(mat2 p);
float trace(mat3 p);
float trace(mat4 p);

float determinant(mat2 p);
float determinant(mat3 p);
float determinant(mat4 p);

Float2 characteristic_poly(mat2 p);
Float3 characteristic_poly(mat3 p);
Float4 characteristic_poly(mat4 p);

Float2 solve_monic(Float2 p);
Float3 solve_monic(Float3 p);
Float4 solve_monic(Float4 p);

Float2 transform(mat2 p, Float2 q);
Float3 transform(mat3 p, Float3 q);
Float4 transform(mat4 p, Float4 q);

mat2 adjugate(mat2 p);
mat3 adjugate(mat3 p);
mat4 adjugate(mat4 p);

mat2 invert(mat2 p);
mat3 invert(mat3 p);
mat4 invert(mat4 p);

Float2 eigenvalues(mat2 p);
Float3 eigenvalues(mat3 p);
Float4 eigenvalues(mat4 p);

Float2 eigenvector(mat2 p, float eigvl);
Float3 eigenvector(mat3 p, float eigvl);
Float4 eigenvector(mat4 p, float eigvl);

mat2 operator *(mat2 a, mat2 b);
mat3 operator *(mat3 a, mat3 b);
mat4 operator *(mat4 a, mat4 b);



// parametric line, 2D: The line is given by line = a + b*t.
struct line2
{
	Float2 a;
	Float2 b;
};

// paramtric line, 3D
struct line3
{
	Float3 a;
	Float3 b;
};

struct line4
{
	Float4 a;
	Float4 b;
};

// plane/hyperplane defined by a point and a normal vector
struct plane_3d
{
	Float3 root_point;
	Float3 normal;				// normalized
};

struct hyperplane_4d
{
	Float4 root_point;
	Float4 normal;				// normalized
};

float param_nearest_on_line(Float2 point, line2 line);
float param_nearest_on_line(Float3 point, line3 line);
float param_nearest_on_line(Float4 point, line4 line);

float point_line_distance(Float2 point, line2 line);
float point_line_distance(Float3 point, line3 line);
float point_line_distance(Float4 point, line4 line);

float point_line_distance_sqr(Float2 point, line2 line);
float point_line_distance_sqr(Float3 point, line3 line);
float point_line_distance_sqr(Float4 point, line4 line);

float point_plane_3d_distance(Float3 point, plane_3d plane);
float point_hyperplane_4d_distance(Float4 point, hyperplane_4d plane);

plane_3d generate_plane_from_points(Float3 point0, Float3 point1, Float3 poInt2);
hyperplane_4d generate_hyperplane_from_points(Float4 point0, Float4 point1, Float4 poInt2, Float4 poInt3);


#endif
