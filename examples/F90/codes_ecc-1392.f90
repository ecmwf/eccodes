! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
program codes_ecc_1392
    use eccodes
    implicit none

    integer           :: ifile
    character(len=60) :: inputfile

    call getarg(1, inputfile)
    call codes_open_file(ifile, inputfile, 'R')
    write (*, *) 'Opened file OK: |',inputfile,'|'
    call grib_close_file(ifile)

end program codes_ecc_1392
