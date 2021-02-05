! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!  Description: set the packingType in a GRIB file
!               e.g. Simple packing, CCSDS
!
program set_packing
  use eccodes
  implicit none
  integer                         :: outfile
  integer                         :: i, igrib, iret, numberOfValues, cnt
  real                            :: d, e
  real, dimension(:), allocatable :: values
  integer, parameter              :: max_strsize = 200
  character(len=max_strsize)      :: outfile_name, packing_type

  call getarg(1, packing_type)
  call getarg(2, outfile_name)

  call codes_open_file(outfile,outfile_name,'w')

  call codes_grib_new_from_samples(igrib, 'gg_sfc_grib2')

  call codes_get_size(igrib,'values', numberOfValues)
  allocate(values(numberOfValues), stat=iret)
  d = 10e-6
  e = d
  cnt = 1
  do i=1,numberOfValues
    if (cnt>100) then
      e = e*1.01
      cnt=1
    endif
    values(i) = d
    d = d + e
    cnt = cnt + 1
  end do

  call codes_set(igrib, 'numberOfBitsContainingEachPackedValue', 16)
  call codes_set(igrib, 'packingType', packing_type)

  ! set data values
  call codes_set(igrib, 'values', values)

  call codes_write(igrib, outfile)
  call codes_release(igrib)
  deallocate(values)

end program set_packing
