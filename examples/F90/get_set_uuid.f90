! Copyright 2005-2018 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!  Description: get/set byte array in a grib2 message, using the uuid as example.
!
!  Original authors: Harald Anlauf, Doerte Liermann (DWD), Luis Kornblueh (MPIfM).
!
program get_set_uuid
  use eccodes
  implicit none
  integer              :: infile, outfile
  integer              :: igrib, ogrib
  integer              :: count1, i, iret, nvg, ffs, length
  character(len=1)     :: uuid_in (16)  ! Array of 16 bytes for uuid on input.
  character(len=1)     :: uuid_out(16)  ! Array of 16 bytes for uuid on output.
  character(len=32)    :: uuid_string   ! Human-readable uuid.
  character(len=32)    :: uuid_string_expected   ! Expected UUID of input

  call codes_open_file (infile,  '../../data/test_uuid.grib2','r')

  call codes_open_file (outfile, 'out_uuid.grib2','w')

  ! Load first grib message from file
  ! igrib is the grib id to be used in subsequent calls
  call codes_grib_new_from_file (infile, igrib, iret)

  uuid_string_expected = '08b1e836bc6911e1951fb51b5624ad8d'
  count1 = 0
  do while (iret/=CODES_END_OF_FILE)
    count1 = count1 + 1
    print *, "### Record:", count1
    call codes_get(igrib,'typeOfFirstFixedSurface',ffs)
    print *, 'typeOfFirstFixedSurface =', ffs
    if (ffs /= 150) then
      print *, "Unexpected typeOfFirstFixedSurface (must be 150)."
      stop
    end if

    call codes_get (igrib,'numberOfVGridUsed',nvg)
    print *, 'numberOfVGridUsed       =',nvg

    ! call codes_get (igrib,'uuidOfVGrid',uuid_in)  ! Assuming length is ok.
    call codes_get (igrib,'uuidOfVGrid',uuid_in,length=length)
    if (length /= 16) then
      print *, "Sorry, bad length of byte_array:", length, ". Expected: 16"
      stop
    end if

    ! Convert byte array to hexadecimal string for printing
    do i = 1, size (uuid_in)
      uuid_string(2*i-1:2*i) = byte2hex(uuid_in(i))
    end do
    print *, "uuidOfVGrid  (on input) = ", uuid_string
    if (uuid_string .ne. uuid_string_expected) then
      print *, "Sorry, bad value of byte_array. Expected: ", uuid_string_expected
      stop
    end if

    call codes_clone (igrib,ogrib)
    ! On output we write a modified uuid (here the input is simply reversed)
    uuid_out(1:16) = uuid_in(16:1:-1)
    call codes_set   (ogrib,'uuidOfVGrid',uuid_out)
    call codes_write (ogrib,outfile)

    call codes_release (igrib)
    call codes_release (ogrib)
    call codes_grib_new_from_file (infile, igrib, iret)
  end do

  call codes_close_file (infile)
  call codes_close_file (outfile)

contains
  ! Convert single byte to 'hexadecimal' string
  pure function byte2hex (c) result (hex)
    character(len=1), intent(in) :: c
    character(len=2)             :: hex
    integer :: x
    x = iachar (c)
    hex(1:1) = nibble (      x / 16)
    hex(2:2) = nibble (iand (x,  15))
  end function byte2hex
  ! Convert 'nibble' to 'hexadecimal'
  pure function nibble (x)
    integer, intent(in) :: x
    character           :: nibble
    select case (x)
    case (0:9)
       nibble = achar (iachar ('0') + x)
    case default
       nibble = achar (iachar ('a') - 10 + x)
    end select
  end function nibble
end program get_set_uuid
