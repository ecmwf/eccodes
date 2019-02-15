! Copyright 2005-2018 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!  Description: how to copy a BUFR key from a message to another.
!
program bufr_copy_keys
  use eccodes
  implicit none
  integer  :: file1, file2, file3
  integer  :: ibufr1,ibufr2,ibufr3

  call codes_open_file(file1, '../../data/bufr/PraticaTemp.bufr', 'r')
  call codes_open_file(file2, '../../data/bufr/aaen_55.bufr', 'r')
  call codes_open_file(file3, 'out.bufr_copy_keys_test_f.bufr', 'w')

  call codes_bufr_new_from_file(file1, ibufr1)
  call codes_bufr_new_from_file(file2, ibufr2)

  call codes_clone(ibufr2, ibufr3)

  call codes_copy_key(ibufr1, 'bufrHeaderCentre', ibufr3)

  call codes_write(ibufr3, file3)

  call codes_close_file(file1)
  call codes_close_file(file2)
  call codes_close_file(file3)

end program bufr_copy_keys
