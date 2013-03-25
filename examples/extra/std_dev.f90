program standard_deviation

	use grib_api

	implicit none

	character(len=100)                  ::  file_name,out_file
	integer                             ::  ifile,ofile
	integer                             ::  igrib,ogrib
	integer                             ::  numberOfValues, nMessages
	integer                             ::  iret,i
	real(8), dimension(:), allocatable  ::  values,sums,square_sums

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
	allocate(square_sums(numberOfValues))

	! --- compute the standard deviation
	sums = 0
	square_sums = 0
	do while (iret/=GRIB_END_OF_FILE)
		call grib_get(igrib,'values',values)
		sums = sums + values
		square_sums = square_sums + values * values
		call grib_new_from_file(ifile,igrib,iret)
	end do
	sums = sums/nMessages
	square_sums = square_sums/nMessages - sums*sums

	do i=1,numberOfValues
		if (square_sums(i) > 0.0) then
			square_sums(i) = sqrt(square_sums(i))
		else
			square_sums(i) = 0.0
		end if
	end do

	! --- write to the out file
	call grib_set(ogrib,'values',square_sums)
	call grib_write(ogrib,ofile)

	! --- release mem
	deallocate(values)
	deallocate(sums)
	call grib_release(igrib)
	call grib_close_file(ifile)
	call grib_close_file(ofile)

end program standard_deviation
