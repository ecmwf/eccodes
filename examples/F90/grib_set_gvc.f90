! Copyright 2005-2016 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! 
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!
!  Description: how to set keys for a grib2 file with
!               a Generalized Vertical Height Coordinate
!
!
program set
  use eccodes
  implicit none
  integer              :: infile,outfile
  integer              :: igrib

  call codes_open_file(infile,  '../../data/sample.grib2','r')

  call codes_open_file(outfile, 'out_gvc.grib2','w')

  call codes_grib_new_from_file(infile,igrib)

  ! Individual ensemble forecast
  call codes_set(igrib,'productDefinitionTemplateNumber', 11)
  
  ! Select level type as Generalized Vertical Height Coordinate
  call codes_set(igrib,'typeOfLevel', 'generalVertical')
  
  ! Now set keys specific to this level type
  call codes_set(igrib,'nlev', 12.21)
  call codes_set(igrib,'numberOfVGridUsed', 13.55)

  ! check integrity of GRIB message
  call check_settings(igrib)

  ! write modified message to a file
  call codes_write(igrib,outfile)

  call codes_release(igrib)
  call codes_close_file(infile)
  call codes_close_file(outfile)

contains

!======================================
subroutine check_settings(gribid)
  implicit none
  integer, intent(in) :: gribid
  
  integer(kind = 4) :: NV,typeOfFirstFixedSurface

  call codes_get(gribid,'NV', NV)
  if (NV /= 6) then
    call codes_check(-2, 'NV_should_be_6', '')
  end if

  call codes_get(gribid,'typeOfFirstFixedSurface', typeOfFirstFixedSurface)
  if (typeOfFirstFixedSurface /= 150) then
    call codes_check(-2, 'typeOfFirstFixedSurface_should_be_150', '')
  end if
   
end subroutine check_settings
end program set
