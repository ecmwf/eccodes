/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/***************************************************************************
 *   Enrico Fucile  - 19.06.2007                                           *
 *                                                                         *
 ***************************************************************************/

int grib_decode_long_array(const unsigned char* p, long *bitp, long bitsPerValue,
                             size_t n_vals,long* val) {
  long i=0;
  unsigned long lvalue = 0;

  if(bitsPerValue%8)
  {
    int j=0;
    for(i=0;i < n_vals;i++) {
      lvalue=0;
      for(j=0; j< bitsPerValue;j++){
        lvalue <<= 1;
        if(grib_get_bit( p, *bitp)) lvalue += 1;
        *bitp += 1;
      }
      val[i] = lvalue;
    }
  }  else  {
    int bc;
    int l = bitsPerValue/8;
    size_t o = *bitp/8;

    for(i=0;i < n_vals;i++)
    {
      lvalue  = 0;
      lvalue  <<= 8;
      lvalue |= p[o++] ;

      for ( bc=1; bc<l; bc++ )
      {
        lvalue <<= 8;
        lvalue |= p[o++] ;
      }
      val[i] = lvalue;
    }
    *bitp+=bitsPerValue*n_vals;
  }

  return 0;
}


int grib_decode_double_array(const unsigned char* p, long *bitp, long bitsPerValue,
                             double reference_value,double s,double d,
                             size_t n_vals,double* val) {
  size_t i=0;
  unsigned long lvalue = 0;

  if(bitsPerValue%8)
  {
    int j=0;
    for(i=0;i < n_vals;i++) {
      lvalue=0;
      for(j=0; j< bitsPerValue;j++){
        lvalue <<= 1;
        if(grib_get_bit( p, *bitp)) lvalue += 1;
        *bitp += 1;
      }
      val[i] = (double) (((lvalue*s)+reference_value)*d);
    }
  }  else  {
    int bc;
    int l = bitsPerValue/8;
    size_t o = 0;

    for(i=0;i < n_vals;i++)
    {
      lvalue  = 0;
      lvalue  <<= 8;
      lvalue |= p[o++] ;

      for ( bc=1; bc<l; bc++ )
      {
        lvalue <<= 8;
        lvalue |= p[o++] ;
      }
      val[i] = (double) (((lvalue*s)+reference_value)*d);
    }
  }

  return 0;
}

int grib_decode_double_array_complex(const unsigned char* p, long *bitp, long nbits,double reference_value,double s,double* d,size_t size,double* val) {
  return GRIB_NOT_IMPLEMENTED;
}

int grib_encode_double_array(size_t n_vals,const double* val,long bits_per_value,double reference_value,double d,double divisor,unsigned char* p,long *off)
{
  size_t i=0;
  unsigned long unsigned_val=0;
  unsigned char *encoded=p;
  if(bits_per_value%8){
    for(i=0;i< n_vals;i++){
      unsigned_val = (unsigned long)((((val[i]*d)-reference_value)*divisor)+0.5);
      grib_encode_unsigned_longb(encoded, unsigned_val, off , bits_per_value);
    }
  } else{
    for(i=0;i< n_vals;i++){
    int blen=0;
      blen = bits_per_value;
      unsigned_val = (unsigned long)((((val[i]*d)-reference_value)*divisor)+0.5);
      while(blen >= 8)
      {
        blen   -= 8;
        *encoded = (unsigned_val >> blen);
        encoded++;
        *off+=8;
      }
    }
  }
  return GRIB_SUCCESS;
}

int grib_encode_double_array_complex(size_t n_vals,double* val,long nbits,double reference_value,
        double* scal,double d,double divisor,unsigned char* p,long *bitp) {
  return GRIB_NOT_IMPLEMENTED;
}

