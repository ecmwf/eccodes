! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
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
   integer            :: count = 0
   integer            :: version = 0
   character(len=32)  :: product_kind
   character(len=120) :: infile_name

   call getarg(1, infile_name)
   write (*, *) 'infile_name|', infile_name, '|'
   call codes_open_file(ifile, infile_name, 'r')

   call codes_get_api_version(version)
   write (*, *) 'API version: ', version

   write (*, *) 'ecCodes settings: '
   write (*, *) '  ECCODES_POSIX_THREADS:  ', ECCODES_SETTINGS_POSIX_THREADS
   write (*, *) '  ECCODES_OMP_THREADS:    ', ECCODES_SETTINGS_OMP_THREADS
   write (*, *) '  ECCODES_SETTINGS_MEMFS: ', ECCODES_SETTINGS_MEMFS
   write (*, *) '  ECCODES_SETTINGS_JPEG:  ', ECCODES_SETTINGS_JPEG
   write (*, *) '  ECCODES_SETTINGS_PNG:   ', ECCODES_SETTINGS_PNG
   write (*, *) '  ECCODES_SETTINGS_AEC:   ', ECCODES_SETTINGS_AEC

   do while (.true.)
      call codes_new_from_file(ifile, ihandle, CODES_PRODUCT_ANY, iret)
      if (iret == CODES_END_OF_FILE) exit

      write (*, *) 'message: ', count

      ! get the product kind
      call codes_get(ihandle, 'kindOfProduct', product_kind)
      write (*, *) '  product: ', product_kind

      ! release the message
      call codes_release(ihandle)

      count = count + 1
   end do

   ! close file
   call codes_close_file(ifile)

end program get_product_kind
