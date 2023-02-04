! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
! Get message lengths using two different interfaces
! See GRIB-292
!
program grib_read_from_file_example
  use eccodes
  implicit none
  character(len=32) :: input_grib_file
  integer, dimension(26) :: message_lengths  ! expected message lengths

  input_grib_file = '../../data/v.grib2'
  message_lengths = (/95917, 96963, 97308, 97386, 97215, 97440, 98451, 98629, 98448, &
                      99186, 97517, 97466, 99307, 98460, 101491, 99361, 100292, 96838, &
                      91093, 83247, 78244, 74872, 72663, 69305, 69881, 68572/)

  ! get the GRIB message length using two different interfaces
  call read_using_size_t()
  call read_using_integer()
  print *, 'Passed'

contains
!======================================
  subroutine read_using_size_t
    implicit none
    integer :: size, intsize
    parameter(intsize=100000, size=intsize*4)
    integer            :: ifile
    integer            :: iret
    integer            :: count1 = 0
    integer(kind=4), dimension(intsize) :: buffer
    integer(kind=kindOfSize_t) :: len1 ! For large messages
    character(len=128) :: error_message

    ifile = 5
    call codes_open_file(ifile, input_grib_file, 'r')

    len1 = size
    call codes_read_from_file(ifile, buffer, len1, iret)

    do while (iret == CODES_SUCCESS)
      count1 = count1 + 1
      if (len1 /= message_lengths(count1)) then
        write (*, '(a,i3,a,i8,a,i8)') 'Error: Message #', count1, ' length=', len1, '. Expected=', message_lengths(count1)
        stop
      end if
      len1 = size
      call codes_read_from_file(ifile, buffer, len1, iret)
    end do

    if (iret /= CODES_END_OF_FILE) then
      call codes_get_error_string(iret, error_message)
      write (*, *) 'error message: ', error_message
      call codes_check(iret, 'grib_read_from_file', '')
    end if
    call codes_close_file(ifile)

  end subroutine read_using_size_t

!======================================
  subroutine read_using_integer
    implicit none
    integer :: size, intsize
    parameter(intsize=100000, size=intsize*4)
    integer            :: ifile
    integer            :: iret
    integer            :: count1 = 0
    integer(kind=4), dimension(intsize) :: buffer
    integer            :: len1
    character(len=128) :: error_message

    ifile = 5
    call codes_open_file(ifile, input_grib_file, 'r')

    len1 = size
    call codes_read_from_file(ifile, buffer, len1, iret)

    do while (iret == CODES_SUCCESS)
      count1 = count1 + 1
      if (len1 /= message_lengths(count1)) then
        write (*, '(a,i3,a,i8,a,i8)') 'Error: Message #', count1, ' length=', len1, '. Expected=', message_lengths(count1)
        stop
      end if
      len1 = size
      call codes_read_from_file(ifile, buffer, len1, iret)
    end do

    if (iret /= CODES_END_OF_FILE) then
      call codes_get_error_string(iret, error_message)
      write (*, *) 'error message: ', error_message
      call codes_check(iret, 'grib_read_from_file', '')
    end if
    call codes_close_file(ifile)

  end subroutine read_using_integer
!======================================
end program
