
  contains

!>
!> Wrapper for eccodes
!>
subroutine codes_set_missing ( gribid, key,  status )
    integer(kind=kindOfInt),                 intent(in)  :: gribid
    character(len=*),                        intent(in)  :: key
    integer(kind=kindOfInt),optional, intent(out)        :: status
    integer(kind=kindOfInt)                              :: iret

    call grib_set_missing ( gribid, key,  status )
end subroutine codes_set_missing 

!
subroutine codes_index_create ( indexid, filename, keys,  status )
    integer(kind=kindOfInt),         intent(inout)  :: indexid
    character(len=*), intent(in)                    :: filename
    character(len=*), intent(in)                    :: keys
    integer(kind=kindOfInt),optional, intent(out)   :: status
    integer(kind=kindOfInt)                         :: iret

    call grib_index_create ( indexid, filename, keys,  status )
end subroutine codes_index_create 

!
subroutine codes_index_add_file ( indexid, filename, status )
    integer(kind=kindOfInt), intent(in)             :: indexid
    character(len=*), intent(in)                    :: filename
    integer(kind=kindOfInt),optional, intent(out)   :: status
    integer(kind=kindOfInt)                         :: iret

    call grib_index_add_file ( indexid, filename, status )
end subroutine codes_index_add_file 

!
subroutine codes_index_get_size_long( indexid, key, size,  status )
    integer(kind=kindOfInt),         intent(in)     :: indexid
    character(len=*), intent(in)                    :: key
    integer(kind=kindOfLong),       intent(out)   :: size
    integer(kind=kindOfInt),optional, intent(out)   :: status
    integer(kind=kindOfInt)                         :: iret

    call grib_index_get_size_long( indexid, key, size,  status )
end subroutine codes_index_get_size_long

!
subroutine codes_index_get_size_int( indexid, key, size,  status )
    integer(kind=kindOfInt),         intent(in)     :: indexid
    character(len=*), intent(in)                    :: key
    integer(kind=kindOfInt),       intent(out)   :: size
    integer(kind=kindOfInt),optional, intent(out)   :: status
    integer(kind=kindOfInt)                         :: iret

    call grib_index_get_size_int( indexid, key, size,  status )
end subroutine codes_index_get_size_int

!
subroutine codes_index_get_int( indexid, key, values,  status )
    integer(kind=kindOfInt),               intent(in)  :: indexid
    character(len=*),      intent(in)  :: key
    integer(kind=kindOfInt), dimension(:), intent(out) :: values
    integer(kind=kindOfInt),optional, intent(out)      :: status
    integer(kind=kindOfInt)                            :: iret
    integer(kind=kindOfInt)                            :: nb_values

    call grib_index_get_int( indexid, key, values,  status )
end subroutine codes_index_get_int

!
subroutine codes_index_get_long( indexid, key, values,  status )
    integer(kind=kindOfInt),               intent(in)  :: indexid
    character(len=*),      intent(in)  :: key
    integer(kind=kindOfLong), dimension(:), intent(out) :: values
    integer(kind=kindOfInt),optional, intent(out)      :: status
    integer(kind=kindOfInt)                            :: iret
    integer(kind=kindOfInt)                            :: nb_values

    call grib_index_get_long( indexid, key, values,  status )
end subroutine codes_index_get_long

!
subroutine codes_index_get_real8( indexid, key, values,  status )
    integer(kind=kindOfInt),               intent(in)  :: indexid
    character(len=*),      intent(in)  :: key
    real(kind=kindOfDouble), dimension(:), intent(out) :: values
    integer(kind=kindOfInt),optional, intent(out)      :: status
    integer(kind=kindOfInt)                            :: iret
    integer(kind=kindOfInt)                            :: nb_values

    call grib_index_get_real8( indexid, key, values,  status )
end subroutine codes_index_get_real8

!
subroutine codes_index_get_string( indexid, key, values,  status )
    integer(kind=kindOfInt),               intent(in)  :: indexid
    character(len=*),      intent(in)  :: key
    character(len=*), dimension(:), intent(out) :: values
    integer(kind=kindOfInt),optional, intent(out)      :: status
    integer(kind=kindOfInt)                            :: iret
    integer(kind=kindOfInt)                            :: nb_values
    integer(kind=kindOfInt)                            :: size_value

    call grib_index_get_string( indexid, key, values,  status )
end subroutine codes_index_get_string

!
subroutine codes_index_select_string( indexid, key, value,  status )
    integer(kind=kindOfInt),               intent(in)  :: indexid
    character(len=*),      intent(in)                  :: key
    character(len=*), intent(in)                :: value
    integer(kind=kindOfInt),optional, intent(out)      :: status
    integer(kind=kindOfInt)                            :: iret

    call grib_index_select_string( indexid, key, value,  status )
end subroutine codes_index_select_string

!
subroutine codes_index_select_int( indexid, key, value,  status )
    integer(kind=kindOfInt),               intent(in)  :: indexid
    character(len=*),      intent(in)                  :: key
    integer(kind=kindOfInt), intent(in)                :: value
    integer(kind=kindOfInt),optional, intent(out)      :: status
    integer(kind=kindOfInt)                            :: iret

    call grib_index_select_int( indexid, key, value,  status )
end subroutine codes_index_select_int

!
subroutine codes_index_select_long( indexid, key, value,  status )
    integer(kind=kindOfInt),               intent(in)  :: indexid
    character(len=*),      intent(in)                  :: key
    integer(kind=kindOfLong), intent(in)               :: value
    integer(kind=kindOfInt),optional, intent(out)      :: status
    integer(kind=kindOfInt)                            :: iret

    call grib_index_select_long( indexid, key, value,  status )
end subroutine codes_index_select_long

!
subroutine codes_index_select_real8( indexid, key, value,  status )
    integer(kind=kindOfInt),               intent(in)  :: indexid
    character(len=*),      intent(in)                  :: key
    real(kind=kindOfDouble), intent(in)                :: value
    integer(kind=kindOfInt),optional, intent(out)      :: status
    integer(kind=kindOfInt)                            :: iret

    call grib_index_select_real8( indexid, key, value,  status )
end subroutine codes_index_select_real8

!
subroutine codes_new_from_index ( indexid, gribid , status)
    integer(kind=kindOfInt),intent(in)              :: indexid
    integer(kind=kindOfInt),intent(out)             :: gribid
    integer(kind=kindOfInt),optional,intent(out)    :: status
    integer(kind=kindOfInt)                         :: iret

    call grib_new_from_index ( indexid, gribid , status)
end subroutine codes_new_from_index 

!
subroutine codes_index_read ( indexid, filename, status )
    integer(kind=kindOfInt),         intent(inout)  :: indexid
    character(len=*), intent(in)                    :: filename
    integer(kind=kindOfInt),optional, intent(out)   :: status
    integer(kind=kindOfInt)                         :: iret

    call grib_index_read ( indexid, filename, status )
end subroutine codes_index_read 

!
subroutine codes_index_write ( indexid, filename, status )
    integer(kind=kindOfInt),         intent(inout)  :: indexid
    character(len=*), intent(in)                    :: filename
    integer(kind=kindOfInt),optional, intent(out)   :: status
    integer(kind=kindOfInt)                         :: iret

    call grib_index_write ( indexid, filename, status )
end subroutine codes_index_write 

!
subroutine codes_index_release ( indexid, status )
    integer(kind=kindOfInt),          intent(in)  :: indexid
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)                       :: iret

    call grib_index_release ( indexid, status )
end subroutine codes_index_release

!
subroutine codes_open_file ( ifile, filename, mode, status )
    integer(kind=kindOfInt),intent(out)               :: ifile
    character(len=*), intent(in)                      :: filename
    character(LEN=*), intent(in)                      :: mode
    integer(kind=kindOfInt),optional, intent(out)     :: status
    integer(kind=kindOfInt)                           :: iret

    call grib_open_file ( ifile, filename, mode, status )
end subroutine codes_open_file 

!
subroutine codes_read_bytes_char ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    character(len=1),dimension(:), intent(out)       :: buffer
    integer(kind=kindOfInt), intent(in)              :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status
    integer(kind=kindOfSize_t)                       :: ibytes
    integer(kind=kindOfInt)                          :: iret

    call grib_read_bytes_char ( ifile, buffer, nbytes, status )
end subroutine codes_read_bytes_char 

!
subroutine codes_read_bytes_char_size_t ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    character(len=1),dimension(:), intent(out)       :: buffer
    integer(kind=kindOfSize_t), intent(in)           :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status
    integer(kind=kindOfInt)                          :: iret

    call grib_read_bytes_char_size_t ( ifile, buffer, nbytes, status )
end subroutine codes_read_bytes_char_size_t 

!
subroutine codes_read_bytes_int4 ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    integer(kind=4),dimension(:), intent(out)        :: buffer
    integer(kind=kindOfInt), intent(in)              :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status
    integer(kind=kindOfSize_t)                       :: ibytes
    integer(kind=kindOfInt)                          :: iret

    call grib_read_bytes_int4 ( ifile, buffer, nbytes, status )
end subroutine codes_read_bytes_int4 

!
subroutine codes_read_bytes_int4_size_t ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    integer(kind=4),dimension(:), intent(out)        :: buffer
    integer(kind=kindOfSize_t), intent(in)           :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status
    integer(kind=kindOfInt)                          :: iret

    call grib_read_bytes_int4_size_t ( ifile, buffer, nbytes, status )
end subroutine codes_read_bytes_int4_size_t 

!
subroutine codes_read_bytes_real4 ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    real(kind=4),dimension(:), intent(out)           :: buffer
    integer(kind=kindOfInt), intent(in)              :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status
    integer(kind=kindOfSize_t)                       :: ibytes
    integer(kind=kindOfInt)                          :: iret

    call grib_read_bytes_real4 ( ifile, buffer, nbytes, status )
end subroutine codes_read_bytes_real4 

!
subroutine codes_read_bytes_real4_size_t ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    real(kind=4),dimension(:), intent(out)           :: buffer
    integer(kind=kindOfSize_t), intent(inout)        :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status
    integer(kind=kindOfInt)                          :: iret

    call grib_read_bytes_real4_size_t ( ifile, buffer, nbytes, status )
end subroutine codes_read_bytes_real4_size_t 

!
subroutine codes_read_bytes_real8 ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    real(kind=8),dimension(:), intent(out)           :: buffer
    integer(kind=kindOfInt), intent(in)              :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status
    integer(kind=kindOfSize_t)                       :: ibytes
    integer(kind=kindOfInt)                          :: iret

    call grib_read_bytes_real8 ( ifile, buffer, nbytes, status )
end subroutine codes_read_bytes_real8 

!
subroutine codes_read_bytes_real8_size_t ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    real(kind=8),dimension(:), intent(out)           :: buffer
    integer(kind=kindOfSize_t), intent(inout)        :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status
    integer(kind=kindOfInt)                          :: iret

    call grib_read_bytes_real8_size_t ( ifile, buffer, nbytes, status )
end subroutine codes_read_bytes_real8_size_t 

!
subroutine codes_read_from_file_int4 ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    integer(kind=4),dimension(:), intent(out)        :: buffer
    integer(kind=kindOfInt), intent(inout)           :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status
    integer(kind=kindOfSize_t)                       :: ibytes
    integer(kind=kindOfInt)                          :: iret

    call grib_read_from_file_int4 ( ifile, buffer, nbytes, status )
end subroutine codes_read_from_file_int4 

!
subroutine codes_read_from_file_int4_size_t ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    integer(kind=4),dimension(:), intent(out)        :: buffer
    integer(kind=kindOfSize_t), intent(inout)        :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status
    integer(kind=kindOfInt)                          :: iret

    call grib_read_from_file_int4_size_t ( ifile, buffer, nbytes, status )
end subroutine codes_read_from_file_int4_size_t 

!
subroutine codes_read_from_file_real4 ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    real(kind=4),dimension(:), intent(out)           :: buffer
    integer(kind=kindOfInt), intent(inout)           :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status
    integer(kind=kindOfSize_t)                       :: ibytes
    integer(kind=kindOfInt)                          :: iret

    call grib_read_from_file_real4 ( ifile, buffer, nbytes, status )
end subroutine codes_read_from_file_real4 

!
subroutine codes_read_from_file_real4_size_t ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    real(kind=4),dimension(:), intent(out)           :: buffer
    integer(kind=kindOfSize_t), intent(inout)        :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status
    integer(kind=kindOfInt)                          :: iret

    call grib_read_from_file_real4_size_t ( ifile, buffer, nbytes, status )
end subroutine codes_read_from_file_real4_size_t 

!
subroutine codes_read_from_file_real8 ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    real(kind=8),dimension(:), intent(out)           :: buffer
    integer(kind=kindOfInt), intent(inout)           :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status
    integer(kind=kindOfSize_t)                       :: ibytes
    integer(kind=kindOfInt)                          :: iret

    call grib_read_from_file_real8 ( ifile, buffer, nbytes, status )
end subroutine codes_read_from_file_real8 

!
subroutine codes_read_from_file_real8_size_t ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    real(kind=8),dimension(:), intent(out)           :: buffer
    integer(kind=kindOfSize_t), intent(inout)        :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status
    integer(kind=kindOfInt)                          :: iret

    call grib_read_from_file_real8_size_t ( ifile, buffer, nbytes, status )
end subroutine codes_read_from_file_real8_size_t 

!
subroutine codes_read_from_file_char ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    character(len=1),dimension(:), intent(out)       :: buffer
    integer(kind=kindOfInt), intent(inout)           :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status
    integer(kind=kindOfSize_t)                       :: ibytes
    integer(kind=kindOfInt)                          :: iret

    call grib_read_from_file_char ( ifile, buffer, nbytes, status )
end subroutine codes_read_from_file_char 

!
subroutine codes_read_from_file_char_size_t ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    character(len=1),dimension(:), intent(out)       :: buffer
    integer(kind=kindOfSize_t), intent(inout)        :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status
    integer(kind=kindOfInt)                          :: iret

    call grib_read_from_file_char_size_t ( ifile, buffer, nbytes, status )
end subroutine codes_read_from_file_char_size_t 

!
subroutine codes_write_bytes_char ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    character(len=1), dimension(:),intent(in)        :: buffer
    integer(kind=kindOfInt), intent(in)              :: nbytes
    integer(kind=kindOfInt),optional,intent(out)     :: status
    integer(kind=kindOfSize_t)                       :: ibytes
    integer(kind=kindOfInt)                          :: iret

    call grib_write_bytes_char ( ifile, buffer, nbytes, status )
end subroutine codes_write_bytes_char 

!
subroutine codes_write_bytes_char_size_t ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    character(len=1), dimension(:),intent(in)        :: buffer
    integer(kind=kindOfSize_t), intent(in)           :: nbytes
    integer(kind=kindOfInt),optional,intent(out)     :: status
    integer(kind=kindOfInt)                          :: iret

    call grib_write_bytes_char_size_t ( ifile, buffer, nbytes, status )
end subroutine codes_write_bytes_char_size_t 

!
subroutine codes_write_bytes_int4 ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    integer(kind=4), dimension(:),intent(in)         :: buffer
    integer(kind=kindOfInt), intent(in)              :: nbytes
    integer(kind=kindOfInt),optional,intent(out)     :: status
    integer(kind=kindOfSize_t)                       :: ibytes
    integer(kind=kindOfInt)                          :: iret

    call grib_write_bytes_int4 ( ifile, buffer, nbytes, status )
end subroutine codes_write_bytes_int4 

!
subroutine codes_write_bytes_int4_size_t ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    integer(kind=4), dimension(:),intent(in)         :: buffer
    integer(kind=kindOfSize_t), intent(in)           :: nbytes
    integer(kind=kindOfInt),optional,intent(out)     :: status
    integer(kind=kindOfInt)                          :: iret

    call grib_write_bytes_int4_size_t ( ifile, buffer, nbytes, status )
end subroutine codes_write_bytes_int4_size_t 

!
subroutine codes_write_bytes_real4 ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    real(kind=4), dimension(:),intent(in)            :: buffer
    integer(kind=kindOfInt), intent(in)              :: nbytes
    integer(kind=kindOfInt),optional,intent(out)     :: status
    integer(kind=kindOfSize_t)                       :: ibytes
    integer(kind=kindOfInt)                          :: iret

    call grib_write_bytes_real4 ( ifile, buffer, nbytes, status )
end subroutine codes_write_bytes_real4 

!
subroutine codes_write_bytes_real4_size_t ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    real(kind=4), dimension(:),intent(in)            :: buffer
    integer(kind=kindOfSize_t), intent(in)           :: nbytes
    integer(kind=kindOfInt),optional,intent(out)     :: status
    integer(kind=kindOfInt)                          :: iret

    call grib_write_bytes_real4_size_t ( ifile, buffer, nbytes, status )
end subroutine codes_write_bytes_real4_size_t 

!
subroutine codes_write_bytes_real8 ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    real(kind=8), dimension(:),intent(in)            :: buffer
    integer(kind=kindOfInt), intent(in)              :: nbytes
    integer(kind=kindOfInt),optional,intent(out)     :: status
    integer(kind=kindOfSize_t)                       :: ibytes
    integer(kind=kindOfInt)                          :: iret

    call grib_write_bytes_real8 ( ifile, buffer, nbytes, status )
end subroutine codes_write_bytes_real8 

!
subroutine codes_write_bytes_real8_size_t ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    real(kind=8), dimension(:),intent(in)            :: buffer
    integer(kind=kindOfSize_t), intent(in)           :: nbytes
    integer(kind=kindOfInt),optional,intent(out)     :: status
    integer(kind=kindOfInt)                          :: iret

    call grib_write_bytes_real8_size_t ( ifile, buffer, nbytes, status )
end subroutine codes_write_bytes_real8_size_t 

!
subroutine codes_close_file ( ifile , status )
    integer(kind=kindOfInt),intent(in)            :: ifile
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)                       :: iret

    call grib_close_file ( ifile , status )
end subroutine codes_close_file 

!
subroutine codes_count_in_file ( ifile, n , status)
    integer(kind=kindOfInt),intent(in)              :: ifile
    integer(kind=kindOfInt),intent(out)             :: n
    integer(kind=kindOfInt),optional,intent(out)    :: status
    integer(kind=kindOfInt)                         :: iret

    call grib_count_in_file ( ifile, n , status)
end subroutine codes_count_in_file 

!
subroutine codes_headers_only_new_from_file ( ifile, gribid , status)
    integer(kind=kindOfInt),intent(in)              :: ifile
    integer(kind=kindOfInt),intent(out)             :: gribid
    integer(kind=kindOfInt),optional,intent(out)    :: status
    integer(kind=kindOfInt)                         :: iret

    call grib_headers_only_new_from_file ( ifile, gribid , status)
end subroutine codes_headers_only_new_from_file 

!------------------
subroutine codes_new_from_file (ifile, gribid , product_kind, status)
    integer(kind=kindOfInt),intent(in)              :: ifile
    integer(kind=kindOfInt),intent(out)             :: gribid
    integer(kind=kindOfInt),intent(in)              :: product_kind
    integer(kind=kindOfInt),optional,intent(out)    :: status
    integer(kind=kindOfInt)                         :: iret

    if (product_kind == CODES_PRODUCT_GRIB) then
        call codes_grib_new_from_file ( ifile, gribid , status)
    else if (product_kind == CODES_PRODUCT_BUFR) then
        call codes_bufr_new_from_file ( ifile, gribid , status)
    else
        if (product_kind /= CODES_PRODUCT_ANY) then
            call grib_check(CODES_INTERNAL_ERROR,'codes_new_from_file','invalid_product_kind')
        end if
        call codes_any_new_from_file ( ifile, gribid , status)
    end if
end subroutine codes_new_from_file 

!
subroutine codes_any_new_from_file ( ifile, id , status)
    integer(kind=kindOfInt),intent(in)              :: ifile
    integer(kind=kindOfInt),intent(out)             :: id
    integer(kind=kindOfInt),optional,intent(out)    :: status
    integer(kind=kindOfInt)                         :: iret

    call any_new_from_file ( ifile, id , status)
end subroutine codes_any_new_from_file 

!
subroutine codes_grib_new_from_file ( ifile, gribid , status)
    integer(kind=kindOfInt),intent(in)              :: ifile
    integer(kind=kindOfInt),intent(out)             :: gribid
    integer(kind=kindOfInt),optional,intent(out)    :: status
    integer(kind=kindOfInt)                         :: iret

    call grib_new_from_file ( ifile, gribid , status)
end subroutine codes_grib_new_from_file 

!
subroutine codes_bufr_new_from_file ( ifile, bufrid , status)
    integer(kind=kindOfInt),intent(in)              :: ifile
    integer(kind=kindOfInt),intent(out)             :: bufrid
    integer(kind=kindOfInt),optional,intent(out)    :: status
    integer(kind=kindOfInt)                         :: iret

    call bufr_new_from_file ( ifile, bufrid, status)
end subroutine codes_bufr_new_from_file


!
subroutine codes_new_from_message_char( gribid, message, status )
    integer(kind=kindOfInt),intent(out)             :: gribid
    character(len=1), dimension(:),intent(in)         :: message
    integer(kind=kindOfInt),optional, intent(out)     :: status
    integer(kind=kindOfSize_t)                        :: size_bytes
    integer(kind=kindOfInt)                           :: iret

    call grib_new_from_message_char( gribid, message, status )
end subroutine codes_new_from_message_char

!
subroutine codes_new_from_message_int4 ( gribid, message, status )
    integer(kind=kindOfInt),intent(out)             :: gribid
    integer(kind=4), dimension(:),intent(in)          :: message
    integer(kind=kindOfInt),optional, intent(out)     :: status
    integer(kind=kindOfSize_t)                        :: size_bytes
    integer(kind=kindOfInt)                           :: iret

    call grib_new_from_message_int4 ( gribid, message, status )
end subroutine codes_new_from_message_int4

!
subroutine codes_new_from_samples  ( gribid, samplename, status )
    integer(kind=kindOfInt),          intent(out) :: gribid
    character(len=*), intent(in)                  :: samplename
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)                       :: iret

    call grib_new_from_samples  ( gribid, samplename, status )
end subroutine codes_new_from_samples

!
subroutine codes_release ( gribid, status )
    integer(kind=kindOfInt),          intent(in)  :: gribid
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)                       :: iret

    call grib_release ( gribid, status )
end subroutine codes_release

!
subroutine codes_clone ( gribid_src, gribid_dest, status )
    integer(kind=kindOfInt),          intent(in)  :: gribid_src
    integer(kind=kindOfInt),          intent(out) :: gribid_dest
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)                       :: iret

    call grib_clone ( gribid_src, gribid_dest, status )
end subroutine codes_clone

!
subroutine codes_grib_util_sections_copy ( gribid_from, gribid_to, what, gribid_out,status )
    integer(kind=kindOfInt),          intent(in)  :: gribid_from
    integer(kind=kindOfInt),          intent(in) :: gribid_to
    integer(kind=kindOfInt),          intent(out) :: gribid_out
    integer(kind=kindOfInt),          intent(in) :: what
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)                       :: iret

    call grib_util_sections_copy ( gribid_from, gribid_to, what, gribid_out,status )
end subroutine codes_grib_util_sections_copy

!
subroutine codes_copy_namespace ( gribid_src, namespace, gribid_dest, status )
    integer(kind=kindOfInt),          intent(in)  :: gribid_src
    integer(kind=kindOfInt),          intent(in)  :: gribid_dest
    character(LEN=*),                 intent(in)  :: namespace
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)                       :: iret

    call grib_copy_namespace ( gribid_src, namespace, gribid_dest, status )
end subroutine codes_copy_namespace 

!
subroutine codes_check ( status,caller,string )
    integer(kind=kindOfInt), intent(in)  :: status
    character(len=*),        intent(in)  :: caller
    character(len=*),        intent(in)  :: string

    call grib_check ( status,caller,string )
end subroutine codes_check

!
subroutine codes_grib_get_data_real4 ( gribid, lats, lons, values, status )
    integer(kind=kindOfInt),                  intent(in)   :: gribid
    real ( kind = kindOfFloat ), dimension(:),intent(out)  :: lats, lons
    real ( kind = kindOfFloat ), dimension(:),intent(out)  :: values
    integer(kind=kindOfInt),optional, intent(out)          :: status
    integer(kind=kindOfInt)                                :: iret
    integer(kind=kindOfSize_t)                             :: npoints

    call grib_get_data_real4 ( gribid, lats, lons, values, status )
end subroutine codes_grib_get_data_real4

!
subroutine codes_grib_get_data_real8 ( gribid, lats, lons, values, status )
    integer(kind=kindOfInt),                   intent(in)   :: gribid
    real ( kind = kindOfDouble ), dimension(:),intent(out)  :: lats, lons
    real ( kind = kindOfDouble ), dimension(:),intent(out)  :: values
    integer(kind=kindOfInt),optional, intent(out)           :: status
    integer(kind=kindOfInt)                                 :: iret
    integer(kind=kindOfSize_t)                              :: npoints

    call grib_get_data_real8 ( gribid, lats, lons, values, status )
end subroutine codes_grib_get_data_real8

!
subroutine codes_keys_iterator_new ( gribid, iterid, namespace, status )
    integer(kind=kindOfInt),          intent(in)   :: gribid
    integer(kind=kindOfInt),          intent(inout)  :: iterid
    character(LEN=*), intent(in)                   :: namespace
    integer(kind=kindOfInt),optional, intent(out)  :: status
    integer(kind=kindOfInt)                        :: iret

    call grib_keys_iterator_new ( gribid, iterid, namespace, status )
end subroutine codes_keys_iterator_new 

!
subroutine codes_keys_iterator_next ( iterid , status)
    integer(kind=kindOfInt),          intent(in)  :: iterid
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)                       :: iret

    call grib_keys_iterator_next ( iterid , status)
end subroutine codes_keys_iterator_next 

!
subroutine codes_keys_iterator_delete ( iterid , status)
    integer(kind=kindOfInt),          intent(in)  :: iterid
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)                       :: iret

    call grib_keys_iterator_delete ( iterid , status)
end subroutine codes_keys_iterator_delete 

!
subroutine codes_keys_iterator_get_name ( iterid, name, status )
    integer(kind=kindOfInt),          intent(in)    :: iterid
    character(LEN=*), intent(out)                   :: name
    integer(kind=kindOfInt),optional, intent(out)   :: status
    integer(kind=kindOfInt)                         :: iret

    call grib_keys_iterator_get_name ( iterid, name, status )
end subroutine codes_keys_iterator_get_name 

!
subroutine codes_keys_iterator_rewind ( iterid, status )
    integer(kind=kindOfInt),          intent(in)    :: iterid
    integer(kind=kindOfInt),optional, intent(out)   :: status
    integer(kind=kindOfInt)                         :: iret

    call grib_keys_iterator_rewind ( iterid, status )
end subroutine codes_keys_iterator_rewind

!
subroutine codes_dump ( gribid , status)
    integer(kind=kindOfInt),          intent(in)  :: gribid
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)                       :: iret

    call grib_dump ( gribid , status)
end subroutine codes_dump

!
subroutine codes_get_error_string ( error, error_message, status )
    integer(kind=kindOfInt),          intent(in)  :: error
    character(len=*), intent(out)                 :: error_message
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)                       :: iret

    call grib_get_error_string ( error, error_message, status )
end subroutine codes_get_error_string

!
subroutine codes_get_size_int ( gribid, key, size , status)
    integer(kind=kindOfInt),          intent(in)  :: gribid
    character(len=*), intent(in)                  :: key
    integer(kind=kindOfInt),          intent(out) :: size
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)                       :: iret

    call grib_get_size_int ( gribid, key, size , status)
end subroutine codes_get_size_int

!
subroutine codes_get_size_long ( gribid, key, size , status)
    integer(kind=kindOfInt),          intent(in)  :: gribid
    character(len=*), intent(in)                  :: key
    integer(kind=kindOfLong),          intent(out) :: size
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)                       :: iret

    call grib_get_size_long ( gribid, key, size , status)
end subroutine codes_get_size_long

!
subroutine codes_get_int(gribid,key,value,status)
    integer(kind=kindOfInt),          intent(in)  :: gribid
    character(len=*), intent(in)                  :: key
    integer(kind = kindOfInt),     intent(out)    :: value
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)                       :: iret

    call grib_get_int(gribid,key,value,status)
end subroutine codes_get_int

!
subroutine codes_get_long(gribid,key,value,status)
    integer(kind=kindOfInt),          intent(in)  :: gribid
    character(len=*), intent(in)                  :: key
    integer(kind = kindOfLong),     intent(out)    :: value
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)                       :: iret

    call grib_get_long(gribid,key,value,status)
end subroutine codes_get_long

!
subroutine codes_is_missing(gribid,key,is_missing,status)
    integer(kind=kindOfInt),          intent(in)  :: gribid
    character(len=*), intent(in)                  :: key
    integer(kind = kindOfInt),     intent(out)    :: is_missing
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)                       :: iret

    call grib_is_missing(gribid,key,is_missing,status)
end subroutine codes_is_missing

!
subroutine codes_is_defined(gribid,key,is_defined,status)
    integer(kind=kindOfInt),          intent(in)  :: gribid
    character(len=*), intent(in)                  :: key
    integer(kind = kindOfInt),     intent(out)    :: is_defined
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)                       :: iret

    call grib_is_defined(gribid,key,is_defined,status)
end subroutine codes_is_defined

!
subroutine codes_get_real4 ( gribid, key, value, status )
    integer(kind=kindOfInt),          intent(in)             :: gribid
    character(len=*),                 intent(in)             :: key
    real(kind = kindOfFloat),         intent(out)            :: value
    integer(kind=kindOfInt),optional, intent(out)            :: status
    integer(kind=kindOfInt)                                  :: iret

    call grib_get_real4 ( gribid, key, value, status )
end subroutine codes_get_real4

!
subroutine codes_get_real8 ( gribid, key, value , status)
    integer(kind=kindOfInt),          intent(in)      :: gribid
    character(len=*),                 intent(in)      :: key
    real(kind = kindOfDouble),        intent(out)     :: value
    integer(kind=kindOfInt),optional, intent(out)     :: status
    integer(kind=kindOfInt)                           :: iret

    call grib_get_real8 ( gribid, key, value , status)
end subroutine codes_get_real8

!
subroutine codes_get_string ( gribid, key, value, status )
    integer(kind=kindOfInt),          intent(in)  :: gribid
    character(len=*),                 intent(in)  :: key
    character(len=*),                 intent(out) :: value
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)                       :: iret

    call grib_get_string ( gribid, key, value, status )
end subroutine codes_get_string

!
subroutine codes_get_int_array ( gribid, key, value, status )
    integer(kind=kindOfInt),               intent(in)  :: gribid
    character(len=*),      intent(in)  :: key
    integer(kind=kindOfInt), dimension(:),allocatable,intent(inout) :: value
    integer(kind=kindOfInt),optional, intent(out)      :: status
    integer(kind=kindOfInt)                            :: iret
    integer(kind=kindOfInt)                            :: nb_values

    call grib_get_int_array ( gribid, key, value, status )
end subroutine codes_get_int_array 

!
subroutine codes_get_long_array ( gribid, key, value, status )
    integer(kind=kindOfInt),               intent(in)  :: gribid
    character(len=*),      intent(in)  :: key
    integer(kind=kindOfLong), dimension(:),allocatable,intent(inout) :: value
    integer(kind=kindOfInt),optional, intent(out)      :: status
    integer(kind=kindOfInt)                            :: iret
    integer(kind=kindOfInt)                            :: nb_values

    call grib_get_long_array ( gribid, key, value, status )
end subroutine codes_get_long_array 

!
subroutine codes_get_byte_array ( gribid, key, value, length, status )
    integer(kind=kindOfInt),               intent(in)  :: gribid
    character(len=*),                      intent(in)  :: key
    character(len=1),        dimension(:), intent(inout) :: value
    integer(kind=kindOfInt), optional,     intent(out) :: length
    integer(kind=kindOfInt), optional,     intent(out) :: status
    integer(kind=kindOfInt)                            :: iret
    integer(kind=kindOfInt)                            :: nb_values
    character                                          :: bytes(size(value))

    call grib_get_byte_array ( gribid, key, value, length, status )
end subroutine codes_get_byte_array 

!
subroutine codes_get_real4_array ( gribid, key, value, status)
    integer(kind=kindOfInt),                 intent(in)  :: gribid
    character(len=*),                        intent(in)  :: key
    real(kind = kindOfFloat), dimension(:),allocatable, intent(inout) :: value
    integer(kind=kindOfInt),optional,        intent(out) :: status
    integer(kind=kindOfInt)                              :: iret
    integer(kind=kindOfInt)                              :: nb_values

    call grib_get_real4_array ( gribid, key, value, status)
end subroutine codes_get_real4_array

!
subroutine codes_get_real8_array ( gribid, key, value, status )
    integer(kind=kindOfInt),                   intent(in)  :: gribid
    character(len=*),                          intent(in)  :: key
    real(kind = kindOfDouble), dimension(:),allocatable, intent(inout) :: value
    integer(kind=kindOfInt),optional,          intent(out) :: status
    integer(kind=kindOfInt)                                :: iret
    integer(kind=kindOfInt)                                :: nb_values

    call grib_get_real8_array ( gribid, key, value, status )
end subroutine codes_get_real8_array 

!
subroutine codes_get_real4_element ( gribid, key, index,value, status )
    integer(kind=kindOfInt),                     intent(in)  :: gribid
    character(len=*),                            intent(in)  :: key
    integer(kind=kindOfInt),                       intent(in)  :: index
    real(kind = kindOfFloat),                    intent(out) :: value
    integer(kind=kindOfInt),optional,            intent(out) :: status
    integer(kind=kindOfInt)                                  :: iret

    call grib_get_real4_element ( gribid, key, index,value, status )
end subroutine codes_get_real4_element 

!
subroutine codes_get_real8_element ( gribid, key, index,value, status )
    integer(kind=kindOfInt),                     intent(in)  :: gribid
    character(len=*),                            intent(in)  :: key
    integer(kind=kindOfInt),                  intent(in)  :: index
    real(kind = kindOfDouble),                   intent(out) :: value
    integer(kind=kindOfInt),optional,            intent(out) :: status
    integer(kind=kindOfInt)                                  :: iret

    call grib_get_real8_element ( gribid, key, index,value, status )
end subroutine codes_get_real8_element

!
subroutine codes_get_real4_elements ( gribid, key, index,value, status )
    integer(kind=kindOfInt),                  intent(in)  :: gribid
    character(len=*),                         intent(in)  :: key
    integer(kind=kindOfInt),dimension(:),  intent(in)  :: index
    real(kind = kindOfFloat), dimension(:),   intent(out) :: value
    integer(kind=kindOfInt),optional,         intent(out) :: status
    integer(kind=kindOfInt)                               :: iret
    integer(kind=kindOfInt)                               ::npoints

    call grib_get_real4_elements ( gribid, key, index,value, status )
end subroutine codes_get_real4_elements 

!
subroutine codes_get_real8_elements ( gribid, key, index,value, status )
    integer(kind=kindOfInt),                   intent(in)  :: gribid
    character(len=*),                          intent(in)  :: key
    integer(kind=kindOfInt),dimension(:),   intent(in)  :: index
    real(kind = kindOfDouble), dimension(:),   intent(out) :: value
    integer(kind=kindOfInt),optional,          intent(out) :: status
    integer(kind=kindOfInt)                                :: iret
    integer(kind=kindOfInt)                                :: npoints

    call grib_get_real8_elements ( gribid, key, index,value, status )
end subroutine codes_get_real8_elements 

!
subroutine codes_set_int ( gribid, key, value, status )
    integer(kind=kindOfInt),          intent(in)  :: gribid
    character(len=*), intent(in)                  :: key
    integer(kind=kindOfInt),          intent(in)  :: value
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)                       :: iret

    call grib_set_int ( gribid, key, value, status )
end subroutine codes_set_int

!
subroutine codes_set_long ( gribid, key, value, status )
    integer(kind=kindOfInt),          intent(in)  :: gribid
    character(len=*), intent(in)                  :: key
    integer(kind=kindOfLong),         intent(in)  :: value
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)                       :: iret

    call grib_set_long ( gribid, key, value, status )
end subroutine codes_set_long

!
subroutine codes_set_real4 ( gribid, key, value, status )
    integer(kind=kindOfInt),                 intent(in)  :: gribid
    character(len=*),                        intent(in)  :: key
    real(kind = kindOfFloat),                intent(in)  :: value
    integer(kind=kindOfInt),optional,        intent(out) :: status
    integer(kind=kindOfInt)                              :: iret

    call grib_set_real4 ( gribid, key, value, status )
end subroutine codes_set_real4 

!
subroutine codes_set_real8 ( gribid, key, value, status )
    integer(kind=kindOfInt),                  intent(in)  :: gribid
    character(len=*),                         intent(in)  :: key
    real(kind = kindOfDouble),                intent(in)  :: value
    integer(kind=kindOfInt),optional,         intent(out) :: status
    integer(kind=kindOfInt)                               :: iret

    call grib_set_real8 ( gribid, key, value, status )
end subroutine codes_set_real8 

!
subroutine codes_set_int_array ( gribid, key, value, status)
    integer(kind=kindOfInt),                intent(in)  :: gribid
    character(len=*),                       intent(in)  :: key
    integer(kind=kindOfInt), dimension(:),  intent(in)  :: value
    integer(kind=kindOfInt),optional,       intent(out) :: status
    integer(kind=kindOfInt)                             :: iret
    integer(kind=kindOfInt)                             :: nb_values

    call grib_set_int_array ( gribid, key, value, status)
end subroutine codes_set_int_array

!
subroutine codes_set_long_array ( gribid, key, value, status)
    integer(kind=kindOfInt),                intent(in)  :: gribid
    character(len=*),                       intent(in)  :: key
    integer(kind=kindOfLong), dimension(:),  intent(in)  :: value
    integer(kind=kindOfInt),optional,       intent(out) :: status
    integer(kind=kindOfInt)                             :: iret
    integer(kind=kindOfInt)                             :: nb_values

    call grib_set_long_array ( gribid, key, value, status)
end subroutine codes_set_long_array

!
subroutine codes_set_byte_array ( gribid, key, value, length, status )
    integer(kind=kindOfInt),               intent(in)  :: gribid
    character(len=*),                      intent(in)  :: key
    character(len=1),        dimension(:), intent(in)  :: value
    integer(kind=kindOfInt), optional,     intent(out) :: length
    integer(kind=kindOfInt), optional,     intent(out) :: status
    integer(kind=kindOfInt)                            :: iret
    integer(kind=kindOfInt)                            :: nb_values
    character                                          :: bytes(size(value))

    call grib_set_byte_array ( gribid, key, value, length, status )
end subroutine codes_set_byte_array 

!
subroutine codes_set_real4_array ( gribid, key, value, status )
    integer(kind=kindOfInt),                 intent(in)  :: gribid
    character(len=*),                        intent(in)  :: key
    real(kind = kindOfFloat), dimension(:),  intent(in)  :: value
    integer(kind=kindOfInt),optional,        intent(out) :: status
    integer(kind=kindOfInt)                              :: iret
    integer(kind=kindOfInt)                              :: nb_values

    call grib_set_real4_array ( gribid, key, value, status )
end subroutine codes_set_real4_array 

!
subroutine codes_set_real8_array ( gribid, key, value, status)
    integer(kind=kindOfInt),                  intent(in)  :: gribid
    character(len=*),                         intent(in)  :: key
    real(kind = kindOfDouble), dimension(:),  intent(in)  :: value
    integer(kind=kindOfInt),optional,         intent(out) :: status
    integer(kind=kindOfInt)                               :: iret
    integer(kind=kindOfInt)                               :: nb_values

    call grib_set_real8_array ( gribid, key, value, status)
end subroutine codes_set_real8_array

!
subroutine codes_set_force_real4_array ( gribid, key, value, status )
    integer(kind=kindOfInt),                 intent(in)  :: gribid
    character(len=*),                        intent(in)  :: key
    real(kind = kindOfFloat), dimension(:),  intent(in)  :: value
    integer(kind=kindOfInt),optional,        intent(out) :: status
    integer(kind=kindOfInt)                              :: iret
    integer(kind=kindOfInt)                              :: nb_values

    call grib_set_force_real4_array ( gribid, key, value, status )
end subroutine codes_set_force_real4_array

!
subroutine codes_set_force_real8_array ( gribid, key, value, status)
    integer(kind=kindOfInt),                  intent(in)  :: gribid
    character(len=*),                         intent(in)  :: key
    real(kind = kindOfDouble), dimension(:),  intent(in)  :: value
    integer(kind=kindOfInt),optional,         intent(out) :: status
    integer(kind=kindOfInt)                               :: iret
    integer(kind=kindOfInt)                               :: nb_values

    call grib_set_force_real8_array ( gribid, key, value, status)
end subroutine codes_set_force_real8_array

!
subroutine codes_set_string  ( gribid, key, value , status)
    integer(kind=kindOfInt),                      intent(in)  :: gribid
    character(len=*),                             intent(in)  :: key
    character(len=*),                             intent(in)  :: value
    integer(kind=kindOfInt),optional,             intent(out) :: status
    integer(kind=kindOfInt)                                   :: iret

    call grib_set_string  ( gribid, key, value , status)
end subroutine codes_set_string

!
subroutine codes_get_message_size_int  ( gribid, nbytes, status)
    integer(kind=kindOfInt),                      intent(in)  :: gribid
    integer(kind=kindOfInt),                      intent(out) :: nbytes
    integer(kind=kindOfInt),optional,             intent(out) :: status
    integer(kind=kindOfInt)                                   :: iret
    integer(kind=kindOfSize_t)                                :: ibytes

    call grib_get_message_size_int  ( gribid, nbytes, status)
end subroutine codes_get_message_size_int

!
subroutine codes_get_message_size_size_t  ( gribid, nbytes, status)
    integer(kind=kindOfInt),                      intent(in)  :: gribid
    integer(kind=kindOfSize_t),                   intent(out) :: nbytes
    integer(kind=kindOfInt),optional,             intent(out) :: status
    integer(kind=kindOfInt)                                   :: iret

    call grib_get_message_size_size_t  ( gribid, nbytes, status)
end subroutine codes_get_message_size_size_t

!
subroutine codes_copy_message ( gribid, message, status )
    integer(kind=kindOfInt),                      intent(in)  :: gribid
    character(len=1), dimension(:),               intent(out) :: message
    integer(kind=kindOfInt),optional,             intent(out) :: status
    integer(kind=kindOfInt)      :: iret
    integer(kind=kindOfSize_t)   :: size_bytes

    call grib_copy_message ( gribid, message, status )
end subroutine codes_copy_message

!
subroutine codes_write ( gribid, ifile  , status)
    integer(kind=kindOfInt),          intent(in)  :: gribid
    integer(kind=kindOfInt),          intent(in)  :: ifile
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)               :: iret

    call grib_write ( gribid, ifile  , status)
end subroutine codes_write

!
subroutine codes_grib_multi_write ( multigribid, ifile  , status)
    integer(kind=kindOfInt),          intent(in)  :: multigribid
    integer(kind=kindOfInt),          intent(in)  :: ifile
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)               :: iret

    call grib_multi_write ( multigribid, ifile  , status)
end subroutine codes_grib_multi_write 

!
subroutine codes_grib_multi_append ( ingribid, startsection, multigribid  , status)
    integer(kind=kindOfInt),          intent(in)  :: ingribid
    integer(kind=kindOfInt),          intent(in)  :: startsection
    integer(kind=kindOfInt),          intent(out) :: multigribid
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)               :: iret

    call grib_multi_append ( ingribid, startsection, multigribid  , status)
end subroutine codes_grib_multi_append 

!
subroutine codes_grib_find_nearest_multiple(gribid,is_lsm,  &
                 inlats,inlons,outlats,outlons,        &
                 values,distances, indexes,status)
    integer(kind=kindOfInt),                 intent(in)    :: gribid
    logical,                                 intent(in)    :: is_lsm
    real(kind = kindOfDouble), dimension(:), intent(in)    :: inlats
    real(kind = kindOfDouble), dimension(:), intent(in)    :: inlons
    real(kind = kindOfDouble), dimension(:), intent(out)   :: outlats
    real(kind = kindOfDouble), dimension(:), intent(out)   :: outlons
    real(kind = kindOfDouble), dimension(:), intent(out)   :: distances
    real(kind = kindOfDouble), dimension(:), intent(out)   :: values
    integer(kind = kindOfInt), dimension(:), intent(out)  :: indexes
    integer(kind=kindOfInt),optional, intent(out)         :: status
    integer(kind=kindOfInt)                               :: iret
    integer(kind=kindOfInt) :: npoints
    integer(kind=kindOfInt) :: intis_lsm

    call grib_find_nearest_multiple(gribid,is_lsm,   &
                 inlats,inlons,outlats,outlons,      &
                 values,distances, indexes,status)
end subroutine codes_grib_find_nearest_multiple

!
subroutine codes_grib_find_nearest_single(gribid,is_lsm,  &
                 inlat,inlon,outlat,outlon,          &
                 value,distance, index,status)
    integer(kind=kindOfInt),   intent(in)   :: gribid
    logical,                   intent(in)   :: is_lsm
    real(kind = kindOfDouble), intent(in)   :: inlat
    real(kind = kindOfDouble), intent(in)   :: inlon
    real(kind = kindOfDouble), intent(out)  :: outlat
    real(kind = kindOfDouble), intent(out)  :: outlon
    real(kind = kindOfDouble), intent(out)  :: distance
    real(kind = kindOfDouble), intent(out)  :: value
    integer(kind = kindOfInt), intent(out)  :: index
    integer(kind=kindOfInt),optional, intent(out)  :: status
    integer(kind=kindOfInt)                        :: iret
    integer(kind=kindOfInt)                 :: intis_lsm =0

    call grib_find_nearest_single(gribid,is_lsm,  &
                 inlat,inlon,outlat,outlon,       &
                 value,distance, index,status)
end subroutine codes_grib_find_nearest_single

!
subroutine codes_grib_find_nearest_four_single(gribid,is_lsm, &
                 inlat,inlon,outlat,outlon,              &
                 value,distance, index,status)
    integer(kind=kindOfInt),                  intent(in)    :: gribid
    logical,                                  intent(in)    :: is_lsm
    real(kind = kindOfDouble), intent(in)    :: inlat
    real(kind = kindOfDouble), intent(in)    :: inlon
    real(kind = kindOfDouble), dimension(4), intent(out)   :: outlat
    real(kind = kindOfDouble), dimension(4), intent(out)   :: outlon
    real(kind = kindOfDouble), dimension(4), intent(out)   :: distance
    real(kind = kindOfDouble), dimension(4), intent(out)   :: value
    integer(kind = kindOfInt), dimension(4), intent(out)  :: index
    integer(kind=kindOfInt),optional, intent(out)         :: status
    integer(kind=kindOfInt)                               :: iret
    integer(kind=kindOfInt) :: intis_lsm =0

    call grib_find_nearest_four_single(gribid,is_lsm, &
                 inlat,inlon,outlat,outlon,           &
                 value,distance, index,status)
  end subroutine codes_grib_find_nearest_four_single

!
subroutine codes_grib_multi_support_on (status  )
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)               :: iret

    call grib_multi_support_on (status  )
end subroutine codes_grib_multi_support_on 

!
subroutine codes_grib_multi_support_off ( status  )
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)               :: iret

    call grib_multi_support_off ( status  )
end subroutine codes_grib_multi_support_off 

!
subroutine codes_gribex_mode_on (status )
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)               :: iret

    call grib_gribex_mode_on (status )
end subroutine codes_gribex_mode_on 

!
subroutine codes_gribex_mode_off (status )
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)               :: iret

    call grib_gribex_mode_off (status )
end subroutine codes_gribex_mode_off 

!
subroutine codes_skip_computed ( iterid , status)
    integer(kind=kindOfInt),          intent(in)  :: iterid
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)   :: iret

    call grib_skip_computed ( iterid , status)
end subroutine codes_skip_computed 

!
subroutine codes_skip_coded  ( iterid, status )
    integer(kind=kindOfInt),          intent(in)  :: iterid
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)   :: iret

    call grib_skip_coded  ( iterid, status )
end subroutine codes_skip_coded  

!
subroutine codes_skip_duplicates ( iterid, status )
    integer(kind=kindOfInt),          intent(in)  :: iterid
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt) :: iret

    call grib_skip_duplicates ( iterid, status )
end subroutine codes_skip_duplicates 

!
subroutine codes_skip_read_only  ( iterid, status )
    integer(kind=kindOfInt),          intent(in)  :: iterid
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)  :: iret

    call grib_skip_read_only  ( iterid, status )
end subroutine codes_skip_read_only  

end module eccodes
