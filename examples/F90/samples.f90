! Copyright 2005-2014 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! 
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!  Description: how to create a new GRIB message from a sample.
!               
!
program sample
  use grib_api
  implicit none  
  integer  :: err
  integer  :: outfile, infile, datafile
  integer  :: igribsample,igribclone,igribdata, size1
  integer  :: date1, startStep, endStep, table2Version, indicatorOfParameter
  integer  :: decimalPrecision
  real     :: missingValue
  character(len=10) stepType
  double precision, dimension(:), allocatable   :: v1,v2,v 

  date1 = 20080104
  startStep = 0
  endStep = 12
  stepType = 'accum'
  table2Version = 2
  indicatorOfParameter = 61
  decimalPrecision = 2

  !     A new grib message is loaded from an existing sample.
  !     Samples are searched in a default sample path (use grib_info
  !     to see where that is). The default sample path can be changed by
  !     setting the environment variable GRIB_SAMPLES_PATH
  call grib_new_from_samples(igribsample, "regular_latlon_surface.grib1")

  call grib_open_file(outfile, 'out.grib1','w')
  call grib_open_file(datafile,'../../data/tp_ecmwf.grib','r')

  call grib_new_from_file(datafile,igribdata,err)

  call grib_get_size(igribdata,'values',size1)
  allocate(v(size1))
  allocate(v1(size1))
  allocate(v2(size1))

  call grib_get(igribdata,'values',v)

  v=v*1000.0 ! different units for the output grib
  v1=v

  do while (err/=GRIB_END_OF_FILE) 
 
    call grib_clone(igribsample,igribclone) ! clone sample before modifying it

    call grib_set(igribclone,'dataDate',date1)
    call grib_set(igribclone,'table2Version',table2Version)
    call grib_set(igribclone,'indicatorOfParameter',indicatorOfParameter)

    call grib_set(igribclone,'stepType',stepType)
    call grib_set(igribclone,'startStep',startStep)
    call grib_set(igribclone,'endStep',endStep)

    call grib_set(igribclone,'decimalPrecision',decimalPrecision)

    call grib_set(igribclone,'values',v)

    call grib_write(igribclone,outfile)
    
    call grib_new_from_file(datafile,igribdata,err)

    if (err==0) then
      call grib_get(igribdata,'values',v2)

      v2=v2*1000.0 ! different units for the output grib

      v=v2-v1 ! accumulation from startStep to endStep

      v1=v2 ! save previous step field

      startStep=startStep+12
      endStep=endStep+12

    endif

  enddo

  call grib_release(igribsample)
  deallocate(v)
  deallocate(v1)
  deallocate(v2)

  call grib_close_file(outfile)

end program sample
