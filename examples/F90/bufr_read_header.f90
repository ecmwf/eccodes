! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
! Description: How to read the header of BUFR messages.
!
!
program bufr_read_header
   use eccodes
   implicit none
   integer            :: ifile
   integer            :: iret
   integer            :: ibufr
   integer            :: count = 0
   integer(kind=4)    :: dataCategory, dataSubCategory, typicalDate
   integer(kind=4)    :: centre, subcentre
   integer(kind=4)    :: masterversion, localversion
   integer(kind=4)    :: numberOfSubsets

   call codes_open_file(ifile, '../../data/bufr/syno_multi.bufr', 'r')

   do while (.true.)
      ! A BUFR message is loaded from file,
      ! ibufr is the BUFR id to be used in subsequent calls
      call codes_bufr_new_from_file(ifile, ibufr, iret)
      if (iret == CODES_END_OF_FILE) exit

      ! Get and print some keys from the BUFR header
      write (*, *) 'message: ', count

      call codes_get(ibufr, 'dataCategory', dataCategory);
      write (*, *) '  dataCategory:', dataCategory

      call codes_get(ibufr, 'dataSubCategory', dataSubCategory);
      write (*, *) '  dataSubCategory:', dataSubCategory

      call codes_get(ibufr, 'typicalDate', typicalDate);
      write (*, *) '  typicalDate:', typicalDate

      call codes_get(ibufr, 'bufrHeaderCentre', centre);
      write (*, *) '  bufrHeaderCentre:', centre

      call codes_get(ibufr, 'bufrHeaderSubCentre', subcentre)
      write (*, *) '  bufrHeaderSubCentre:', subcentre

      call codes_get(ibufr, 'masterTablesVersionNumber', masterversion)
      write (*, *) '  masterTablesVersionNumber:', masterversion

      call codes_get(ibufr, 'localTablesVersionNumber', localversion)
      write (*, *) '  localTablesVersionNumber:', localversion

      call codes_get(ibufr, 'numberOfSubsets', numberOfSubsets)
      write (*, *) '  numberOfSubsets:', numberOfSubsets

      ! Release the BUFR message
      call codes_release(ibufr)

      count = count + 1

   end do

   ! Close file
   call codes_close_file(ifile)

end program bufr_read_header
