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
!  Description: how to set pv values in a GRIB message
!
!
program grib_set_pv
  use eccodes
  implicit none
  integer                         :: numberOfLevels
  integer                         :: numberOfCoefficients
  integer                         :: outfile, igrib
  integer                         :: i, ios
  real, dimension(:),allocatable  :: pv

  numberOfLevels=60
  numberOfCoefficients=2*(numberOfLevels+1)

  allocate(pv(numberOfCoefficients))

  ! read the model level coefficients from file
  open( unit=1, file="../../data/60_model_levels", &
                form="formatted",action="read")

  do i=1,numberOfCoefficients,2
    read(unit=1,fmt=*, iostat=ios) pv(i), pv(i+1)
    if (ios /= 0) then
      print *, "I/O error: ",ios
      exit
    end if
  end do

  ! print coefficients
  !do i=1,numberOfCoefficients,2
  !  print *,"  a=",pv(i)," b=",pv(i+1)
  !end do

  close(unit=1)

  call codes_open_file(outfile, 'out.pv.grib1','w')

  ! A new grib message is loaded from file
  ! igrib is the grib id to be used in subsequent calls
  call codes_grib_new_from_samples(igrib, "reduced_gg_sfc_grib1")

  ! set levtype to ml (model level)
  call codes_set(igrib,'typeOfLevel','hybrid')

  ! set level
  call codes_set(igrib,'level',2)

  ! set PVPresent as an integer
  call codes_set(igrib,'PVPresent',1)

  call codes_set(igrib,'pv',pv)

  ! write modified message to a file
  call codes_write(igrib,outfile)

  ! Free memory
  call codes_release(igrib)
  deallocate(pv)

  call codes_close_file(outfile)

end program grib_set_pv
