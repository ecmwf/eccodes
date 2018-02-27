! Copyright 2005-2018 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!
!  Description: set the data contained in a GRIB file.
!               In this example no missing values are present.
!               If there are missing values, refer to: grib_set_bitmap
!
program set_data
  use eccodes
  implicit none
  integer                         :: outfile
  integer                         :: i, igrib, iret, numberOfValues, cnt
  real                            :: d, e
  real, dimension(:), allocatable :: values
  integer, parameter              :: max_strsize = 200
  character(len=max_strsize)      :: outfile_name

  call getarg(1, outfile_name)
  call codes_open_file(outfile,outfile_name,'w')

  ! Note: the full name of the sample file is "regular_ll_pl_grib1.tmpl"
  ! Sample files are stored in the samples directory (use codes_info to
  ! see where that is).  The default sample path can be changed by
  ! setting the environment variable ECCODES_SAMPLES_PATH
  call codes_grib_new_from_samples(igrib, 'regular_ll_pl_grib1')

  ! Here we're changing the data values only, so the number of values
  ! will be the same as the sample GRIB.
  ! But if your data array has a different size, then specify the grid geometry
  ! (e.g. keys Ni, Nj etc) and set the correct number of data values
  call codes_get_size(igrib,'values',numberOfValues)

  allocate(values(numberOfValues), stat=iret)
  d = 10e-8
  e = d
  cnt = 1
  do i=1,numberOfValues
    if (cnt>100) then
      e = e*10
      cnt=1
    endif
    values(i) = d
    !print *, values(i)
    d = d + e
    cnt = cnt + 1
  end do

  call codes_set(igrib, 'bitsPerValue', 16)

  ! set data values
  call codes_set(igrib,'values', values)
  call codes_write(igrib,outfile)
  call codes_release(igrib)
  deallocate(values)

end program set_data
