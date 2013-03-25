  !> Get the distinct values of the key in argument contained in the index. The key must belong to the index.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref index.f90 "index.f90"
  !>
  !> @param  indexid   id of an index created from a file. The index must have been created with the key in argument.
  !> @param key        key for wich the values are returned
  !> @param values     array of values. The array must be allocated before entering this function and its size must be enough to contain all the values.
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  interface grib_index_get
    module procedure grib_index_get_int, &
                     grib_index_get_string, &
                     grib_index_get_real8
  end interface grib_index_get

  !> Get the number of distinct values of the key in argument contained in the index. The key must belong to the index.
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref index.f90 "index.f90"
  !>
  !> @param  indexid     id of an index created from a file. The index must have been created with the key in argument.
  !> @param key        key for which the number of values is computed
  !> @param size       number of distinct values of the key in the index
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  interface grib_index_get_size
  module procedure  grib_index_get_size_int
  end interface grib_index_get_size
                                 
  !> Select the message subset with key==value. 
  !>
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref index.f90 "index.f90"
  !>
  !> @param  indexid   id of an index created from a file. The index must have been created with the key in argument.
  !> @param key        key to be selected
  !> @param value      value of the key to select
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  interface grib_index_select
    module procedure grib_index_select_int, &
                     grib_index_select_string, &
                     grib_index_select_real8
  end interface grib_index_select
          
  !> Get the value for a key from a grib message.
  !>
  !> Given a \em gribid and \em key as input a \em value for the \em key is returned.
  !> In some cases the \em value can be an array rather than a scalar.
  !> As examples of array keys we have "values","pl", "pv" respectively the data values,
  !> the list of number of points for each latitude in a reduced grid and the list of
  !> vertical levels. In these cases the \em value array must be allocated by the caller
  !> and their required dimension can be obtained with \ref grib_get_size. \n
  !> The \em value can be integer(4), real(4), real(8), character.
  !> Although each key has its own native type, a key of type integer
  !> can be retrieved (with \ref grib_get) as real(4), real(8) or character.
  !> Analogous conversions are always provided when possible.
  !> Illegal conversions are real to integer and character to any other type.
  !>
  !> The \em gribid references to a grib message loaded in memory.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref get.f90 "get.f90", \ref print_data.f90 "print_data.f90"
  !>
  !> @see grib_new_from_file, grib_release, grib_set
  !>
  !>
  !> @param[in] gribid      id of the grib loaded in memory
  !> @param[in] key     key name
  !> @param[out] value       value can be a scalar or array of integer(4),real(4),real(8),character
  !> @param[out] status      GRIB_SUCCESS if OK, integer value on error
  interface grib_get
    module procedure grib_get_int, &
                     grib_get_real4, &
                     grib_get_real8, &
                     grib_get_string, &
                     grib_get_int_array, &
                     grib_get_real4_array, &
                     grib_get_real8_array
  end interface grib_get

  !> Get the size of an array key.
  !>
  !> To get the size of a key representing an array.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param key         name of the key
  !> @param size        size of the array key
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  interface grib_get_size
      module procedure  grib_get_size_int 
  end interface grib_get_size

  !> Create a new message in memory from an integer or character array containting the coded message.
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
  !> \b Examples: \ref copy_message.f90 "copy_message.f90"
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param message     array containing the coded message
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  interface grib_new_from_message 
      module procedure grib_new_from_message_int4 
      module procedure grib_new_from_message_char 
  end interface grib_new_from_message

  !> Reads a message in the buffer array from the file opened with grib_open_file.
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
  interface grib_read_from_file
      module procedure grib_read_from_file_int4
      module procedure grib_read_from_file_char
  end interface grib_read_from_file

  !> Reads nbytes bytes into the buffer from a file opened with grib_open_file.
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
  interface grib_read_bytes 
      module procedure grib_read_bytes_int4 
      module procedure grib_read_bytes_char 
      module procedure grib_read_bytes_real8
      module procedure grib_read_bytes_real4
  end interface grib_read_bytes

  !> Write nbytes bytes from the buffer in a file opened with grib_open_file.
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
  interface grib_write_bytes  
      module procedure grib_write_bytes_int4 
      module procedure grib_write_bytes_char
      module procedure grib_write_bytes_real8
      module procedure grib_write_bytes_real4
  end interface grib_write_bytes  

  !> Set the value for a key in a grib message.
  !>
  !> The given \em value is set for the \em key in the \em gribid message.
  !> In some cases the \em value can be an array rather than a scalar.
  !> As examples of array keys we have "values","pl", "pv" respectively the data values,
  !> the list of number of points for each latitude in a reduced grid and the list of
  !> vertical levels. In these cases the \em value array must be allocated by the caller
  !> and their required dimension can be obtained with \ref grib_get_size. \n
  !> The gribid references to a grib message loaded in memory.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref set.f90 "set.f90"
  !>
  !> @see grib_new_from_file, grib_release, grib_get
  !>
  !> @param[in] gribid      id of the grib loaded in memory
  !> @param[in] key          key name
  !> @param[out] value       value can be a scalar or array of integer(4),real(4),real(8)
  !> @param[out] status      GRIB_SUCCESS if OK, integer value on error
  interface grib_set
    module procedure grib_set_int, &
                     grib_set_real4, &
                     grib_set_real8, &
                     grib_set_string, &
                     grib_set_int_array, &
                     grib_set_real4_array, &
                     grib_set_real8_array
  end interface grib_set

  !> Get a value of specified index from an array key.
  !>
  !> Given a gribid and key name as input a value corresponding to the given index
  !> is returned. The index is zero based i.e. the first element has
  !> zero index, the second element index one and so on.
  !> If the parameter index is an array all the values correspondig to the indexes
  !> list is returned.
  !> The gribid references to a grib message loaded in memory.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref nearest.f90 "nearest.f90"
  !>
  !> @see grib_new_from_file, grib_release, grib_get
  !>
  !> @param[in] gribid      id of the grib loaded in memory
  !> @param[in] key         key name
  !> @param[in] index       index can be a scalar or array of integer(4)
  !> @param[out] value      value can be a scalar or array of integer(4),real(4),real(8)
  !> @param[out] status     GRIB_SUCCESS if OK, integer value on error
  interface grib_get_element
    module procedure grib_get_real4_element, &
                     grib_get_real8_element, &
                     grib_get_real4_elements, &
                     grib_get_real8_elements

  end interface grib_get_element

  !> Find the nearest point/points of a given latitude/longitude point.
  !>
  !> The value in the nearest point (or the four nearest points) is returned as well as the
  !> zero based index (which can be used in @ref grib_get_element)
  !> and its distance from the given point using the following
  !> formula (acos(sin(lat1)*sin(lat2)+cos(lat1)*cos(lat2)*cos(lon1-lon2))).
  !>
  !> If the is_lsm flag is .true. the input field gribid is considered as
  !> a land sea mask and the nearest land point is returned.\n
  !> The nearest land point among the four neighbours is:
  !> - the nearest point with land sea mask value >= 0.5
  !> - the nearest without any other condition if all the four have land sea mask value <0.5.
  !>
  !> Arrays (real(8)) of latitude/longitude can be provided to find with one call
  !> the values,indexes and distances for all the lat/lon points listed in the arrays.
  !>
  !> If a single latitude/longitude point is provided and outlat,outlon,value,distance,index
  !> are defined as arrays with four elements the lat/lon coordinates and values, distances
  !> and indexes of the four nearest points are returned.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref nearest.f90 "nearest.f90"
  !>
  !> @param[in] gribid     id of the grib loaded in memory
  !> @param[in] is_lsm      .true. if the nearest land point is required otherwise .false.
  !> @param[in] inlat       latitude of the point in degrees
  !> @param[in] inlon       longitudes of the point in degrees
  !> @param[out] outlat     latitude of the nearest point in degrees
  !> @param[out] outlon     longitude of the nearest point in degrees
  !> @param[out] distance   distance between the given point and its nearest
  !> @param[out] index      zero based index
  !> @param[out] value      value of the field in the nearest point
  !> @param[out] status     GRIB_SUCCESS if OK, integer value on error
  interface grib_find_nearest
    module procedure grib_find_nearest_single, &
                     grib_find_nearest_four_single, &
                     grib_find_nearest_multiple
  end interface grib_find_nearest

  !> Get latitude/longitude and data values.
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
  !> \b Examples: \ref get_data.f90 "get_data.f90"
  !>
  !> @param[in] gribid       id of the grib loaded in memory
  !> @param[out] lats        latitudes array with dimension "size"
  !> @param[out] lons        longitudes array with dimension "size"
  !> @param[out] values      data values array with dimension "size"
  !> @param[out] status      GRIB_SUCCESS if OK, integer value on error
  interface grib_get_data
    module procedure grib_get_data_real4, &
                     grib_get_data_real8
  end interface grib_get_data

