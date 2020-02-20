! (C) Copyright 2005- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!  Description: Practical example of an ensemble mean using grib_api
!
!
!  Author: Cristian D Codorean
!
!
program ensemble_mean

	use grib_api

	implicit none

	character(len=100)                  ::  file_name,out_file
	integer                             ::  ifile,ofile
	integer                             ::  igrib,ogrib
	integer                             ::  numberOfValues, nMessages
	integer                             ::  iret
	real(8), dimension(:), allocatable  ::  values,sums

	! --- get input arguments
	call getarg(1,file_name)
	call getarg(2,out_file)

	call grib_open_file(ifile,file_name,'r')
	call grib_open_file(ofile,out_file,'w')

	! --- count number of messages in grib
	call grib_count_in_file(ifile,nMessages)

	call grib_new_from_file(ifile,igrib,iret)

	! --- create ouput grib from this one
	call grib_clone(igrib,ogrib)

	! --- get the number of values, we suppose all messages have
	! -     the same size for the sake of this example
	call grib_get(igrib,'numberOfValues',numberOfValues)

	! --- one should check if allocation was successfull
	allocate(values(numberOfValues))
	allocate(sums(numberOfValues))

	! --- compute the ensemble mean
	sums = 0
	do while (iret/=GRIB_END_OF_FILE)
		call grib_get(igrib,'values',values)
		sums = sums + values
		call grib_new_from_file(ifile,igrib,iret)
	end do
	sums = sums/nMessages

	! --- write to the out file
	call grib_set(ogrib,'values',sums)
	call grib_write(ogrib,ofile)

	! --- release mem
	deallocate(values)
	deallocate(sums)
	call grib_release(igrib)
	call grib_close_file(ifile)
	call grib_close_file(ofile)

end program ensemble_mean
