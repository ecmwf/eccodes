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

grib_rule_entry *grib_new_rule_entry(grib_context* c,const char* name,grib_expression* expression)
{
	grib_rule_entry* e = (grib_rule_entry*)grib_context_malloc_clear_persistent(c,sizeof(grib_rule_entry));
	e->name            = grib_context_strdup_persistent(c,name);
	e->value           = expression;
	return e;
}

#if 0
/* new GCC compiler v4.5.0 complains function is defined but not used*/
static void grib_rule_entry_delete(grib_context* c,grib_rule_entry* e)
{
	if(e)
	{
		grib_context_free_persistent(c,e->name);
		grib_rule_entry_delete(c,e->next);
		grib_context_free_persistent(c,e);

	}
}
#endif

grib_rule* grib_new_rule(grib_context* c,grib_expression* condition,grib_rule_entry* entries)
{
	grib_rule* r = (grib_rule*)grib_context_malloc_clear_persistent(c,sizeof(grib_rule));
	r->condition = condition;
	r->entries   = entries;
	return r;
}
