! Copyright 2005-2018 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!  Description: set string key with spaces
!
!
USE eccodes

 INTEGER :: IGRIBH
 CHARACTER (LEN=17) :: CLNOMA1, CLNOMA2

 CALL codes_grib_new_from_samples(IGRIBH, "regular_ll_sfc_grib2")

 ! set centre to MeteoFrance and use their local definition
 CALL codes_set (IGRIBH, 'centre', 85)
 CALL codes_set (IGRIBH, 'grib2LocalSectionPresent', 1)
 CALL codes_set (IGRIBH, 'grib2LocalSectionNumber', 1)

 CLNOMA1 = 'SUNSHI. DURATION'
 CALL codes_set_string(IGRIBH, 'CLNOMA', CLNOMA1)
 CALL codes_get_string(IGRIBH, 'CLNOMA', CLNOMA2)

 PRINT *, " CLNOMA1 = ", CLNOMA1
 PRINT *, " CLNOMA2 = ", CLNOMA2

 IF (CLNOMA1 /= CLNOMA2) STOP 1

END
