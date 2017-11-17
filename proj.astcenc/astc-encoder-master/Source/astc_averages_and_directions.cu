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
 *	@brief	Implements functions for finding dominant direction of a set of
 *			colors, using ARM patent pending method.
 */ 
/*----------------------------------------------------------------------------*/ 

#include "astc_codec_internals.cuh"

#include <math.h>
#include "mathlib.cuh"

#ifdef DEBUG_CAPTURE_NAN
	#ifndef _GNU_SOURCE
		#define _GNU_SOURCE
	#endif

	#include <fenv.h>
#endif

/* routines to compute average colors and dominant directions for blocks with 3 and 4 components. */

/*
	for a full block, functions to compute averages and dominant directions. The averages and directions are computed separately for each partition.
	We have separate versions for blocks with and without alpha, since the processing for blocks with alpha is significantly more expensive.
	The direction vectors it produces are NOT normalized. 
*/
void compute_averages_and_directions_rgba(const partition_info * pt,
										  const imageblock * blk,
										  const error_weight_block * ewb,
										  const Float4 * color_scalefactors,
										  Float4 * averages, Float4 * directions_rgba, Float3 * directions_gba, Float3 * directions_rba, Float3 * directions_rga, Float3 * directions_rgb)
{
	int i;
	int partition_count = pt->partition_count;
	int partition;

	for (partition = 0; partition < partition_count; partition++)
	{
		const uint8_t *weights = pt->texels_of_partition[partition];
		int texelcount = pt->texels_per_partition[partition];

		Float4 base_sum = Float4(0, 0, 0, 0);
		float partition_weight = 0.0f;

		for (i = 0; i < texelcount; i++)
		{
			int iwt = weights[i];
			float weight = ewb->texel_weight[iwt];
			Float4 texel_datum = Float4(blk->work_data[4 * iwt],
										blk->work_data[4 * iwt + 1],
										blk->work_data[4 * iwt + 2],
										blk->work_data[4 * iwt + 3]) * weight;
			partition_weight += weight;

			base_sum = base_sum + texel_datum;
		}

		Float4 average = base_sum * 1.0f / MAX(partition_weight, 1e-7f);
		averages[partition] = average * color_scalefactors[partition];


		Float4 sum_xp = Float4(0, 0, 0, 0);
		Float4 sum_yp = Float4(0, 0, 0, 0);
		Float4 sum_zp = Float4(0, 0, 0, 0);
		Float4 sum_wp = Float4(0, 0, 0, 0);

		for (i = 0; i < texelcount; i++)
		{
			int iwt = weights[i];
			float weight = ewb->texel_weight[iwt];
			Float4 texel_datum = Float4(blk->work_data[4 * iwt],
										blk->work_data[4 * iwt + 1],
										blk->work_data[4 * iwt + 2],
										blk->work_data[4 * iwt + 3]);
			texel_datum = (texel_datum - average) * weight;

			if (texel_datum.x > 0.0f)
				sum_xp = sum_xp + texel_datum;
			if (texel_datum.y > 0.0f)
				sum_yp = sum_yp + texel_datum;
			if (texel_datum.z > 0.0f)
				sum_zp = sum_zp + texel_datum;
			if (texel_datum.w > 0.0f)
				sum_wp = sum_wp + texel_datum;
		}

		float prod_xp = dot(sum_xp, sum_xp);
		float prod_yp = dot(sum_yp, sum_yp);
		float prod_zp = dot(sum_zp, sum_zp);
		float prod_wp = dot(sum_wp, sum_wp);

		Float4 best_vector = sum_xp;
		float best_sum = prod_xp;
		if (prod_yp > best_sum)
		{
			best_vector = sum_yp;
			best_sum = prod_yp;
		}
		if (prod_zp > best_sum)
		{
			best_vector = sum_zp;
			best_sum = prod_zp;
		}
		if (prod_wp > best_sum)
		{
			best_vector = sum_wp;
			best_sum = prod_wp;
		}

		directions_rgba[partition] = best_vector;
		directions_rgb[partition] = best_vector.xyz;
		directions_rga[partition] = best_vector.xyw;
		directions_rba[partition] = best_vector.xzw;
		directions_gba[partition] = best_vector.yzw;
	}
}




void compute_averages_and_directions_rgb(const partition_info * pt,
										 const imageblock * blk,
										 const error_weight_block * ewb,
										 const Float4 * color_scalefactors, Float3 * averages, Float3 * directions_rgb, Float2 * directions_rg, Float2 * directions_rb, Float2 * directions_gb)
{
	int i;
	int partition_count = pt->partition_count;
	int partition;

	const float *texel_weights = ewb->texel_weight_rgb;

	for (partition = 0; partition < partition_count; partition++)
	{
		const uint8_t *weights = pt->texels_of_partition[partition];
		int texelcount = pt->texels_per_partition[partition];

		Float3 base_sum = Float3(0, 0, 0);
		float partition_weight = 0.0f;

		for (i = 0; i < texelcount; i++)
		{
			int iwt = weights[i];
			float weight = texel_weights[iwt];
			Float3 texel_datum = Float3(blk->work_data[4 * iwt],
										blk->work_data[4 * iwt + 1],
										blk->work_data[4 * iwt + 2]) * weight;
			partition_weight += weight;

			base_sum = base_sum + texel_datum;
		}

		Float4 csf = color_scalefactors[partition];
		Float3 average = base_sum * 1.0f / MAX(partition_weight, 1e-7f);
		averages[partition] = average * csf.xyz;


		Float3 sum_xp = Float3(0, 0, 0);
		Float3 sum_yp = Float3(0, 0, 0);
		Float3 sum_zp = Float3(0, 0, 0);

		for (i = 0; i < texelcount; i++)
		{
			int iwt = weights[i];
			float weight = texel_weights[iwt];
			Float3 texel_datum = Float3(blk->work_data[4 * iwt],
										blk->work_data[4 * iwt + 1],
										blk->work_data[4 * iwt + 2]);
			texel_datum = (texel_datum - average) * weight;

			if (texel_datum.x > 0.0f)
				sum_xp = sum_xp + texel_datum;
			if (texel_datum.y > 0.0f)
				sum_yp = sum_yp + texel_datum;
			if (texel_datum.z > 0.0f)
				sum_zp = sum_zp + texel_datum;
		}

		float prod_xp = dot(sum_xp, sum_xp);
		float prod_yp = dot(sum_yp, sum_yp);
		float prod_zp = dot(sum_zp, sum_zp);

		Float3 best_vector = sum_xp;
		float best_sum = prod_xp;
		if (prod_yp > best_sum)
		{
			best_vector = sum_yp;
			best_sum = prod_yp;
		}
		if (prod_zp > best_sum)
		{
			best_vector = sum_zp;
			best_sum = prod_zp;
		}

		directions_rgb[partition] = best_vector;
		directions_gb[partition] = best_vector.yz;
		directions_rb[partition] = best_vector.xz;
		directions_rg[partition] = best_vector.xy;
	}
}

void compute_averages_and_directions_3_components(const partition_info * pt,
												  const imageblock * blk,
												  const error_weight_block * ewb,
												  const Float3 * color_scalefactors, int component1, int component2, int component3, Float3 * averages, Float3 * directions)
{
	int i;
	int partition_count = pt->partition_count;
	int partition;

	const float *texel_weights;
	if (component1 == 1 && component2 == 2 && component3 == 3)
		texel_weights = ewb->texel_weight_gba;
	else if (component1 == 0 && component2 == 2 && component3 == 3)
		texel_weights = ewb->texel_weight_rba;
	else if (component1 == 0 && component2 == 1 && component3 == 3)
		texel_weights = ewb->texel_weight_rga;
	else if (component1 == 0 && component2 == 1 && component3 == 2)
		texel_weights = ewb->texel_weight_rgb;
	else
	{
		texel_weights = ewb->texel_weight_gba;
		ASTC_CODEC_INTERNAL_ERROR;
	}


	for (partition = 0; partition < partition_count; partition++)
	{
		const uint8_t *weights = pt->texels_of_partition[partition];
		int texelcount = pt->texels_per_partition[partition];

		Float3 base_sum = Float3(0, 0, 0);
		float partition_weight = 0.0f;

		for (i = 0; i < texelcount; i++)
		{
			int iwt = weights[i];
			float weight = texel_weights[iwt];
			Float3 texel_datum = Float3(blk->work_data[4 * iwt + component1],
										blk->work_data[4 * iwt + component2],
										blk->work_data[4 * iwt + component3]) * weight;
			partition_weight += weight;

			base_sum = base_sum + texel_datum;
		}

		Float3 csf = color_scalefactors[partition];

		Float3 average = base_sum * 1.0f / MAX(partition_weight, 1e-7f);
		averages[partition] = average * csf.xyz;


		Float3 sum_xp = Float3(0, 0, 0);
		Float3 sum_yp = Float3(0, 0, 0);
		Float3 sum_zp = Float3(0, 0, 0);

		for (i = 0; i < texelcount; i++)
		{
			int iwt = weights[i];
			float weight = texel_weights[iwt];
			Float3 texel_datum = Float3(blk->work_data[4 * iwt + component1],
										blk->work_data[4 * iwt + component2],
										blk->work_data[4 * iwt + component3]);
			texel_datum = (texel_datum - average) * weight;

			if (texel_datum.x > 0.0f)
				sum_xp = sum_xp + texel_datum;
			if (texel_datum.y > 0.0f)
				sum_yp = sum_yp + texel_datum;
			if (texel_datum.z > 0.0f)
				sum_zp = sum_zp + texel_datum;
		}

		float prod_xp = dot(sum_xp, sum_xp);
		float prod_yp = dot(sum_yp, sum_yp);
		float prod_zp = dot(sum_zp, sum_zp);

		Float3 best_vector = sum_xp;
		float best_sum = prod_xp;
		if (prod_yp > best_sum)
		{
			best_vector = sum_yp;
			best_sum = prod_yp;
		}
		if (prod_zp > best_sum)
		{
			best_vector = sum_zp;
			best_sum = prod_zp;
		}

		if (dot(best_vector, best_vector) < 1e-18)
			best_vector = Float3(1, 1, 1);
		directions[partition] = best_vector;
	}

}




void compute_averages_and_directions_2_components(const partition_info * pt,
												  const imageblock * blk,
												  const error_weight_block * ewb, const Float2 * color_scalefactors, int component1, int component2, Float2 * averages, Float2 * directions)
{
	int i;
	int partition_count = pt->partition_count;
	int partition;

	const float *texel_weights;
	if (component1 == 0 && component2 == 1)
		texel_weights = ewb->texel_weight_rg;
	else if (component1 == 0 && component2 == 2)
		texel_weights = ewb->texel_weight_rb;
	else if (component1 == 1 && component2 == 2)
		texel_weights = ewb->texel_weight_gb;
	else
	{
		texel_weights = ewb->texel_weight_rg;
		// unsupported set of color components.
		ASTC_CODEC_INTERNAL_ERROR;
		exit(1);
	}


	for (partition = 0; partition < partition_count; partition++)
	{
		const uint8_t *weights = pt->texels_of_partition[partition];
		int texelcount = pt->texels_per_partition[partition];

		Float2 base_sum = Float2(0, 0);
		float partition_weight = 0.0f;

		for (i = 0; i < texelcount; i++)
		{
			int iwt = weights[i];
			float weight = texel_weights[iwt];
			Float2 texel_datum = Float2(blk->work_data[4 * iwt + component1],
										blk->work_data[4 * iwt + component2]) * weight;
			partition_weight += weight;

			base_sum = base_sum + texel_datum;
		}

		Float2 csf = color_scalefactors[partition];

		Float2 average = base_sum * 1.0f / MAX(partition_weight, 1e-7f);
		averages[partition] = average * csf.xy;


		Float2 sum_xp = Float2(0, 0);
		Float2 sum_yp = Float2(0, 0);

		for (i = 0; i < texelcount; i++)
		{
			int iwt = weights[i];
			float weight = texel_weights[iwt];
			Float2 texel_datum = Float2(blk->work_data[4 * iwt + component1],
										blk->work_data[4 * iwt + component2]);
			texel_datum = (texel_datum - average) * weight;

			if (texel_datum.x > 0.0f)
				sum_xp = sum_xp + texel_datum;
			if (texel_datum.y > 0.0f)
				sum_yp = sum_yp + texel_datum;
		}

		float prod_xp = dot(sum_xp, sum_xp);
		float prod_yp = dot(sum_yp, sum_yp);

		Float2 best_vector = sum_xp;
		float best_sum = prod_xp;
		if (prod_yp > best_sum)
		{
			best_vector = sum_yp;
			best_sum = prod_yp;
		}

		directions[partition] = best_vector;
	}

}


#define XPASTE(x,y) x##y
#define PASTE(x,y) XPASTE(x,y)

#define TWO_COMPONENT_ERROR_FUNC( funcname, c0_iwt, c1_iwt, c01_name, c01_rname ) \
float funcname( \
	const partition_info *pt, \
	const imageblock *blk, \
	const error_weight_block *ewb, \
	const processed_line2 *plines, \
	float *length_of_lines \
	) \
	{ \
	int i; \
	float errorsum = 0.0f; \
	int partition; \
	for(partition=0; partition<pt->partition_count; partition++) \
		{ \
		const uint8_t *weights = pt->texels_of_partition[ partition ]; \
		int texelcount = pt->texels_per_partition[ partition ]; \
		float lowparam = 1e10f; \
		float highparam = -1e10f; \
		processed_line2 l = plines[partition]; \
		if( ewb->contains_zeroweight_texels ) \
			{ \
			for(i=0;i<texelcount;i++) \
				{ \
				int iwt = weights[i]; \
				float texel_weight = ewb-> PASTE(texel_weight_ , c01_rname) [i]; \
				if( texel_weight > 1e-20f ) \
					{ \
					Float2 point = Float2(blk->work_data[4*iwt + c0_iwt], blk->work_data[4*iwt + c1_iwt] ); \
					float param = dot( point, l.bs ); \
					Float2 rp1 = l.amod + param*l.bis; \
					Float2 dist = rp1 - point; \
					Float4 ews = ewb->error_weights[iwt]; \
					errorsum += dot( ews. c01_name, dist*dist ); \
					if( param < lowparam ) lowparam = param; \
					if( param > highparam ) highparam = param; \
					} \
				} \
			} \
		else \
			{ \
			for(i=0;i<texelcount;i++) \
				{ \
				int iwt = weights[i]; \
				Float2 point = Float2(blk->work_data[4*iwt + c0_iwt], blk->work_data[4*iwt + c1_iwt] ); \
				float param = dot( point, l.bs ); \
				Float2 rp1 = l.amod + param*l.bis; \
				Float2 dist = rp1 - point; \
				Float4 ews = ewb->error_weights[iwt]; \
				errorsum += dot( ews. c01_name, dist*dist ); \
				if( param < lowparam ) lowparam = param; \
				if( param > highparam ) highparam = param; \
				} \
			} \
		float linelen = highparam - lowparam; \
		if( !(linelen > 1e-7f) ) \
			linelen = 1e-7f; \
		length_of_lines[partition] = linelen; \
		} \
	return errorsum; \
	}


TWO_COMPONENT_ERROR_FUNC(compute_error_squared_rg, 0, 1, xy, rg)
TWO_COMPONENT_ERROR_FUNC(compute_error_squared_rb, 0, 2, xz, rb)
TWO_COMPONENT_ERROR_FUNC(compute_error_squared_gb, 1, 2, yz, gb)
TWO_COMPONENT_ERROR_FUNC(compute_error_squared_ra, 0, 3, zw, ra)

// function to compute the error across a tile when using a particular set of
// lines for a particular partitioning. Also compute the length of each
// color-space line in each partitioning.

#define THREE_COMPONENT_ERROR_FUNC( funcname, c0_iwt, c1_iwt, c2_iwt, c012_name, c012_rname ) \
float funcname( \
	const partition_info *pt, \
	const imageblock *blk, \
	const error_weight_block *ewb, \
	const processed_line3 *plines, \
	float *length_of_lines \
	) \
	{ \
	int i; \
	float errorsum = 0.0f; \
	int partition; \
	for(partition=0; partition<pt->partition_count; partition++) \
		{ \
		const uint8_t *weights = pt->texels_of_partition[ partition ]; \
		int texelcount = pt->texels_per_partition[ partition ]; \
		float lowparam = 1e10f; \
		float highparam = -1e10f; \
		processed_line3 l = plines[partition]; \
		if( ewb->contains_zeroweight_texels ) \
			{ \
			for(i=0;i<texelcount;i++) \
				{ \
				int iwt = weights[i]; \
				float texel_weight = ewb-> PASTE(texel_weight_ , c012_rname) [i]; \
				if( texel_weight > 1e-20f ) \
					{ \
					Float3 point = Float3(blk->work_data[4*iwt + c0_iwt], blk->work_data[4*iwt + c1_iwt], blk->work_data[4*iwt + c2_iwt] ); \
					float param = dot( point, l.bs ); \
					Float3 rp1 = l.amod + param*l.bis; \
					Float3 dist = rp1 - point; \
					Float4 ews = ewb->error_weights[iwt]; \
					errorsum += dot( ews. c012_name, dist*dist ); \
					if( param < lowparam ) lowparam = param; \
					if( param > highparam ) highparam = param; \
					} \
				} \
			} \
		else \
			{ \
			for(i=0;i<texelcount;i++) \
				{ \
				int iwt = weights[i]; \
				Float3 point = Float3(blk->work_data[4*iwt + c0_iwt], blk->work_data[4*iwt + c1_iwt], blk->work_data[4*iwt + c2_iwt] ); \
				float param = dot( point, l.bs ); \
				Float3 rp1 = l.amod + param*l.bis; \
				Float3 dist = rp1 - point; \
				Float4 ews = ewb->error_weights[iwt]; \
				errorsum += dot( ews. c012_name, dist*dist ); \
				if( param < lowparam ) lowparam = param; \
				if( param > highparam ) highparam = param; \
				} \
			} \
		float linelen = highparam - lowparam; \
		if( !(linelen > 1e-7f) ) \
			linelen = 1e-7f; \
		length_of_lines[partition] = linelen; \
		} \
	return errorsum; \
	}

THREE_COMPONENT_ERROR_FUNC(compute_error_squared_gba, 1, 2, 3, yzw, gba)
THREE_COMPONENT_ERROR_FUNC(compute_error_squared_rba, 0, 2, 3, xzw, rba)
THREE_COMPONENT_ERROR_FUNC(compute_error_squared_rga, 0, 1, 3, xyw, rga)
THREE_COMPONENT_ERROR_FUNC(compute_error_squared_rgb, 0, 1, 2, xyz, rgb)

float compute_error_squared_rgba(const partition_info * pt,	// the partition that we use when computing the squared-error.
								 const imageblock * blk, const error_weight_block * ewb, const processed_line4 * plines, float *length_of_lines)
{
	int i;

	float errorsum = 0.0f;
	int partition;
	for (partition = 0; partition < pt->partition_count; partition++)
	{
		const uint8_t *weights = pt->texels_of_partition[partition];
		int texelcount = pt->texels_per_partition[partition];
		float lowparam = 1e10;
		float highparam = -1e10;

		processed_line4 l = plines[partition];

		if (ewb->contains_zeroweight_texels)
		{
			for (i = 0; i < texelcount; i++)
			{
				int iwt = weights[i];
				if (ewb->texel_weight[iwt] > 1e-20)
				{
					Float4 point = Float4(blk->work_data[4 * iwt], blk->work_data[4 * iwt + 1], blk->work_data[4 * iwt + 2], blk->work_data[4 * iwt + 3]);
					float param = dot(point, l.bs);
					Float4 rp1 = l.amod + param * l.bis;
					Float4 dist = rp1 - point;
					Float4 ews = ewb->error_weights[iwt];
					errorsum += dot(ews, dist * dist);
					if (param < lowparam)
						lowparam = param;
					if (param > highparam)
						highparam = param;
				}
			}
		}
		else
		{
			for (i = 0; i < texelcount; i++)
			{
				int iwt = weights[i];
				Float4 point = Float4(blk->work_data[4 * iwt], blk->work_data[4 * iwt + 1], blk->work_data[4 * iwt + 2], blk->work_data[4 * iwt + 3]);
				float param = dot(point, l.bs);
				Float4 rp1 = l.amod + param * l.bis;
				Float4 dist = rp1 - point;
				Float4 ews = ewb->error_weights[iwt];
				errorsum += dot(ews, dist * dist);
				if (param < lowparam)
					lowparam = param;
				if (param > highparam)
					highparam = param;
			}
		}

		float linelen = highparam - lowparam;
		if (!(linelen > 1e-7f))
			linelen = 1e-7f;
		length_of_lines[partition] = linelen;
	}

	return errorsum;
}



// function to compute the error across a tile when using a particular line for
// a particular partition.
float compute_error_squared_rgb_single_partition(int partition_to_test, int xdim, int ydim, int zdim, const partition_info * pt,	// the partition that we use when computing the squared-error.
												 const imageblock * blk, const error_weight_block * ewb, const processed_line3 * lin	// the line for the partition.
	)
{
	int i;

	int texels_per_block = xdim * ydim * zdim;

	float errorsum = 0.0f;

	for (i = 0; i < texels_per_block; i++)
	{
		int partition = pt->partition_of_texel[i];
		float texel_weight = ewb->texel_weight_rgb[i];
		if (partition != partition_to_test || texel_weight < 1e-20)
			continue;
		Float3 point = Float3(blk->work_data[4 * i], blk->work_data[4 * i + 1], blk->work_data[4 * i + 2]);

		float param = dot(point, lin->bs);
		Float3 rp1 = lin->amod + param * lin->bis;
		Float3 dist = rp1 - point;
		Float4 ews = ewb->error_weights[i];

		errorsum += dot(ews.xyz, dist * dist);
	}
	return errorsum;
}
