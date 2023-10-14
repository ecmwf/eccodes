! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
program grib_ecc_1316
   use eccodes
   implicit none

   integer              :: iret, ostep, olevel, onumber
   character(len=80)    :: parameterName
   integer              :: idx, igrib, count1 = 0

   ! create an index from a grib file using some keys
   call codes_index_create(idx, '../../data/tigge_cf_ecmwf.grib2', &
                                'parameterName,number,level,step')

   call codes_index_select(idx, 'step',   96)
   call codes_index_select(idx, 'level',  0)
   call codes_index_select(idx, 'number', 0)
   call codes_index_select(idx, 'parameterName', 'Soil moisture')

   do while (.true.)
      call codes_new_from_index(idx, igrib, iret)
      if (iret == CODES_END_OF_INDEX) exit

      count1 = count1 + 1
      call codes_get(igrib, 'parameterName', parameterName)
      call codes_get(igrib, 'number', onumber)
      call codes_get(igrib, 'level', olevel)
      call codes_get(igrib, 'step', ostep)
      write (*, '(A,A,A,i3,A,i4,A,i3)') 'parameterName=', parameterName, &
             '   number=', onumber, &
             '   level=',  olevel, &
             '    step=',  ostep
      call codes_release(igrib)
   end do
   call codes_release(igrib)

   write (*, '(i4,a)') count1, ' messages selected'
   if (count1 /= 1) stop 1

   call codes_index_release(idx)

end program grib_ecc_1316
