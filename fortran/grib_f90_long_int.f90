! Copyright 2005-2018 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
! 
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.


  !> Get the distinct values of the key in argument contained in the index. The key must belong to the index.
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
  interface grib_index_get
    module procedure grib_index_get_int, &
                     grib_index_get_long, &
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
  !> \b Examples: \ref grib_index.f90 "grib_index.f90"
  !>
  !> @param  indexid     id of an index created from a file. The index must have been created with the key in argument.
  !> @param key        key for which the number of values is computed
  !> @param size       number of distinct values of the key in the index
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  interface grib_index_get_size
    module procedure grib_index_get_size_int, &
                     grib_index_get_size_long
  end interface grib_index_get_size
                                 
  !> Select the message subset with key==value. 
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
  interface grib_index_select
    module procedure grib_index_select_int, &
                     grib_index_select_long, &
                     grib_index_select_string, &
                     grib_index_select_real8
  end interface grib_index_select
          
  !> Get the value for a key from a message.
  !>
  !> Given a \em msgid and \em key as input a \em value for the \em key is returned.
  !> In some cases the \em value can be an array rather than a scalar.
  !> As examples of GRIB array keys we have "values" (the data values),
  !> "pl" (the list of number of points for each latitude in a reduced grid) and
  !> "pv" (the list of vertical levels).
  !> In these cases the \em value array must be allocated by the caller
  !> and their required dimension can be obtained with \ref grib_get_size. \n
  !> The \em value can be integer(4), real(4), real(8), character.
  !> Although each key has its own native type, a key of type integer
  !> can be retrieved (with \ref grib_get) as real(4), real(8) or character.
  !> Analogous conversions are always provided when possible.
  !> Illegal conversions are real to integer and character to any other type.
  !>
  !> The \em msgid references to a message loaded in memory.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref grib_get_keys.f90 "grib_get_keys.f90", \ref grib_print_data.f90 "grib_print_data.f90"
  !>
  !> @see grib_new_from_file, grib_release, grib_set
  !>
  !>
  !> @param[in] msgid     id of the message loaded in memory
  !> @param[in] key       key name
  !> @param[out] value    value can be a scalar or array of integer(4),real(4),real(8),character.
  !> @param[out] status   GRIB_SUCCESS if OK, integer value on error
  interface grib_get
    module procedure grib_get_int, &
                     grib_get_long, &
                     grib_get_real4, &
                     grib_get_real8, &
                     grib_get_string, &
                     grib_get_int_array, &
                     grib_get_byte_array, &
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
  !> @param msgid       id of the message loaded in memory
  !> @param key         name of the key
  !> @param size        size of the array key
  !> @param status      GRIB_SUCCESS if OK, integer value on error
  interface grib_get_size
      module procedure  grib_get_size_int, &
                        grib_get_size_long 
  end interface grib_get_size

  !> Set the value for a key in a message.
  !>
  !> The given \em value is set for the \em key in the \em msgid message.
  !> In some cases the \em value can be an array rather than a scalar.
  !> As examples of GRIB array keys we have "values" (the data values),
  !> "pl" (the list of number of points for each latitude in a reduced grid) and
  !> "pv" (the list of vertical levels).
  !> In these cases the \em value array must be allocated by the caller
  !> and their required dimension can be obtained with \ref grib_get_size. \n
  !> The msgid references to a message loaded in memory.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref grib_get_error_string.
  !>
  !> \b Examples: \ref grib_set_keys.f90 "grib_set_keys.f90"
  !>
  !> @see grib_new_from_file, grib_release, grib_get
  !>
  !> @param[in] msgid       id of the message loaded in memory
  !> @param[in] key         key name
  !> @param[out] value      value can be a scalar or array of integer(4),real(4),real(8)
  !> @param[out] status     GRIB_SUCCESS if OK, integer value on error
  interface grib_set
    module procedure grib_set_int, &
                     grib_set_long, &
                     grib_set_real4, &
                     grib_set_real8, &
                     grib_set_string, &
                     grib_set_int_array, &
                     grib_set_long_array, &
                     grib_set_byte_array, &
                     grib_set_real4_array, &
                     grib_set_real8_array
  end interface grib_set

  interface grib_set_force
    module procedure grib_set_force_real4_array, &
                     grib_set_force_real8_array
  end interface grib_set_force
