! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!  Description: issue with grid_second_order hanging when encoding fields with infinite values
!
!
USE eccodes
USE ieee_arithmetic

IMPLICIT NONE

INTEGER :: IGRIBH
REAL(KIND=8) :: ZT (421*441)

CHARACTER*32 :: CLPACKING  ! grid_simple / grid_second_order
CHARACTER*32 :: CLINFINITY ! 0 / 1

CALL GETARG (1, CLPACKING)
CALL GETARG (2, CLINFINITY)

CALL codes_grib_new_from_samples(IGRIBH, "regular_ll_sfc_grib2")
CALL codes_set_long             (IGRIBH,  "Ni",           421_8)
CALL codes_set_long             (IGRIBH,  "Nj",           441_8)
CALL codes_set_long             (IGRIBH,  "bitsPerValue", 16_8)
CALL codes_set_string           (IGRIBH,  "packingType",  TRIM (CLPACKING))

ZT = 0._8

IF (CLINFINITY == '1') THEN
  ZT (1) = IEEE_VALUE (0._8, IEEE_POSITIVE_INF)
ENDIF

CALL codes_set_real8_array (IGRIBH,  "values", ZT)
! write (*,*) 'status=',status
! write (*,*) 'ZT=    ',ZT(1)

END
