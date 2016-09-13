!
!Copyright 2005-2016 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
!which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
! FORTRAN 90 Implementation: get_product_kind
!
! Description: how to process a file containing a mix of messages
!              and print the kind of product (e.g. GRIB, BUFR etc)
!
program get_product_kind
  use eccodes
  implicit none
  integer            :: ifile
  integer            :: iret
  integer            :: ihandle
  integer            :: count=0
  character(len=32)  :: product_kind
  character(len=120) :: infile_name

  call getarg(1, infile_name)
  write(*,*) 'infile_name|',infile_name,'|'
  call codes_open_file(ifile,infile_name,'r')

! the first message is loaded from file
! ihandle is the message id to be used in subsequent calls
  call codes_new_from_file(ifile,ihandle,CODES_PRODUCT_ANY,iret)

  do while (iret/=CODES_END_OF_FILE)

    write(*,*) 'message: ',count

    ! get the product kind
    call codes_get(ihandle,'kindOfProduct',product_kind)
    write(*,*) '  product: ',product_kind

    ! release the message
    call codes_release(ihandle)

    ! load the next message
    call codes_new_from_file(ifile,ihandle,CODES_PRODUCT_ANY,iret)
    
    count=count+1
    
  end do  

  ! close file  
  call codes_close_file(ifile)

end program get_product_kind
