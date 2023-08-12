! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
program codes_datetime_julian
    use eccodes
    implicit none

    integer(kind=8) :: year=2012, month=1, day=2, hour=13, min=0, sec=48
    integer(kind=8) :: year2, month2, day2, hour2, min2, sec2
    real(kind=8) :: julian

    CALL codes_datetime_to_julian(year, month, day, hour, min, sec, julian)
    PRINT *, 'datetime to julian ', julian

    CALL codes_julian_to_datetime(julian, year2, month2, day2, hour2, min2, sec2)
    PRINT *, 'julian to datetime ', year2, month2, day2, hour2, min2, sec2

    IF (year /= year2) THEN
        call codes_check(CODES_INTERNAL_ERROR, 'Error', 'Year is wrong')
    END IF
    IF (month /= month2) THEN
        call codes_check(CODES_INTERNAL_ERROR, 'Error', 'Month is wrong')
    END IF
    IF (day /= day2) THEN
        call codes_check(CODES_INTERNAL_ERROR, 'Error', 'Day is wrong')
    END IF

end program codes_datetime_julian
