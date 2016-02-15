!
! Copyright 2005-2016 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! 
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
! Description: how to get values using keys.
!
program read_message
use eccodes
implicit none
  integer                      ::  ifile,ofile
  integer                      ::  iret,igrib
  integer , dimension(50000)   :: buffer
  integer(kind=kindOfSize_t)   :: len1
  integer                      :: step,level

  call codes_open_file(ifile,'../../data/index.grib','r')
  call codes_open_file(ofile,'out.readmsg.grib','w')
 
! a grib message is read from file into buffer
  len1=size(buffer)*4
  call  codes_read_from_file(ifile,buffer,len1,iret) 

  do while (iret/=CODES_END_OF_FILE)

!   a new grib message is created from buffer
    call codes_new_from_message(igrib,buffer)

!   get as a integer
    call codes_get(igrib,'step', step) 
    write(*,*) 'step=',step

    call codes_get(igrib,'level',level)
    write(*,*) 'level=',level

    call codes_release(igrib)

    call codes_write_bytes(ofile,buffer,len1)

!   a grib message is read from file into buffer
    len1=size(buffer)*4
    call  codes_read_from_file(ifile,buffer,len1,iret) 

  enddo

  call codes_close_file(ifile)
  call codes_close_file(ofile)

end program read_message

