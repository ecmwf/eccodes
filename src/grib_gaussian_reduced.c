/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */
#include "grib_api_internal.h"

/*
 * C Implementation: gaussian_reduced
 *
 * Description: computes the number of points within the range
 *              lon_first->lon_last and the zero based indexes
 *              ilon_first,ilon_last of the first and last point
 *              given the number of points along a parallel (pl)
 *
 */
#define EFDEBUG 0

// Fraction struct
typedef long long Fraction_value_type;

typedef struct Fraction_type {
    Fraction_value_type top_;
    Fraction_value_type bottom_;
} Fraction_type;

/*
const Fraction_value_type MAX_DENOM = sqrt(ULLONG_MAX);
static Fraction_value_type fraction_gcd(Fraction_value_type a, Fraction_value_type b)
{
    while (b != 0)
    {
        Fraction_value_type r = a % b;
        a = b;
        b = r;
    }
    return a;
}
static Fraction_type fraction_construct(Fraction_value_type top, Fraction_value_type bottom)
{
    Fraction_type result;
    Assert(bottom != 0);

    /// @note in theory we also assume that numerator and denominator are both representable in
    ///       double without loss
    //    ASSERT(top == Fraction_value_type(double(top)));
    //    ASSERT(bottom == Fraction_value_type(double(bottom)));

    Fraction_value_type sign = 1;
    if (top < 0) {
        top = -top;
        sign = -sign;
    }

    if (bottom < 0) {
        bottom = -bottom;
        sign = -sign;
    }

    Fraction_value_type g = fraction_gcd(top, bottom);
    top =  top / g;
    bottom = bottom / g;

    result.top_ = sign * top;
    result.bottom_ = bottom;
    return result;
}

static Fraction_type fraction_construct_from_double(double x)
{
    Fraction_type result;
    double value = x;

    Assert(!std::isnan(value));
    // ASSERT(fabs(value) < 1e30);

    value_type sign = 1;
    if (x < 0) {
        sign = -sign;
        x = -x;
    }

    value_type m00 = 1, m11 = 1, m01 = 0, m10 = 0;
    value_type a = x;
    value_type t2 = m10 * a + m11;

    size_t cnt = 0;

    while (t2 <= MAX_DENOM) {

        value_type t1  = m00 * a + m01;
        m01 = m00;
        m00 = t1;

        m11 = m10;
        m10 = t2;

        if (x == a) {
            break;
        }

        x = 1.0 / (x - a);

        if (x > std::numeric_limits<Fraction::value_type>::max()) {
            break;
        }

        a = x;
        t2 = m10 * a + m11;

        if (cnt++ > 10000) {
            std::ostringstream oss;
            oss << "Cannot compute fraction from " << value << std::endl;
            throw std::runtime_error(oss.str()); //eckit::BadValue(oss.str());
        }
    }

    while (m10 >= MAX_DENOM || m00 >= MAX_DENOM) {
        m00 >>= 1;
        m10 >>= 1;
    }

    value_type top = m00;
    value_type bottom = m10;

    value_type g = gcd(top, bottom);
    top =  top / g;
    bottom = bottom / g;

    result.top_ = sign * top;
    result.bottom_ = bottom;
    return result;
}

Fraction_value_type fraction_integralPart(const Fraction_type& frac)
{
    return frac.top_ / frac.bottom_;
}

static int fraction_operator_equality(Fraction_type self, Fraction_type other)
{
    // Assumes canonical form
    return self.top_ == other.top_ && self.bottom_ == other.bottom_;
}
static double fraction_operator_double(Fraction_type self)
{
    return double(self.top_) / double(self.bottom_);
}

static Fraction_value_type fraction_mul(int* overflow, Fraction_value_type a, Fraction_value_type b)
{
    if(*overflow) { return 0; }

    if (b != 0) {
        overflow = llabs(a) > ULLONG_MAX / llabs(b);
    }

    return a * b;
}

static Fraction_type fraction_operator_divide(Fraction_type self, Fraction_type other)
{
    int overflow = 0; //boolean

    Fraction_value_type top    = fraction_mul(overflow, self.top_, other.bottom_);

    Fraction_value_type bottom = fraction_mul(overflow, self.bottom_, other.top_);

    if (!overflow) {
        return fraction_construct(top, bottom);
    }

    return Fraction(double(*this) / double(other)); //??
}


static void GaussianIteratorResetToRow(
        long long Ni_globe,   // plj
        const Fraction_type w,// lon_first
        const Fraction_type e,// lon_last
        long*   pNi, // npoints
        double* pLon1,
        double* pLon2)
{
    assert(Ni_globe > 1);
    Fraction_type inc = fraction_construct(360ll, Ni_globe);
    //eckit::Fraction inc(360ll, Ni_globe);

    //auto Nw = (w / inc).integralPart();
    Fraction_value_type Nw = fraction_integralPart( fraction_operator_divide(w, inc) );
    if (Nw * inc < w) { //??
        Nw += 1;
    }

    auto Ne = (e / inc).integralPart();
    if (Ne * inc > e && Ne > Nw) {
        Ne -= 1;
    }
    assert(Ne >= Nw);

    Ni = std::min(Ni_globe, Ne - Nw + 1);
    lon1 = Nw * inc;
    lon2 = Ne * inc;
}
*/

/* --------------------------------------------------------------------------------------------------- */
void grib_get_reduced_row_wrapper(grib_handle* h, long pl, double lon_first, double lon_last, long* npoints, long* ilon_first, long* ilon_last)
{
    int err = 0;
    long expandedBoundingBox = 0;
    err = grib_get_long(h, "expandedBoundingBox", &expandedBoundingBox);
    if (!err && expandedBoundingBox == 1) {
        grib_get_reduced_row2(pl, lon_first, lon_last, npoints, ilon_first, ilon_last);
    } else {
        grib_get_reduced_row(pl, lon_first, lon_last, npoints, ilon_first, ilon_last);
    }
}

void grib_get_reduced_row(long pl, double lon_first, double lon_last, long* npoints, long* ilon_first, long* ilon_last )
{
  double range=0,dlon_first=0,dlon_last=0;
  long irange;

  range=lon_last-lon_first;
  if (range<0) {range+=360;lon_first-=360;}

  /* computing integer number of points and coordinates without using floating point resolution*/
  *npoints=(range*pl)/360.0+1;
  *ilon_first=(lon_first*pl)/360.0;
  *ilon_last=(lon_last*pl)/360.0;

  irange=*ilon_last-*ilon_first+1;

#if EFDEBUG
  printf("  pl=%ld npoints=%ld range=%.10e ilon_first=%ld ilon_last=%ld irange=%ld\n",
    pl,*npoints,range,*ilon_first,*ilon_last,irange);
#endif

  if (irange != *npoints) {
#if EFDEBUG
    printf("       ---> (irange=%ld) != (npoints=%ld) ",irange,*npoints);
#endif
    if (irange > *npoints) {
      /* checking if the first point is out of range*/
      dlon_first=((*ilon_first)*360.0)/pl;
      if (dlon_first < lon_first) {(*ilon_first)++;irange--;
#if EFDEBUG
        printf(" dlon_first=%.10e < lon_first=%.10e\n",dlon_first,lon_first );
#endif
      }

      /* checking if the last point is out of range*/
      dlon_last=((*ilon_last)*360.0)/pl;
      if (dlon_last > lon_last) {(*ilon_last)--;irange--;
#if EFDEBUG
        printf(" dlon_last=%.10e < lon_last=%.10e\n",dlon_last,lon_last );
#endif
      }
    } else {
      int ok=0;
      /* checking if the point before the first is in the range*/
      dlon_first=((*ilon_first-1)*360.0)/pl;
      if (dlon_first > lon_first) {(*ilon_first)--;irange++;ok=1;
#if EFDEBUG
        printf(" dlon_first1=%.10e > lon_first=%.10e\n",dlon_first,lon_first );
#endif
      }

      /* checking if the point after the last is in the range*/
      dlon_last=((*ilon_last+1)*360.0)/pl;
      if (dlon_last < lon_last) {(*ilon_last)++;irange++;ok=1;
#if EFDEBUG
        printf(" dlon_last1=%.10e > lon_last=%.10e\n",dlon_last,lon_first );
#endif
      }

      /* if neither of the two are triggered then npoints is too large */
      if (!ok) {(*npoints)--;
#if EFDEBUG
        printf(" (*npoints)--=%ld\n",*npoints);
#endif
      }
    }

    Assert(*npoints==irange);
#if EFDEBUG
	printf("--  pl=%ld npoints=%ld range=%.10e ilon_first=%ld ilon_last=%ld irange=%ld\n",
		   pl,*npoints,range,*ilon_first,*ilon_last,irange);
#endif
  } else {
	  /* checking if the first point is out of range*/
	  dlon_first=((*ilon_first)*360.0)/pl;
	  if (dlon_first < lon_first) {
		  (*ilon_first)++;(*ilon_last)++;
#if EFDEBUG
		  printf("       ---> dlon_first=%.10e < lon_first=%.10e\n",dlon_first,lon_first );
		  printf("--  pl=%ld npoints=%ld range=%.10e ilon_first=%ld ilon_last=%ld irange=%ld\n",
				 pl,*npoints,range,*ilon_first,*ilon_last,irange);
#endif
	  }

  }

  if (*ilon_first<0) *ilon_first+=pl;

  return;
}

void grib_get_reduced_row2(long pl, double lon_first, double lon_last, long* npoints, long* ilon_first, long* ilon_last )
{
    double range=0,dlon_first=0,dlon_last=0;
    long irange;

    range=lon_last-lon_first;
    if (range<0) {range+=360;lon_first-=360;}

    /* computing integer number of points and coordinates without using floating point resolution*/
    *npoints=(range*pl)/360.0+1;
    *ilon_first=(lon_first*pl)/360.0;
    *ilon_last=(lon_last*pl)/360.0;

    irange=*ilon_last-*ilon_first+1;

#if EFDEBUG
    printf("  pl=%ld npoints=%ld range=%.10e ilon_first=%ld ilon_last=%ld irange=%ld\n",
            pl,*npoints,range,*ilon_first,*ilon_last,irange);
#endif

    if (irange != *npoints) {
#if EFDEBUG
        printf("       ---> (irange=%ld) != (npoints=%ld) ",irange,*npoints);
#endif
        if (irange > *npoints) {
            /* checking if the first point is out of range*/
            dlon_first=((*ilon_first)*360.0)/pl;
            if (dlon_first < lon_first) {(*ilon_first)++;irange--;
#if EFDEBUG
            printf(" dlon_first=%.10e < lon_first=%.10e\n",dlon_first,lon_first );
#endif
            }

            /* checking if the last point is out of range*/
            dlon_last=((*ilon_last)*360.0)/pl;
            if (dlon_last > lon_last) {(*ilon_last)--;irange--;
#if EFDEBUG
            printf(" dlon_last=%.10e < lon_last=%.10e\n",dlon_last,lon_last );
#endif
            }
        } else {
            int ok=0;
            /* checking if the point before the first is in the range*/
            dlon_first=((*ilon_first-1)*360.0)/pl;
            if (dlon_first > lon_first) {(*ilon_first)--;irange++;ok=1;
#if EFDEBUG
            printf(" dlon_first1=%.10e > lon_first=%.10e\n",dlon_first,lon_first );
#endif
            }

            /* checking if the point after the last is in the range*/
            dlon_last=((*ilon_last+1)*360.0)/pl;
            if (dlon_last < lon_last) {(*ilon_last)++;irange++;ok=1;
#if EFDEBUG
            printf(" dlon_last1=%.10e > lon_last=%.10e\n",dlon_last,lon_first );
#endif
            }

            /* if neither of the two are triggered then npoints is too large */
            if (!ok) {(*npoints)--;
#if EFDEBUG
            printf(" (*npoints)--=%ld\n",*npoints);
#endif
            }
        }

        Assert(*npoints==irange);
#if EFDEBUG
        printf("--  pl=%ld npoints=%ld range=%.10e ilon_first=%ld ilon_last=%ld irange=%ld\n",
                pl,*npoints,range,*ilon_first,*ilon_last,irange);
#endif
    } else {
        /* checking if the first point is out of range*/
        dlon_first=((*ilon_first)*360.0)/pl;
        if (dlon_first < lon_first) {
            (*ilon_first)++;(*ilon_last)++;
#if EFDEBUG
            printf("       ---> dlon_first=%.10e < lon_first=%.10e\n",dlon_first,lon_first );
            printf("--  pl=%ld npoints=%ld range=%.10e ilon_first=%ld ilon_last=%ld irange=%ld\n",
                    pl,*npoints,range,*ilon_first,*ilon_last,irange);
#endif
        }
        /* checking if the last point is out of range*/
        dlon_last=((*ilon_last)*360.0)/pl;
        if (dlon_last > lon_last) {(*ilon_last)--;(*npoints)--;
#if EFDEBUG
        printf(" dlon_last=%.10e < lon_last=%.10e\n",dlon_last,lon_last );
#endif
        }
    }

    if (*ilon_first<0) *ilon_first+=pl;

    return;
}
