
  contains

!>
!> Wrapper for eccodes
!>

!> Set as missing the value for a key in a message.
!>
!> It can be used to set a missing value in the header but not in \n
!> the data values. To set missing data values see the bitmap examples.\n
!>
!> In case of error, if the status parameter (optional) is not given, the program will
!> exit with an error message.\n Otherwise the error message can be
!> gathered with @ref codes_get_error_string.
!>
!> \b Examples: \ref grib_set_missing.f90 "grib_set_missing.f90"
!>
!> @param id      ID of the message loaded in memory
!> @param key     key name
!> @param status  CODES_SUCCESS if OK, integer value on error
subroutine codes_set_missing ( id, key,  status )
    integer(kind=kindOfInt),                 intent(in)  :: id
    character(len=*),                        intent(in)  :: key
    integer(kind=kindOfInt),optional, intent(out)        :: status

    call grib_set_missing ( id, key,  status )
end subroutine codes_set_missing 

!> Create a new index form a file. The file is indexed with the keys in argument.
!>
!> In case of error, if the status parameter (optional) is not given, the program will
!> exit with an error message.\n Otherwise the error message can be
!> gathered with @ref codes_get_error_string.
!>
!> \b Examples: \ref grib_index.f90 "grib_index.f90"
!>
!> @param indexid   id of the newly created index
!> @param filename  name of the file of messages to be indexed
!> @param keys      comma separated list of keys for the index.
!>                  The type of the key can be explicitly declared appending :l for long (or alternatively :i), :d for double, :s for string to the key name.
!>                  If the type is not declared explicitly, the native type is assumed.
!> @param status    CODES_SUCCESS if OK, integer value on error
subroutine codes_index_create ( indexid, filename, keys,  status )
    integer(kind=kindOfInt),         intent(inout)  :: indexid
    character(len=*), intent(in)                    :: filename
    character(len=*), intent(in)                    :: keys
    integer(kind=kindOfInt),optional, intent(out)   :: status

    call grib_index_create ( indexid, filename, keys,  status )
end subroutine codes_index_create 

  !> Add a file to an index.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param indexid     id of the index I want to add a file to
  !> @param filename    name of the file I want to add to the index
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_index_add_file ( indexid, filename, status )
    integer(kind=kindOfInt), intent(in)             :: indexid
    character(len=*), intent(in)                    :: filename
    integer(kind=kindOfInt),optional, intent(out)   :: status

    call grib_index_add_file ( indexid, filename, status )
end subroutine codes_index_add_file 

  !> Get the number of distinct values of the key in argument contained in the index. The key must belong to the index.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param  indexid     id of an index created from a file. The index must have been created with the key in argument.
  !> @param key        key for which the number of values is computed
  !> @param size       number of distinct values of the key in the index
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_index_get_size_long( indexid, key, size,  status )
    integer(kind=kindOfInt),         intent(in)     :: indexid
    character(len=*), intent(in)                    :: key
    integer(kind=kindOfLong),       intent(out)     :: size
    integer(kind=kindOfInt),optional, intent(out)   :: status

    call grib_index_get_size_long( indexid, key, size,  status )
end subroutine codes_index_get_size_long

  !> Get the number of distinct values of the key in argument contained in the index. The key must belong to the index.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param  indexid     id of an index created from a file. The index must have been created with the key in argument.
  !> @param key        key for which the number of values is computed
  !> @param size       number of distinct values of the key in the index
  !> @param status     CODES_SUCCESS if OK, integer value on error
subroutine codes_index_get_size_int( indexid, key, size,  status )
    integer(kind=kindOfInt),         intent(in)     :: indexid
    character(len=*), intent(in)                    :: key
    integer(kind=kindOfInt),       intent(out)      :: size
    integer(kind=kindOfInt),optional, intent(out)   :: status

    call grib_index_get_size_int( indexid, key, size,  status )
end subroutine codes_index_get_size_int

  !> Get the distinct values of the key in argument contained in the index. The key must belong to the index. This function is used when the type of the key was explicitly defined as long or when the native type of the key is long.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param  indexid   id of an index created from a file. The index must have been created with the key in argument.
  !> @param key        key for wich the values are returned
  !> @param values     array of values. The array must be allocated before entering this function and its size must be enough to contain all the values.
  !> @param status     CODES_SUCCESS if OK, integer value on error
subroutine codes_index_get_int( indexid, key, values,  status )
    integer(kind=kindOfInt),               intent(in)  :: indexid
    character(len=*),      intent(in)                  :: key
    integer(kind=kindOfInt), dimension(:), intent(out) :: values
    integer(kind=kindOfInt),optional, intent(out)      :: status

    call grib_index_get_int( indexid, key, values,  status )
end subroutine codes_index_get_int

  !> Get the distinct values of the key in argument contained in the index. The key must belong to the index. This function is used when the type of the key was explicitly defined as long or when the native type of the key is long.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param  indexid   id of an index created from a file. The index must have been created with the key in argument.
  !> @param key        key for wich the values are returned
  !> @param values     array of values. The array must be allocated before entering this function and its size must be enough to contain all the values.
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_index_get_long( indexid, key, values,  status )
    integer(kind=kindOfInt),               intent(in)   :: indexid
    character(len=*),      intent(in)                   :: key
    integer(kind=kindOfLong), dimension(:), intent(out) :: values
    integer(kind=kindOfInt),optional, intent(out)       :: status

    call grib_index_get_long( indexid, key, values,  status )
end subroutine codes_index_get_long

  !> Get the distinct values of the key in argument contained in the index. The key must belong to the index. This function is used when the type of the key was explicitly defined as long or when the native type of the key is long.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param  indexid   id of an index created from a file. The index must have been created with the key in argument.
  !> @param key        key for wich the values are returned
  !> @param values     array of values. The array must be allocated before entering this function and its size must be enough to contain all the values.
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_index_get_real8( indexid, key, values,  status )
    integer(kind=kindOfInt),               intent(in)  :: indexid
    character(len=*),      intent(in)                  :: key
    real(kind=kindOfDouble), dimension(:), intent(out) :: values
    integer(kind=kindOfInt),optional, intent(out)      :: status

    call grib_index_get_real8( indexid, key, values,  status )
end subroutine codes_index_get_real8

  !> Get the distinct values of the key in argument contained in the index. 
  !> The key must belong to the index. 
  !> This function is used when the type of the key was explicitly defined as string or when the native type of the key is string.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param  indexid   id of an index created from a file. The index must have been created with the key in argument.
  !> @param key        key for wich the values are returned
  !> @param values     array of values. The array must be allocated before entering this function and its size must be enough to contain all the values.
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_index_get_string( indexid, key, values,  status )
    integer(kind=kindOfInt),        intent(in)     :: indexid
    character(len=*),      intent(in)              :: key
    character(len=*), dimension(:), intent(out)    :: values
    integer(kind=kindOfInt),optional, intent(out)  :: status

    call grib_index_get_string( indexid, key, values,  status )
end subroutine codes_index_get_string

  !> Select the message subset with key==value. The value is a integer. 
  !> The key must have been created with string type or have string as native type if the type was not explicitly defined in the index creation.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param  indexid   id of an index created from a file. The index must have been created with the key in argument.
  !> @param key        key to be selected
  !> @param value      value of the key to select
  !> @param status     CODES_SUCCESS if OK, integer value on error
subroutine codes_index_select_string( indexid, key, value,  status )
    integer(kind=kindOfInt),               intent(in)  :: indexid
    character(len=*),      intent(in)                  :: key
    character(len=*), intent(in)                       :: value
    integer(kind=kindOfInt),optional, intent(out)      :: status

    call grib_index_select_string( indexid, key, value,  status )
end subroutine codes_index_select_string

  !> Select the message subset with key==value. The value is a integer. The key must have been created with integer type or have integer as native type if the type was not explicitly defined in the index creation.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param  indexid   id of an index created from a file. The index must have been created with the key in argument.
  !> @param key        key to be selected
  !> @param value      value of the key to select
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_index_select_int( indexid, key, value,  status )
    integer(kind=kindOfInt),               intent(in)  :: indexid
    character(len=*),      intent(in)                  :: key
    integer(kind=kindOfInt), intent(in)                :: value
    integer(kind=kindOfInt),optional, intent(out)      :: status

    call grib_index_select_int( indexid, key, value,  status )
end subroutine codes_index_select_int

  !> Select the message subset with key==value. The value is a integer. The key must have been created with integer type or have integer as native type if the type was not explicitly defined in the index creation.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param  indexid   id of an index created from a file. The index must have been created with the key in argument.
  !> @param key        key to be selected
  !> @param value      value of the key to select
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_index_select_long( indexid, key, value,  status )
    integer(kind=kindOfInt),               intent(in)  :: indexid
    character(len=*),      intent(in)                  :: key
    integer(kind=kindOfLong), intent(in)               :: value
    integer(kind=kindOfInt),optional, intent(out)      :: status

    call grib_index_select_long( indexid, key, value,  status )
end subroutine codes_index_select_long

  !> Select the message subset with key==value. The value is a real. The key must have been created with real type or have real as native type if the type was not explicitly defined in the index creation.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param  indexid   id of an index created from a file. The index must have been created with the key in argument.
  !> @param key        key to be selected
  !> @param value      value of the key to select
  !> @param status     CODES_SUCCESS if OK, integer value on error
subroutine codes_index_select_real8( indexid, key, value,  status )
    integer(kind=kindOfInt),               intent(in)  :: indexid
    character(len=*),      intent(in)                  :: key
    real(kind=kindOfDouble), intent(in)                :: value
    integer(kind=kindOfInt),optional, intent(out)      :: status

    call grib_index_select_real8( indexid, key, value,  status )
end subroutine codes_index_select_real8

  !> Create a new handle from an index after having selected the key values.
  !> All the keys belonging to the index must be selected before calling this function. Successive calls to this function will return all the handles compatible with the constraints defined selecting the values of the index keys.
  !> When no more handles are available from the index a NULL pointer is returned and the err variable is set to CODES_END_OF_INDEX.
  !>
  !> The message can be accessed through its msgid and it will be available\n
  !> until @ref codes_release is called.\n
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param indexid   id of an index created from a file.
  !> @param msgid     id of the message loaded in memory
  !> @param status    CODES_SUCCESS if OK, CODES_END_OF_FILE at the end of file, or error code
subroutine codes_new_from_index ( indexid, msgid , status)
    integer(kind=kindOfInt),intent(in)              :: indexid
    integer(kind=kindOfInt),intent(out)             :: msgid
    integer(kind=kindOfInt),optional,intent(out)    :: status

    call grib_new_from_index ( indexid, msgid , status)
end subroutine codes_new_from_index 

  !> Load an index file previously created with @ref codes_index_write.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param indexid      id of loaded index
  !> @param filename     name of the index file to load
  !> @param status       CODES_SUCCESS if OK, integer value on error
subroutine codes_index_read ( indexid, filename, status )
    integer(kind=kindOfInt),         intent(inout)  :: indexid
    character(len=*), intent(in)                    :: filename
    integer(kind=kindOfInt),optional, intent(out)   :: status

    call grib_index_read ( indexid, filename, status )
end subroutine codes_index_read 

  !> Saves an index to a file for later reuse. Index files can be read with
  !> @ref codes_index_read.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param indexid      id of the index to save to file
  !> @param filename     name of file to save the index to
  !> @param status       CODES_SUCCESS if OK, integer value on error
subroutine codes_index_write ( indexid, filename, status )
    integer(kind=kindOfInt),         intent(inout)  :: indexid
    character(len=*), intent(in)                    :: filename
    integer(kind=kindOfInt),optional, intent(out)   :: status

    call grib_index_write ( indexid, filename, status )
end subroutine codes_index_write 

  !> Delete the index.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> @param indexid   id of an index created from a file.
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_index_release ( indexid, status )
    integer(kind=kindOfInt),          intent(in)  :: indexid
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_index_release ( indexid, status )
end subroutine codes_index_release

  !> Open a file according to a mode.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> \b Examples: \ref grib_get_keys.f90 "grib_get_keys.f90"
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param filename    name of the file to be open
  !> @param mode        open mode can be 'r' (read only) or 'w' (write only)
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_open_file ( ifile, filename, mode, status )
    integer(kind=kindOfInt),intent(out)               :: ifile
    character(len=*), intent(in)                      :: filename
    character(LEN=*), intent(in)                      :: mode
    integer(kind=kindOfInt),optional, intent(out)     :: status

    call grib_open_file ( ifile, filename, mode, status )
end subroutine codes_open_file 

  !> Reads nbytes bytes into the buffer from a file opened with codes_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      binary buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_read_bytes_char ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    character(len=1),dimension(:), intent(out)       :: buffer
    integer(kind=kindOfInt), intent(in)              :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status

    call grib_read_bytes_char ( ifile, buffer, nbytes, status )
end subroutine codes_read_bytes_char 

  !> Reads nbytes bytes into the buffer from a file opened with codes_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      binary buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_read_bytes_char_size_t ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    character(len=1),dimension(:), intent(out)       :: buffer
    integer(kind=kindOfSize_t), intent(in)           :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status

    call grib_read_bytes_char_size_t ( ifile, buffer, nbytes, status )
end subroutine codes_read_bytes_char_size_t 

  !> Reads nbytes bytes into the buffer from a file opened with codes_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_read_bytes_int4 ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    integer(kind=4),dimension(:), intent(out)        :: buffer
    integer(kind=kindOfInt), intent(in)              :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status

    call grib_read_bytes_int4 ( ifile, buffer, nbytes, status )
end subroutine codes_read_bytes_int4 

  !> Reads nbytes bytes into the buffer from a file opened with codes_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_read_bytes_int4_size_t ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    integer(kind=4),dimension(:), intent(out)        :: buffer
    integer(kind=kindOfSize_t), intent(in)           :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status

    call grib_read_bytes_int4_size_t ( ifile, buffer, nbytes, status )
end subroutine codes_read_bytes_int4_size_t 

  !> Reads nbytes bytes into the buffer from a file opened with codes_open_file.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_read_bytes_real4 ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    real(kind=4),dimension(:), intent(out)           :: buffer
    integer(kind=kindOfInt), intent(in)              :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status

    call grib_read_bytes_real4 ( ifile, buffer, nbytes, status )
end subroutine codes_read_bytes_real4 

  !> Reads nbytes bytes into the buffer from a file opened with codes_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_read_bytes_real4_size_t ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    real(kind=4),dimension(:), intent(out)           :: buffer
    integer(kind=kindOfSize_t), intent(inout)        :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status

    call grib_read_bytes_real4_size_t ( ifile, buffer, nbytes, status )
end subroutine codes_read_bytes_real4_size_t 

  !> Reads nbytes bytes into the buffer from a file opened with codes_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_read_bytes_real8 ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    real(kind=8),dimension(:), intent(out)           :: buffer
    integer(kind=kindOfInt), intent(in)              :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status

    call grib_read_bytes_real8 ( ifile, buffer, nbytes, status )
end subroutine codes_read_bytes_real8 

  !> Reads nbytes bytes into the buffer from a file opened with codes_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_read_bytes_real8_size_t ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    real(kind=8),dimension(:), intent(out)           :: buffer
    integer(kind=kindOfSize_t), intent(inout)        :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status

    call grib_read_bytes_real8_size_t ( ifile, buffer, nbytes, status )
end subroutine codes_read_bytes_real8_size_t 

  !> Reads a message in the buffer array from the file opened with codes_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      binary buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_read_from_file_int4 ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    integer(kind=4),dimension(:), intent(out)        :: buffer
    integer(kind=kindOfInt), intent(inout)           :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status

    call grib_read_from_file_int4 ( ifile, buffer, nbytes, status )
end subroutine codes_read_from_file_int4 

  !> Reads a message in the buffer array from the file opened with codes_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      binary buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_read_from_file_int4_size_t ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    integer(kind=4),dimension(:), intent(out)        :: buffer
    integer(kind=kindOfSize_t), intent(inout)        :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status

    call grib_read_from_file_int4_size_t ( ifile, buffer, nbytes, status )
end subroutine codes_read_from_file_int4_size_t 

  !> Reads a message in the buffer array from the file opened with codes_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      binary buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_read_from_file_real4 ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    real(kind=4),dimension(:), intent(out)           :: buffer
    integer(kind=kindOfInt), intent(inout)           :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status

    call grib_read_from_file_real4 ( ifile, buffer, nbytes, status )
end subroutine codes_read_from_file_real4 

  !> Reads a message in the buffer array from the file opened with codes_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      binary buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_read_from_file_real4_size_t ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    real(kind=4),dimension(:), intent(out)           :: buffer
    integer(kind=kindOfSize_t), intent(inout)        :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status

    call grib_read_from_file_real4_size_t ( ifile, buffer, nbytes, status )
end subroutine codes_read_from_file_real4_size_t 

  !> Reads a message in the buffer array from the file opened with codes_open_file.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      binary buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_read_from_file_real8 ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    real(kind=8),dimension(:), intent(out)           :: buffer
    integer(kind=kindOfInt), intent(inout)           :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status

    call grib_read_from_file_real8 ( ifile, buffer, nbytes, status )
end subroutine codes_read_from_file_real8 

  !> Reads a message in the buffer array from the file opened with codes_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      binary buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_read_from_file_real8_size_t ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    real(kind=8),dimension(:), intent(out)           :: buffer
    integer(kind=kindOfSize_t), intent(inout)        :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status

    call grib_read_from_file_real8_size_t ( ifile, buffer, nbytes, status )
end subroutine codes_read_from_file_real8_size_t 

  !> Reads a message in the buffer array from the file opened with codes_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_read_from_file_char ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    character(len=1),dimension(:), intent(out)       :: buffer
    integer(kind=kindOfInt), intent(inout)           :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status

    call grib_read_from_file_char ( ifile, buffer, nbytes, status )
end subroutine codes_read_from_file_char

  !> Reads a message in the buffer array from the file opened with codes_open_file.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_read_from_file_char_size_t ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    character(len=1),dimension(:), intent(out)       :: buffer
    integer(kind=kindOfSize_t), intent(inout)        :: nbytes
    integer(kind=kindOfInt),optional, intent(out)    :: status

    call grib_read_from_file_char_size_t ( ifile, buffer, nbytes, status )
end subroutine codes_read_from_file_char_size_t 

  !> Write nbytes bytes from the buffer in a file opened with codes_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be written
  !> @param nbytes      number of bytes to be written
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_write_bytes_char ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    character(len=1), dimension(:),intent(in)        :: buffer
    integer(kind=kindOfInt), intent(in)              :: nbytes
    integer(kind=kindOfInt),optional,intent(out)     :: status

    call grib_write_bytes_char ( ifile, buffer, nbytes, status )
end subroutine codes_write_bytes_char 

  !> Write nbytes bytes from the buffer in a file opened with codes_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be written
  !> @param nbytes      number of bytes to be written
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_write_bytes_char_size_t ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    character(len=1), dimension(:),intent(in)        :: buffer
    integer(kind=kindOfSize_t), intent(in)           :: nbytes
    integer(kind=kindOfInt),optional,intent(out)     :: status

    call grib_write_bytes_char_size_t ( ifile, buffer, nbytes, status )
end subroutine codes_write_bytes_char_size_t 

  !> Write nbytes bytes from the buffer in a file opened with codes_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be written
  !> @param nbytes      number of bytes to be written
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_write_bytes_int4 ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    integer(kind=4), dimension(:),intent(in)         :: buffer
    integer(kind=kindOfInt), intent(in)              :: nbytes
    integer(kind=kindOfInt),optional,intent(out)     :: status

    call grib_write_bytes_int4 ( ifile, buffer, nbytes, status )
end subroutine codes_write_bytes_int4 

  !> Write nbytes bytes from the buffer in a file opened with codes_open_file.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be written
  !> @param nbytes      number of bytes to be written
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_write_bytes_int4_size_t ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    integer(kind=4), dimension(:),intent(in)         :: buffer
    integer(kind=kindOfSize_t), intent(in)           :: nbytes
    integer(kind=kindOfInt),optional,intent(out)     :: status

    call grib_write_bytes_int4_size_t ( ifile, buffer, nbytes, status )
end subroutine codes_write_bytes_int4_size_t 

  !> Write nbytes bytes from the buffer in a file opened with codes_open_file.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be written
  !> @param nbytes      number of bytes to be written
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_write_bytes_real4 ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    real(kind=4), dimension(:),intent(in)            :: buffer
    integer(kind=kindOfInt), intent(in)              :: nbytes
    integer(kind=kindOfInt),optional,intent(out)     :: status

    call grib_write_bytes_real4 ( ifile, buffer, nbytes, status )
end subroutine codes_write_bytes_real4

  !> Write nbytes bytes from the buffer in a file opened with codes_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be written
  !> @param nbytes      number of bytes to be written
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_write_bytes_real4_size_t ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    real(kind=4), dimension(:),intent(in)            :: buffer
    integer(kind=kindOfSize_t), intent(in)           :: nbytes
    integer(kind=kindOfInt),optional,intent(out)     :: status

    call grib_write_bytes_real4_size_t ( ifile, buffer, nbytes, status )
end subroutine codes_write_bytes_real4_size_t 

  !> Write nbytes bytes from the buffer in a file opened with codes_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be written
  !> @param nbytes      number of bytes to be written
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_write_bytes_real8 ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    real(kind=8), dimension(:),intent(in)            :: buffer
    integer(kind=kindOfInt), intent(in)              :: nbytes
    integer(kind=kindOfInt),optional,intent(out)     :: status

    call grib_write_bytes_real8 ( ifile, buffer, nbytes, status )
end subroutine codes_write_bytes_real8

  !> Write nbytes bytes from the buffer in a file opened with codes_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be written
  !> @param nbytes      number of bytes to be written
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_write_bytes_real8_size_t ( ifile, buffer, nbytes, status )
    integer(kind=kindOfInt),intent(in)               :: ifile
    real(kind=8), dimension(:),intent(in)            :: buffer
    integer(kind=kindOfSize_t), intent(in)           :: nbytes
    integer(kind=kindOfInt),optional,intent(out)     :: status

    call grib_write_bytes_real8_size_t ( ifile, buffer, nbytes, status )
end subroutine codes_write_bytes_real8_size_t 

  !> Close a file.
  !>
  !> If the \em fileid does not refer to an opened file an error code
  !> is returned in status.\n
  !>
  !> \b Examples: \ref grib_get_keys.f90 "grib_get_keys.f90"
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param ifile is the id of the file to be closed.
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_close_file ( ifile , status )
    integer(kind=kindOfInt),intent(in)            :: ifile
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_close_file ( ifile , status )
end subroutine codes_close_file

  !> Counts the messages in a file
  !>
  !> \b Examples: \ref count_messages.f90 "count_messages.f90"
  !>
  !> @param ifile    id of the file opened with @ref codes_open_file
  !> @param n        number of messages in the file
  !> @param status   CODES_SUCCESS if OK or error code
subroutine codes_count_in_file ( ifile, n , status)
    integer(kind=kindOfInt),intent(in)              :: ifile
    integer(kind=kindOfInt),intent(out)             :: n
    integer(kind=kindOfInt),optional,intent(out)    :: status

    call grib_count_in_file ( ifile, n , status)
end subroutine codes_count_in_file 

!
subroutine codes_headers_only_new_from_file ( ifile, gribid , status)
    integer(kind=kindOfInt),intent(in)              :: ifile
    integer(kind=kindOfInt),intent(out)             :: gribid
    integer(kind=kindOfInt),optional,intent(out)    :: status

    call grib_headers_only_new_from_file ( ifile, gribid , status)
end subroutine codes_headers_only_new_from_file 

  !> Load in memory a message from a file.
  !>
  !> The message can be accessed through its message ID and it will be available\n
  !> until @ref codes_release is called.\n
  !>
  !> \b Examples: \ref grib_get_keys.f90 "grib_get_keys.f90"
  !>
  !> @param ifile     id of the file opened with @ref codes_open_file
  !> @param msgid     id of the message loaded in memory
  !> @param product_kind One of CODES_PRODUCT_GRIB, CODES_PRODUCT_BUFR or CODES_PRODUCT_ANY
  !> @param status    CODES_SUCCESS if OK, CODES_END_OF_FILE at the end of file, or error code
subroutine codes_new_from_file (ifile, msgid , product_kind, status)
    integer(kind=kindOfInt),intent(in)              :: ifile
    integer(kind=kindOfInt),intent(out)             :: msgid
    integer(kind=kindOfInt),intent(in)              :: product_kind
    integer(kind=kindOfInt),optional,intent(out)    :: status

    if (product_kind == CODES_PRODUCT_GRIB) then
        call codes_grib_new_from_file ( ifile, msgid , status)
    else if (product_kind == CODES_PRODUCT_BUFR) then
        call codes_bufr_new_from_file ( ifile, msgid , status)
    else
        if (product_kind /= CODES_PRODUCT_ANY) then
            call grib_check(CODES_INTERNAL_ERROR,'new_from_file','invalid_product_kind')
        end if
        call codes_any_new_from_file ( ifile, msgid , status)
    end if
end subroutine codes_new_from_file

  !> Load in memory a message from a file.
  !>
  !> The message can be accessed through its msgid and it will be available\n
  !> until @ref codes_release is called.\n
  !>
  !> @param ifile     id of the file opened with @ref codes_open_file
  !> @param msgid     id of the message loaded in memory
  !> @param status    CODES_SUCCESS if OK, GRIB_END_OF_FILE at the end of file, or error code
subroutine codes_any_new_from_file ( ifile, msgid , status)
    integer(kind=kindOfInt),intent(in)              :: ifile
    integer(kind=kindOfInt),intent(out)             :: msgid
    integer(kind=kindOfInt),optional,intent(out)    :: status

    call any_new_from_file ( ifile, msgid , status)
end subroutine codes_any_new_from_file 

  !> Load in memory a GRIB message from a file.
  !>
  !> The message can be accessed through its gribid and it will be available\n
  !> until @ref codes_release is called.\n
  !>
  !> \b Examples: \ref grib_get_keys.f90 "grib_get_keys.f90"
  !>
  !> @param ifile     id of the file opened with @ref codes_open_file
  !> @param gribid    id of the GRIB loaded in memory
  !> @param status    CODES_SUCCESS if OK, GRIB_END_OF_FILE at the end of file, or error code
subroutine codes_grib_new_from_file ( ifile, gribid , status)
    integer(kind=kindOfInt),intent(in)              :: ifile
    integer(kind=kindOfInt),intent(out)             :: gribid
    integer(kind=kindOfInt),optional,intent(out)    :: status

    call grib_new_from_file ( ifile, gribid , status)
end subroutine codes_grib_new_from_file 

  !> Load in memory a BUFR message from a file.
  !>
  !> The message can be accessed through its bufrid and it will be available\n
  !> until @ref codes_release is called.\n
  !>
  !> \b Examples: \ref bufr_get_keys.f90 "bufr_get_keys.f90"
  !>
  !> @param ifile     id of the file opened with @ref codes_open_file
  !> @param bufrid    id of the BUFR loaded in memory
  !> @param status    CODES_SUCCESS if OK, GRIB_END_OF_FILE at the end of file, or error code
subroutine codes_bufr_new_from_file ( ifile, bufrid , status)
    integer(kind=kindOfInt),intent(in)              :: ifile
    integer(kind=kindOfInt),intent(out)             :: bufrid
    integer(kind=kindOfInt),optional,intent(out)    :: status

    call bufr_new_from_file ( ifile, bufrid, status)
end subroutine codes_bufr_new_from_file


  !> Create a new message in memory from a character array containting the coded message.
  !>
  !> The message can be accessed through its msgid and it will be available\n
  !> until @ref codes_release is called. A reference to the original coded\n
  !> message is kept in the new message structure.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> \b Examples: \ref grib_copy_message.f90 "grib_copy_message.f90"
  !>
  !> @param msgid       id of the message loaded in memory
  !> @param message     character array containing the coded message
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_new_from_message_char( msgid, message, status )
    integer(kind=kindOfInt),intent(out)               :: msgid
    character(len=1), dimension(:),intent(in)         :: message
    integer(kind=kindOfInt),optional, intent(out)     :: status

    call grib_new_from_message_char( msgid, message, status )
end subroutine codes_new_from_message_char

  !> Create a new message in memory from an integer array containting the coded message.
  !>
  !> The message can be accessed through its msgid and it will be available\n
  !> until @ref codes_release is called. A reference to the original coded\n
  !> message is kept in the new message structure.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> \b Examples: \ref grib_copy_message.f90 "grib_copy_message.f90"
  !>
  !> @param msgid       id of the message loaded in memory
  !> @param message     integer array containing the coded message
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_new_from_message_int4 ( msgid, message, status )
    integer(kind=kindOfInt),intent(out)               :: msgid
    integer(kind=4), dimension(:),intent(in)          :: message
    integer(kind=kindOfInt),optional, intent(out)     :: status

    call grib_new_from_message_int4 ( msgid, message, status )
end subroutine codes_new_from_message_int4

  !> Create a new valid gribid from a GRIB sample contained in a samples directory pointed
  !> by the environment variable ECCODES_SAMPLES_PATH.
  !> To know where the samples directory is run the codes_info tool.\n
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> \b Examples: \ref grib_samples.f90 "grib_samples.f90"
  !>
  !> @param gribid       id of the grib loaded in memory
  !> @param samplename name of the sample to be used
  !> @param status       CODES_SUCCESS if OK, integer value on error
subroutine codes_grib_new_from_samples  ( gribid, samplename, status )
    integer(kind=kindOfInt),          intent(out) :: gribid
    character(len=*), intent(in)                  :: samplename
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_new_from_samples  ( gribid, samplename, status )
end subroutine codes_grib_new_from_samples

  !> Create a new valid bufrid from a BUFR sample contained in a samples directory pointed
  !> by the environment variable ECCODES_SAMPLES_PATH.
  !> To know where the samples directory is run the codes_info tool.\n
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param bufrid       id of the BUFR loaded in memory
  !> @param samplename   name of the BUFR sample to be used
  !> @param status       CODES_SUCCESS if OK, integer value on error
subroutine codes_bufr_new_from_samples  ( bufrid, samplename, status )
      integer(kind=kindOfInt),          intent(out) :: bufrid
      character(len=*), intent(in)                  :: samplename
      integer(kind=kindOfInt),optional, intent(out) :: status
      integer(kind=kindOfInt)                       :: iret

      iret=codes_bufr_f_new_from_samples ( bufrid, samplename )
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'bufr_new_from_samples','('//samplename//')')
      endif
end subroutine codes_bufr_new_from_samples

  !> Free the memory for the message referred as msgid.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> \b Examples: \ref grib_get_keys.f90 "grib_get_keys.f90"
  !>
  !> @param msgid       id of the message loaded in memory
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_release ( msgid, status )
    integer(kind=kindOfInt),          intent(in)  :: msgid
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_release ( msgid, status )
end subroutine codes_release

  !> Create a copy of a message.
  !>
  !> Create a copy of a given message (\em msgid_src) giving a new\n
  !> message in memory (\em msgid_dest) exactly identical to the original one.\n
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> \b Examples: \ref grib_clone.f90 "grib_clone.f90"
  !>
  !> @param msgid_src     message to be cloned
  !> @param msgid_dest    new message returned
  !> @param status        CODES_SUCCESS if OK, integer value on error
subroutine codes_clone ( msgid_src, msgid_dest, status )
    integer(kind=kindOfInt),          intent(in)  :: msgid_src
    integer(kind=kindOfInt),          intent(out) :: msgid_dest
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_clone ( msgid_src, msgid_dest, status )
end subroutine codes_clone

!
subroutine codes_grib_util_sections_copy ( gribid_from, gribid_to, what, gribid_out,status )
    integer(kind=kindOfInt),          intent(in)  :: gribid_from
    integer(kind=kindOfInt),          intent(in) :: gribid_to
    integer(kind=kindOfInt),          intent(out) :: gribid_out
    integer(kind=kindOfInt),          intent(in) :: what
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_util_sections_copy ( gribid_from, gribid_to, what, gribid_out,status )
end subroutine codes_grib_util_sections_copy

  !> Copy the value of all the keys belonging to a namespace from the source message
  !> to the destination message
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !>
  !> @param gribid_src     source message
  !> @param gribid_dest    destination message
  !> @param namespace      namespace to be copied
  !> @param status         CODES_SUCCESS if OK, integer value on error
subroutine codes_copy_namespace ( gribid_src, namespace, gribid_dest, status )
    integer(kind=kindOfInt),          intent(in)  :: gribid_src
    integer(kind=kindOfInt),          intent(in)  :: gribid_dest
    character(LEN=*),                 intent(in)  :: namespace
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_copy_namespace ( gribid_src, namespace, gribid_dest, status )
end subroutine codes_copy_namespace

  !> Check the status returned by a subroutine.
  !>
  !> In case of error it stops the program, returns the error code to the shell
  !> and prints the error message.\n
  !>
  !> @param status      the status to be checked
  !> @param caller      name of the caller soubroutine
  !> @param string      a string variable from the caller routine (e.g. key,filename)
subroutine codes_check ( status,caller,string )
    integer(kind=kindOfInt), intent(in)  :: status
    character(len=*),        intent(in)  :: caller
    character(len=*),        intent(in)  :: string

    call grib_check ( status,caller,string )
end subroutine codes_check

  !> Get latitudes/longitudes/data values (real(4)).
  !>
  !> Latitudes, longitudes, data values arrays are returned.
  !> They must be properly allocated by the caller and their required
  !> dimension can be obtained with \ref codes_get_size or by getting (with \ref codes_get)
  !> the value of the integer key "numberOfPoints".
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param lats        latitudes array with dimension "size"
  !> @param lons        longitudes array with dimension "size"
  !> @param values      data values array with dimension "size"
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_grib_get_data_real4 ( gribid, lats, lons, values, status )
    integer(kind=kindOfInt),                  intent(in)   :: gribid
    real ( kind = kindOfFloat ), dimension(:),intent(out)  :: lats, lons
    real ( kind = kindOfFloat ), dimension(:),intent(out)  :: values
    integer(kind=kindOfInt),optional, intent(out)          :: status

    call grib_get_data_real4 ( gribid, lats, lons, values, status )
end subroutine codes_grib_get_data_real4

  !> Get latitudes/longitudes/data values (real(8)).
  !>
  !> Latitudes, longitudes, data values arrays are returned.
  !> They must be properly allocated by the calling program/function.
  !> Their required dimension can be obtained by getting (with \ref codes_get)
  !> the value of the integer key "numberOfPoints".
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param lats        latitudes array
  !> @param lons        longitudes array
  !> @param values      data values array
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_grib_get_data_real8 ( gribid, lats, lons, values, status )
    integer(kind=kindOfInt),                   intent(in)   :: gribid
    real ( kind = kindOfDouble ), dimension(:),intent(out)  :: lats, lons
    real ( kind = kindOfDouble ), dimension(:),intent(out)  :: values
    integer(kind=kindOfInt),optional, intent(out)           :: status

    call grib_get_data_real8 ( gribid, lats, lons, values, status )
end subroutine codes_grib_get_data_real8

  !> Create a new iterator on the keys.
  !>
  !> The keys iterator can be navigated to give all the key names which
  !> can then be used to get or set the key values with \ref codes_get or
  !> \ref codes_set.
  !> The set of keys returned can be controlled with the input variable
  !> namespace or using the functions
  !> \ref codes_skip_read_only, \ref codes_skip_duplicates,
  !> \ref codes_skip_coded,\ref codes_skip_computed.
  !> If namespace is a non-empty string only the keys belonging to
  !> that namespace are returned. Example namespaces are "ls" (to get the same
  !> default keys as the grib_ls) and "mars" to get the keys used by mars.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid       id of the message loaded in memory
  !> @param iterid      keys iterator id to be used in the keys iterator functions
  !> @param namespace   the namespace of the keys to search for (all the keys if empty)
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_keys_iterator_new ( msgid, iterid, namespace, status )
    integer(kind=kindOfInt),          intent(in)     :: msgid
    integer(kind=kindOfInt),          intent(inout)  :: iterid
    character(LEN=*), intent(in)                     :: namespace
    integer(kind=kindOfInt),optional, intent(out)    :: status

    call grib_keys_iterator_new ( msgid, iterid, namespace, status )
end subroutine codes_keys_iterator_new

  !> Advance to the next keys iterator value.
  !>
  !> @param iterid   keys iterator id created with @ref codes_keys_iterator_new
  !> @param status   CODES_SUCCESS if next iterator exists, integer value if no more elements to iterate on
subroutine codes_keys_iterator_next ( iterid , status)
    integer(kind=kindOfInt),          intent(in)  :: iterid
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_keys_iterator_next ( iterid , status)
end subroutine codes_keys_iterator_next

  !> Delete a keys iterator and free memory.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param iterid      keys iterator id created with @ref codes_keys_iterator_new
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_keys_iterator_delete ( iterid , status)
    integer(kind=kindOfInt),          intent(in)  :: iterid
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_keys_iterator_delete ( iterid , status)
end subroutine codes_keys_iterator_delete

  !> Get the name of a key from a keys iterator.
  !>
  !> If the status parameter (optional) is not given the program will exit with an error message\n
  !> otherwise the error message can be gathered with @ref codes_get_error_string.\n
  !>
  !> @param iterid      keys iterator id created with @ref codes_keys_iterator_new
  !> @param name        key name to be retrieved
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_keys_iterator_get_name ( iterid, name, status )
    integer(kind=kindOfInt),          intent(in)    :: iterid
    character(LEN=*), intent(out)                   :: name
    integer(kind=kindOfInt),optional, intent(out)   :: status

    call grib_keys_iterator_get_name ( iterid, name, status )
end subroutine codes_keys_iterator_get_name

  !> Rewind a keys iterator.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param iterid      keys iterator id created with @ref codes_keys_iterator_new
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_keys_iterator_rewind ( iterid, status )
    integer(kind=kindOfInt),          intent(in)    :: iterid
    integer(kind=kindOfInt),optional, intent(out)   :: status

    call grib_keys_iterator_rewind ( iterid, status )
end subroutine codes_keys_iterator_rewind

  !> Dump the content of a message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid      id of the message loaded in memory
  !> @param status     CODES_SUCCESS if OK, integer value on error
subroutine codes_dump ( msgid , status)
    integer(kind=kindOfInt),          intent(in)  :: msgid
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_dump (msgid, status)
end subroutine codes_dump

!> Get the error message given an error code
!>
!> @param error          error code
!> @param error_message  error message
!> @param status         CODES_SUCCESS if OK, integer value on error
subroutine codes_get_error_string ( error, error_message, status )
    integer(kind=kindOfInt),          intent(in)  :: error
    character(len=*), intent(out)                 :: error_message
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_get_error_string ( error, error_message, status )
end subroutine codes_get_error_string

  !> Get the size of an array key.
  !>
  !> To get the size of a key representing an array.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid       id of the message loaded in memory
  !> @param key         name of the key
  !> @param size        size of the array key
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_get_size_int ( msgid, key, size , status)
    integer(kind=kindOfInt),          intent(in)  :: msgid
    character(len=*), intent(in)                  :: key
    integer(kind=kindOfInt),          intent(out) :: size
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_get_size_int ( msgid, key, size , status)
end subroutine codes_get_size_int

  !> Get the size of an array key.
  !>
  !> To get the size of a key representing an array.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid       id of the message loaded in memory
  !> @param key         name of the key
  !> @param size        size of the array key
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_get_size_long ( msgid, key, size , status)
    integer(kind=kindOfInt),          intent(in)  :: msgid
    character(len=*), intent(in)                  :: key
    integer(kind=kindOfLong),          intent(out) :: size
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_get_size_long ( msgid, key, size , status)
end subroutine codes_get_size_long

  !> Get the integer value of a key from a message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid      id of the message loaded in memory
  !> @param key         key name
  !> @param value       the integer(4) value
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_get_int(msgid,key,value,status)
    integer(kind=kindOfInt),          intent(in)  :: msgid
    character(len=*), intent(in)                  :: key
    integer(kind = kindOfInt),     intent(out)    :: value
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_get_int(msgid,key,value,status)
end subroutine codes_get_int

  !> Get the integer value of a key from a message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid       id of the message loaded in memory
  !> @param key         key name
  !> @param value       the integer(4) value
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_get_long(msgid,key,value,status)
    integer(kind=kindOfInt),          intent(in)  :: msgid
    character(len=*), intent(in)                  :: key
    integer(kind = kindOfLong),     intent(out)    :: value
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_get_long(msgid,key,value,status)
end subroutine codes_get_long

  !> Check if the value of a key is MISSING.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid       id of the message loaded in memory
  !> @param key         key name
  !> @param is_missing  0->not missing, 1->missing
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_is_missing(msgid,key,is_missing,status)
    integer(kind=kindOfInt),          intent(in)  :: msgid
    character(len=*), intent(in)                  :: key
    integer(kind = kindOfInt),     intent(out)    :: is_missing
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_is_missing(msgid,key,is_missing,status)
end subroutine codes_is_missing

  !> Check if a key is defined (exists in the message)
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid       id of the message loaded in memory
  !> @param key         key name
  !> @param is_defined  0->not defined, 1->defined
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_is_defined(msgid,key,is_defined,status)
    integer(kind=kindOfInt),          intent(in)  :: msgid
    character(len=*), intent(in)                  :: key
    integer(kind = kindOfInt),     intent(out)    :: is_defined
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_is_defined(msgid,key,is_defined,status)
end subroutine codes_is_defined

  !> Get the real(4) value of a key from a message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid       id of the message loaded in memory
  !> @param key         key name
  !> @param value       the real(4) value
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_get_real4 ( msgid, key, value, status )
    integer(kind=kindOfInt),          intent(in)             :: msgid
    character(len=*),                 intent(in)             :: key
    real(kind = kindOfFloat),         intent(out)            :: value
    integer(kind=kindOfInt),optional, intent(out)            :: status

    call grib_get_real4 ( msgid, key, value, status )
end subroutine codes_get_real4

  !> Get the real(8) value of a key from a message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid       id of the message loaded in memory
  !> @param key         key name
  !> @param value       the real(8) value
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_get_real8 ( msgid, key, value , status)
    integer(kind=kindOfInt),          intent(in)      :: msgid
    character(len=*),                 intent(in)      :: key
    real(kind = kindOfDouble),        intent(out)     :: value
    integer(kind=kindOfInt),optional, intent(out)     :: status

    call grib_get_real8 ( msgid, key, value , status)
end subroutine codes_get_real8

  !> Get the character value of a key from a message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid       id of the message loaded in memory
  !> @param key         key name
  !> @param value       the real(8) value
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_get_string ( msgid, key, value, status )
    integer(kind=kindOfInt),          intent(in)  :: msgid
    character(len=*),                 intent(in)  :: key
    character(len=*),                 intent(out) :: value
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_get_string ( msgid, key, value, status )
end subroutine codes_get_string

  !> Get the string array of values for a key from a message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.\n
  !> Note: This function supports the \b allocatable array attribute
  !>
  !> @param msgid       id of the message loaded in memory
  !> @param key         key name
  !> @param value       string array value
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_get_string_array ( msgid, key, value, status )
    integer(kind=kindOfInt),               intent(in)        :: msgid
    character(len=*),      intent(in)                        :: key
    character(len=*), dimension(:),allocatable,intent(inout) :: value
    integer(kind=kindOfInt),optional, intent(out)            :: status

    character                 :: cvalue(size(value)*len(value(0)))
    integer(kind=kindOfInt)                            :: iret
    integer(kind=kindOfInt)                            :: nb_values
    integer(kind=kindOfInt)                            :: slen

    if (allocated(value) .eqv. .false.) then
      iret=CODES_NULL_POINTER
      if (present(status)) then
        status = iret
      else
        call grib_check(iret,'get',key)
      endif
    end if

    nb_values=size(value)
    slen=len(value(0))
    iret=grib_f_get_string_array ( msgid, key, cvalue , nb_values, slen )
    value=transfer(cvalue,value)

    if (iret /= 0) then
      call grib_f_write_on_fail(msgid)
    endif
    if (present(status)) then
      status = iret
    else
      call grib_check(iret,'get',key)
    endif

end subroutine codes_get_string_array 

  !> Set the string values for an array key in a message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid      id of the message loaded in memory
  !> @param key        key name
  !> @param value      string array value
  !> @param status     CODES_SUCCESS if OK, integer value on error
subroutine codes_set_string_array ( msgid, key, value, status )
    integer(kind=kindOfInt),                     intent(in)   :: msgid
    character(len=*),                            intent(in)   :: key
    character(len=*), dimension(:),allocatable,  intent(in)   :: value
    integer(kind=kindOfInt),optional,            intent(out)  :: status

    character                 :: cvalue(size(value)*len(value(0)))
    character                 :: svalue(len(value(0)))
    integer(kind=kindOfInt)   :: iret
    integer(kind=kindOfInt)   :: nb_values
    integer(kind=kindOfInt)   :: slen
    integer(kind=kindOfInt)   :: i,j

    nb_values=size(value)
    slen=len(value(0))
    j=1
    do i=1,nb_values
      !cvalue(j:j+slen-1)=transfer(trim(value(i)),svalue)
      cvalue(j:j+slen-1)=transfer(value(i),svalue)
      j=j+slen
    enddo

    iret=grib_f_set_string_array ( msgid, key, cvalue , nb_values, slen )

    if (iret /= 0) then
      call grib_f_write_on_fail(msgid)
    endif
    if (present(status)) then
      status = iret
    else
      call grib_check(iret,'set',key)
    endif

end subroutine codes_set_string_array 


  !> Get the integer array of values for a key from a message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.\n
  !> Note: This function supports the \b allocatable array attribute
  !>
  !> @param msgid       id of the message loaded in memory
  !> @param key         key name
  !> @param value       integer(4) array value
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_get_int_array ( msgid, key, value, status )
    integer(kind=kindOfInt),               intent(in)  :: msgid
    character(len=*),      intent(in)                  :: key
    integer(kind=kindOfInt), dimension(:),allocatable,intent(inout) :: value
    integer(kind=kindOfInt),optional, intent(out)      :: status

    integer(kind=kindOfInt)                            :: iret
    integer(kind=kindOfInt)                            :: nb_values
    integer(kind=kindOfInt)                            :: size_value
    integer(kind=kindOfInt)                            :: i

    iret=grib_f_get_size_int(msgid,key,nb_values)
    if (iret /= 0) then
      call grib_f_write_on_fail(msgid)
      if (present(status)) then
        status = iret
      else
        call grib_check(iret,'get',key)
      endif
      return
    endif
    if (allocated(value) .eqv. .false.) then
      allocate(value(nb_values))
    end if
    size_value=size(value)
    iret=grib_f_get_int_array ( msgid, key, value , nb_values )
    if (iret==0 .and. nb_values==1 .and. size_value/=1) then
      do i=2,size_value
        value(i)=value(1)
      enddo
    endif
  if (iret /= 0) then
    call grib_f_write_on_fail(msgid)
  endif
    if (present(status)) then
      status = iret
    else
      call grib_check(iret,'get',key)
    endif

end subroutine codes_get_int_array 

  !> Get the integer array of values for a key from a message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.\n
  !> Note: This function supports the \b allocatable array attribute
  !>
  !> @param msgid       id of the message loaded in memory
  !> @param key         key name
  !> @param value       integer(4) array value
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_get_long_array ( msgid, key, value, status )
    integer(kind=kindOfInt),               intent(in)  :: msgid
    character(len=*),      intent(in)                  :: key
    integer(kind=kindOfLong), dimension(:),allocatable,intent(inout) :: value
    integer(kind=kindOfInt),optional, intent(out)      :: status
    integer(kind=kindOfInt)                            :: iret

    integer(kind=kindOfInt)                            :: nb_values
    integer(kind=kindOfInt)                            :: size_value
    integer(kind=kindOfInt)                            :: i

    iret=grib_f_get_size_int(msgid,key,nb_values)
    if (iret /= 0) then
      call grib_f_write_on_fail(msgid)
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'get',key)
      endif
      return
    endif
    if (allocated(value) .eqv. .false.) then
      allocate(value(nb_values))
    end if
    size_value=size(value)
    iret=grib_f_get_long_array ( msgid, key, value , nb_values )
    if (iret==0 .and. nb_values==1 .and. size_value/=1) then
      do i=2,size_value
        value(i)=value(1)
      enddo
    endif
    if (iret /= 0) then
      call grib_f_write_on_fail(msgid)
    endif
    if (present(status)) then
      status = iret
    else
      call grib_check(iret,'get',key)
    endif

end subroutine codes_get_long_array 

  !> Get the array of bytes (character) for a key from a message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid       id of the message loaded in memory
  !> @param key         key name
  !> @param value       character(len=1) array of byte values
  !> @param length      (optional) output: number of values retrieved
  !> @param status      (optional) CODES_SUCCESS if OK, integer value on error
subroutine codes_get_byte_array ( msgid, key, value, length, status )
    integer(kind=kindOfInt),               intent(in)    :: msgid
    character(len=*),                      intent(in)    :: key
    character(len=1),        dimension(:), intent(inout) :: value
    integer(kind=kindOfInt), optional,     intent(out)   :: length
    integer(kind=kindOfInt), optional,     intent(out)   :: status

    call grib_get_byte_array ( msgid, key, value, length, status )
end subroutine codes_get_byte_array 

  !> Get the real(4) array of values for a key from a message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.\n
  !> Note: This function supports the \b allocatable array attribute
  !>
  !> @param msgid       id of the message loaded in memory
  !> @param key         key name
  !> @param value       real(4) array value
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_get_real4_array ( msgid, key, value, status)
    integer(kind=kindOfInt),                 intent(in)  :: msgid
    character(len=*),                        intent(in)  :: key
    real(kind = kindOfFloat), dimension(:),allocatable, intent(inout) :: value
    integer(kind=kindOfInt),optional,        intent(out) :: status
    integer(kind=kindOfInt)                              :: iret

    integer(kind=kindOfInt)                              :: nb_values
    integer(kind=kindOfInt)                              :: size_value
    integer(kind=kindOfInt)                              :: i

    iret=grib_f_get_size_int(msgid,key,nb_values)
    if (iret /= 0) then
      call grib_f_write_on_fail(msgid)
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'get',key)
      endif
      return
    endif
    if (allocated(value) .eqv. .false.) then
      allocate(value(nb_values))
    end if
    size_value=size(value)
    iret=grib_f_get_real4_array ( msgid, key, value , nb_values )
    if (iret==0 .and. nb_values==1 .and. size_value/=1) then
      do i=2,size_value
        value(i)=value(1)
      enddo
    endif
    if (iret /= 0) then
      call grib_f_write_on_fail(msgid)
    endif
    if (present(status)) then
       status = iret
    else
       call grib_check(iret,'get',key)
    endif
end subroutine codes_get_real4_array

  !> Get the real(8) array of values for a key from a message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.\n
  !> Note: This function supports the \b allocatable array attribute
  !>
  !> @param msgid       id of the message loaded in memory
  !> @param key         key name
  !> @param value       real(8) array value
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_get_real8_array ( msgid, key, value, status )
    integer(kind=kindOfInt),                   intent(in)  :: msgid
    character(len=*),                          intent(in)  :: key
    real(kind = kindOfDouble), dimension(:),allocatable, intent(inout) :: value
    integer(kind=kindOfInt),optional,          intent(out) :: status
    integer(kind=kindOfInt)                                :: iret
    integer(kind=kindOfInt)                                :: nb_values

    integer(kind=kindOfInt)                                :: size_value
    integer(kind=kindOfInt)                                :: i

    iret=grib_f_get_size_int(msgid,key,nb_values)
    if (iret /= 0) then
      call grib_f_write_on_fail(msgid)
      if (present(status)) then
        status = iret
      else
        call grib_check(iret,'get',key)
      endif
      return
    endif
    if (allocated(value) .eqv. .false.) then
      allocate(value(nb_values))
    end if
    size_value=size(value)
    iret=grib_f_get_real8_array ( msgid, key, value, nb_values )
    if (iret==0 .and. nb_values==1 .and. size_value/=1) then
      do i=2,size_value
        value(i)=value(1)
      enddo
    endif
    if (iret /= 0) then
      call grib_f_write_on_fail(msgid)
    endif
    if (present(status)) then
      status = iret
    else
      call grib_check(iret,'get',key)
    endif
end subroutine codes_get_real8_array 


  !> Get a real(4) value of specified index from an array key.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid   id of the grib loaded in memory
  !> @param key     key name
  !> @param index   integer(4) index
  !> @param value   real(4) value
  !> @param status  CODES_SUCCESS if OK, integer value on error
subroutine codes_get_real4_element ( msgid, key, index,value, status )
    integer(kind=kindOfInt),                     intent(in)  :: msgid
    character(len=*),                            intent(in)  :: key
    integer(kind=kindOfInt),                     intent(in)  :: index
    real(kind = kindOfFloat),                    intent(out) :: value
    integer(kind=kindOfInt),optional,            intent(out) :: status

    call grib_get_real4_element ( msgid, key, index,value, status )
end subroutine codes_get_real4_element 


  !> Get a real(8) value of specified index from an array key.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid      id of the grib loaded in memory
  !> @param key        key name
  !> @param index      integer(4) index
  !> @param value      real(8) value
  !> @param status     CODES_SUCCESS if OK, integer value on error
subroutine codes_get_real8_element ( msgid, key, index,value, status )
    integer(kind=kindOfInt),                     intent(in)  :: msgid
    character(len=*),                            intent(in)  :: key
    integer(kind=kindOfInt),                     intent(in)  :: index
    real(kind = kindOfDouble),                   intent(out) :: value
    integer(kind=kindOfInt),optional,            intent(out) :: status

    call grib_get_real8_element ( msgid, key, index,value, status )
end subroutine codes_get_real8_element


  !> Get the real(4) values whose indexes are stored in the array "index" from an array key.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid      id of the grib loaded in memory
  !> @param key        key name
  !> @param index      integer(4) array indexes
  !> @param value      real(4) array value
  !> @param status     CODES_SUCCESS if OK, integer value on error
subroutine codes_get_real4_elements ( msgid, key, index,value, status )
    integer(kind=kindOfInt),                  intent(in)  :: msgid
    character(len=*),                         intent(in)  :: key
    integer(kind=kindOfInt),dimension(:),  intent(in)     :: index
    real(kind = kindOfFloat), dimension(:),   intent(out) :: value
    integer(kind=kindOfInt),optional,         intent(out) :: status

    call grib_get_real4_elements ( msgid, key, index,value, status )
end subroutine codes_get_real4_elements 

  !> Get the real(8) values whose indexes are stored in the array "index" from an array key.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid      id of the grib loaded in memory
  !> @param key        key name
  !> @param index      integer(4) array index
  !> @param value      real(8) array value
  !> @param status     CODES_SUCCESS if OK, integer value on error
subroutine codes_get_real8_elements ( msgid, key, index,value, status )
    integer(kind=kindOfInt),                   intent(in)  :: msgid
    character(len=*),                          intent(in)  :: key
    integer(kind=kindOfInt),dimension(:),   intent(in)     :: index
    real(kind = kindOfDouble), dimension(:),   intent(out) :: value
    integer(kind=kindOfInt),optional,          intent(out) :: status

    call grib_get_real8_elements ( msgid, key, index,value, status )
end subroutine codes_get_real8_elements 

  !> Set the integer value for a key in a message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid       id of the message loaded in memory
  !> @param key         key name
  !> @param value       integer(4) value
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_set_int ( msgid, key, value, status )
    integer(kind=kindOfInt),          intent(in)  :: msgid
    character(len=*), intent(in)                  :: key
    integer(kind=kindOfInt),          intent(in)  :: value
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_set_int ( msgid, key, value, status )
end subroutine codes_set_int


  !> Set the integer value for a key in a message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid       id of the message loaded in memory
  !> @param key         key name
  !> @param value       integer(4) value
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_set_long ( msgid, key, value, status )
    integer(kind=kindOfInt),          intent(in)  :: msgid
    character(len=*), intent(in)                  :: key
    integer(kind=kindOfLong),         intent(in)  :: value
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_set_long ( msgid, key, value, status )
end subroutine codes_set_long


  !> Set the real(4) value for a key in a message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid    id of the message loaded in memory
  !> @param key      key name
  !> @param value    real(4) value
  !> @param status   CODES_SUCCESS if OK, integer value on error
subroutine codes_set_real4 ( msgid, key, value, status )
    integer(kind=kindOfInt),                 intent(in)  :: msgid
    character(len=*),                        intent(in)  :: key
    real(kind = kindOfFloat),                intent(in)  :: value
    integer(kind=kindOfInt),optional,        intent(out) :: status

    call grib_set_real4 ( msgid, key, value, status )
end subroutine codes_set_real4 

  !> Set the real(8) value for a key in a message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid      id of the message loaded in memory
  !> @param key        key name
  !> @param value      real(8) value
  !> @param status     CODES_SUCCESS if OK, integer value on error
subroutine codes_set_real8 ( msgid, key, value, status )
    integer(kind=kindOfInt),                  intent(in)  :: msgid
    character(len=*),                         intent(in)  :: key
    real(kind = kindOfDouble),                intent(in)  :: value
    integer(kind=kindOfInt),optional,         intent(out) :: status

    call grib_set_real8 ( msgid, key, value, status )
end subroutine codes_set_real8 


  !> Set the integers values for an array key in a message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid      id of the message loaded in memory
  !> @param key        key name
  !> @param value      integer(4) array value
  !> @param status     CODES_SUCCESS if OK, integer value on error
subroutine codes_set_int_array ( msgid, key, value, status)
    integer(kind=kindOfInt),                intent(in)  :: msgid
    character(len=*),                       intent(in)  :: key
    integer(kind=kindOfInt), dimension(:),  intent(in)  :: value
    integer(kind=kindOfInt),optional,       intent(out) :: status

    call grib_set_int_array ( msgid, key, value, status)
end subroutine codes_set_int_array

  !> Set the integers values for an array key in a message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid      id of the message loaded in memory
  !> @param key        key name
  !> @param value      integer(4) array value
  !> @param status     CODES_SUCCESS if OK, integer value on error
subroutine codes_set_long_array ( msgid, key, value, status)
    integer(kind=kindOfInt),                intent(in)  :: msgid
    character(len=*),                       intent(in)  :: key
    integer(kind=kindOfLong), dimension(:),  intent(in)  :: value
    integer(kind=kindOfInt),optional,       intent(out) :: status

    call grib_set_long_array ( msgid, key, value, status)
end subroutine codes_set_long_array

  !> Set the array of bytes (character) for a key in a message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid       id of the message loaded in memory
  !> @param key         key name
  !> @param value       character(len=1) array of byte values
  !> @param length      (optional) output: number of values written
  !> @param status      (optional) CODES_SUCCESS if OK, integer value on error
subroutine codes_set_byte_array ( msgid, key, value, length, status )
    integer(kind=kindOfInt),               intent(in)  :: msgid
    character(len=*),                      intent(in)  :: key
    character(len=1),        dimension(:), intent(in)  :: value
    integer(kind=kindOfInt), optional,     intent(out) :: length
    integer(kind=kindOfInt), optional,     intent(out) :: status

    call grib_set_byte_array ( msgid, key, value, length, status )
end subroutine codes_set_byte_array 

  !> Set the real(4) values for an array key in a message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid      id of the message loaded in memory
  !> @param key        key name
  !> @param value      real(4) array value
  !> @param status     CODES_SUCCESS if OK, integer value on error
subroutine codes_set_real4_array ( msgid, key, value, status )
    integer(kind=kindOfInt),                 intent(in)  :: msgid
    character(len=*),                        intent(in)  :: key
    real(kind = kindOfFloat), dimension(:),  intent(in)  :: value
    integer(kind=kindOfInt),optional,        intent(out) :: status

    call grib_set_real4_array ( msgid, key, value, status )
end subroutine codes_set_real4_array 

  !> Set the real(8) values for an array key in a message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid      id of the message loaded in memory
  !> @param key        key name
  !> @param value      real(8) array value
  !> @param status     CODES_SUCCESS if OK, integer value on error
subroutine codes_set_real8_array ( msgid, key, value, status)
    integer(kind=kindOfInt),                  intent(in)  :: msgid
    character(len=*),                         intent(in)  :: key
    real(kind = kindOfDouble), dimension(:),  intent(in)  :: value
    integer(kind=kindOfInt),optional,         intent(out) :: status

    call grib_set_real8_array ( msgid, key, value, status)
end subroutine codes_set_real8_array

  !> @cond
  !> Set the real(4) values for an array key in a grib message, forces the set if the key is read-only.
  !> Use with great caution!!
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid      id of the grib loaded in memory
  !> @param key        key name
  !> @param value      real(4) array value
  !> @param status     CODES_SUCCESS if OK, integer value on error
subroutine codes_set_force_real4_array ( msgid, key, value, status )
    integer(kind=kindOfInt),                 intent(in)  :: msgid
    character(len=*),                        intent(in)  :: key
    real(kind = kindOfFloat), dimension(:),  intent(in)  :: value
    integer(kind=kindOfInt),optional,        intent(out) :: status

    call grib_set_force_real4_array ( msgid, key, value, status )
end subroutine codes_set_force_real4_array

  !> Set the real(8) values for an array key in a grib message, forces the set if the key is read-only.
  !> Use with great caution!!
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid      id of the grib loaded in memory
  !> @param key        key name
  !> @param value      real(8) array value
  !> @param status     CODES_SUCCESS if OK, integer value on error
subroutine codes_set_force_real8_array ( msgid, key, value, status)
    integer(kind=kindOfInt),                  intent(in)  :: msgid
    character(len=*),                         intent(in)  :: key
    real(kind = kindOfDouble), dimension(:),  intent(in)  :: value
    integer(kind=kindOfInt),optional,         intent(out) :: status

    call grib_set_force_real8_array ( msgid, key, value, status)
end subroutine codes_set_force_real8_array
  !> @endcond

  !> Set the character value for a string key in a message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid      id of the message loaded in memory
  !> @param key        key name
  !> @param value      character value
  !> @param status     CODES_SUCCESS if OK, integer value on error
subroutine codes_set_string  ( msgid, key, value , status)
    integer(kind=kindOfInt),                      intent(in)  :: msgid
    character(len=*),                             intent(in)  :: key
    character(len=*),                             intent(in)  :: value
    integer(kind=kindOfInt),optional,             intent(out) :: status

    call grib_set_string  ( msgid, key, value , status)
end subroutine codes_set_string

  !> Get the size of a coded message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid       id of the grib loaded in memory
  !> @param nbytes      size in bytes of the message
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_get_message_size_int  ( msgid, nbytes, status)
    integer(kind=kindOfInt),                      intent(in)  :: msgid
    integer(kind=kindOfInt),                      intent(out) :: nbytes
    integer(kind=kindOfInt),optional,             intent(out) :: status

    call grib_get_message_size_int  ( msgid, nbytes, status)
end subroutine codes_get_message_size_int


  !> Get the size of a coded message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid       id of the grib loaded in memory
  !> @param nbytes      size in bytes of the message
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_get_message_size_size_t  ( msgid, nbytes, status)
    integer(kind=kindOfInt),                      intent(in)  :: msgid
    integer(kind=kindOfSize_t),                   intent(out) :: nbytes
    integer(kind=kindOfInt),optional,             intent(out) :: status

    call grib_get_message_size_size_t  ( msgid, nbytes, status)
end subroutine codes_get_message_size_size_t

  !> Copy the coded message into an array.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid       id of the grib loaded in memory
  !> @param message     array containing the coded message to be copied
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_copy_message ( msgid, message, status )
    integer(kind=kindOfInt),                      intent(in)  :: msgid
    character(len=1), dimension(:),               intent(out) :: message
    integer(kind=kindOfInt),optional,             intent(out) :: status

    call grib_copy_message ( msgid, message, status )
end subroutine codes_copy_message

  !> Write the coded message to a file.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param msgid       id of the grib loaded in memory
  !> @param ifile       file id of a file opened with \ref codes_open_file
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_write ( msgid, ifile  , status)
    integer(kind=kindOfInt),          intent(in)  :: msgid
    integer(kind=kindOfInt),          intent(in)  :: ifile
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_write ( msgid, ifile  , status)
end subroutine codes_write

  !> Write a multi field message to a file.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param multigribid      id of the multi field grib loaded in memory
  !> @param ifile       file id of a file opened with \ref codes_open_file
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_grib_multi_write ( multigribid, ifile  , status)
    integer(kind=kindOfInt),          intent(in)  :: multigribid
    integer(kind=kindOfInt),          intent(in)  :: ifile
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_multi_write ( multigribid, ifile  , status)
end subroutine codes_grib_multi_write 


  !> Append a single field grib message to a multi field grib message.
  !> Only the sections with section number greather or equal "startsection" are copied from the input single message to the multi field output grib.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param ingribid      id of the input single grib 
  !> @param startsection  starting from startsection (included) all the sections are copied from the input single grib to the output multi grib
  !> @param multigribid    id of the output multi filed grib
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_grib_multi_append ( ingribid, startsection, multigribid  , status)
    integer(kind=kindOfInt),          intent(in)  :: ingribid
    integer(kind=kindOfInt),          intent(in)  :: startsection
    integer(kind=kindOfInt),          intent(out) :: multigribid
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_multi_append ( ingribid, startsection, multigribid  , status)
end subroutine codes_grib_multi_append 

  !> Find the nearest point of a set of points whose latitudes and longitudes
  !> are given in the inlats, inlons arrays respectively.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param is_lsm      .true. if the nearest land point is required otherwise .false.
  !> @param inlats      input real(8) array of the latitudes of the points
  !> @param inlons      input real(8) array of the longitudes of the points
  !> @param outlats     output real(8) array of the latitudes of the nearest points
  !> @param outlons     output real(8) array of the longitudes of the nearest points
  !> @param distances   output real(8) array of the distances
  !> @param indexes     output integer(4) array of the zero based indexes
  !> @param values      output real(8) array of the values
  !> @param status      CODES_SUCCESS if OK, integer value on error
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

    call grib_find_nearest_multiple(gribid,is_lsm,   &
                 inlats,inlons,outlats,outlons,      &
                 values,distances, indexes,status)
end subroutine codes_grib_find_nearest_multiple

  !> Find the nearest point of a given latitude/longitude point.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param gribid     id of the grib loaded in memory
  !> @param is_lsm     .true. if the nearest land point is required otherwise .false.
  !> @param inlat      latitude of the point
  !> @param inlon      longitudes of the point
  !> @param outlat     latitude of the nearest point
  !> @param outlon     longitude of the nearest point
  !> @param distance   distance between the given point and its nearest
  !> @param index      zero based index
  !> @param value      value of the field in the nearest point
  !> @param status     CODES_SUCCESS if OK, integer value on error
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

    call grib_find_nearest_single(gribid,is_lsm,  &
                 inlat,inlon,outlat,outlon,       &
                 value,distance, index,status)
end subroutine codes_grib_find_nearest_single


  !> Find the 4 nearest points of a latitude longitude point.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param gribid     id of the GRIB loaded in memory
  !> @param is_lsm     .true. if the nearest land point is required otherwise .false.
  !> @param inlat      latitude of the point
  !> @param inlon      longitudes of the point
  !> @param outlat     latitude of the nearest point
  !> @param outlon     longitude of the nearest point
  !> @param distance   distance between the given point and its nearest
  !> @param index      zero based index
  !> @param value      value of the field in the nearest point
  !> @param status     CODES_SUCCESS if OK, integer value on error
subroutine codes_grib_find_nearest_four_single(gribid,is_lsm, &
                 inlat,inlon,outlat,outlon,              &
                 value,distance, index,status)
    integer(kind=kindOfInt),                  intent(in)    :: gribid
    logical,                                  intent(in)    :: is_lsm
    real(kind = kindOfDouble), intent(in)                   :: inlat
    real(kind = kindOfDouble), intent(in)                   :: inlon
    real(kind = kindOfDouble), dimension(4), intent(out)    :: outlat
    real(kind = kindOfDouble), dimension(4), intent(out)    :: outlon
    real(kind = kindOfDouble), dimension(4), intent(out)    :: distance
    real(kind = kindOfDouble), dimension(4), intent(out)    :: value
    integer(kind = kindOfInt), dimension(4), intent(out)    :: index
    integer(kind=kindOfInt),optional, intent(out)           :: status

    call grib_find_nearest_four_single(gribid,is_lsm, &
                 inlat,inlon,outlat,outlon,           &
                 value,distance, index,status)
  end subroutine codes_grib_find_nearest_four_single


  !> Turn on the support for multiple fields in a single message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_grib_multi_support_on (status  )
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_multi_support_on (status  )
end subroutine codes_grib_multi_support_on 

  !> Turn off the support for multiple fields in a single message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_grib_multi_support_off ( status  )
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_multi_support_off ( status  )
end subroutine codes_grib_multi_support_off 

  !> Turn on the compatibility mode with gribex.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_gribex_mode_on (status )
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_gribex_mode_on (status )
end subroutine codes_gribex_mode_on 

  !> Turn off the compatibility mode with GRIBEX.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_gribex_mode_off (status )
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_gribex_mode_off (status )
end subroutine codes_gribex_mode_off 

  !> Skip the computed keys in a keys iterator.
  !>
  !> The computed keys are not coded in the message they are computed
  !> from other keys.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @see codes_keys_iterator_new, codes_keys_iterator_next, codes_keys_iterator_delete
  !>
  !> @param iterid      keys iterator id
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_skip_computed ( iterid , status)
    integer(kind=kindOfInt),          intent(in)  :: iterid
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_skip_computed ( iterid , status)
end subroutine codes_skip_computed 

  !> Skip the coded keys in a keys iterator.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> The coded keys are actually coded in the message.
  !>
  !> @see codes_keys_iterator_new, codes_keys_iterator_next, codes_keys_iterator_delete
  !>
  !> @param iterid      keys iterator id
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_skip_coded  ( iterid, status )
    integer(kind=kindOfInt),          intent(in)  :: iterid
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_skip_coded  ( iterid, status )
end subroutine codes_skip_coded  

  !> Skip the duplicated keys in a keys iterator.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @see codes_keys_iterator_new, codes_keys_iterator_next, codes_keys_iterator_delete
  !>
  !> @param iterid      keys iterator id
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_skip_duplicates ( iterid, status )
    integer(kind=kindOfInt),          intent(in)  :: iterid
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_skip_duplicates ( iterid, status )
end subroutine codes_skip_duplicates 

  !> Skip the read_only keys in a keys iterator.
  !>
  !> Read only keys cannot be set.
  !>
  !> @see codes_keys_iterator_new, codes_keys_iterator_next, codes_keys_iterator_delete
  !>
  !> @param iterid      keys iterator id
  !> @param status      CODES_SUCCESS if OK, integer value on error
subroutine codes_skip_read_only  ( iterid, status )
    integer(kind=kindOfInt),          intent(in)  :: iterid
    integer(kind=kindOfInt),optional, intent(out) :: status

    call grib_skip_read_only  ( iterid, status )
end subroutine codes_skip_read_only  


  !> Set the definition path
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param path       definitions path
  !> @param status     CODES_SUCCESS if OK, integer value on error
subroutine codes_set_definitions_path ( path,  status )
    character(len=*),                 intent(in)   :: path
    integer(kind=kindOfInt),optional, intent(out)  :: status

    call grib_set_definitions_path ( path, status )
end subroutine codes_set_definitions_path

  !> Set the samples path
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param path       samples path
  !> @param status     CODES_SUCCESS if OK, integer value on error
subroutine codes_set_samples_path ( path,  status )
    character(len=*),                 intent(in)   :: path
    integer(kind=kindOfInt),optional, intent(out)  :: status

    call grib_set_samples_path ( path, status )
end subroutine codes_set_samples_path



end module eccodes

