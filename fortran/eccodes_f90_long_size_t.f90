! Copyright 2005-2016 ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
! virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.

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
  interface codes_read_from_file
      module procedure codes_read_from_file_int4
      module procedure codes_read_from_file_int4_size_t
      module procedure codes_read_from_file_char
      module procedure codes_read_from_file_char_size_t
  end interface codes_read_from_file

  !> Reads nbytes bytes into the buffer from a file opened with codes_open_file.
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
  interface codes_read_bytes
      module procedure codes_read_bytes_int4
      module procedure codes_read_bytes_int4_size_t
      module procedure codes_read_bytes_char
      module procedure codes_read_bytes_char_size_t
      module procedure codes_read_bytes_real8
      module procedure codes_read_bytes_real8_size_t
      module procedure codes_read_bytes_real4
      module procedure codes_read_bytes_real4_size_t
  end interface codes_read_bytes

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
  interface codes_write_bytes
      module procedure codes_write_bytes_int4
      module procedure codes_write_bytes_int4_size_t
      module procedure codes_write_bytes_char
      module procedure codes_write_bytes_char_size_t
      module procedure codes_write_bytes_real8
      module procedure codes_write_bytes_real8_size_t
      module procedure codes_write_bytes_real4
      module procedure codes_write_bytes_real4_size_t
  end interface codes_write_bytes

  !> Get the size of a coded message.
  !>
  !> In case of error, if the status parameter (optional) is not given, the program will
  !> exit with an error message.\n Otherwise the error message can be
  !> gathered with @ref codes_get_error_string.
  !>
  !> @param gribid      id of the grib loaded in memory
  !> @param nbytes      size in bytes of the message
  !> @param status      CODES_SUCCESS if OK, integer value on error
  interface codes_get_message_size
    module procedure codes_get_message_size_int
    module procedure codes_get_message_size_size_t
  end interface codes_get_message_size
