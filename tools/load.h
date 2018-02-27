/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

extern char *prog;

int load_file(const char*,const char*);
int dump_file(const char*,const char*);

void load_finish();
void load_long(const char* name,long value) ;
void load_missing(const char* name);
void load_string(const char* name,const char* value);
void load_double(const char* name,double value);
void load_start_array();
void load_end_array(const char* name);
void load_double_value(double value);
void load_long_value(long value);
