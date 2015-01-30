!
!Copyright 2005-2015 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
!which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
! FOTRAN 90 Implementation: bufr_read_header
!
! Description: how to read the header of BUFR messages.
!
!
program bufr_read_header
use eccodes
implicit none
integer            :: ifile
integer            :: iret
integer            :: ibufr
integer(kind=4)    :: centre,subcentre
integer(kind=4)    :: masterversion,localversion
integer(kind=4)    :: numberofsubsets   

  call codes_open_file(ifile,'../../data/bufr/aaen_55.bufr','r')

! the first bufr message is loaded from file
! igrib is the grib id to be used in subsequent calls
  call codes_bufr_new_from_file(ifile,ibufr,iret)

! get and print some keys form the BUFR header 
    
  call codes_get(ibufr,'centre',centre);
  write(*,*) '  centre=',centre
  
  call codes_get(ibufr,'subCentre',subcentre)
  write(*,*) '  subCentre=',subcentre
  
  call codes_get(ibufr,'masterTablesVersionNumber',masterversion)
  write(*,*) '  masterTablesVersionNumbe',masterversion

  call codes_get(ibufr,'localTablesVersionNumber',localversion)
  write(*,*) '  localTablesVersionNumber',localversion
    
  call codes_get(ibufr,'numberOfSubsets',numberofsubsets)
  write(*,*) '  numberOfSubsets',numberofsubsets   
  
  call codes_release(ibufr)

! close file  
  call codes_close_file(ifile)
 

end program bufr_read_header
