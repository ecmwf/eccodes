! Copyright 2005-2014 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! 
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
!
!
!  Description: How to create and use and index to access messages from a file.
!               Demo also loading and writing an index from a file.
!
!
!
program index
  use grib_api
  implicit none
  
  integer              :: iret
  character(len = 256) :: error
  integer,dimension(:),allocatable :: step,level,number
  character(len=20),dimension(:),allocatable :: shortName
  integer                          :: ostep,olevel,onumber
  character(len=20)                :: oshortName
  integer                          :: shortNameSize,numberSize,levelSize,stepSize
  integer              :: i,j,k,l
  integer              :: idx,igrib,count1
  logical              :: index_exists
  character(len=10)    :: index_file='index.idx'

  ! uncomment following line to load index from file
  !call grib_index_read(idx,index_file)

  ! create an index from a grib file using some keys
  call grib_index_create(idx,'../../data/index.grib','shortName,number,level,step')

  ! get the number of distinct values of shortName in the index
  call grib_index_get_size(idx,'shortName',shortNameSize)
  ! allocate the array to contain the list of distinct shortName
  allocate(shortName(shortNameSize))
  ! get the list of distinct shortName from the index
  call grib_index_get(idx,'shortName',shortName)
  write(*,'(a,i3)') 'shortNameSize=',shortNameSize

  ! get the number of distinct values of number in the index
  call grib_index_get_size(idx,'number',numberSize)
  ! allocate the array to contain the list of distinct numbers
  allocate(number(numberSize))
  ! get the list of distinct numbers from the index
  call grib_index_get(idx,'number',number)
  write(*,'(a,i3)') 'numberSize=',numberSize

  ! get the number of distinct values of level in the index
  call grib_index_get_size(idx,'level',levelSize)
  ! allocate the array to contain the list of distinct levels
  allocate(level(levelSize))
  ! get the list of distinct levels from the index
  call grib_index_get(idx,'level',level)
  write(*,'(a,i3)') 'levelSize=',levelSize

  ! get the number of distinct values of step in the index
  call grib_index_get_size(idx,'step',stepSize)
  ! allocate the array to contain the list of distinct steps
  allocate(step(stepSize))
  ! get the list of distinct steps from the index
  call grib_index_get(idx,'step',step)
  write(*,'(a,i3)') 'stepSize=',stepSize

  count1=0
  do l=1,stepSize ! loop on step
    ! select step=step(l)
    call grib_index_select(idx,'step',step(l))

    do j=1,numberSize ! loop on number
      ! select number=number(j)
      call grib_index_select(idx,'number',number(j))

      do k=1,levelSize ! loop on level
        ! select level=level(k)
        call grib_index_select(idx,'level',level(k))

        do i=1,shortNameSize ! loop on shortName
          ! select shortName=shortName(i)
          call grib_index_select(idx,'shortName',shortName(i))

          call grib_new_from_index(idx,igrib, iret)
          do while (iret /= GRIB_END_OF_INDEX)
             count1=count1+1
             call grib_get(igrib,'shortName',oshortName)
             call grib_get(igrib,'number',onumber)
             call grib_get(igrib,'level',olevel)
             call grib_get(igrib,'step',ostep)
             write(*,'(A,A,A,i3,A,i4,A,i3)') 'shortName=',trim(oshortName),&
                     '   number=',onumber,&
                     '   level=' ,olevel, &
                     '   step='  ,ostep
  
             call grib_release(igrib)
             call grib_new_from_index(idx,igrib, iret)
          end do
          call grib_release(igrib)

        end do ! loop on step
      end do ! loop on level
    end do ! loop on number
  end do ! loop on shortName
  write(*,'(i4,a)') count1,' messages selected'

  ! save the index to a file for later reuse
  call grib_index_write(idx,index_file)

  call grib_index_release(idx)

end program index
