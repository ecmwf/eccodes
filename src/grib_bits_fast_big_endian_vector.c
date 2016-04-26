/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/***************************************************************************
 *   Enrico Fucile  - 06.2010                                              *
 *                                                                         *
 ***************************************************************************/

int grib_decode_long_array(const unsigned char* p, long *bitp, long nbits,size_t size,long* val) {
  long countOfLeftmostBits=0,leftmostBits=0;
  long startBit[size],startByte,off;
  long remainingBits = nbits;
  long bitpv=*bitp;
  int inited=0;
  unsigned long uval=0;
  long i=0,n;
  unsigned long *pp=(unsigned long*)p;
  unsigned long *x;
  size_t sizel =(size*nbits)/max_nbits;
  long vaux[size],vaux2[size],vaux3[size];
  long imax,j;

/* (void) ftrace_region_begin("gdda-1"); */
  if (size*nbits%max_nbits) sizel++;

  x=malloc(sizel*sizeof(unsigned long));
  Assert(x);
  memcpy(x,p+bitpv/8,sizel*sizeof(*x));
/* (void) ftrace_region_end  ("gdda-1"); */

  if ( (max_nbits%nbits == 0) && (bitpv%nbits == 0) ) {
    
/* long GRIB_MASK = -1;        Mask of sword bits

p:x[startBit/max_nbits]
q:startBit%max_nbits
b:nbits
 define VALUE(p,q,b) \
 (((b)==max_nbits ? GRIB_MASK : ~(GRIB_MASK<<(b))) & ((p)>>(max_nbits-((q)+(b))))) */

/* (void) ftrace_region_begin("gdda-2"); */
    for (i=0;i<size;i++) {
/*    startBit=i*nbits ;
      val[i]=VALUE(x[startBit/max_nbits],startBit%max_nbits,nbits);  */
      if (nbits==max_nbits)
        vaux[i]  =GRIB_MASK;
      else
        vaux[i]  =~(GRIB_MASK<<nbits); 
      startBit[i]=i*nbits ;
      vaux3[i]=max_nbits-(startBit[i]%max_nbits+nbits);
      vaux2[i]=x[startBit[i]/max_nbits];
    }
    imax=0;
    for (i=0;i<size;i++) {
      if (vaux3[i]>imax)
         imax=vaux3[i];
    }
    for (j=1;j<=imax;j++) {
      for (i=0;i<size;i++) {
        if (j<=vaux3[i])
        vaux2[i]=vaux2[i]>>1;
/*      vaux2[i]=vaux5[i]>>vaux3[i];
        vaux2[i]=x[startBit[i]/max_nbits]>>(max_nbits-(startBit[i]%max_nbits+nbits)); */
      }
    }
    for (i=0;i<size;i++) {
      val[i]=vaux[i]   & vaux2[i];
    }
/* (void) ftrace_region_end  ("gdda-2"); */
    
  } else {

/* (void) ftrace_region_begin("gdda-3"); */
    for (i=0;i<size;i++) {

      startBit[i]=i*nbits;

      off=startBit[i]/max_nbits;
      startBit[i] %= max_nbits;

      remainingBits = nbits;
      countOfLeftmostBits = startBit[i] + nbits;
      if (countOfLeftmostBits > max_nbits) {
        countOfLeftmostBits = max_nbits - startBit[i];
        remainingBits -= countOfLeftmostBits;
        leftmostBits=(VALUE(x[off],startBit[i],countOfLeftmostBits)) << remainingBits;
	off++;
        startBit[i] = 0;
      } else 
        leftmostBits = 0;

      val[i]=(leftmostBits+(VALUE(x[off],startBit[i],remainingBits)) );
    }
/* (void) ftrace_region_end  ("gdda-3"); */
  }
/* (void) ftrace_region_begin("gdda-4"); */
  free(x);

  *bitp+=size*nbits;
/* (void) ftrace_region_end  ("gdda-4"); */

  return GRIB_SUCCESS;
}


int grib_decode_double_array(const unsigned char* p, long *bitp, long nbits,double reference_value,double s,double d,size_t size,double* val) {
  long countOfLeftmostBits=0,leftmostBits=0;
  long startBit[size],startByte,off;
  long remainingBits = nbits;
  long bitpv=*bitp;
  int inited=0;
  unsigned long uval=0;
  double fact=s*d;
  double bias=reference_value*d;
  long i=0,n;
  unsigned long *pp=(unsigned long*)p;
  unsigned long *x;
  size_t sizel =(size*nbits)/max_nbits;
  long vaux[size],vaux2[size],vaux3[size];
  long imax,j;

/* (void) ftrace_region_begin("gdda-1"); */
  if (size*nbits%max_nbits) sizel++;

  x=malloc(sizel*sizeof(unsigned long));
  Assert(x);
  memcpy(x,p+bitpv/8,sizel*sizeof(*x));
/* (void) ftrace_region_end  ("gdda-1"); */

  if ( (max_nbits%nbits == 0) && (bitpv%nbits == 0) ) {
    
/* long GRIB_MASK = -1;        Mask of sword bits

p:x[startBit/max_nbits]
q:startBit%max_nbits
b:nbits
 define VALUE(p,q,b) \
 (((b)==max_nbits ? GRIB_MASK : ~(GRIB_MASK<<(b))) & ((p)>>(max_nbits-((q)+(b))))) */

/* (void) ftrace_region_begin("gdda-2"); */
    for (i=0;i<size;i++) {
/*    startBit=i*nbits ;
      val[i]=VALUE(x[startBit/max_nbits],startBit%max_nbits,nbits);  */
      if (nbits==max_nbits)
        vaux[i]  =GRIB_MASK;
      else
        vaux[i]  =~(GRIB_MASK<<nbits); 
      startBit[i]=i*nbits ;
      vaux3[i]=max_nbits-(startBit[i]%max_nbits+nbits);
      vaux2[i]=x[startBit[i]/max_nbits];
    }
    imax=0;
    for (i=0;i<size;i++) {
      if (vaux3[i]>imax)
         imax=vaux3[i];
    }
    for (j=1;j<=imax;j++) {
      for (i=0;i<size;i++) {
        if (j<=vaux3[i])
        vaux2[i]=vaux2[i]>>1;
/*      vaux2[i]=vaux5[i]>>vaux3[i];
        vaux2[i]=x[startBit[i]/max_nbits]>>(max_nbits-(startBit[i]%max_nbits+nbits)); */
      }
    }
    for (i=0;i<size;i++) {
      val[i]=vaux[i]   & vaux2[i];
      val[i]=val[i] * fact + bias;
    }
/* (void) ftrace_region_end  ("gdda-2"); */
    
  } else {

/* (void) ftrace_region_begin("gdda-3"); */
    for (i=0;i<size;i++) {

      startBit[i]=i*nbits;

      off=startBit[i]/max_nbits;
      startBit[i] %= max_nbits;

      remainingBits = nbits;
      countOfLeftmostBits = startBit[i] + nbits;
      if (countOfLeftmostBits > max_nbits) {
        countOfLeftmostBits = max_nbits - startBit[i];
        remainingBits -= countOfLeftmostBits;
        leftmostBits=(VALUE(x[off],startBit[i],countOfLeftmostBits)) << remainingBits;
	off++;
        startBit[i] = 0;
      } else 
        leftmostBits = 0;

      val[i]=(leftmostBits+(VALUE(x[off],startBit[i],remainingBits)) ) * fact + bias;
    }
/* (void) ftrace_region_end  ("gdda-3"); */
  }
/* (void) ftrace_region_begin("gdda-4"); */
  free(x);

  *bitp+=size*nbits;
/* (void) ftrace_region_end  ("gdda-4"); */

  return GRIB_SUCCESS;
}

int grib_decode_double_array_complex(const unsigned char* p, long *bitp, long nbits,double reference_value,double s,double* restrict d,size_t size,double* restrict val) {
  long i=0;
  long countOfLeftmostBits=0,leftmostBits=0;
  long startBit,off;
  long remainingBits = nbits;
  unsigned long *pp=(unsigned long *)p;
  unsigned long bitpv=*bitp;
  unsigned long uval=0;
  unsigned long *x;
  size_t sizel =(size*nbits)/max_nbits;
  if (size*nbits%max_nbits) sizel++;

  x=malloc(sizel*sizeof(unsigned long));
  Assert(x);
  memcpy(x,p+bitpv/8,sizel*sizeof(*x));

  if ( (max_nbits%nbits == 0) && (bitpv%nbits == 0) ) {

    for (i=0;i<size;i++) {
      startBit=i*nbits ;
      val[i]= ((( (VALUE(x[startBit/max_nbits],startBit%max_nbits,nbits)) * s)+reference_value)*d[i/2]);
    }

  } else {
    for (i=0;i<size;i++) {

      startBit=i*nbits;

      off=startBit/max_nbits;
      startBit %= max_nbits;

      remainingBits = nbits;
      countOfLeftmostBits = startBit + nbits;
      if (countOfLeftmostBits > max_nbits) {
        countOfLeftmostBits = max_nbits - startBit;
        remainingBits -= countOfLeftmostBits;
        leftmostBits=(VALUE(x[off],startBit,countOfLeftmostBits)) << remainingBits;
	off++;
        startBit = 0;
      } else 
        leftmostBits = 0;

      val[i]= ((( (leftmostBits+(VALUE(x[off],startBit,remainingBits))) * s)+
	  				reference_value)*d[i/2]);
    }

  }
  free(x);

  *bitp+=size*nbits;

  return GRIB_SUCCESS;
}

int grib_encode_double_array(size_t size,const double* val,long nbits,double reference_value,double d,double divisor,unsigned char* p,long *bitp)
{
  unsigned long* restrict destination;
  long countOfLeftmostBits=0,startBit=0,remainingBits=0,rightmostBits=0;
  long off;
  size_t nbytes;
  unsigned long uval=0;
  size_t i=0;
  size_t sizel =(size*nbits)/max_nbits;
  if (size*nbits%max_nbits) sizel++;
  
  destination=malloc(sizel*sizeof(unsigned long));
  Assert(destination);

  if ( (max_nbits%nbits == 0)) {

    for (i=0;i<size;i++) {

      startBit=i*nbits;
      off=startBit/max_nbits;
      startBit %= max_nbits;

      uval  = (unsigned long)(((((val[i])*d)-reference_value)*divisor)+0.5);
      rightmostBits = VALUE(uval,max_nbits-nbits,nbits);
      destination[off] = (destination[off] & ~MASKVALUE(startBit,nbits))
                        + (rightmostBits << max_nbits-(nbits+startBit));
    }

  } else {

    for(i=0;i< size;i++){

      startBit=i*nbits;
      off=startBit/max_nbits;
      startBit %= max_nbits;
      remainingBits =nbits;

      countOfLeftmostBits = startBit + remainingBits;
      uval  = (unsigned long)((((val[i]*d)-reference_value)*divisor)+0.5);
      if (countOfLeftmostBits > max_nbits) {
        countOfLeftmostBits = max_nbits - startBit;
        startBit = max_nbits - remainingBits;
        remainingBits -= countOfLeftmostBits;
        destination[off] = (((destination[off]) >> countOfLeftmostBits) 
                               << countOfLeftmostBits)
                               + (VALUE(uval,startBit,countOfLeftmostBits));
        startBit = 0;
        off++;
      }

      rightmostBits = VALUE(uval,max_nbits-remainingBits,remainingBits);
      destination[off] = ((destination[off]) & ~MASKVALUE(startBit,remainingBits))
        + (rightmostBits << max_nbits-(remainingBits+startBit));
    }

  }

  
  nbytes=(size*nbits)/8;
  if ((size*nbits)%8) nbytes++;
  memcpy(p+*bitp,destination,nbytes);
  free(destination);

  *bitp+=size*nbits;

  return GRIB_SUCCESS;
}

int grib_encode_double_array_complex(size_t size,double* restrict val,long nbits,double reference_value,
             double* restrict scal,double d,double divisor,unsigned char* p,long *bitp) {
  unsigned long* restrict destination;
  double* v=val;
  long countOfLeftmostBits=0,startBit=0,remainingBits=0,rightmostBits=0,off=0;
  unsigned long uval=0;
  size_t i=0;
  size_t nbytes;
  size_t sizel =(size*nbits)/max_nbits;
  if (size*nbits%max_nbits) sizel++;
  
  destination=malloc(sizel*sizeof(unsigned long));
  Assert(destination);

  if ( (max_nbits%nbits == 0)) {

    for (i=0;i<size;i++) {

      startBit=i*nbits;
      off=startBit/max_nbits;
      startBit %= max_nbits;

      uval  = (unsigned long)(((((val[i])*d*scal[i/2])-reference_value)*divisor)+0.5);
      rightmostBits = VALUE(uval,max_nbits-nbits,nbits);
      destination[off] = (destination[off] & ~MASKVALUE(startBit,nbits))
                        + (rightmostBits << max_nbits-(nbits+startBit));
    }

  } else {

    for(i=0;i< size;i++){

      startBit=i*nbits;
      off=startBit/max_nbits;
      startBit %= max_nbits;
      remainingBits =nbits;

      countOfLeftmostBits = startBit + remainingBits;
      uval  = (unsigned long)((((val[i]*d*scal[i/2])-reference_value)*divisor)+0.5);
      if (countOfLeftmostBits > max_nbits) {
        countOfLeftmostBits = max_nbits - startBit;
        startBit = max_nbits - remainingBits;
        remainingBits -= countOfLeftmostBits;
        destination[off] = (((destination[off]) >> countOfLeftmostBits) 
                               << countOfLeftmostBits)
                               + (VALUE(uval,startBit,countOfLeftmostBits));
        startBit = 0;
        off++;
      }

      rightmostBits = VALUE(uval,max_nbits-remainingBits,remainingBits);
      destination[off] = ((destination[off]) & ~MASKVALUE(startBit,remainingBits))
        + (rightmostBits << max_nbits-(remainingBits+startBit));
    }

  }
  nbytes=(size*nbits)/8;
  if ((size*nbits)%8) nbytes++;
  memcpy(p+*bitp,destination,nbytes);
  free(destination);

  *bitp+=size*nbits;

  return 0;
}

