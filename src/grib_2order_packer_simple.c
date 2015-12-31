/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

static unsigned long calc_pow_2(unsigned long op){
  unsigned long a = 1;
  while(op--) a*=2;
  return a;
}

static int calc_bits_needed(unsigned long spread){
  int nbit = 0;
  if (spread == 0) return nbit;
  while (spread>0){
    spread/=2;
    nbit++;
  }
  return nbit;
}

static int find_next_group(const unsigned long* vals, size_t len, unsigned long w, unsigned long l,unsigned long* nbits, unsigned long* groupsize, long* r_val){
  long lmin = 0;
  long lmax = 0;

  size_t i = 0;
  if  (len <=0 ) return GRIB_ARRAY_TOO_SMALL;
  lmin = vals[0];
  lmax = lmin;

  while(i < len){
    if(vals[i] < lmin) lmin = vals[i];
    if(vals[i] > lmax) lmax = vals[i];
    Assert((lmax-lmin) >= 0);
    *nbits = calc_bits_needed(lmax-lmin);
    *r_val = lmin;
    i++;
    *groupsize = i;

    if(*groupsize > l-2) return GRIB_SUCCESS;
    if(*nbits     > w-2) return GRIB_SUCCESS;
  }

  return GRIB_SUCCESS;
}

void grib_free_second_order_groups(grib_context *c,second_order_packed* sp){
  if(!sp) return;
  grib_context_free(c,sp->array_of_group_size);
  grib_context_free(c,sp->array_of_group_refs);
  grib_context_free(c,sp->array_of_group_width);
  grib_context_free(c,sp);
}
second_order_packed* grib_get_second_order_groups(grib_context *c, const unsigned long* vals, size_t len)
{
    second_order_packed* s = (second_order_packed*)grib_context_malloc_clear(c,sizeof(second_order_packed));
    const unsigned long* group_val = vals;
    size_t nv = len;
    size_t i = 0;
    unsigned long nbit_per_group;
    unsigned long size_of_group;
    long ref_of_group;

    s->packed_byte_count      = 0;
    s->nbits_per_group_size   = 6;
    s->nbits_per_widths       = 4;
    s->size_of_group_array    = 0;

  while(find_next_group(group_val, nv, calc_pow_2(s->nbits_per_widths), calc_pow_2(s->nbits_per_group_size ), &nbit_per_group, &size_of_group,  &ref_of_group) == GRIB_SUCCESS){
    s->size_of_group_array += 1;
    nv                   -= size_of_group;
    group_val            += size_of_group;
        s->packed_byte_count += size_of_group*nbit_per_group;
  }
    s->packed_byte_count     =  ((s->packed_byte_count+7)/8);

    s->array_of_group_size  = (unsigned long*)grib_context_malloc_clear(c,sizeof(unsigned long)*s->size_of_group_array);
    s->array_of_group_width = (unsigned long*)grib_context_malloc_clear(c,sizeof(unsigned long)*s->size_of_group_array);
    s->array_of_group_refs  = (long*)grib_context_malloc_clear(c,sizeof( long)*s->size_of_group_array);

     group_val = vals;
     nv = len;

  while(find_next_group(group_val, nv, calc_pow_2(s->nbits_per_widths), calc_pow_2(s->nbits_per_group_size ), &nbit_per_group, &size_of_group,  &ref_of_group) == GRIB_SUCCESS){
    nv                   -= size_of_group;
    group_val            += size_of_group;
        Assert(i<s->size_of_group_array);
    s->array_of_group_size[i]  = size_of_group;
    s->array_of_group_width[i] = nbit_per_group;
    s->array_of_group_refs[i]  = ref_of_group;
    i++;

  }

  return s;
}
