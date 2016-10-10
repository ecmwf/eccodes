
  contains

  !> Set as missing the value for a key in a grib message.
  !>
  !> It can be used to set a missing value in the grib header but not in \n
  !> the data values. To set missing data values see the bitmap examples.\n
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref grib_set_missing.f90 "grib_set_missing.f90"
  !>
  !> @param  gribid     id of the grib loaded in memory
  !> @param key     key name
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_set_missing ( gribid, key,  status )
      integer(kind=kindOfInt),                 intent(in)  :: gribid
      character(len=*),                        intent(in)  :: key
      integer(kind=kindOfInt),optional, intent(out)        :: status
      integer(kind=kindOfInt)                              :: iret

      iret=grib_f_set_missing ( gribid, key )
      if (iret /= 0) then
         call grib_f_write_on_fail(gribid)
      endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'set_missing','('//key//')')
      endif
  end subroutine grib_set_missing

  !> Create a new index form a file. The file is indexed with the keys in argument.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param  indexid     id of the newly created index
  !> @param filename     name of the file of messages to be indexed
  !> @param keys        : comma separated list of keys for the index. The type of the key can be explicitly declared appending :l for long, :d for double, :s for string to the key name. If the type is not declared explicitly, the native type is assumed.
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_index_create ( indexid, filename, keys,  status )
      integer(kind=kindOfInt),         intent(inout)  :: indexid
      character(len=*), intent(in)                    :: filename
      character(len=*), intent(in)                    :: keys
      integer(kind=kindOfInt),optional, intent(out)   :: status
      integer(kind=kindOfInt)                         :: iret

      iret=grib_f_index_new_from_file(filename,keys,indexid)
      if (present(status)) then
        status = iret
      else
        call grib_check(iret,'index_create','('//filename//')')
      endif
  end subroutine grib_index_create

  !> Add a file to an index.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param indexid     id of the index I want to add a file to
  !> @param filename    name of the file I want to add to the index
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_index_add_file ( indexid, filename, status )
      integer(kind=kindOfInt), intent(in)             :: indexid
      character(len=*), intent(in)                    :: filename
      integer(kind=kindOfInt),optional, intent(out)   :: status
      integer(kind=kindOfInt)                         :: iret

      iret=grib_f_index_add_file(indexid,filename)
      if (present(status)) then
          status = iret
      else
          call grib_check(iret,'index_add_file','('//filename//')')
      endif
  end subroutine grib_index_add_file

  !> Get the number of distinct values of the key in argument contained in the index. The key must belong to the index.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param  indexid     id of an index created from a file. The index must have been created with the key in argument.
  !> @param key        key for which the number of values is computed
  !> @param size       number of distinct values of the key in the index
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_index_get_size_long( indexid, key, size,  status )
      integer(kind=kindOfInt),         intent(in)     :: indexid
      character(len=*), intent(in)                    :: key
      integer(kind=kindOfLong),       intent(out)   :: size
      integer(kind=kindOfInt),optional, intent(out)   :: status
      integer(kind=kindOfInt)                         :: iret

      iret=grib_f_index_get_size_long(indexid,key,size)
      if (present(status)) then
        status = iret
      else
        call grib_check(iret,'index_get_size','('//key//')')
      endif
  end subroutine grib_index_get_size_long

  !> Get the number of distinct values of the key in argument contained in the index. The key must belong to the index.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param  indexid     id of an index created from a file. The index must have been created with the key in argument.
  !> @param key        key for which the number of values is computed
  !> @param size       number of distinct values of the key in the index
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_index_get_size_int( indexid, key, size,  status )
      integer(kind=kindOfInt),         intent(in)     :: indexid
      character(len=*), intent(in)                    :: key
      integer(kind=kindOfInt),       intent(out)   :: size
      integer(kind=kindOfInt),optional, intent(out)   :: status
      integer(kind=kindOfInt)                         :: iret

      iret=grib_f_index_get_size_int(indexid,key,size)
      if (present(status)) then
        status = iret
      else
        call grib_check(iret,'index_get_size','('//key//')')
      endif
  end subroutine grib_index_get_size_int

  !> Get the distinct values of the key in argument contained in the index. The key must belong to the index. This function is used when the type of the key was explicitly defined as long or when the native type of the key is long.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param  indexid   id of an index created from a file. The index must have been created with the key in argument.
  !> @param key        key for wich the values are returned
  !> @param values     array of values. The array must be allocated before entering this function and its size must be enough to contain all the values.
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_index_get_int( indexid, key, values,  status )
      integer(kind=kindOfInt),               intent(in)  :: indexid
      character(len=*),      intent(in)  :: key
      integer(kind=kindOfInt), dimension(:), intent(out) :: values
      integer(kind=kindOfInt),optional, intent(out)      :: status
      integer(kind=kindOfInt)                            :: iret
      integer(kind=kindOfInt)                            :: nb_values

      nb_values = size(values)
      iret=grib_f_index_get_int ( indexid, key, values , nb_values )
      if (present(status)) then
        status = iret
      else
        call grib_check(iret,'index_get','('//key//')')
      endif
  end subroutine grib_index_get_int

  !> Get the distinct values of the key in argument contained in the index. The key must belong to the index. This function is used when the type of the key was explicitly defined as long or when the native type of the key is long.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param  indexid   id of an index created from a file. The index must have been created with the key in argument.
  !> @param key        key for wich the values are returned
  !> @param values     array of values. The array must be allocated before entering this function and its size must be enough to contain all the values.
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_index_get_long( indexid, key, values,  status )
      integer(kind=kindOfInt),               intent(in)  :: indexid
      character(len=*),      intent(in)  :: key
      integer(kind=kindOfLong), dimension(:), intent(out) :: values
      integer(kind=kindOfInt),optional, intent(out)      :: status
      integer(kind=kindOfInt)                            :: iret
      integer(kind=kindOfInt)                            :: nb_values

      nb_values = size(values)
      iret=grib_f_index_get_long ( indexid, key, values , nb_values )
      if (present(status)) then
        status = iret
      else
        call grib_check(iret,'index_get','('//key//')')
      endif
  end subroutine grib_index_get_long

  !> Get the distinct values of the key in argument contained in the index. The key must belong to the index. This function is used when the type of the key was explicitly defined as long or when the native type of the key is long.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param  indexid   id of an index created from a file. The index must have been created with the key in argument.
  !> @param key        key for wich the values are returned
  !> @param values     array of values. The array must be allocated before entering this function and its size must be enough to contain all the values.
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_index_get_real8( indexid, key, values,  status )
      integer(kind=kindOfInt),               intent(in)  :: indexid
      character(len=*),      intent(in)  :: key
      real(kind=kindOfDouble), dimension(:), intent(out) :: values
      integer(kind=kindOfInt),optional, intent(out)      :: status
      integer(kind=kindOfInt)                            :: iret
      integer(kind=kindOfInt)                            :: nb_values

      nb_values = size(values)
      iret=grib_f_index_get_real8 ( indexid, key, values , nb_values )
      if (present(status)) then
        status = iret
      else
        call grib_check(iret,'index_get','('//key//')')
      endif
  end subroutine grib_index_get_real8

  !> Get the distinct values of the key in argument contained in the index. 
  !> The key must belong to the index. 
  !> This function is used when the type of the key was explicitly defined as string or when the native type of the key is string.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param  indexid   id of an index created from a file. The index must have been created with the key in argument.
  !> @param key        key for wich the values are returned
  !> @param values     array of values. The array must be allocated before entering this function and its size must be enough to contain all the values.
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_index_get_string( indexid, key, values,  status )
  integer(kind=kindOfInt),               intent(in)  :: indexid
  character(len=*),      intent(in)  :: key
  character(len=*), dimension(:), intent(out) :: values
  integer(kind=kindOfInt),optional, intent(out)      :: status
  integer(kind=kindOfInt)                            :: iret
  integer(kind=kindOfInt)                            :: nb_values
  integer(kind=kindOfInt)                            :: size_value
    size_value = len(values(1))
    nb_values = size(values)
    iret=grib_f_index_get_string ( indexid, key, values , size_value,nb_values )
    if (present(status)) then
      status = iret
    else
      call grib_check(iret,'index_get','('//key//')')
    endif
  end subroutine grib_index_get_string

  !> Select the message subset with key==value. The value is a integer. 
  !> The key must have been created with string type or have string as native type if the type was not explicitly defined in the index creation.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param  indexid   id of an index created from a file. The index must have been created with the key in argument.
  !> @param key        key to be selected
  !> @param value      value of the key to select
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_index_select_string( indexid, key, value,  status )
  integer(kind=kindOfInt),               intent(in)  :: indexid
  character(len=*),      intent(in)                  :: key
  character(len=*), intent(in)                :: value
  integer(kind=kindOfInt),optional, intent(out)      :: status
  integer(kind=kindOfInt)                            :: iret

    iret=grib_f_index_select_string ( indexid, key, value )
    if (present(status)) then
      status = iret
    else
      call grib_check(iret,'index_select','('//key//')')
    endif
  end subroutine grib_index_select_string

  !> Select the message subset with key==value. The value is a integer. The key must have been created with integer type or have integer as native type if the type was not explicitly defined in the index creation.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param  indexid   id of an index created from a file. The index must have been created with the key in argument.
  !> @param key        key to be selected
  !> @param value      value of the key to select
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_index_select_int( indexid, key, value,  status )
    integer(kind=kindOfInt),               intent(in)  :: indexid
    character(len=*),      intent(in)                  :: key
    integer(kind=kindOfInt), intent(in)                :: value
    integer(kind=kindOfInt),optional, intent(out)      :: status
    integer(kind=kindOfInt)                            :: iret

    iret=grib_f_index_select_int ( indexid, key, value )
    if (present(status)) then
      status = iret
    else
      call grib_check(iret,'index_select','('//key//')')
    endif
  end subroutine grib_index_select_int

  !> Select the message subset with key==value. The value is a integer. The key must have been created with integer type or have integer as native type if the type was not explicitly defined in the index creation.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param  indexid   id of an index created from a file. The index must have been created with the key in argument.
  !> @param key        key to be selected
  !> @param value      value of the key to select
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_index_select_long( indexid, key, value,  status )
    integer(kind=kindOfInt),               intent(in)  :: indexid
    character(len=*),      intent(in)                  :: key
    integer(kind=kindOfLong), intent(in)               :: value
    integer(kind=kindOfInt),optional, intent(out)      :: status
    integer(kind=kindOfInt)                            :: iret

      iret=grib_f_index_select_long ( indexid, key, value )
      if (present(status)) then
        status = iret
      else
        call grib_check(iret,'index_select','('//key//')')
      endif
  end subroutine grib_index_select_long

  !> Select the message subset with key==value. The value is a real. The key must have been created with real type or have real as native type if the type was not explicitly defined in the index creation.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param  indexid   id of an index created from a file. The index must have been created with the key in argument.
  !> @param key        key to be selected
  !> @param value      value of the key to select
  !> @param status     GRIB_SUCCESS if OK, integer value on error
  subroutine grib_index_select_real8( indexid, key, value,  status )
  integer(kind=kindOfInt),               intent(in)  :: indexid
  character(len=*),      intent(in)                  :: key
  real(kind=kindOfDouble), intent(in)                :: value
  integer(kind=kindOfInt),optional, intent(out)      :: status
  integer(kind=kindOfInt)                            :: iret

    iret=grib_f_index_select_real8 ( indexid, key, value )
    if (present(status)) then
      status = iret
    else
      call grib_check(iret,'index_select','('//key//')')
    endif
    end subroutine grib_index_select_real8

  !> Create a new handle from an index after having selected the key values.
  !> All the keys belonging to the index must be selected before calling this function. Successive calls to this function will return all the handles compatible with the constraints defined selecting the values of the index keys.
  !> When no more handles are available from the index a NULL pointer is returned and the err variable is set to GRIB_END_OF_INDEX.
  !>
  !> The message can be accessed through its gribid and it will be available\n
  !> until @ref grib_release is called.\n
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param indexid   id of an index created from a file.
  !> @param gribid    id of the grib loaded in memory
  !> @param status    GRIB_SUCCESS if OK, GRIB_END_OF_FILE at the end of file, or error code
  subroutine grib_new_from_index ( indexid, gribid , status)
  integer(kind=kindOfInt),intent(in)              :: indexid
  integer(kind=kindOfInt),intent(out)             :: gribid
  integer(kind=kindOfInt),optional,intent(out)    :: status
  integer(kind=kindOfInt)                         :: iret

    iret=grib_f_new_from_index( indexid, gribid )
    if (present(status)) then
      status = iret
    else
      call grib_check(iret,'new_from_index','')
    endif
  end subroutine grib_new_from_index

  !> Load an index file previously created with @ref grib_index_write.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param indexid      id of loaded index
  !> @param filename     name of the index file to load
  !> @param status       GRIB_SUCCESS if OK, integer value on error
  subroutine grib_index_read ( indexid, filename, status )
  integer(kind=kindOfInt),         intent(inout)  :: indexid
  character(len=*), intent(in)                    :: filename
  integer(kind=kindOfInt),optional, intent(out)   :: status
  integer(kind=kindOfInt)                         :: iret

    iret=grib_f_index_read(filename,indexid)
    if (present(status)) then
        status = iret
    else
        call grib_check(iret,'index_read','('//filename//')')
    endif
      
  end subroutine grib_index_read

  !> Saves an index to a file for later reuse. Index files can be read with
  !> @ref grib_index_read.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param indexid      id of the index to save to file
  !> @param filename     name of file to save the index to
  !> @param status       GRIB_SUCCESS if OK, integer value on error
  subroutine grib_index_write ( indexid, filename, status )
  integer(kind=kindOfInt),         intent(inout)  :: indexid
  character(len=*), intent(in)                    :: filename
  integer(kind=kindOfInt),optional, intent(out)   :: status
  integer(kind=kindOfInt)                         :: iret

    iret=grib_f_index_write(indexid,filename)
    if (present(status)) then
        status = iret
    else
        call grib_check(iret,'index_write','('//filename//')')
    endif
      
  end subroutine grib_index_write

  !> Delete the index.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param indexid   id of an index created from a file.
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_index_release ( indexid, status )
  integer(kind=kindOfInt),          intent(in)  :: indexid
  integer(kind=kindOfInt),optional, intent(out) :: status
  integer(kind=kindOfInt)                       :: iret

          iret=grib_f_index_release ( indexid )
          if (present(status)) then
            status = iret
          else
            call grib_check(iret,'index_release','')
          endif
  end subroutine grib_index_release

  !> Open a file according to a mode.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref grib_get_keys.f90 "grib_get_keys.f90"
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param filename    name of the file to be open
  !> @param mode        open mode can be 'r' (read only) or 'w' (write only)
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_open_file ( ifile, filename, mode, status )
        integer(kind=kindOfInt),intent(out)               :: ifile
        character(len=*), intent(in)                      :: filename
        character(LEN=*), intent(in)                      :: mode
        integer(kind=kindOfInt),optional, intent(out)     :: status
        integer(kind=kindOfInt)                           :: iret

        iret=grib_f_open_file(ifile, filename, mode )
        if (present(status)) then
           status = iret
        else
           call grib_check(iret,'open_file','('//filename//')')
        endif
  end subroutine grib_open_file

  !> Reads nbytes bytes into the buffer from a file opened with grib_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      binary buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_read_bytes_char ( ifile, buffer, nbytes, status )
        integer(kind=kindOfInt),intent(in)               :: ifile
        character(len=1),dimension(:), intent(out)       :: buffer
        integer(kind=kindOfInt), intent(in)              :: nbytes
        integer(kind=kindOfInt),optional, intent(out)    :: status
        integer(kind=kindOfSize_t)                       :: ibytes
        integer(kind=kindOfInt)                          :: iret

        ibytes=nbytes
        iret=grib_f_read_file(ifile,buffer,ibytes)
        if (present(status)) then
           status = iret
        else
           call grib_check(iret,'read_bytes','')
        endif
  end subroutine grib_read_bytes_char

  !> Reads nbytes bytes into the buffer from a file opened with grib_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      binary buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_read_bytes_char_size_t ( ifile, buffer, nbytes, status )
        integer(kind=kindOfInt),intent(in)               :: ifile
        character(len=1),dimension(:), intent(out)       :: buffer
        integer(kind=kindOfSize_t), intent(in)           :: nbytes
        integer(kind=kindOfInt),optional, intent(out)    :: status
        integer(kind=kindOfInt)                          :: iret

        iret=grib_f_read_file(ifile,buffer,nbytes)
        if (present(status)) then
           status = iret
        else
           call grib_check(iret,'read_bytes','')
        endif
  end subroutine grib_read_bytes_char_size_t

  !> Reads nbytes bytes into the buffer from a file opened with grib_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_read_bytes_int4 ( ifile, buffer, nbytes, status )
        integer(kind=kindOfInt),intent(in)               :: ifile
        integer(kind=4),dimension(:), intent(out)        :: buffer
        integer(kind=kindOfInt), intent(in)              :: nbytes
        integer(kind=kindOfInt),optional, intent(out)    :: status
        integer(kind=kindOfSize_t)                       :: ibytes
        integer(kind=kindOfInt)                          :: iret

        ibytes=nbytes
        iret=grib_f_read_file(ifile,buffer,ibytes)
        if (present(status)) then
           status = iret
        else
           call grib_check(iret,'read_bytes','')
        endif
  end subroutine grib_read_bytes_int4

  !> Reads nbytes bytes into the buffer from a file opened with grib_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_read_bytes_int4_size_t ( ifile, buffer, nbytes, status )
        integer(kind=kindOfInt),intent(in)               :: ifile
        integer(kind=4),dimension(:), intent(out)        :: buffer
        integer(kind=kindOfSize_t), intent(in)           :: nbytes
        integer(kind=kindOfInt),optional, intent(out)    :: status
        integer(kind=kindOfInt)                          :: iret

        iret=grib_f_read_file(ifile,buffer,nbytes)
        if (present(status)) then
           status = iret
        else
           call grib_check(iret,'read_bytes','')
        endif
  end subroutine grib_read_bytes_int4_size_t

  !> Reads nbytes bytes into the buffer from a file opened with grib_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_read_bytes_real4 ( ifile, buffer, nbytes, status )
      integer(kind=kindOfInt),intent(in)               :: ifile
      real(kind=4),dimension(:), intent(out)           :: buffer
      integer(kind=kindOfInt), intent(in)              :: nbytes
      integer(kind=kindOfInt),optional, intent(out)    :: status
      integer(kind=kindOfSize_t)                       :: ibytes
      integer(kind=kindOfInt)                          :: iret

      ibytes=nbytes
      iret=grib_f_read_file(ifile,buffer,ibytes)
      if (present(status)) then
        status = iret
      else
        call grib_check(iret,'read_bytes','')
      endif
  end subroutine grib_read_bytes_real4

  !> Reads nbytes bytes into the buffer from a file opened with grib_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_read_bytes_real4_size_t ( ifile, buffer, nbytes, status )
      integer(kind=kindOfInt),intent(in)               :: ifile
      real(kind=4),dimension(:), intent(out)           :: buffer
      integer(kind=kindOfSize_t), intent(inout)        :: nbytes
      integer(kind=kindOfInt),optional, intent(out)    :: status
      integer(kind=kindOfInt)                          :: iret

      iret=grib_f_read_file(ifile,buffer,nbytes)
      if (present(status)) then
        status = iret
      else
        call grib_check(iret,'read_bytes','')
      endif
  end subroutine grib_read_bytes_real4_size_t

  !> Reads nbytes bytes into the buffer from a file opened with grib_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_read_bytes_real8 ( ifile, buffer, nbytes, status )
        integer(kind=kindOfInt),intent(in)               :: ifile
        real(kind=8),dimension(:), intent(out)           :: buffer
        integer(kind=kindOfInt), intent(in)              :: nbytes
        integer(kind=kindOfInt),optional, intent(out)    :: status
        integer(kind=kindOfSize_t)                       :: ibytes
        integer(kind=kindOfInt)                          :: iret

        ibytes=nbytes
        iret=grib_f_read_file(ifile,buffer,ibytes)
        if (present(status)) then
           status = iret
        else
           call grib_check(iret,'read_bytes','')
        endif
  end subroutine grib_read_bytes_real8

  !> Reads nbytes bytes into the buffer from a file opened with grib_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_read_bytes_real8_size_t ( ifile, buffer, nbytes, status )
        integer(kind=kindOfInt),intent(in)               :: ifile
        real(kind=8),dimension(:), intent(out)           :: buffer
        integer(kind=kindOfSize_t), intent(inout)        :: nbytes
        integer(kind=kindOfInt),optional, intent(out)    :: status
        integer(kind=kindOfInt)                          :: iret

        iret=grib_f_read_file(ifile,buffer,nbytes)
        if (present(status)) then
           status = iret
        else
           call grib_check(iret,'read_bytes','')
        endif
  end subroutine grib_read_bytes_real8_size_t

  !> Reads a message in the buffer array from the file opened with grib_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      binary buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_read_from_file_int4 ( ifile, buffer, nbytes, status )
        integer(kind=kindOfInt),intent(in)               :: ifile
        integer(kind=4),dimension(:), intent(out)        :: buffer
        integer(kind=kindOfInt), intent(inout)           :: nbytes
        integer(kind=kindOfInt),optional, intent(out)    :: status
        integer(kind=kindOfSize_t)                       :: ibytes
        integer(kind=kindOfInt)                          :: iret

        ibytes=nbytes
        iret=grib_f_read_any_from_file(ifile,buffer,ibytes)
        if (iret == GRIB_SUCCESS .and. ibytes > huge(nbytes)) then
          iret = GRIB_MESSAGE_TOO_LARGE
        endif
        nbytes=ibytes
        if (present(status)) then
           status = iret
        else
           call grib_check(iret,'read_from_file','')
        endif
  end subroutine grib_read_from_file_int4

  !> Reads a message in the buffer array from the file opened with grib_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      binary buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_read_from_file_int4_size_t ( ifile, buffer, nbytes, status )
        integer(kind=kindOfInt),intent(in)               :: ifile
        integer(kind=4),dimension(:), intent(out)        :: buffer
        integer(kind=kindOfSize_t), intent(inout)        :: nbytes
        integer(kind=kindOfInt),optional, intent(out)    :: status
        integer(kind=kindOfInt)                          :: iret

        iret=grib_f_read_any_from_file(ifile,buffer,nbytes)
        if (present(status)) then
           status = iret
        else
           call grib_check(iret,'read_from_file','')
        endif
  end subroutine grib_read_from_file_int4_size_t

  !> Reads a message in the buffer array from the file opened with grib_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      binary buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_read_from_file_real4 ( ifile, buffer, nbytes, status )
        integer(kind=kindOfInt),intent(in)               :: ifile
        real(kind=4),dimension(:), intent(out)           :: buffer
        integer(kind=kindOfInt), intent(inout)           :: nbytes
        integer(kind=kindOfInt),optional, intent(out)    :: status
        integer(kind=kindOfSize_t)                       :: ibytes
        integer(kind=kindOfInt)                          :: iret

        ibytes=nbytes
        iret=grib_f_read_any_from_file(ifile,buffer,ibytes)
        if (iret == GRIB_SUCCESS .and. ibytes > huge(nbytes)) then
          iret = GRIB_MESSAGE_TOO_LARGE
        endif
        nbytes=ibytes
        if (present(status)) then
           status = iret
        else
           call grib_check(iret,'read_from_file','')
        endif
  end subroutine grib_read_from_file_real4

  !> Reads a message in the buffer array from the file opened with grib_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      binary buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_read_from_file_real4_size_t ( ifile, buffer, nbytes, status )
        integer(kind=kindOfInt),intent(in)               :: ifile
        real(kind=4),dimension(:), intent(out)           :: buffer
        integer(kind=kindOfSize_t), intent(inout)        :: nbytes
        integer(kind=kindOfInt),optional, intent(out)    :: status
        integer(kind=kindOfInt)                          :: iret

        iret=grib_f_read_any_from_file(ifile,buffer,nbytes)
        if (present(status)) then
           status = iret
        else
           call grib_check(iret,'read_from_file','')
        endif
  end subroutine grib_read_from_file_real4_size_t

  !> Reads a message in the buffer array from the file opened with grib_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      binary buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_read_from_file_real8 ( ifile, buffer, nbytes, status )
        integer(kind=kindOfInt),intent(in)               :: ifile
        real(kind=8),dimension(:), intent(out)           :: buffer
        integer(kind=kindOfInt), intent(inout)           :: nbytes
        integer(kind=kindOfInt),optional, intent(out)    :: status
        integer(kind=kindOfSize_t)                       :: ibytes
        integer(kind=kindOfInt)                          :: iret

        ibytes=nbytes
        iret=grib_f_read_any_from_file(ifile,buffer,ibytes)
        if (iret == GRIB_SUCCESS .and. ibytes > huge(nbytes)) then
          iret = GRIB_MESSAGE_TOO_LARGE
        endif
        nbytes=ibytes
        if (present(status)) then
           status = iret
        else
           call grib_check(iret,'read_from_file','')
        endif
  end subroutine grib_read_from_file_real8

  !> Reads a message in the buffer array from the file opened with grib_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      binary buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_read_from_file_real8_size_t ( ifile, buffer, nbytes, status )
        integer(kind=kindOfInt),intent(in)               :: ifile
        real(kind=8),dimension(:), intent(out)           :: buffer
        integer(kind=kindOfSize_t), intent(inout)        :: nbytes
        integer(kind=kindOfInt),optional, intent(out)    :: status
        integer(kind=kindOfInt)                          :: iret

        iret=grib_f_read_any_from_file(ifile,buffer,nbytes)
        if (present(status)) then
           status = iret
        else
           call grib_check(iret,'read_from_file','')
        endif
  end subroutine grib_read_from_file_real8_size_t

  !> Reads a message in the buffer array from the file opened with grib_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_read_from_file_char ( ifile, buffer, nbytes, status )
        integer(kind=kindOfInt),intent(in)               :: ifile
        character(len=1),dimension(:), intent(out)       :: buffer
        integer(kind=kindOfInt), intent(inout)           :: nbytes
        integer(kind=kindOfInt),optional, intent(out)    :: status
        integer(kind=kindOfSize_t)                       :: ibytes
        integer(kind=kindOfInt)                          :: iret

        ibytes=nbytes
        iret=grib_f_read_any_from_file(ifile,buffer,ibytes)
        if (iret == GRIB_SUCCESS .and. ibytes > huge(nbytes)) then
          iret = GRIB_MESSAGE_TOO_LARGE
        endif
        nbytes=ibytes
        if (present(status)) then
           status = iret
        else
           call grib_check(iret,'read_from_file','')
        endif
  end subroutine grib_read_from_file_char

  !> Reads a message in the buffer array from the file opened with grib_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be read
  !> @param nbytes      number of bytes to be read
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_read_from_file_char_size_t ( ifile, buffer, nbytes, status )
        integer(kind=kindOfInt),intent(in)               :: ifile
        character(len=1),dimension(:), intent(out)       :: buffer
        integer(kind=kindOfSize_t), intent(inout)        :: nbytes
        integer(kind=kindOfInt),optional, intent(out)    :: status
        integer(kind=kindOfInt)                          :: iret

        iret=grib_f_read_any_from_file(ifile,buffer,nbytes)
        if (present(status)) then
           status = iret
        else
           call grib_check(iret,'read_from_file','')
        endif
  end subroutine grib_read_from_file_char_size_t

  !> Write nbytes bytes from the buffer in a file opened with grib_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be written
  !> @param nbytes      number of bytes to be written
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_write_bytes_char ( ifile, buffer, nbytes, status )
        integer(kind=kindOfInt),intent(in)               :: ifile
        character(len=1), dimension(:),intent(in)        :: buffer
        integer(kind=kindOfInt), intent(in)              :: nbytes
        integer(kind=kindOfInt),optional,intent(out)     :: status
        integer(kind=kindOfSize_t)                       :: ibytes
        integer(kind=kindOfInt)                          :: iret

        ibytes=nbytes
        iret=grib_f_write_file(ifile,buffer,ibytes)
        if (present(status)) then
           status = iret
        else
           call grib_check(iret,'write_bytes','')
        endif
  end subroutine grib_write_bytes_char

  !> Write nbytes bytes from the buffer in a file opened with grib_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be written
  !> @param nbytes      number of bytes to be written
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_write_bytes_char_size_t ( ifile, buffer, nbytes, status )
        integer(kind=kindOfInt),intent(in)               :: ifile
        character(len=1), dimension(:),intent(in)        :: buffer
        integer(kind=kindOfSize_t), intent(in)           :: nbytes
        integer(kind=kindOfInt),optional,intent(out)     :: status
        integer(kind=kindOfInt)                          :: iret

        iret=grib_f_write_file(ifile,buffer,nbytes)
        if (present(status)) then
           status = iret
        else
           call grib_check(iret,'write_bytes','')
        endif
  end subroutine grib_write_bytes_char_size_t

  !> Write nbytes bytes from the buffer in a file opened with grib_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be written
  !> @param nbytes      number of bytes to be written
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_write_bytes_int4 ( ifile, buffer, nbytes, status )
        integer(kind=kindOfInt),intent(in)               :: ifile
        integer(kind=4), dimension(:),intent(in)         :: buffer
        integer(kind=kindOfInt), intent(in)              :: nbytes
        integer(kind=kindOfInt),optional,intent(out)     :: status
        integer(kind=kindOfSize_t)                       :: ibytes
        integer(kind=kindOfInt)                          :: iret

        ibytes=nbytes
        iret=grib_f_write_file(ifile,buffer,ibytes)
        if (present(status)) then
           status = iret
        else
           call grib_check(iret,'write_bytes','')
        endif
  end subroutine grib_write_bytes_int4

  !> Write nbytes bytes from the buffer in a file opened with grib_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be written
  !> @param nbytes      number of bytes to be written
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_write_bytes_int4_size_t ( ifile, buffer, nbytes, status )
        integer(kind=kindOfInt),intent(in)               :: ifile
        integer(kind=4), dimension(:),intent(in)         :: buffer
        integer(kind=kindOfSize_t), intent(in)           :: nbytes
        integer(kind=kindOfInt),optional,intent(out)     :: status
        integer(kind=kindOfInt)                          :: iret

        iret=grib_f_write_file(ifile,buffer,nbytes)
        if (present(status)) then
           status = iret
        else
           call grib_check(iret,'write_bytes','')
        endif
  end subroutine grib_write_bytes_int4_size_t

  !> Write nbytes bytes from the buffer in a file opened with grib_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be written
  !> @param nbytes      number of bytes to be written
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_write_bytes_real4 ( ifile, buffer, nbytes, status )
        integer(kind=kindOfInt),intent(in)               :: ifile
        real(kind=4), dimension(:),intent(in)            :: buffer
        integer(kind=kindOfInt), intent(in)              :: nbytes
        integer(kind=kindOfInt),optional,intent(out)     :: status
        integer(kind=kindOfSize_t)                       :: ibytes
        integer(kind=kindOfInt)                          :: iret

        ibytes=nbytes
        iret=grib_f_write_file(ifile,buffer,ibytes)
        if (present(status)) then
           status = iret
        else
           call grib_check(iret,'write_bytes','')
        endif
  end subroutine grib_write_bytes_real4

  !> Write nbytes bytes from the buffer in a file opened with grib_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be written
  !> @param nbytes      number of bytes to be written
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_write_bytes_real4_size_t ( ifile, buffer, nbytes, status )
        integer(kind=kindOfInt),intent(in)               :: ifile
        real(kind=4), dimension(:),intent(in)            :: buffer
        integer(kind=kindOfSize_t), intent(in)           :: nbytes
        integer(kind=kindOfInt),optional,intent(out)     :: status
        integer(kind=kindOfInt)                          :: iret

        iret=grib_f_write_file(ifile,buffer,nbytes)
        if (present(status)) then
           status = iret
        else
           call grib_check(iret,'write_bytes','')
        endif
  end subroutine grib_write_bytes_real4_size_t

  !> Write nbytes bytes from the buffer in a file opened with grib_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be written
  !> @param nbytes      number of bytes to be written
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_write_bytes_real8 ( ifile, buffer, nbytes, status )
        integer(kind=kindOfInt),intent(in)               :: ifile
        real(kind=8), dimension(:),intent(in)            :: buffer
        integer(kind=kindOfInt), intent(in)              :: nbytes
        integer(kind=kindOfInt),optional,intent(out)     :: status
        integer(kind=kindOfSize_t)                       :: ibytes
        integer(kind=kindOfInt)                          :: iret

        ibytes=nbytes
        iret=grib_f_write_file(ifile,buffer,ibytes)
        if (present(status)) then
           status = iret
        else
           call grib_check(iret,'write_bytes','')
        endif
  end subroutine grib_write_bytes_real8

  !> Write nbytes bytes from the buffer in a file opened with grib_open_file.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> @param ifile       id of the opened file to be used in all the file functions.
  !> @param buffer      buffer to be written
  !> @param nbytes      number of bytes to be written
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_write_bytes_real8_size_t ( ifile, buffer, nbytes, status )
        integer(kind=kindOfInt),intent(in)               :: ifile
        real(kind=8), dimension(:),intent(in)            :: buffer
        integer(kind=kindOfSize_t), intent(in)           :: nbytes
        integer(kind=kindOfInt),optional,intent(out)     :: status
        integer(kind=kindOfInt)                          :: iret

        iret=grib_f_write_file(ifile,buffer,nbytes)
        if (present(status)) then
           status = iret
        else
           call grib_check(iret,'write_bytes','')
        endif
  end subroutine grib_write_bytes_real8_size_t

  !> Close a file.
  !>
  !> If the \em fileid does not refer to an opened file an error code
  !> is returned in status.\n
  !>
  !> \b Examples: \ref grib_get_keys.f90 "grib_get_keys.f90"
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param ifile is the id of the file to be closed.
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_close_file ( ifile , status )
        integer(kind=kindOfInt),intent(in)            :: ifile
        integer(kind=kindOfInt),optional, intent(out) :: status
        integer(kind=kindOfInt)                       :: iret

        iret=grib_f_close_file(ifile)
        if (present(status)) then
           status = iret
        else
           call grib_check(iret,'close_file','')
        endif
  end subroutine grib_close_file

  !> Counts the messages in a file
  !>
  !> \b Examples: \ref count_messages.f90 "count_messages.f90"
  !>
  !> @param ifile    id of the file opened with @ref grib_open_file
  !> @param n        number of messages in the file
  !> @param status    GRIB_SUCCESS if OK or error code
  subroutine grib_count_in_file ( ifile, n , status)
      integer(kind=kindOfInt),intent(in)              :: ifile
      integer(kind=kindOfInt),intent(out)             :: n
      integer(kind=kindOfInt),optional,intent(out)    :: status
      integer(kind=kindOfInt)                         :: iret

      iret=grib_f_count_in_file( ifile, n )
      if (present(status)) then
         status = iret
      else
          call grib_check(iret,'count_in_file','')
      endif
  end subroutine grib_count_in_file

  !> Load in memory only the headers of a grib message from a file.
  !>
  !> The message can be accessed through its gribid and it will be available\n
  !> until @ref grib_release is called.\n
  !>
  !> \b Examples: \ref grib_get_keys.f90 "grib_get_keys.f90"
  !>
  !> @param ifile     id of the file opened with @ref grib_open_file
  !> @param gribid    id of the grib loaded in memory
  !> @param status    GRIB_SUCCESS if OK, GRIB_END_OF_FILE at the end of file, or error code
  subroutine grib_headers_only_new_from_file ( ifile, gribid , status)
      integer(kind=kindOfInt),intent(in)              :: ifile
      integer(kind=kindOfInt),intent(out)             :: gribid
      integer(kind=kindOfInt),optional,intent(out)    :: status
      integer(kind=kindOfInt)                         :: iret

      iret=grib_f_headers_only_new_from_file( ifile, gribid )
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'headers_only_new_from_file','')
      endif
  end subroutine grib_headers_only_new_from_file

  !> Load in memory a grib message from a file.
  !>
  !> The message can be accessed through its gribid and it will be available\n
  !> until @ref grib_release is called.\n
  !>
  !> \b Examples: \ref grib_get_keys.f90 "grib_get_keys.f90"
  !>
  !> @param ifile     id of the file opened with @ref grib_open_file
  !> @param gribid    id of the grib loaded in memory
  !> @param status    GRIB_SUCCESS if OK, GRIB_END_OF_FILE at the end of file, or error code
  subroutine grib_new_from_file ( ifile, gribid , status)
      integer(kind=kindOfInt),intent(in)              :: ifile
      integer(kind=kindOfInt),intent(out)             :: gribid
      integer(kind=kindOfInt),optional,intent(out)    :: status
      integer(kind=kindOfInt)                         :: iret

      iret=grib_f_new_from_file( ifile, gribid )
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'grib_new_from_file','')
      endif
  end subroutine grib_new_from_file

  !> Load in memory a BUFR message from a file.
  !>
  !> The message can be accessed through its bufrid and it will be available\n
  !> until @ref codes_release is called.\n
  !>
  !> \b Examples: \ref grib_get_keys.f90 "grib_get_keys.f90"
  !>
  !> @param ifile     id of the file opened with @ref codes_open_file
  !> @param bufrid    id of the BUFR loaded in memory
  !> @param status    GRIB_SUCCESS if OK, GRIB_END_OF_FILE at the end of file, or error code
  subroutine bufr_new_from_file ( ifile, bufrid , status)
      integer(kind=kindOfInt),intent(in)              :: ifile
      integer(kind=kindOfInt),intent(out)             :: bufrid
      integer(kind=kindOfInt),optional,intent(out)    :: status
      integer(kind=kindOfInt)                         :: iret

      iret=bufr_f_new_from_file( ifile, bufrid )
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'bufr_new_from_file','')
      endif
  end subroutine bufr_new_from_file

  !> Load in memory a message from a file.
  !>
  !> The message can be accessed through its msgid and it will be available\n
  !> until @ref codes_release is called.\n
  !>
  !> @param ifile     id of the file opened with @ref codes_open_file
  !> @param msgid     id of the message loaded in memory
  !> @param status    GRIB_SUCCESS if OK, GRIB_END_OF_FILE at the end of file, or error code
  subroutine any_new_from_file ( ifile, msgid , status)
      integer(kind=kindOfInt),intent(in)              :: ifile
      integer(kind=kindOfInt),intent(out)             :: msgid
      integer(kind=kindOfInt),optional,intent(out)    :: status
      integer(kind=kindOfInt)                         :: iret

      iret=any_f_new_from_file( ifile, msgid )
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'any_new_from_file','')
      endif
  end subroutine any_new_from_file


  !> Create a new message in memory from a character array containting the coded message.
  !>
  !> The message can be accessed through its gribid and it will be available\n
  !> until @ref grib_release is called. A reference to the original coded\n
  !> message is kept in the new message structure.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref grib_copy_message.f90 "grib_copy_message.f90"
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param message     character array containing the coded message
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_new_from_message_char( gribid, message, status )
      integer(kind=kindOfInt),intent(out)               :: gribid
      character(len=1), dimension(:),intent(in)         :: message
      integer(kind=kindOfInt),optional, intent(out)     :: status
      integer(kind=kindOfSize_t)                        :: size_bytes
      integer(kind=kindOfInt)                           :: iret

      size_bytes=size(message,dim=1)
      iret = grib_f_new_from_message ( gribid, message, size_bytes )
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'new_from_message','')
      endif

  end subroutine grib_new_from_message_char

  !> Create a new message in memory from an integer array containting the coded message.
  !>
  !> The message can be accessed through its gribid and it will be available\n
  !> until @ref grib_release is called. A reference to the original coded\n
  !> message is kept in the new message structure.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> \b Examples: \ref grib_copy_message.f90 "grib_copy_message.f90"
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param message     integer array containing the coded message
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_new_from_message_int4 ( gribid, message, status )
      integer(kind=kindOfInt),intent(out)             :: gribid
      integer(kind=4), dimension(:),intent(in)        :: message
      integer(kind=kindOfInt),optional, intent(out)   :: status
      integer(kind=kindOfSize_t)                      :: size_bytes
      integer(kind=kindOfInt)                         :: iret

      size_bytes=size(message,dim=1)*sizeOfInteger4
      iret = grib_f_new_from_message ( gribid, message, size_bytes )
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'new_from_message','')
      endif

  end subroutine grib_new_from_message_int4

  !> Create a new valid gribid from a sample contained in a samples directory pointed
  !> by the environment variable ECCODES_SAMPLES_PATH.
  !> To know where the samples directory is run the codes_info tool.\n
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> \b Examples: \ref grib_samples.f90 "grib_samples.f90"
  !>
  !> @param gribid       id of the grib loaded in memory
  !> @param samplename name of the sample to be used
  !> @param status       GRIB_SUCCESS if OK, integer value on error
  subroutine grib_new_from_samples  ( gribid, samplename, status )
      integer(kind=kindOfInt),          intent(out) :: gribid
      character(len=*), intent(in)                  :: samplename
      integer(kind=kindOfInt),optional, intent(out) :: status
      integer(kind=kindOfInt)                       :: iret

      iret=grib_f_new_from_samples ( gribid, samplename )
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'grib_new_from_samples','('//samplename//')')
      endif
  end subroutine grib_new_from_samples


  !> Free the memory for the message referred as gribid.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref grib_get_keys.f90 "grib_get_keys.f90"
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_release ( gribid, status )
      integer(kind=kindOfInt),          intent(in)  :: gribid
      integer(kind=kindOfInt),optional, intent(out) :: status
      integer(kind=kindOfInt)                       :: iret

      iret=grib_f_release ( gribid )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'release','')
      endif
  end subroutine grib_release

  !> Create a copy of a message.
  !>
  !> Create a copy of a given message (\em gribid_src) giving a new\n
  !> message in memory (\em gribid_dest) exactly identical to the original one.\n
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !> \b Examples: \ref grib_clone.f90 "grib_clone.f90"
  !>
  !> @param gribid_src     grib to be cloned
  !> @param gribid_dest    new grib returned
  !> @param status         GRIB_SUCCESS if OK, integer value on error
  subroutine grib_clone ( gribid_src, gribid_dest, status )
      integer(kind=kindOfInt),          intent(in)  :: gribid_src
      integer(kind=kindOfInt),          intent(out) :: gribid_dest
      integer(kind=kindOfInt),optional, intent(out) :: status
      integer(kind=kindOfInt)                       :: iret

      iret=grib_f_clone(gribid_src,gribid_dest)
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid_src)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'clone','')
      endif
  end subroutine grib_clone

  subroutine grib_util_sections_copy ( gribid_from, gribid_to, what, gribid_out,status )
      integer(kind=kindOfInt),          intent(in)  :: gribid_from
      integer(kind=kindOfInt),          intent(in) :: gribid_to
      integer(kind=kindOfInt),          intent(out) :: gribid_out
      integer(kind=kindOfInt),			intent(in) :: what
      integer(kind=kindOfInt),optional, intent(out) :: status
      integer(kind=kindOfInt)                       :: iret

      iret=grib_f_util_sections_copy(gribid_from,gribid_to,what,gribid_out)
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid_from)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'grib_util_sections_copy','')
      endif
  end subroutine grib_util_sections_copy

  !> Copy the value of all the keys belonging to a namespace from the source message
  !> to the destination message
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !>
  !>
  !> @param gribid_src     source message
  !> @param gribid_dest    destination message
  !> @param namespace      namespace to be copied
  !> @param status         GRIB_SUCCESS if OK, integer value on error
  subroutine grib_copy_namespace ( gribid_src, namespace, gribid_dest, status )
      integer(kind=kindOfInt),          intent(in)  :: gribid_src
      integer(kind=kindOfInt),          intent(in)  :: gribid_dest
      character(LEN=*),                 intent(in)  :: namespace
      integer(kind=kindOfInt),optional, intent(out) :: status
      integer(kind=kindOfInt)                       :: iret

      iret=grib_f_copy_namespace(gribid_src,namespace,gribid_dest)
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'grib_copy_namespace','('//namespace//')')
      endif
  end subroutine grib_copy_namespace

  !> Check the status returned by a subroutine.
  !>
  !> In case of error it stops the program, returns the error code to the shell
  !> and prints the error message.\n
  !>
  !> @param status      the status to be checked
  !> @param caller      name of the caller soubroutine
  !> @param string      a string variable from the caller routine (e.g. key,filename)
  subroutine grib_check ( status,caller,string )
      integer(kind=kindOfInt), intent(in)  :: status
      character(len=*),        intent(in)  :: caller
      character(len=*),        intent(in)  :: string

      call grib_f_check( status,caller,string )
  end subroutine grib_check


  !> Get latitudes/longitudes/data values (real(4)).
  !>
  !> Latitudes, longitudes, data values arrays are returned.
  !> They must be properly allocated by the caller and their required
  !> dimension can be obtained with \ref grib_get_size or by getting (with \ref grib_get)
  !> the value of the integer key "numberOfPoints".
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param lats        latitudes array with dimension "size"
  !> @param lons        longitudes array with dimension "size"
  !> @param values      data values array with dimension "size"
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_get_data_real4 ( gribid, lats, lons, values, status )
      integer(kind=kindOfInt),                  intent(in)   :: gribid
      real ( kind = kindOfFloat ), dimension(:),intent(out)  :: lats, lons
      real ( kind = kindOfFloat ), dimension(:),intent(out)  :: values
      integer(kind=kindOfInt),optional, intent(out)          :: status
      integer(kind=kindOfInt)                                :: iret
      integer(kind=kindOfSize_t)                             :: npoints

      npoints=size(lats)
      iret = grib_f_get_data_real4 ( gribid, lats, lons, values,npoints )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'grib_get_data','')
      endif

  end subroutine grib_get_data_real4

  !> Get latitudes/longitudes/data values (real(8)).
  !>
  !> Latitudes, longitudes, data values arrays are returned.
  !> They must be properly allocated by the calling program/function.
  !> Their required dimension can be obtained by getting (with \ref grib_get)
  !> the value of the integer key "numberOfPoints".
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param lats        latitudes array
  !> @param lons        longitudes array
  !> @param values      data values array
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_get_data_real8 ( gribid, lats, lons, values, status )
      integer(kind=kindOfInt),                   intent(in)   :: gribid
      real ( kind = kindOfDouble ), dimension(:),intent(out)  :: lats, lons
      real ( kind = kindOfDouble ), dimension(:),intent(out)  :: values
      integer(kind=kindOfInt),optional, intent(out)           :: status
      integer(kind=kindOfInt)                                 :: iret
      integer(kind=kindOfSize_t)                              :: npoints

      npoints=size(lats)
      iret = grib_f_get_data_real8 ( gribid, lats, lons, values,npoints )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'grib_get_data','')
      endif

  end subroutine grib_get_data_real8

  !> Create a new iterator on the keys.
  !>
  !> The keys iterator can be navigated to give all the key names which
  !> can then be used to get or set the key values with \ref grib_get or
  !> \ref grib_set.
  !> The set of keys returned can be controlled with the input variable
  !> namespace or using the functions
  !> \ref grib_skip_read_only, \ref grib_skip_duplicates,
  !> \ref grib_skip_coded,\ref grib_skip_computed.
  !> If namespace is a non empty string only the keys belonging to
  !> that namespace are returned. Available namespaces are "ls" (to get the same
  !> default keys as the grib_ls and "mars" to get the keys used by mars.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param iterid      keys iterator id to be used in the keys iterator functions
  !> @param namespace   the namespace of the keys to search for (all the keys if empty)
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_keys_iterator_new ( gribid, iterid, namespace, status )
      integer(kind=kindOfInt),          intent(in)   :: gribid
      integer(kind=kindOfInt),          intent(inout)  :: iterid
      character(LEN=*), intent(in)                   :: namespace
      integer(kind=kindOfInt),optional, intent(out)  :: status
      integer(kind=kindOfInt)                        :: iret

      iret = grib_f_keys_iterator_new ( gribid, iterid, namespace )
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'keys_iterator_new',namespace)
      endif
  end subroutine grib_keys_iterator_new

  !> Advance to the next keys iterator value.
  !>
  !> @param iterid   keys iterator id created with @ref grib_keys_iterator_new
  !> @param status   GRIB_SUCCESS if next iterator exists, integer value if no more elements to iterate on
  subroutine grib_keys_iterator_next ( iterid , status)
      integer(kind=kindOfInt),          intent(in)  :: iterid
      integer(kind=kindOfInt),          intent(out) :: status
      integer(kind=kindOfInt)                       :: iret

      status = GRIB_SUCCESS
      iret = grib_f_keys_iterator_next ( iterid )
      if (iret == 0) then
          ! no more elements
          status = GRIB_END
      endif
  end subroutine grib_keys_iterator_next

  !> Delete a keys iterator and free memory.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param iterid      keys iterator id created with @ref grib_keys_iterator_new
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_keys_iterator_delete ( iterid , status)
      integer(kind=kindOfInt),          intent(in)  :: iterid
      integer(kind=kindOfInt),optional, intent(out) :: status
      integer(kind=kindOfInt)                       :: iret

      iret = grib_f_keys_iterator_delete ( iterid )
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'keys_iterator_delete','')
      endif
  end subroutine grib_keys_iterator_delete

  !> Get the name of a key from a keys iterator.
  !>
  !> If the status parameter (optional) is not given the program will exit with an error message\n
  !> otherwise the error message can be gathered with @ref grib_get_error_string.\n
  !>
  !> @param iterid      keys iterator id created with @ref grib_keys_iterator_new
  !> @param name        key name to be retrieved
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_keys_iterator_get_name ( iterid, name, status )
      integer(kind=kindOfInt),          intent(in)    :: iterid
      character(LEN=*), intent(out)                   :: name
      integer(kind=kindOfInt),optional, intent(out)   :: status
      integer(kind=kindOfInt)                         :: iret

      iret = grib_f_keys_iterator_get_name ( iterid, name )
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'keys_iterator_get_name',name)
      endif
  end subroutine grib_keys_iterator_get_name

  !> Rewind a keys iterator.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param iterid      keys iterator id created with @ref grib_keys_iterator_new
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_keys_iterator_rewind ( iterid, status )
      integer(kind=kindOfInt),          intent(in)    :: iterid
      integer(kind=kindOfInt),optional, intent(out)   :: status
      integer(kind=kindOfInt)                         :: iret

      iret  =  grib_f_keys_iterator_rewind ( iterid )
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'keys_iterator_rewind','')
      endif
  end subroutine grib_keys_iterator_rewind

  !> Dump the content of a grib message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_dump ( gribid , status)
      integer(kind=kindOfInt),          intent(in)  :: gribid
      integer(kind=kindOfInt),optional, intent(out) :: status
      integer(kind=kindOfInt)                       :: iret

      iret=grib_f_dump ( gribid )
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'dump','')
      endif
  end subroutine grib_dump

  !> Get the error message given an error code
  !>
  !> @param error   error code
  !> @param error_message error message
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_get_error_string ( error, error_message, status )
      integer(kind=kindOfInt),          intent(in)  :: error
      character(len=*), intent(out)                 :: error_message
      integer(kind=kindOfInt),optional, intent(out) :: status
      integer(kind=kindOfInt)                       :: iret

      iret=grib_f_get_error_string ( error, error_message )
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'get_error_string','')
      endif
  end subroutine grib_get_error_string

  !> Get the size of an array key.
  !>
  !> To get the size of a key representing an array.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     name of the key
  !> @param size        size of the array key
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_get_size_int ( gribid, key, size , status)
      integer(kind=kindOfInt),          intent(in)  :: gribid
      character(len=*), intent(in)                  :: key
      integer(kind=kindOfInt),          intent(out) :: size
      integer(kind=kindOfInt),optional, intent(out) :: status
      integer(kind=kindOfInt)                       :: iret

      iret=grib_f_get_size_int ( gribid, key, size )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'get_size',key)
      endif
  end subroutine grib_get_size_int

  !> Get the size of an array key.
  !>
  !> To get the size of a key representing an array.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     name of the key
  !> @param size        size of the array key
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_get_size_long ( gribid, key, size , status)
  integer(kind=kindOfInt),          intent(in)  :: gribid
  character(len=*), intent(in)                  :: key
  integer(kind=kindOfLong),          intent(out) :: size
  integer(kind=kindOfInt),optional, intent(out) :: status
  integer(kind=kindOfInt)                       :: iret

  iret=grib_f_get_size_long ( gribid, key, size )
  if (iret /= 0) then
    call grib_f_write_on_fail(gribid)
  endif
  if (present(status)) then
    status = iret
  else
    call grib_check(iret,'get_size',key)
  endif
  end subroutine grib_get_size_long

  !> Get the integer value of a key from a grib message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     key name
  !> @param value       the integer(4) value
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_get_int(gribid,key,value,status)
      integer(kind=kindOfInt),          intent(in)  :: gribid
      character(len=*), intent(in)                  :: key
      integer(kind = kindOfInt),     intent(out)    :: value
      integer(kind=kindOfInt),optional, intent(out) :: status
      integer(kind=kindOfInt)                       :: iret

      iret=grib_f_get_int ( gribid, key, value )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'get',key)
      endif
  end subroutine grib_get_int

  !> Get the integer value of a key from a grib message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     key name
  !> @param value       the integer(4) value
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_get_long(gribid,key,value,status)
    integer(kind=kindOfInt),          intent(in)  :: gribid
    character(len=*), intent(in)                  :: key
    integer(kind = kindOfLong),     intent(out)    :: value
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)                       :: iret

    iret=grib_f_get_long ( gribid, key, value )
	 if (iret /= 0) then
	   call grib_f_write_on_fail(gribid)
	 endif
    if (present(status)) then
      status = iret
    else
      call grib_check(iret,'get',key)
    endif
  end subroutine grib_get_long

  !> Check if the value of a key is MISSING.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key         key name
  !> @param is_missing  0->not missing, 1->missing
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_is_missing(gribid,key,is_missing,status)
    integer(kind=kindOfInt),          intent(in)  :: gribid
    character(len=*), intent(in)                  :: key
    integer(kind = kindOfInt),     intent(out)    :: is_missing
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)                       :: iret

    iret=grib_f_is_missing ( gribid, key, is_missing )
	 if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	 endif
    if (present(status)) then
    status = iret
    else
    call grib_check(iret,'is_missing',key)
    endif
  end subroutine grib_is_missing

  !> Check if a key is DEFINED.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key         key name
  !> @param is_defined  0->not defined, 1->defined
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_is_defined(gribid,key,is_defined,status)
  integer(kind=kindOfInt),          intent(in)  :: gribid
  character(len=*), intent(in)                  :: key
  integer(kind = kindOfInt),     intent(out)    :: is_defined
  integer(kind=kindOfInt),optional, intent(out) :: status
  integer(kind=kindOfInt)                       :: iret

    iret=grib_f_is_defined( gribid, key, is_defined )
	 if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	 endif
    if (present(status)) then
    status = iret
    else
    call grib_check(iret,'is_defined',key)
    endif
  end subroutine grib_is_defined

  !> Get the real(4) value of a key from a grib message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     key name
  !> @param value       the real(4) value
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_get_real4 ( gribid, key, value, status )
      integer(kind=kindOfInt),          intent(in)             :: gribid
      character(len=*),                 intent(in)             :: key
      real(kind = kindOfFloat),         intent(out)            :: value
      integer(kind=kindOfInt),optional, intent(out)            :: status
      integer(kind=kindOfInt)                                  :: iret

      iret=grib_f_get_real4 ( gribid, key, value )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'get',key)
      endif
  end subroutine grib_get_real4

  !> Get the real(8) value of a key from a grib message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     key name
  !> @param value       the real(8) value
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_get_real8 ( gribid, key, value , status)
      integer(kind=kindOfInt),          intent(in)      :: gribid
      character(len=*),                 intent(in)      :: key
      real(kind = kindOfDouble),        intent(out)     :: value
      integer(kind=kindOfInt),optional, intent(out)     :: status
      integer(kind=kindOfInt)                           :: iret

      iret=grib_f_get_real8 ( gribid, key, value )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'get',key)
      endif
  end subroutine grib_get_real8

  !> Get the character value of a key from a grib message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     key name
  !> @param value       the real(8) value
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_get_string ( gribid, key, value, status )
      integer(kind=kindOfInt),          intent(in)  :: gribid
      character(len=*),                 intent(in)  :: key
      character(len=*),                 intent(out) :: value
      integer(kind=kindOfInt),optional, intent(out) :: status
      integer(kind=kindOfInt)                       :: iret

      iret=grib_f_get_string ( gribid, key, value )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'get',key)
      endif
  end subroutine grib_get_string


  !> Get the integer array of values for a key from a grib message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     key name
  !> @param value       integer(4) array value
  !> @param status       GRIB_SUCCESS if OK, integer value on error
  subroutine grib_get_int_array ( gribid, key, value, status )
      integer(kind=kindOfInt),               intent(in)  :: gribid
      character(len=*),      intent(in)                  :: key
      integer(kind=kindOfInt), dimension(:), intent(out) :: value
      integer(kind=kindOfInt),optional, intent(out)      :: status

      integer(kind=kindOfInt)                            :: iret
      integer(kind=kindOfInt)                            :: nb_values

      nb_values = size(value)
      iret=grib_f_get_int_array ( gribid, key, value , nb_values )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'get',key)
      endif
  end subroutine grib_get_int_array

  !> Get the integer array of values for a key from a grib message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     key name
  !> @param value       integer(4) array value
  !> @param status       GRIB_SUCCESS if OK, integer value on error
  subroutine grib_get_long_array ( gribid, key, value, status )
    integer(kind=kindOfInt),               intent(in)  :: gribid
    character(len=*),      intent(in)  :: key
    integer(kind = kindOfLong),dimension(:), intent(out) :: value
    integer(kind=kindOfInt),optional, intent(out)      :: status
    integer(kind=kindOfInt)                            :: iret
    integer(kind=kindOfInt)                            :: nb_values

    nb_values = size(value)
    iret=grib_f_get_long_array ( gribid, key, value , nb_values )
	if (iret /= 0) then
	  call grib_f_write_on_fail(gribid)
	endif
    if (present(status)) then
      status = iret
    else
      call grib_check(iret,'get',key)
    endif
  end subroutine grib_get_long_array

  !> Get the array of bytes (character) for a key from a grib message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key         key name
  !> @param value       character(len=1) array of byte values
  !> @param length      (optional) output: number of values retrieved
  !> @param status      (optional) GRIB_SUCCESS if OK, integer value on error
  subroutine grib_get_byte_array ( gribid, key, value, length, status )
    integer(kind=kindOfInt),               intent(in)  :: gribid
    character(len=*),                      intent(in)  :: key
    character(len=1),dimension(:),intent(out) :: value
    integer(kind=kindOfInt), optional,     intent(out) :: length
    integer(kind=kindOfInt), optional,     intent(out) :: status
    integer(kind=kindOfInt)                            :: iret
    integer(kind=kindOfInt)                            :: nb_values
    character                                          :: bytes(size(value))

    nb_values = size (value)
    bytes = ACHAR(0)
    iret = grib_f_get_byte_array ( gribid, key, bytes, nb_values )
    value = transfer (bytes, value)
    if (iret /= 0) then
       call grib_f_write_on_fail(gribid)
    endif
    if (present(length)) then
       length = nb_values
    end if
    if (present(status)) then
       status = iret
    else
       call grib_check(iret,'get',key)
    endif
  end subroutine grib_get_byte_array

  !> Get the real(4) array of values for a key from a grib message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     key name
  !> @param value       real(4) array value
  !> @param status       GRIB_SUCCESS if OK, integer value on error
  subroutine grib_get_real4_array ( gribid, key, value, status )
      integer(kind=kindOfInt),                 intent(in)  :: gribid
      character(len=*),                        intent(in)  :: key
      real(kind = kindOfFloat), dimension(:), intent(out)  :: value
      integer(kind=kindOfInt),optional,        intent(out) :: status
      integer(kind=kindOfInt)                              :: iret
      integer(kind=kindOfInt)                              :: nb_values

      nb_values = size(value)
      iret=grib_f_get_real4_array ( gribid, key, value , nb_values )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
        status = iret
      else
        call grib_check(iret,'get',key)
      endif
  end subroutine grib_get_real4_array

  !> Get the real(8) array of values for a key from a grib message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     key name
  !> @param value       real(8) array value
  !> @param status       GRIB_SUCCESS if OK, integer value on error
  subroutine grib_get_real8_array ( gribid, key, value, status )
      integer(kind=kindOfInt),                   intent(in)  :: gribid
      character(len=*),                          intent(in)  :: key
      real(kind = kindOfDouble),dimension(:), intent(out) :: value
      integer(kind=kindOfInt),optional,          intent(out) :: status
      integer(kind=kindOfInt)                                :: iret
      integer(kind=kindOfInt)                                :: nb_values

      nb_values = size(value)
      iret=grib_f_get_real8_array ( gribid, key, value, nb_values )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'get',key)
      endif
  end subroutine grib_get_real8_array

  !> Get a real(4) value of specified index from an array key.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     key name
  !> @param index       integer(4) index
  !> @param value       real(4) value
  !> @param status       GRIB_SUCCESS if OK, integer value on error
  subroutine grib_get_real4_element ( gribid, key, index,value, status )
      integer(kind=kindOfInt),                     intent(in)  :: gribid
      character(len=*),                            intent(in)  :: key
      integer(kind=kindOfInt),                       intent(in)  :: index
      real(kind = kindOfFloat),intent(out) :: value
      integer(kind=kindOfInt),optional,            intent(out) :: status
      integer(kind=kindOfInt)                                  :: iret

      iret=grib_f_get_real4_element ( gribid, key, index,value )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'get',key)
      endif
  end subroutine grib_get_real4_element

  !> Get a real(8) value of specified index from an array key.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     key name
  !> @param index       integer(4) index
  !> @param value       real(8) value
  !> @param status       GRIB_SUCCESS if OK, integer value on error
  subroutine grib_get_real8_element ( gribid, key, index,value, status )
      integer(kind=kindOfInt),                     intent(in)  :: gribid
      character(len=*),                            intent(in)  :: key
      integer(kind=kindOfInt),                  intent(in)  :: index
      real(kind = kindOfDouble),                   intent(out) :: value
      integer(kind=kindOfInt),optional,            intent(out) :: status
      integer(kind=kindOfInt)                                  :: iret

      iret=grib_f_get_real8_element ( gribid, key, index,value )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'get',key)
      endif
  end subroutine grib_get_real8_element

  !> Get the real(4) values whose indexes are stored in the array "index" from an array key.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     key name
  !> @param index       integer(4) array indexes
  !> @param value       real(4) array value
  !> @param status       GRIB_SUCCESS if OK, integer value on error
  subroutine grib_get_real4_elements ( gribid, key, index,value, status )
      integer(kind=kindOfInt),                  intent(in)  :: gribid
      character(len=*),                         intent(in)  :: key
      integer(kind=kindOfInt),dimension(:),  intent(in)  :: index
      real(kind = kindOfFloat), dimension(:),   intent(out) :: value
      integer(kind=kindOfInt),optional,         intent(out) :: status
      integer(kind=kindOfInt)                               :: iret
      integer(kind=kindOfInt)                               ::npoints

      npoints=size(value)
      iret=grib_f_get_real4_elements ( gribid, key, index,value,npoints )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'get',key)
      endif
  end subroutine grib_get_real4_elements

  !> Get the real(8) values whose indexes are stored in the array "index" from an array key.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     key name
  !> @param index       integer(4) array index
  !> @param value       real(8) array value
  !> @param status       GRIB_SUCCESS if OK, integer value on error
  subroutine grib_get_real8_elements ( gribid, key, index,value, status )
      integer(kind=kindOfInt),                   intent(in)  :: gribid
      character(len=*),                          intent(in)  :: key
      integer(kind=kindOfInt),dimension(:),   intent(in)  :: index
      real(kind = kindOfDouble), dimension(:),   intent(out) :: value
      integer(kind=kindOfInt),optional,          intent(out) :: status
      integer(kind=kindOfInt)                                :: iret
      integer(kind=kindOfInt)                                :: npoints

      npoints=size(value)
      iret=grib_f_get_real8_elements ( gribid, key, index,value,npoints )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'get',key)
      endif
  end subroutine grib_get_real8_elements

  !> Set the integer value for a key in a grib message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     key name
  !> @param value       integer(4) value
  !> @param status       GRIB_SUCCESS if OK, integer value on error
  subroutine grib_set_int ( gribid, key, value, status )
      integer(kind=kindOfInt),          intent(in)  :: gribid
      character(len=*), intent(in)                  :: key
      integer(kind=kindOfInt),          intent(in)  :: value
      integer(kind=kindOfInt),optional, intent(out) :: status
      integer(kind=kindOfInt)                       :: iret

      iret=grib_f_set_int ( gribid, key, value )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'set',key)
      endif
  end subroutine grib_set_int

  !> Set the integer value for a key in a grib message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     key name
  !> @param value       integer(4) value
  !> @param status       GRIB_SUCCESS if OK, integer value on error
  subroutine grib_set_long ( gribid, key, value, status )
    integer(kind=kindOfInt),          intent(in)  :: gribid
    character(len=*), intent(in)                  :: key
    integer(kind=kindOfLong),         intent(in)  :: value
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)                       :: iret

    iret=grib_f_set_long ( gribid, key, value )
    if (iret /= 0) then
      call grib_f_write_on_fail(gribid)
    endif
    if (present(status)) then
      status = iret
    else
      call grib_check(iret,'set',key)
    endif
  end subroutine grib_set_long

  !> Set the real(4) value for a key in a grib message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     key name
  !> @param value       real(4) value
  !> @param status       GRIB_SUCCESS if OK, integer value on error
  subroutine grib_set_real4 ( gribid, key, value, status )
      integer(kind=kindOfInt),                 intent(in)  :: gribid
      character(len=*),                        intent(in)  :: key
      real(kind = kindOfFloat),                intent(in)  :: value
      integer(kind=kindOfInt),optional,        intent(out) :: status
      integer(kind=kindOfInt)                              :: iret

      iret=grib_f_set_real4 ( gribid, key, value )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'set',key)
      endif
  end subroutine grib_set_real4

  !> Set the real(8) value for a key in a grib message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     key name
  !> @param value       real(8) value
  !> @param status       GRIB_SUCCESS if OK, integer value on error
  subroutine grib_set_real8 ( gribid, key, value, status )
      integer(kind=kindOfInt),                  intent(in)  :: gribid
      character(len=*),                         intent(in)  :: key
      real(kind = kindOfDouble),                intent(in)  :: value
      integer(kind=kindOfInt),optional,         intent(out) :: status
      integer(kind=kindOfInt)                               :: iret

      iret=grib_f_set_real8 ( gribid, key, value )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'set',key)
      endif
  end subroutine grib_set_real8

  !> Set the integers values for an array key in a grib message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     key name
  !> @param value       integer(4) array value
  !> @param status       GRIB_SUCCESS if OK, integer value on error
  subroutine grib_set_int_array ( gribid, key, value, status)
      integer(kind=kindOfInt),                intent(in)  :: gribid
      character(len=*),                       intent(in)  :: key
      integer(kind=kindOfInt), dimension(:),  intent(in)  :: value
      integer(kind=kindOfInt),optional,       intent(out) :: status
      integer(kind=kindOfInt)                             :: iret
      integer(kind=kindOfInt)                             :: nb_values

      nb_values = size(value)
      iret=grib_f_set_int_array ( gribid, key, value, nb_values )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'set',key)
      endif
  end subroutine grib_set_int_array

  !> Set the integers values for an array key in a grib message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     key name
  !> @param value       integer(4) array value
  !> @param status       GRIB_SUCCESS if OK, integer value on error
  subroutine grib_set_long_array ( gribid, key, value, status)
    integer(kind=kindOfInt),                intent(in)  :: gribid
    character(len=*),                       intent(in)  :: key
    integer(kind=kindOfLong), dimension(:),  intent(in)  :: value
    integer(kind=kindOfInt),optional,       intent(out) :: status
    integer(kind=kindOfInt)                             :: iret
    integer(kind=kindOfInt)                             :: nb_values

    nb_values = size(value)
    iret=grib_f_set_long_array ( gribid, key, value, nb_values )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
    if (present(status)) then
      status = iret
    else
      call grib_check(iret,'set',key)
    endif
      
  end subroutine grib_set_long_array

  !> Set the array of bytes (character) for a key in a grib message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key         key name
  !> @param value       character(len=1) array of byte values
  !> @param length      (optional) output: number of values written
  !> @param status      (optional) GRIB_SUCCESS if OK, integer value on error
  subroutine grib_set_byte_array ( gribid, key, value, length, status )
    integer(kind=kindOfInt),               intent(in)  :: gribid
    character(len=*),                      intent(in)  :: key
    character(len=1),        dimension(:), intent(in)  :: value
    integer(kind=kindOfInt), optional,     intent(out) :: length
    integer(kind=kindOfInt), optional,     intent(out) :: status
    integer(kind=kindOfInt)                            :: iret
    integer(kind=kindOfInt)                            :: nb_values
    character                                          :: bytes(size(value))

    nb_values = size (value)
    bytes = transfer (value, bytes)
    iret = grib_f_set_byte_array ( gribid, key, bytes, nb_values )
    if (iret /= 0) then
       call grib_f_write_on_fail(gribid)
    endif
    if (present(length)) then
       length = nb_values
    end if
    if (present(status)) then
       status = iret
    else
       call grib_check(iret,'set',key)
    endif
  end subroutine grib_set_byte_array

  !> Set the real(4) values for an array key in a grib message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     key name
  !> @param value       real(4) array value
  !> @param status       GRIB_SUCCESS if OK, integer value on error
  subroutine grib_set_real4_array ( gribid, key, value, status )
      integer(kind=kindOfInt),                 intent(in)  :: gribid
      character(len=*),                        intent(in)  :: key
      real(kind = kindOfFloat), dimension(:),  intent(in)  :: value
      integer(kind=kindOfInt),optional,        intent(out) :: status
      integer(kind=kindOfInt)                              :: iret
      integer(kind=kindOfInt)                              :: nb_values

      nb_values = size(value)
      iret=grib_f_set_real4_array ( gribid, key, value, nb_values )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'set',key)
      endif
  end subroutine grib_set_real4_array

  !> Set the real(8) values for an array key in a grib message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     key name
  !> @param value       real(8) array value
  !> @param status       GRIB_SUCCESS if OK, integer value on error
  subroutine grib_set_real8_array ( gribid, key, value, status)
      integer(kind=kindOfInt),                  intent(in)  :: gribid
      character(len=*),                         intent(in)  :: key
      real(kind = kindOfDouble), dimension(:),  intent(in)  :: value
      integer(kind=kindOfInt),optional,         intent(out) :: status
      integer(kind=kindOfInt)                               :: iret
      integer(kind=kindOfInt)                               :: nb_values

      nb_values = size(value)
      iret=grib_f_set_real8_array ( gribid, key, value, nb_values )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'set',key)
      endif
  end subroutine grib_set_real8_array

  !> Set the real(4) values for an array key in a grib message, forces the set if the key is read-only.
  !> Use with great caution!!
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     key name
  !> @param value       real(4) array value
  !> @param status       GRIB_SUCCESS if OK, integer value on error
  subroutine grib_set_force_real4_array ( gribid, key, value, status )
      integer(kind=kindOfInt),                 intent(in)  :: gribid
      character(len=*),                        intent(in)  :: key
      real(kind = kindOfFloat), dimension(:),  intent(in)  :: value
      integer(kind=kindOfInt),optional,        intent(out) :: status
      integer(kind=kindOfInt)                              :: iret
      integer(kind=kindOfInt)                              :: nb_values

      nb_values = size(value)
      iret=grib_f_set_force_real4_array ( gribid, key, value, nb_values )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'set',key)
      endif
  end subroutine grib_set_force_real4_array

  !> Set the real(8) values for an array key in a grib message, forces the set if the key is read-only.
  !> Use with great caution!!
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     key name
  !> @param value       real(8) array value
  !> @param status       GRIB_SUCCESS if OK, integer value on error
  subroutine grib_set_force_real8_array ( gribid, key, value, status)
      integer(kind=kindOfInt),                  intent(in)  :: gribid
      character(len=*),                         intent(in)  :: key
      real(kind = kindOfDouble), dimension(:),  intent(in)  :: value
      integer(kind=kindOfInt),optional,         intent(out) :: status
      integer(kind=kindOfInt)                               :: iret
      integer(kind=kindOfInt)                               :: nb_values

      nb_values = size(value)
      iret=grib_f_set_force_real8_array ( gribid, key, value, nb_values )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'set',key)
      endif
  end subroutine grib_set_force_real8_array

  !> Set the character value for a string key in a grib message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key     key name
  !> @param value       character value
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_set_string  ( gribid, key, value , status)
      integer(kind=kindOfInt),                      intent(in)  :: gribid
      character(len=*),                             intent(in)  :: key
      character(len=*),                             intent(in)  :: value
      integer(kind=kindOfInt),optional,             intent(out) :: status
      integer(kind=kindOfInt)                                   :: iret

      iret=grib_f_set_string  ( gribid, key, value )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'set',key)
      endif
  end subroutine grib_set_string

  !> Get the size of a coded message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param nbytes      size in bytes of the message
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_get_message_size_int  ( gribid, nbytes, status)
      integer(kind=kindOfInt),                      intent(in)  :: gribid
      integer(kind=kindOfInt),                      intent(out) :: nbytes
      integer(kind=kindOfInt),optional,             intent(out) :: status
      integer(kind=kindOfInt)                                   :: iret
      integer(kind=kindOfSize_t)                                :: ibytes

      iret = grib_f_get_message_size ( gribid, ibytes )
      if (iret == GRIB_SUCCESS .and. ibytes > huge(nbytes)) then
        iret = GRIB_MESSAGE_TOO_LARGE
      endif
      nbytes = ibytes
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'get_message_size','')
      endif
  end subroutine grib_get_message_size_int

  !> Get the size of a coded message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param nbytes      size in bytes of the message
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_get_message_size_size_t  ( gribid, nbytes, status)
      integer(kind=kindOfInt),                      intent(in)  :: gribid
      integer(kind=kindOfSize_t),                   intent(out) :: nbytes
      integer(kind=kindOfInt),optional,             intent(out) :: status
      integer(kind=kindOfInt)                                   :: iret

      iret=grib_f_get_message_size ( gribid, nbytes )
	  if (iret /= 0) then
	    call grib_f_write_on_fail(gribid)
	  endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'get_message_size','')
      endif
  end subroutine grib_get_message_size_size_t

  !> Copy the coded message into an array.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param message     array containing the coded message to be copied
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_copy_message ( gribid, message, status )
      integer(kind=kindOfInt),                      intent(in)  :: gribid
      character(len=1), dimension(:),               intent(out) :: message
      integer(kind=kindOfInt),optional,             intent(out) :: status
      integer(kind=kindOfInt)      :: iret
      integer(kind=kindOfSize_t)   :: size_bytes

      size_bytes = size(message,dim=1)
      iret=grib_f_copy_message ( gribid, message, size_bytes )
      if (iret /= 0) then
        call grib_f_write_on_fail(gribid)
      endif
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'copy_message','')
      endif
  end subroutine grib_copy_message

  !> Write the coded message to a file.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param ifile       file id of a file opened with \ref grib_open_file
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_write ( gribid, ifile  , status)
      integer(kind=kindOfInt),          intent(in)  :: gribid
      integer(kind=kindOfInt),          intent(in)  :: ifile
      integer(kind=kindOfInt),optional, intent(out) :: status

      integer(kind=kindOfInt)               :: iret

      iret = grib_f_write( gribid, ifile )
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'write','')
      endif
  end subroutine grib_write

  !> Write a multi field message to a file.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param multigribid      id of the multi field grib loaded in memory
  !> @param ifile       file id of a file opened with \ref grib_open_file
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_multi_write ( multigribid, ifile  , status)
    integer(kind=kindOfInt),          intent(in)  :: multigribid
    integer(kind=kindOfInt),          intent(in)  :: ifile
    integer(kind=kindOfInt),optional, intent(out) :: status
    integer(kind=kindOfInt)               :: iret

    iret = grib_f_multi_write( multigribid, ifile )
    if (present(status)) then
      status = iret
    else
      call grib_check(iret,'grib_multi_write','')
    endif
    end subroutine grib_multi_write

  !> Append a single field grib message to a multi field grib message.
  !> Only the sections with section number greather or equal "startsection" are copied from the input single message to the multi field output grib.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param ingribid      id of the input single grib 
  !> @param startsection  starting from startsection (included) all the sections are copied from the input single grib to the output multi grib
  !> @param multigribid    id of the output multi filed grib
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_multi_append ( ingribid, startsection, multigribid  , status)
    integer(kind=kindOfInt),          intent(in)  :: ingribid
    integer(kind=kindOfInt),          intent(in)  :: startsection
    integer(kind=kindOfInt),          intent(out) :: multigribid
    integer(kind=kindOfInt),optional, intent(out) :: status

    integer(kind=kindOfInt)               :: iret

    iret = grib_f_multi_append( ingribid, startsection, multigribid )
    if (present(status)) then
      status = iret
    else
      call grib_check(iret,'grib_multi_append','')
    endif
  end subroutine grib_multi_append

  !> Find the nearest point of a set of points whose latitudes and longitudes
  !> are given in the inlats, inlons arrays respectively.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
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
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_find_nearest_multiple(gribid,is_lsm,       &
                 inlats,inlons,outlats,outlons,              &
                 values,distances, indexes,status)
    integer(kind=kindOfInt),                                  intent(in)    :: gribid
    logical,                                  intent(in)    :: is_lsm
    real(kind = kindOfDouble), dimension(:), intent(in)    :: inlats
    real(kind = kindOfDouble), dimension(:), intent(in)    :: inlons
    real(kind = kindOfDouble), dimension(:), intent(out)   :: outlats
    real(kind = kindOfDouble), dimension(:), intent(out)   :: outlons
    real(kind = kindOfDouble), dimension(:), intent(out)   :: distances
    real(kind = kindOfDouble), dimension(:), intent(out)   :: values
    integer(kind = kindOfInt), dimension(:), intent(out)  :: indexes
    integer(kind=kindOfInt),optional, intent(out)                           :: status
    integer(kind=kindOfInt)                                                 :: iret
    integer(kind=kindOfInt) :: npoints
    integer(kind=kindOfInt) :: intis_lsm

    intis_lsm = 0
    if (is_lsm) intis_lsm=1
    npoints=size(inlats)
    iret=grib_f_find_nearest_multiple(gribid,intis_lsm,inlats,inlons,outlats,outlons, &
      values,distances,indexes,npoints)

    if (present(status)) then
         status = iret
    else
         call grib_check(iret,'grib_find_nearest_multiple','')
    endif
  end subroutine grib_find_nearest_multiple

  !> Find the nearest point of a given latitude/longitude point.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
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
  !> @param status     GRIB_SUCCESS if OK, integer value on error
  subroutine grib_find_nearest_single(gribid,is_lsm,                  &
                 inlat,inlon,outlat,outlon,              &
                 value,distance, index,status)
    integer(kind=kindOfInt),                                  intent(in)    :: gribid
    logical,                                  intent(in)    :: is_lsm
    real(kind = kindOfDouble), intent(in)    :: inlat
    real(kind = kindOfDouble), intent(in)    :: inlon
    real(kind = kindOfDouble), intent(out)   :: outlat
    real(kind = kindOfDouble), intent(out)   :: outlon
    real(kind = kindOfDouble), intent(out)   :: distance
    real(kind = kindOfDouble), intent(out)   :: value
    integer(kind = kindOfInt), intent(out)  :: index
    integer(kind=kindOfInt),optional, intent(out)                           :: status
    integer(kind=kindOfInt)                                                 :: iret
    integer(kind=kindOfInt) :: intis_lsm =0

    if (is_lsm) intis_lsm=1
    iret=grib_f_find_nearest_single(gribid,intis_lsm,inlat,inlon,outlat,outlon, &
      value,distance,index)

    if (present(status)) then
         status = iret
    else
         call grib_check(iret,'grib_find_nearest_single','')
    endif
  end subroutine grib_find_nearest_single

  !> Find the 4 nearest points of a latitude longitude point.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
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
  !> @param status     GRIB_SUCCESS if OK, integer value on error
  subroutine grib_find_nearest_four_single(gribid,is_lsm, &
                 inlat,inlon,outlat,outlon,               &
                 value,distance, index,status)
    integer(kind=kindOfInt),                                  intent(in)    :: gribid
    logical,                                  intent(in)    :: is_lsm
    real(kind = kindOfDouble), intent(in)    :: inlat
    real(kind = kindOfDouble), intent(in)    :: inlon
    real(kind = kindOfDouble), dimension(4), intent(out)   :: outlat
    real(kind = kindOfDouble), dimension(4), intent(out)   :: outlon
    real(kind = kindOfDouble), dimension(4), intent(out)   :: distance
    real(kind = kindOfDouble), dimension(4), intent(out)   :: value
    integer(kind = kindOfInt), dimension(4), intent(out)  :: index
    integer(kind=kindOfInt),optional, intent(out)                           :: status
    integer(kind=kindOfInt)                                                 :: iret
    integer(kind=kindOfInt) :: intis_lsm =0

    if (is_lsm) intis_lsm=1

    iret=grib_f_find_nearest_four_single(gribid,intis_lsm,inlat,inlon,outlat,outlon, &
      value,distance,index)

    if (present(status)) then
       status = iret
    else
       call grib_check(iret,'grib_find_nearest_four_single','')
    endif
  end subroutine grib_find_nearest_four_single

  !> Turn on the support for multiple fields in a single message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_multi_support_on (status  )
      integer(kind=kindOfInt),optional, intent(out) :: status
      integer(kind=kindOfInt)               :: iret

      iret = grib_f_multi_support_on ( )
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'grib_multi_support_on','')
      endif
  end subroutine grib_multi_support_on

  !> Turn off the support for multiple fields in a single message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_multi_support_off ( status  )
      integer(kind=kindOfInt),optional, intent(out) :: status
      integer(kind=kindOfInt)               :: iret

      iret = grib_f_multi_support_off ( )
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'grib_multi_support_off','')
      endif
  end subroutine grib_multi_support_off

  !> Turn on the compatibility mode with gribex.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_gribex_mode_on (status )
      integer(kind=kindOfInt),optional, intent(out) :: status
      integer(kind=kindOfInt)               :: iret

      iret=grib_f_gribex_mode_on()
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'grib_gribex_mode_on','')
      endif

  end subroutine grib_gribex_mode_on

  !> Turn off the compatibility mode with gribex.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_gribex_mode_off (status )
      integer(kind=kindOfInt),optional, intent(out) :: status
      integer(kind=kindOfInt)               :: iret

      iret=grib_f_gribex_mode_off()
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'grib_gribex_mode_off','')
      endif

  end subroutine grib_gribex_mode_off

  !> Skip the computed keys in a keys iterator.
  !>
  !> The computed keys are not coded in the message they are computed
  !> from other keys.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @see grib_keys_iterator_new,grib_keys_iterator_next,grib_keys_iterator_release
  !>
  !> @param iterid      keys iterator id
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_skip_computed ( iterid , status)
      integer(kind=kindOfInt),          intent(in)  :: iterid
      integer(kind=kindOfInt),optional, intent(out) :: status
      integer(kind=kindOfInt)   :: iret

      iret = grib_f_skip_computed ( iterid )
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'skip_computed','')
      endif
  end subroutine grib_skip_computed

  !> Skip the coded keys in a keys iterator.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> The coded keys are actually coded in the message.
  !>
  !> @see grib_keys_iterator_new,grib_keys_iterator_next,grib_keys_iterator_release
  !>
  !> @param iterid      keys iterator id
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_skip_coded  ( iterid, status )
      integer(kind=kindOfInt),          intent(in)  :: iterid
      integer(kind=kindOfInt),optional, intent(out) :: status
      integer(kind=kindOfInt)   :: iret

      iret  = grib_f_skip_coded  ( iterid )
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'skip_coded','')
      endif
  end subroutine grib_skip_coded


  !> Skip the duplicated keys in a keys iterator.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @see grib_keys_iterator_new,grib_keys_iterator_next,grib_keys_iterator_release
  !>
  !> @param iterid      keys iterator id
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_skip_duplicates ( iterid, status )
      integer(kind=kindOfInt),          intent(in)  :: iterid
      integer(kind=kindOfInt),optional, intent(out) :: status
      integer(kind=kindOfInt) :: iret

      iret = grib_f_skip_duplicates ( iterid )
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'skip_duplicates','')
      endif
  end subroutine grib_skip_duplicates


  !> Skip the read_only keys in a keys iterator.
  !>
  !> Read only keys cannot be set.
  !>
  !> @see grib_keys_iterator_new,grib_keys_iterator_next,grib_keys_iterator_release
  !>
  !> @param iterid      keys iterator id
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  subroutine grib_skip_read_only  ( iterid, status )
      integer(kind=kindOfInt),          intent(in)  :: iterid
      integer(kind=kindOfInt),optional, intent(out) :: status
      integer(kind=kindOfInt)  :: iret

      iret = grib_f_skip_read_only  ( iterid )
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'skip_read_only','')
      endif
  end subroutine grib_skip_read_only

  !> Set the definition path
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param path       definitions path
  !> @param status     GRIB_SUCCESS if OK, integer value on error
  subroutine grib_set_definitions_path ( path,  status )
      character(len=*),                        intent(in)  :: path
      integer(kind=kindOfInt),optional, intent(out)        :: status
      integer(kind=kindOfInt)                              :: iret

      iret=grib_f_set_definitions_path ( path )
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'set_definitions_path','('//path//')')
      endif
  end subroutine grib_set_definitions_path

  !> Set the samples path
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param path       samples path
  !> @param status     GRIB_SUCCESS if OK, integer value on error
  subroutine grib_set_samples_path ( path,  status )
      character(len=*),                        intent(in)  :: path
      integer(kind=kindOfInt),optional, intent(out)        :: status
      integer(kind=kindOfInt)                              :: iret

      iret=grib_f_set_samples_path ( path )
      if (present(status)) then
         status = iret
      else
         call grib_check(iret,'set_samples_path','('//path//')')
      endif
  end subroutine grib_set_samples_path

end module grib_api

