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

typedef struct grib_action_if {
    grib_action          act;  
/* Members defined in section */
/* Members defined in if */
	grib_expression *expression;
	grib_action     *block_true;
	grib_action     *block_false;
} grib_action_if;

typedef struct grib_action_list {
    grib_action          act;  
/* Members defined in section */
/* Members defined in list */
	grib_expression *expression;
	grib_action     *block_list;
} grib_action_list;

void print_names(grib_action* a) {
	while (a) {
		if (a->op && !strcmp(a->op,"label") ) { a=a->next; continue; }
		if (!strcmp(a->cclass->name,"action_class_noop") || 
			!strcmp(a->cclass->name,"action_class_when") ) 
			{ a=a->next; continue; }

		if (!strcmp(a->cclass->name,"action_class_list")) {
			grib_action_list* al=(grib_action_list*)a;
			printf("%s\n",a->name);
			print_names(al->block_list);
		} else if (!strcmp(a->cclass->name,"action_class_if")) {
			grib_action_if* ai=(grib_action_if*)a;
			print_names(ai->block_false);
			print_names(ai->block_true);
		} else {
			/* printf("%s (%s %s)\n",a->name,a->op,a->cclass->name); */
			printf("%s\n",a->name);
		}

		a=a->next;
	}
}

int main(int argc, char *argv[])
{
  int i = 0;
  int fail = 0;

  grib_context* c = grib_context_get_default();
  grib_action* a=NULL;

  for(i = 1; i < argc; i++){
    /* printf("%s ... \n",argv[i]);fflush(stdout); */
    if(!(a=grib_parse_file(c,argv[i])))
    {
      fail++;
      printf("FAILED\n");
      exit(1);
    } else print_names(a);
  }

  return fail;
}
