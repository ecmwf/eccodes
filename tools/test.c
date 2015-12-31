/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"



int main(int argc, char *argv[])
{
	grib_context *c = grib_context_get_default();
	
	grib_trie *t = grib_trie_new(c);
	
	grib_trie_insert(t,"hello","w");
	grib_trie_insert(t,"hola","e");
	
	printf("%s\n",(char*)grib_trie_get(t,"hello"));
	printf("%s\n",(char*)grib_trie_get(t,"hela"));
	printf("%s\n",(char*)grib_trie_get(t,"hola"));

	grib_trie_remove(t,"hela");
	grib_trie_remove(t,"hola");

	printf("%s\n",(char*)grib_trie_get(t,"hello"));
	printf("%s\n",(char*)grib_trie_get(t,"hola"));

	return 0;
}
