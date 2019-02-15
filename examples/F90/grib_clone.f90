! Copyright 2005-2018 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!  FORTRAN 90 Implementation: grib_clone
!
!  Description: how to create a new GRIB message by cloning
!               an existing message.
!
!
program clone
  use eccodes
  implicit none
  integer                                       :: err,i
  integer                                       :: nx, ny
  integer                                       :: infile,outfile
  integer                                       :: igrib_in
  integer                                       :: igrib_out
  character(len=2)                              :: step
  real(kind=8), dimension(:,:), allocatable     :: field2D

  call codes_open_file(infile,'../../data/constant_field.grib1','r')
  call codes_open_file(outfile,'out.clone.grib1','w')

  ! A new GRIB message is loaded from file.
  ! igrib is the GRIB id to be used in subsequent calls
  call codes_grib_new_from_file(infile,igrib_in)

  call codes_get(igrib_in,'Ni', nx)
  call codes_get(igrib_in,'Nj', ny)

  allocate(field2D(nx,ny),stat=err)

  if (err .ne. 0) then
    print*, 'Failed to allocate ', nx*ny, ' values'
    STOP
  end if
  ! clone the constant field to create 4 new GRIB messages
  do i=0,18,6
    call codes_clone(igrib_in, igrib_out)
    write(step,'(i2)') i
    ! Careful: stepRange is a string (could be 0-6, 12-24, etc.)
    ! use adjustl to remove blank from the left.
    call codes_set(igrib_out,'stepRange',adjustl(step))

    call generate_field(field2D)

    ! use pack to create 1D values
    call codes_set(igrib_out,'values',pack(field2D, mask=.true.))

    ! write cloned messages to a file
    call codes_write(igrib_out,outfile)
    call codes_release(igrib_out)
  end do

  call codes_release(igrib_in)
  call codes_close_file(infile)
  call codes_close_file(outfile)
  deallocate(field2D)

contains
!======================================
subroutine generate_field(gfield2D)
 real(kind=8), dimension(:,:) :: gfield2D

 call random_number(gfield2D)
end subroutine generate_field
!======================================

end program clone
