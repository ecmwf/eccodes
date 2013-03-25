integer function kind_of_size_t()
  integer(2), dimension(2) :: x2 = (/1, 2/)
  integer(4), dimension(2) :: x4 = (/1, 2/)
  integer(8), dimension(2) :: x8 = (/1, 2/)
  character(len=1) :: ret

  kind_of_size_t=-1

  call check_size_t(x2(1),x2(2),ret)
  if (ret == 't') then
   kind_of_size_t=2
   return
  endif

  call check_size_t(x4(1),x4(2),ret)
  if (ret == 't') then
   kind_of_size_t=4
   return
  endif

  call check_size_t(x8(1),x8(2),ret)
  if (ret == 't') then
   kind_of_size_t=8
   return
  endif

end function kind_of_size_t

integer function kind_of_long()
  integer(2), dimension(2) :: x2 = (/1, 2/)
  integer(4), dimension(2) :: x4 = (/1, 2/)
  integer(8), dimension(2) :: x8 = (/1, 2/)
  character(len=1) :: ret

  kind_of_long=-1

  call check_long(x2(1),x2(2),ret)
  if (ret == 't') then
   kind_of_long=2
   return
  endif

  call check_long(x4(1),x4(2),ret)
  if (ret == 't') then
   kind_of_long=4
   return
  endif

  call check_long(x8(1),x8(2),ret)
  if (ret == 't') then
   kind_of_long=8
   return
  endif

end function kind_of_long

integer function kind_of_int()
  integer(2), dimension(2) :: x2 = (/1, 2/)
  integer(4), dimension(2) :: x4 = (/1, 2/)
  integer(8), dimension(2) :: x8 = (/1, 2/)
  character(len=1) :: ret

  kind_of_int=-1

  call check_int(x2(1),x2(2),ret)
  if (ret == 't') then
   kind_of_int=2
   return
  endif

  call check_int(x4(1),x4(2),ret)
  if (ret == 't') then
   kind_of_int=4
   return
  endif

  call check_int(x8(1),x8(2),ret)
  if (ret == 't') then
   kind_of_int=8
   return
  endif

end function kind_of_int

integer function kind_of_float()
  real(4), dimension(2) :: x4 = (/1., 2./)
  real(8), dimension(2) :: x8 = (/1., 2./)
  character(len=1) :: ret

  kind_of_float=-1

  call check_float(x4(1),x4(2),ret)
  if (ret == 't') then
   kind_of_float=4
   return
  endif

  call check_float(x8(1),x8(2),ret)
  if (ret == 't') then
   kind_of_float=8
   return
  endif

end function kind_of_float

integer function kind_of_double()
  real(4), dimension(2) :: real4 = (/1., 2./)
  real(8), dimension(2) :: real8 = (/1., 2./)
  character(len=1) :: ret

  kind_of_double=-1

  call check_double(real4(1),real4(2),ret)
  if (ret == 't') then
   kind_of_double=4
   return
  endif

  call check_double(real8(1),real8(2),ret)
  if (ret == 't') then
   kind_of_double=8
   return
  endif

end function kind_of_double

program same_int_long
  integer ki,kl

  ki=kind_of_int()
  kl=kind_of_long()
  if (ki /= kl) then
    write (*,'(i1)') 0
  else
    write (*,'(i1)') 1
  endif
end program same_int_long

