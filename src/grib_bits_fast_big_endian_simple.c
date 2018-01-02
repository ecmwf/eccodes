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

int grib_decode_long_array(const unsigned char* p, long *bitp, long nbits,size_t size,long* val) {
  long i=0;
  long countOfLeftmostBits=0,leftmostBits=0;
  long startBit,startByte;
  long remainingBits = nbits;
  long *pp=(long*)p;
  int inited=0;
  unsigned long uval=0;

  startBit=*bitp;
  remainingBits = nbits;
  if (startBit >= max_nbits) {
    pp+=startBit/max_nbits;
    startBit %= max_nbits;
  }

  if ( (max_nbits%nbits == 0) && (*bitp%nbits == 0) ) {

    for (i=0;i<size;i++) {
      if (startBit == max_nbits) {
        startBit = 0;
        pp++;
      }

      val[i]=VALUE(*pp,startBit,remainingBits);
      startBit+=remainingBits;
      remainingBits=nbits;
    }

  } else {
    for (i=0;i<size;i++) {
      countOfLeftmostBits = startBit + remainingBits;
      if (countOfLeftmostBits > max_nbits) {
        countOfLeftmostBits = max_nbits - startBit;
        remainingBits -= countOfLeftmostBits;
        leftmostBits=(VALUE(*(pp++),startBit,countOfLeftmostBits)) << remainingBits;
        startBit = 0;
      } else
        leftmostBits = 0;

      val[i]=leftmostBits+(VALUE(*pp,startBit,remainingBits));
      startBit+=remainingBits;
      remainingBits=nbits;
    }

  }

  *bitp+=size*nbits;

  return GRIB_SUCCESS;
}


int grib_decode_double_array(const unsigned char* p, long *bitp, long nbits,double reference_value,double s,double d,size_t size,double* val) {
  long i=0;
  long countOfLeftmostBits=0,leftmostBits=0;
  long startBit,startByte;
  long remainingBits = nbits;
  long *pp=(long*)p;
  int inited=0;
  unsigned long uval=0;
  double fact=s*d;
  double bias=reference_value*d;

  startBit=*bitp;
  remainingBits = nbits;
  if (startBit >= max_nbits) {
    pp+=startBit/max_nbits;
    startBit %= max_nbits;
  }

  if ( (max_nbits%nbits == 0) && (*bitp%nbits == 0) ) {

    for (i=0;i<size;i++) {
      if (startBit == max_nbits) {
        startBit = 0;
        pp++;
      }

      val[i]=VALUE(*pp,startBit,remainingBits);
      val[i]= val[i] * fact + bias ;
      startBit+=remainingBits;
      remainingBits=nbits;
    }

  } else {
    for (i=0;i<size;i++) {
      countOfLeftmostBits = startBit + remainingBits;
      if (countOfLeftmostBits > max_nbits) {
        countOfLeftmostBits = max_nbits - startBit;
        remainingBits -= countOfLeftmostBits;
        leftmostBits=(VALUE(*(pp++),startBit,countOfLeftmostBits)) << remainingBits;
        startBit = 0;
      } else
        leftmostBits = 0;

      val[i]=leftmostBits+(VALUE(*pp,startBit,remainingBits));
      val[i]= val[i] * fact + bias ;
      startBit+=remainingBits;
      remainingBits=nbits;
    }

  }

  *bitp+=size*nbits;

  return GRIB_SUCCESS;
}

int grib_decode_double_array_complex(const unsigned char* p, long *bitp, long nbits,double reference_value,double s,double* d,size_t size,double* val) {
  long i=0;
  long countOfLeftmostBits=0,leftmostBits=0;
  long startBit;
  long remainingBits = nbits;
  long *pp=(long*)p;
  int inited=0;
  unsigned long uval=0;

  startBit=*bitp;
  remainingBits = nbits;
  if (startBit >= max_nbits) {
     pp+=startBit/max_nbits;
     startBit %= max_nbits;
  }

  if ( (max_nbits%nbits == 0) && (*bitp%nbits == 0) ) {
    for (i=0;i<size;i++) {
      if (startBit == max_nbits) {
        startBit = 0;
        pp++;
      }

      val[i]=VALUE(*pp,startBit,remainingBits);
      val[i]= ((( (val[i]) * s)+reference_value)*d[i/2]);
      startBit+=remainingBits;
      remainingBits=nbits;
    }

  } else {

    for (i=0;i<size;i++) {
      countOfLeftmostBits = startBit + remainingBits;
      if (countOfLeftmostBits > max_nbits) {
        countOfLeftmostBits = max_nbits - startBit;
        remainingBits -= countOfLeftmostBits;
        leftmostBits=(VALUE(*pp,startBit,countOfLeftmostBits)) << remainingBits;
        startBit = 0;
        pp++;
      } else
        leftmostBits = 0;

      val[i]=leftmostBits+(VALUE(*pp,startBit,remainingBits));
      val[i]= ((( (val[i]) * s)+reference_value)*d[i/2]);
      startBit+=remainingBits;
      remainingBits=nbits;
    }
  }

  *bitp+=size*nbits;

  return GRIB_SUCCESS;
}

int grib_encode_double_array(size_t n_vals,const double* val,long nbits,double reference_value,double d,double divisor,unsigned char* p,long *bitp)
{
  long* destination = (long*)p;
  double* v=(double*)val;
  long countOfLeftmostBits=0,startBit=0,remainingBits=0,rightmostBits=0;
  unsigned long uval=0;
  size_t i=0;

  startBit=*bitp;
  remainingBits = nbits;

  if (startBit >= max_nbits) {
    destination += startBit / max_nbits;
    startBit %= max_nbits;
  }

  if ( (max_nbits%nbits == 0) && (*bitp%nbits == 0) ) {

    for(i=0;i< n_vals;i++){
      uval  = (unsigned long)(((((*v)*d)-reference_value)*divisor)+0.5);
      if (startBit == max_nbits) {
        startBit = 0;
        destination++;
      }

      rightmostBits = VALUE(uval,max_nbits-remainingBits,remainingBits);
      *destination = ((*destination) & ~MASKVALUE(startBit,remainingBits))
        + (rightmostBits << max_nbits-(remainingBits+startBit));
      startBit+=remainingBits;
      remainingBits=nbits;
      v++;
    }

  } else {

    for(i=0;i< n_vals;i++){
      countOfLeftmostBits = startBit + remainingBits;
      uval  = (unsigned long)(((((*v)*d)-reference_value)*divisor)+0.5);
      if (countOfLeftmostBits > max_nbits) {
        countOfLeftmostBits = max_nbits - startBit;
        startBit = max_nbits - remainingBits;
        remainingBits -= countOfLeftmostBits;
        *destination = (((*destination) >> countOfLeftmostBits) << countOfLeftmostBits)
          + (VALUE(uval,startBit,countOfLeftmostBits));
        startBit = 0;
        destination++;
      }

      rightmostBits = VALUE(uval,max_nbits-remainingBits,remainingBits);
      *destination = ((*destination) & ~MASKVALUE(startBit,remainingBits))
        + (rightmostBits << max_nbits-(remainingBits+startBit));
      startBit+=remainingBits;
      remainingBits=nbits;
      v++;
    }

  }

  *bitp+=n_vals*nbits;

  return GRIB_SUCCESS;
}

int grib_encode_double_array_complex(size_t n_vals,double* val,long nbits,double reference_value,
        double* scal,double d,double divisor,unsigned char* p,long *bitp) {
  long* destination = (long*)p;
  double* v=val;
  long countOfLeftmostBits=0,startBit=0,remainingBits=0,rightmostBits=0;
  unsigned long uval=0;
  size_t i=0;

  startBit=*bitp;
  remainingBits = nbits;

  if (startBit >= max_nbits) {
    destination += startBit / max_nbits;
    startBit %= max_nbits;
  }

  if ( (max_nbits%nbits == 0) && (*bitp%nbits == 0) ) {

    for(i=0;i< n_vals;i++) {
      uval  = (unsigned long)(((((*v)*d*scal[i/2])-reference_value)*divisor)+0.5);
      if (startBit == max_nbits) {
        startBit = 0;
        destination++;
      }

      rightmostBits = VALUE(uval,max_nbits-remainingBits,remainingBits);
      *destination = ((*destination) & ~MASKVALUE(startBit,remainingBits))
        + (rightmostBits << max_nbits-(remainingBits+startBit));
      startBit+=remainingBits;
      remainingBits=nbits;

      v++;
    }

  } else {

    for(i=0;i< n_vals;i++) {
      countOfLeftmostBits = startBit + remainingBits;
      uval  = (unsigned long)(((((*v)*d*scal[i/2])-reference_value)*divisor)+0.5);
      if (countOfLeftmostBits > max_nbits) {
        countOfLeftmostBits = max_nbits - startBit;
        startBit = max_nbits - remainingBits;
        remainingBits -= countOfLeftmostBits;
        *destination = (((*destination) >> countOfLeftmostBits) << countOfLeftmostBits)
          + (VALUE(uval,startBit,countOfLeftmostBits));
        startBit = 0;
        destination++;
      }

      rightmostBits = VALUE(uval,max_nbits-remainingBits,remainingBits);
      *destination = ((*destination) & ~MASKVALUE(startBit,remainingBits))
        + (rightmostBits << max_nbits-(remainingBits+startBit));
      startBit+=remainingBits;
      remainingBits=nbits;

      v++;
    }

  }

  *bitp+=n_vals*nbits;

  return 0;
}

