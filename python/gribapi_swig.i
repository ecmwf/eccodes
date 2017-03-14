%module gribapi_swig

%include "cpointer.i"
%include "cstring.i"
%include "typemaps.i"
%include "cdata.i"
%include "carrays.i"
%include "grib_errors.h"

%{
#define SWIG_FILE_WITH_INIT
#include "grib_interface.h"
%}

%include "numpy.i"
%init %{
import_array();
%}

/* Converts a PyFile instance to a stdio FILE* */
%typemap(in) FILE* {
    if ( PyFile_Check($input) ){
        $1 = PyFile_AsFile($input);
    } else {
        PyErr_SetString(PyExc_TypeError, "$1_name must be a file type.");
        return NULL;
    }
}

%pointer_class(int, intp);
%pointer_class(size_t, sizetp);
%pointer_class(long, longp);
%pointer_class(double, doublep);
%array_functions(double, doubleArray);
%array_functions(long, longArray);
%array_functions(int, intArray);
%array_functions(char*, stringArray);

// creation
int grib_c_new_from_file(FILE* f, int* INOUT, int headers_only);
int grib_c_new_any_from_file(FILE* f, int headers_only, int* INOUT);
int grib_c_new_bufr_from_file(FILE* f, int headers_only, int* INOUT);
int grib_c_new_gts_from_file(FILE* f, int headers_only, int* INOUT);
int grib_c_new_metar_from_file(FILE* f, int headers_only, int* INOUT);
int grib_c_iterator_new(int* INPUT, int* OUTPUT, int* INPUT);
int grib_c_keys_iterator_new(int* INPUT, int* OUTPUT, char* name_space);
int codes_c_bufr_keys_iterator_new(int* INPUT, int* OUTPUT);
int grib_c_grib_new_from_samples(int* INOUT, char* name);
int grib_c_bufr_new_from_samples(int* INOUT, char* name);
int grib_c_index_new_from_file(char* file, char* keys, int* OUTPUT);
int grib_c_index_add_file(int* INPUT, char* file);
int grib_c_new_from_index(int *INPUT, int *INOUT);
int grib_c_index_write(int* INPUT, char* file);
int grib_c_index_read(char* file, int* OUTPUT);
int grib_c_new_from_message(int *INOUT, char *binmsg, size_t *INPUT);
// ---

%apply int* INPUT { int* fid };
%apply int* INPUT { int* gid };
%apply int* INPUT { int* iterid };
%apply int* INPUT { int* iid };

// file operations
int grib_c_count_in_file(FILE* f,int* OUTPUT);
// ---

// grib handle operations
int grib_c_release(int* gid);
int grib_c_write(int* gid, FILE* f);
int grib_c_get_size_long(int* gid, char* key, long* OUTPUT);
int grib_c_get_string_length(int* gid, char* key, size_t* OUTPUT);
int grib_c_clone(int* gid,int* INOUT);
int grib_c_copy_namespace(int* gid, char* name, int* INPUT);
int grib_c_get_message_size(int* gid, size_t* OUTPUT);
int grib_c_get_message_offset(int* gid, size_t* OUTPUT);
int grib_c_get_native_type(int* gid, char* key, int* OUTPUT);
// ---

// multi support
int grib_c_multi_new(int* OUTPUT);
int grib_c_multi_support_on(void);
int grib_c_multi_write(int* gid, FILE* f);
int grib_c_multi_support_off(void);
int grib_c_multi_release(int* gid);
int grib_c_multi_append(int* INPUT, int* INPUT,int* INPUT);
// ---

// gribex support
int grib_c_gribex_mode_on(void);
int grib_c_gribex_mode_off(void);
// ---

// keys iterator
int grib_c_keys_iterator_next(int* iterid);
int codes_c_bufr_keys_iterator_next(int* iterid);
int grib_c_keys_iterator_delete(int* iterid);
int codes_c_bufr_keys_iterator_delete(int* iterid);
int grib_c_skip_computed(int* iterid);
int grib_c_skip_coded(int* iterid);
int grib_c_skip_edition_specific(int* iterid);
int grib_c_skip_duplicates(int* iterid);
int grib_c_skip_read_only(int* iterid);
int grib_c_skip_function(int* iterid);
int grib_c_keys_iterator_rewind(int* iterid);
int codes_c_bufr_keys_iterator_rewind(int* iterid);
int grib_c_bufr_copy_data(int* gid, int* INOUT);


%cstring_bounded_output(char* name, 1024);
int grib_c_keys_iterator_get_name(int* iterid, char* name, int len);
int codes_c_bufr_keys_iterator_get_name(int* iterid, char* name, int len);
// ---

// indexing routines
int grib_c_index_get_size_long(int* iid, char* key, long* OUTPUT);
int grib_c_index_get_long(int* iid, char* key, long* val, int* size);
int grib_c_index_get_real8(int* iid, char* key, double* val, int* size);
%cstring_bounded_output(char* index_string_output, 1024*1024);
int grib_c_index_get_string(int* iid, char* key, char* index_string_output, int* INPUT, int* INOUT);
int grib_c_index_select_long(int *iid, char *key, long *INPUT);
int grib_c_index_select_real8(int *iid, char *key, double *INPUT);
int grib_c_index_select_string(int *iid, char *key, char *val);
int grib_c_index_release(int* iid);
// ---

// values iterator
int grib_c_iterator_delete(int* iterid);
int grib_c_iterator_next(int* iterid, double* OUTPUT, double* OUTPUT, double* OUTPUT);
// ---

// getting/setting key values
%cstring_output_withsize(char* string_val, size_t* string_size)
int grib_c_get_string(int* gid, char* key, char* string_val, size_t* string_size);
int grib_c_get_string_array(int* gid, char* key, char** array_string_val, size_t* size);
int grib_c_set_string(int* gid, char* key, char* sval, int len2);
int grib_c_get_long(int* gid, char* key, long* OUTPUT);
int grib_c_set_long(int* gid, char* key, long* INPUT);
int grib_c_get_double(int* gid, char* key, double* OUTPUT);
int grib_c_set_double(int* gid, char* key, double* INPUT);
int grib_c_set_real8_array(int* gid, char* key, double* val, int* size);
int grib_c_get_real8_array(int* gid, char* key, double* val, int* size);
int grib_c_get_long_array(int* gid, char* key, long* val, int* size);
int grib_c_set_long_array(int* gid, char* key, long* val, int* size);
int grib_c_get_real8_element(int* gid, char* key, int* INPUT, double* OUTPUT);
int grib_c_get_real8_elements(int* gid, char* key, int* index, double* val, int* size);
int grib_c_set_missing(int* gid, char* key);
int grib_c_set_key_vals(int* gid, char* keyvals);
int grib_c_is_missing(int* gid, char* key, int* OUTPUT);
int grib_c_is_defined(int* gid, char* key, int* OUTPUT);

// http://www.swig.org/Doc1.3/Python.html
// This tells SWIG to treat char ** as a special case
%typemap(in) char ** {
  /* Check if is a list */
  if (PyList_Check($input)) {
    int size = PyList_Size($input);
    int i = 0;
    $1 = (char **) malloc((size+1)*sizeof(char *));
    for (i = 0; i < size; i++) {
      PyObject *o = PyList_GetItem($input,i);
      if (PyString_Check(o))
        $1[i] = PyString_AsString(PyList_GetItem($input,i));
      else {
        PyErr_SetString(PyExc_TypeError,"list must contain strings");
        free($1);
        return NULL;
      }
    }
    $1[i] = 0;  /* Last entry set to NULL */
  } else {
    PyErr_SetString(PyExc_TypeError,"not a list");
    return NULL;
  }
}
// This cleans up the char ** array we malloc'd before the function call
%typemap(freearg) char ** {
  free((char *) $1);
}
int grib_c_set_string_array(int *gid, char *key, const char** val);

// Numpy Support
%apply (double* IN_ARRAY1, int DIM1) {(double* dpin_val, int dpin_val_dim1)};
%apply (long* IN_ARRAY1, int DIM1) {(long* lpin_val, int lpin_val_dim1)};
%apply (int* IN_ARRAY1, int DIM1) {(int* ipin_index, int ipin_index_dim1)};
%apply (double* ARGOUT_ARRAY1, int DIM1) {(double* dpout_val, int dpout_val_dim1)};
%apply (long* ARGOUT_ARRAY1, int DIM1) {(long* lpout_val, int lpout_val_dim1)};

%inline %{
//void with_numpy() {
//    return;
//}
int grib_set_double_ndarray(int* gid, char* key, double* dpin_val, int dpin_val_dim1) {
    return grib_c_set_real8_array(gid,key,dpin_val,&dpin_val_dim1);
}
int grib_set_long_ndarray(int* gid, char* key, long* lpin_val, int lpin_val_dim1) {
    return grib_c_set_long_array(gid,key,lpin_val,&lpin_val_dim1);
}
int grib_get_double_ndarray(int* gid, char* key, double* dpout_val, int dpout_val_dim1) {
    return grib_c_get_real8_array(gid,key,dpout_val,&dpout_val_dim1);
}
int grib_get_long_ndarray(int* gid, char* key, long* lpout_val, int lpout_val_dim1) {
    return grib_c_get_long_array(gid,key,lpout_val,&lpout_val_dim1);
}
int grib_get_double_ndelements(int* gid, char* key, int* ipin_index, int ipin_index_dim1, double* dpout_val, int dpout_val_dim1) {
    return grib_c_get_real8_elements(gid,key,ipin_index,dpout_val,&dpout_val_dim1);
}
%}
%clear double* dpin_val, int dpin_val_dim1;
%clear long* lpin_val, int lpin_val_dim1;
%clear int* ipout_val, int ipout_val_dim1;
%clear double* dpout_val, int dpout_val_dim1;
%clear long* lpout_val, int lpout_val_dim1;

// ---

// nearest
int grib_c_find_nearest_single(int* gid, int* INPUT, double* INPUT, double* INPUT, double* OUTPUT, double* OUTPUT, double* OUTPUT, double* OUTPUT, int* OUTPUT);
int grib_c_find_nearest_four_single(int* gid, int* INPUT, double* INPUT, double* INPUT, double* outlats, double* outlons, double* values, double* distances, int* indexes);
// ---

/*
* Get the binary string message for a grib.
*
* Set the 3rd argument to nothing in 'cstring_output_allocate_size'.
* This is kind of difficult to explain, but, *msg will point directly to 
* the binary message data of the current grib (which is stored in 
* handle->buffer->data if I remember correctly) so freeing it will cause
* the binary message data in the grib_handle structure to be freed. This
* is a problem as grib_api does not know that, so it tries to free it
* itself (grib_release does that) resulting in a 'Segmentation fault'.
*/
%cstring_output_allocate_size(const void **binmsg, size_t *binmsglen,);
int grib_c_get_message(int *gid, const void **binmsg, size_t *binmsglen);
%clear const void **binmsg, size_t *binmsglen;

%clear int* fid;
%clear int* gid;
%clear int* iterid;

%cstring_bounded_output(char* error_message, 1024);
int grib_c_get_error_string(int* INPUT, char* error_message,  int len);

void no_fail_on_wrong_length(int flag);
long grib_c_get_api_version();
void grib_c_gts_header_on();
void grib_c_gts_header_off();
void grib_c_set_definitions_path(const char* path);
void grib_c_set_samples_path(const char* path);



