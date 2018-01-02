/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"
#include "grib_fortran_prototypes.h"

int grib_read_file_(int* fid, char* buffer, size_t* nbytes) {
  return grib_f_read_file_(fid,buffer,nbytes);
}

int grib_read_file__(int* fid, char* buffer, size_t* nbytes) {
  return grib_f_read_file_(fid,buffer,nbytes);
}

int grib_open_file_(int* fid, char* name , char* op,  int lname, int lop){
  return grib_f_open_file_(       fid,       name ,       op,     lname,      lop);
}

int grib_open_file__(int* fid, char* name , char* op,  int lname, int lop){
  return grib_f_open_file_(       fid,       name ,       op,     lname,      lop);
}

int grib_close_file_(int* fid){
  return grib_f_close_file_(fid);
}

int grib_close_file__(int* fid){
  return grib_f_close_file_(fid);
}

int grib_multi_support_on_(){
  return grib_f_multi_support_on_();
}

int grib_multi_support_on__(){
  return grib_f_multi_support_on_();
}

int grib_multi_support_off_(){
  return grib_f_multi_support_off_();
}

int grib_multi_support_off__(){
  return grib_f_multi_support_off_();
}

int grib_iterator_new_(int* gid,int* iterid,int* mode) {
  return grib_f_iterator_new_(gid,iterid,mode);
}

int grib_iterator_new__(int* gid,int* iterid,int* mode) {
  return grib_f_iterator_new_(gid,iterid,mode);
}

int grib_iterator_next_(int* iterid,double* lat,double* lon,double* value) {
  return grib_f_iterator_next_(iterid,lat,lon,value);
}

int grib_iterator_next__(int* iterid,double* lat,double* lon,double* value) {
  return grib_f_iterator_next_(iterid,lat,lon,value);
}

int grib_iterator_delete_(int* iterid) {
  return grib_f_iterator_delete_(iterid);
}

int grib_iterator_delete__(int* iterid) {
  return grib_f_iterator_delete_(iterid);
}

int grib_keys_iterator_new__(int* gid,int* iterid,char* name_space,int len) {
  return grib_f_keys_iterator_new_(gid,iterid,name_space,len);
}

int grib_keys_iterator_new_(int* gid,int* iterid,char* name_space,int len) {
  return grib_f_keys_iterator_new_(gid,iterid,name_space,len);
}

int grib_keys_iterator_next_(int* iterid) {
  return grib_f_keys_iterator_next_(iterid);
}

int grib_keys_iterator_next__(int* iterid) {
  return grib_f_keys_iterator_next_(iterid);
}

int grib_keys_iterator_delete_(int* iterid) {
  return grib_f_keys_iterator_delete_(iterid);
}

int grib_keys_iterator_delete__(int* iterid) {
  return grib_f_keys_iterator_delete_(iterid);
}

int grib_gribex_mode_on_() {
   grib_gribex_mode_on(0);
   return GRIB_SUCCESS;
}

int grib_gribex_mode_on__() {
   grib_gribex_mode_on(0);
   return GRIB_SUCCESS;
}

int grib_gribex_mode_off_() {
   grib_gribex_mode_off(0);
   return GRIB_SUCCESS;
}

int grib_gribex_mode_off__() {
   grib_gribex_mode_off(0);
   return GRIB_SUCCESS;
}

int grib_skip_computed_(int* iterid) {
  return grib_skip_computed_(iterid);
}

int grib_skip_computed__(int* iterid) {
  return grib_skip_computed_(iterid);
}

int grib_skip_coded_(int* iterid) {
  return grib_f_skip_coded_(iterid);
}

int grib_skip_coded__(int* iterid) {
  return grib_f_skip_coded_(iterid);
}

int grib_skip_edition_specific_(int* iterid) {
  return grib_f_skip_edition_specific_(iterid);
}

int grib_skip_edition_specific__(int* iterid) {
  return grib_f_skip_edition_specific_(iterid);
}

int grib_skip_duplicates_(int* iterid) {
  return grib_f_skip_duplicates_(iterid);
}

int grib_skip_duplicates__(int* iterid) {
  return grib_f_skip_duplicates_(iterid);
}

int grib_skip_read_only_(int* iterid) {
  return grib_f_skip_read_only_(iterid);
}

int grib_skip_read_only__(int* iterid) {
  return grib_f_skip_read_only_(iterid);
}

int grib_skip_function_(int* iterid) {
  return grib_f_skip_function_(iterid);
}

int grib_skip_function__(int* iterid) {
  return grib_f_skip_function_(iterid);
}

int grib_keys_iterator_get_name_(int* kiter,char* name,int len) {
  return grib_f_keys_iterator_get_name_(kiter,name,len);
}

int grib_keys_iterator_get_name__(int* kiter,char* name,int len) {
  return grib_f_keys_iterator_get_name_(kiter,name,len);
}

int grib_keys_iterator_rewind_(int* kiter) {
   return grib_f_keys_iterator_rewind_(kiter);
}

int grib_keys_iterator_rewind__(int* kiter) {
   return grib_f_keys_iterator_rewind_(kiter);
}

int grib_new_from_message_(int* gid, void* buffer , size_t* bufsize){
  return grib_f_new_from_message_(gid,  buffer ,  bufsize);
}

int grib_new_from_message__(int* gid, void* buffer , size_t* bufsize){
  return grib_f_new_from_message_(gid,  buffer ,  bufsize);
}

int grib_new_from_message_copy_(int* gid, void* buffer , size_t* bufsize){
  return grib_f_new_from_message_copy_(gid,  buffer ,  bufsize);
}

int grib_new_from_message_copy__(int* gid, void* buffer , size_t* bufsize){
  return grib_f_new_from_message_copy_(gid,  buffer ,  bufsize);
}

int grib_new_from_samples_(int* gid, char* name , int lname){
  return  grib_f_new_from_samples_( gid,  name ,  lname);
}

int grib_new_from_samples__(int* gid, char* name , int lname){
  return  grib_f_new_from_samples_( gid,  name ,  lname);
}

int grib_clone_(int* gidsrc,int* giddest){
  return grib_f_clone_(gidsrc, giddest);
}

int grib_clone__(int* gidsrc,int* giddest){
  return grib_f_clone_(gidsrc, giddest);
}

int grib_new_from_file_(int* fid, int* gid){
  return grib_f_new_from_file_( fid, gid);
}

int grib_new_from_file__(int* fid, int* gid){
  return grib_f_new_from_file_( fid, gid);
}

int grib_release_(int* hid){
  return grib_f_release_( hid);
}

int grib_release__(int* hid){
  return grib_f_release_( hid);
}

int grib_dump_(int* gid){
  return grib_f_dump_( gid);
}

int grib_dump__(int* gid){
  return grib_f_dump_( gid);
}

int grib_get_error_string_(int* err, char* buf,  int len){
  return grib_f_get_error_string_(err,buf,len);
}

int grib_get_error_string__(int* err, char* buf,  int len){
  return grib_f_get_error_string_(err,buf,len);
}

int grib_get_size_(int* gid, char* key, int* val,  int len){
  return grib_f_get_size_int_( gid,  key,  val,  len);
}

int grib_get_size__(int* gid, char* key, int* val,  int len){
  return grib_f_get_size_int_( gid,  key,  val,  len);
}

int grib_get_int_(int* gid, char* key, int* val,  int len){
  return grib_f_get_int_( gid,  key,  val,   len);
}

int grib_get_int__(int* gid, char* key, int* val,  int len){
  return grib_f_get_int_( gid,  key,  val,   len);
}

int grib_get_int_array_(int* gid, char* key, int*val, int* size, int len){
  return grib_f_get_int_array_( gid,  key, val,  size,  len);
}

int grib_get_int_array__(int* gid, char* key, int*val, int* size, int len){
  return grib_f_get_int_array_( gid,  key, val,  size,  len);
}

int grib_set_int_array_(int* gid, char* key, int* val, int* size,  int len){
  return grib_f_set_int_array_( gid,  key,  val,  size,   len);
}

int grib_set_int_array__(int* gid, char* key, int* val, int* size,  int len){
  return grib_f_set_int_array_( gid,  key,  val,  size,   len);
}

int grib_set_int_(int* gid, char* key, int* val, int len){
  return  grib_f_set_int_( gid,  key,  val, len);
}

int grib_set_int__(int* gid, char* key, int* val, int len){
  return  grib_f_set_int_( gid,  key,  val, len);
}

int grib_set_missing_(int* gid, char* key,  int len){
  return grib_f_set_missing_( gid,  key,  len);
}

int grib_set_missing__(int* gid, char* key,  int len){
  return grib_f_set_missing_( gid,  key,  len);
}

int grib_set_real4_(int* gid, char* key, float* val,  int len){
  return grib_f_set_real4_( gid,  key,  val,   len);
}

int grib_set_real4__(int* gid, char* key, float* val,  int len){
  return grib_f_set_real4_( gid,  key,  val,   len);
}

int grib_get_real4_(int* gid, char* key, float* val,  int len){
  return grib_f_get_real4_( gid,  key,  val,  len);
}

int grib_get_real4__(int* gid, char* key, float* val,  int len){
  return grib_f_get_real4_( gid,  key,  val,  len);
}

int grib_get_real4_array_(int* gid, char* key, float* val, int* size, int len){
  return grib_f_get_real4_array_( gid,  key, val,  size,  len);
}

int grib_get_real4_element_(int* gid, char* key, int* index,float* val,  int len){
  return grib_f_get_real4_element_( gid,  key,  index,val,  len);
}

int grib_get_real4_element__(int* gid, char* key,int* index, float* val,  int len){
  return grib_f_get_real4_element_( gid,  key, index, val, len);
}

int grib_get_real4_elements__(int* gid, char* key,int* index, float* val,int* len,int size){
  return grib_f_get_real4_elements_( gid,  key, index, val,  len,size);
}

int grib_get_real4_elements_(int* gid, char* key,int* index, float* val,int* len,int size){
  return grib_f_get_real4_elements_( gid,  key, index, val,  len,size);
}

int grib_get_real4_array__(int* gid, char* key, float* val, int* size, int len){
  return grib_f_get_real4_array_( gid,  key, val,  size,  len);
}

int grib_set_real4_array_(int* gid, char* key, float*val, int* size, int len){
  return grib_f_set_real4_array_( gid,  key, val,  size, len);
}

int grib_set_real4_array__(int* gid, char* key, float*val, int* size, int len){
  return grib_f_set_real4_array_( gid,  key, val,  size, len);
}

int grib_set_real8_(int* gid, char* key, double* val, int len){
  Assert(sizeof(double) == 8);
  return  grib_f_set_real8_( gid,  key,  val, len);
}

int grib_set_real8__(int* gid, char* key, double* val, int len){
  Assert(sizeof(double) == 8);
  return  grib_f_set_real8_( gid,  key,  val, len);
}

int grib_get_real8_(int* gid, char* key, double* val, int len){
  Assert(sizeof(double) == 8);
  return grib_f_get_real8_( gid,  key,  val, len);
}

int grib_get_real8__(int* gid, char* key, double* val, int len){
  Assert(sizeof(double) == 8);
  return grib_f_get_real8_( gid,  key,  val, len);
}

int grib_get_real8_element_(int* gid, char* key,int* index, double* val, int len){
  Assert(sizeof(double) == 8);
  return grib_f_get_real8_element_( gid,  key,  index,val, len);
}

int grib_get_real8_element__(int* gid, char* key,int* index, double* val, int len){
  Assert(sizeof(double) == 8);
  return grib_f_get_real8_element_( gid,  key,  index,val, len);
}

int grib_get_real8_elements_(int* gid, char* key, int* index,double* val, int* len,int size){
  return grib_f_get_real8_elements_( gid,  key, index, val,len,size);
}

int grib_get_real8_elements__(int* gid, char* key, int* index,double* val, int* len,int size){
  return grib_f_get_real8_elements_( gid,  key, index, val,len,size);
}


int grib_get_real8_array_(int* gid, char* key, double*val, int* size, int len){
  Assert(sizeof(double) == 8);
  return grib_f_get_real8_array_( gid,  key, val,  size,  len);
}

int grib_get_real8_array__(int* gid, char* key, double*val, int* size, int len){
  Assert(sizeof(double) == 8);
  return grib_f_get_real8_array_( gid,  key, val,  size,  len);
}

int grib_set_real8_array_(int* gid, char* key, double *val, int* size, int len){
  Assert(sizeof(double) == 8);
  return grib_f_set_real8_array_( gid,  key, val,  size, len);
}

int grib_set_real8_array__(int* gid, char* key, double *val, int* size, int len){
  Assert(sizeof(double) == 8);
  return grib_f_set_real8_array_( gid,  key, val,  size, len);
}

int grib_get_string_(int* gid, char* key, char* val,  int len, int len2){
  return  grib_f_get_string_( gid,  key,  val,   len,  len2);
}

int grib_get_string__(int* gid, char* key, char* val,  int len, int len2){
  return  grib_f_get_string_( gid,  key,  val,   len,  len2);
}

int grib_set_string_(int* gid, char* key, char* val, int len, int len2){
  return  grib_f_set_string_( gid,  key,  val,   len, len2);
}

int grib_set_string__(int* gid, char* key, char* val, int len, int len2){
  return  grib_f_set_string_( gid,  key,  val,   len, len2);
}

int grib_get_message_size_(int* gid, size_t *len){
  return  grib_f_get_message_size_( gid, len);
}

int grib_get_message_size__(int* gid, size_t *len){
  return  grib_f_get_message_size_( gid, len);
}

void grib_check_(int* err){
  grib_f_check_(err,"","",0,0);
}

void grib_check__(int* err){
  grib_f_check_(err,"","",0,0);
}

int grib_write_(int* gid, int* fid) {
  return grib_f_write_(gid,fid);
}

int grib_write__(int* gid, int* fid) {
  return grib_f_write_(gid,fid);
}

int grib_multi_write_(int* gid, int* fid) {
  return grib_f_multi_write_(gid,fid);
}

int grib_multi_write__(int* gid, int* fid) {
  return grib_f_multi_write_(gid,fid);
}

int grib_multi_append_(int* ingid, int* sec,int* mgid) {
  return grib_f_multi_append_(ingid,sec,mgid);
}

int grib_multi_append__(int* ingid, int* sec,int* mgid) {
  return grib_f_multi_append_(ingid,sec,mgid);
}

int grib_find_nearest_multiple_(int* gid,int* is_lsm,
    double* inlats,double* inlons,
    double* outlats,double* outlons,
    double* values,double* distances,
    int* indexes, int* npoints) {

  return grib_f_find_nearest_multiple_(gid,is_lsm,
    inlats,inlons,outlats,outlons,values,
    distances,indexes,npoints);
}
int grib_find_nearest_multiple__(int* gid,int* is_lsm,
    double* inlats,double* inlons,
    double* outlats,double* outlons,
    double* values,double* distances,
    int* indexes, int* npoints) {

  return grib_f_find_nearest_multiple_(gid,is_lsm,
    inlats,inlons,outlats,outlons,values,
    distances,indexes,npoints);
}

int grib_find_nearest_single_(int* gid,int* is_lsm,
    double* inlats,double* inlons,
    double* outlats,double* outlons,
    double* values,double* distances,
    int* indexes) {

  return grib_f_find_nearest_single_(gid,is_lsm,
    inlats,inlons,outlats,outlons,values,
    distances,indexes);
}
int grib_find_nearest_single__(int* gid,int* is_lsm,
    double* inlats,double* inlons,
    double* outlats,double* outlons,
    double* values,double* distances,
    int* indexes) {

  return grib_f_find_nearest_single_(gid,is_lsm,
    inlats,inlons,outlats,outlons,values,
    distances,indexes);
}

int grib_copy_message_(int* gid, void* mess,size_t* len){
 return grib_f_copy_message_(gid, mess,len);
}
int grib_copy_message__(int* gid, void* mess,size_t* len){
 return grib_f_copy_message_(gid, mess,len);
}


