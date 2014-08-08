! Copyright 2005-2014 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! 
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!
!  Description: how to create a new GRIB message by cloning 
!               an existing message.
!
!
!
program clone
  use grib_api
  implicit none
  integer                                       :: err,i,iret
  integer                                       :: nx, ny
  integer                                       :: infile,outfile
  integer                                       :: igrib_in
  integer                                       :: igrib_out
  character(len=2)                              :: step
  double precision, dimension(:,:), allocatable :: field2D
  
  call grib_open_file(infile,'../../data/constant_field.grib1','r')
  call grib_open_file(outfile,'out.grib1','w')

  !     a new grib message is loaded from file
  !     igrib is the grib id to be used in subsequent calls
  call grib_new_from_file(infile,igrib_in)

  call grib_get(igrib_in,"Ni", nx)
  
  call grib_get(igrib_in,"Nj",ny)

  allocate(field2D(nx,ny),stat=err)

  if (err .ne. 0) then
     print*, 'Failed to allocate ', nx*ny, ' values'
     STOP
  end if
  ! clone the constant field to create 4 new GRIB messages
  do i=0,18,6
    call grib_clone(igrib_in, igrib_out)
    write(step,'(i2)') i
    ! Careful: stepRange is a string (could be 0-6, 12-24, etc.)
    ! use adjustl to remove blank from the left.
    call grib_set(igrib_out,'stepRange',adjustl(step))

    call generate_field(field2D)

    ! use pack to create 1D values
    call grib_set(igrib_out,'values',pack(field2D, mask=.true.))
 
    ! write cloned messages to a file
    call grib_write(igrib_out,outfile)
    call grib_release(igrib_out)
  end do

  call grib_release(igrib_in)
  call grib_close_file(infile)
  call grib_close_file(outfile)
  deallocate(field2D)

contains
!======================================
subroutine generate_field(gfield2D)
 double precision, dimension(:,:) :: gfield2D

 call random_number(gfield2D)
end subroutine generate_field
!======================================

end program clone
