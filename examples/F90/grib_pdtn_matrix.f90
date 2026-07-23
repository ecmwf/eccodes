! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!  Description: Demonstrate the PDTN matrix auto-selection mechanism.
!    When a key is not present in the current Product Definition Template,
!    ecCodes automatically selects a compatible template that supports it.
!
!    Requires: ECCODES_PDTN_MATRIX_ENABLE=1
!
program grib_pdtn_matrix
   use eccodes
   implicit none

   integer              :: infile, outfile
   integer              :: igrib
   integer(kind=4)      :: pdtn_before, pdtn_after, tosp
   character(len=64)    :: env_val

   ! Check that the matrix mechanism is enabled
   call get_environment_variable('ECCODES_PDTN_MATRIX_ENABLE', env_val)
   if (trim(env_val) == '' .or. trim(env_val) == '0') then
      print *, 'Set ECCODES_PDTN_MATRIX_ENABLE=1 to enable the PDTN matrix mechanism'
      stop 1
   end if

   call codes_open_file(infile,  'sample.grib2',       'r')
   call codes_open_file(outfile, 'out_pdtn_matrix.grib2', 'w')

   call codes_grib_new_from_file(infile, igrib)

   ! Report the starting template number
   call codes_get(igrib, 'productDefinitionTemplateNumber', pdtn_before)
   print *, 'productDefinitionTemplateNumber before:', pdtn_before

   ! typeOfStatisticalProcessing is not defined in PDT 0.
   ! The matrix will select PDT 8 (time-range statistically processed deterministic).
   call codes_set(igrib, 'typeOfStatisticalProcessing', 0)   ! 0 = Average

   call codes_get(igrib, 'productDefinitionTemplateNumber', pdtn_after)
   call codes_get(igrib, 'typeOfStatisticalProcessing',     tosp)

   print *, 'productDefinitionTemplateNumber after: ', pdtn_after
   print *, 'typeOfStatisticalProcessing:           ', tosp

   if (pdtn_after == pdtn_before) then
      print *, 'ERROR: expected a PDTN switch'
      stop 1
   end if
   if (tosp /= 0) then
      print *, 'ERROR: unexpected typeOfStatisticalProcessing value'
      stop 1
   end if

   call codes_write(igrib, outfile)
   call codes_release(igrib)

   call codes_close_file(infile)
   call codes_close_file(outfile)

   print *, 'Written to out_pdtn_matrix.grib2'

end program grib_pdtn_matrix
