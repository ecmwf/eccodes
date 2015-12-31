/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * Description: concept index
 *
 */

#include "grib_api_internal.h"

static grib_concept_index_entry* index_entry_new(grib_context* c,grib_concept_index_keys* keys) {
	grib_concept_index_entry* entry=NULL;

	Assert(keys);

	if (!c) c=grib_context_get_default();

	entry=grib_context_malloc_clear(c,sizeof(grib_concept_index_entry));
	if (!entry) 
		grib_context_log(c,GRIB_LOG_FATAL,"grib_concept_index_entry unable to allocate");
	e=entry;

	while (keys && keys->name) {
		e->name=grib_context_strdup(c,keys->name);
		e->type=keys->type;
		e->next=grib_context_malloc_clear(c,sizeof(grib_concept_index_entry));
		if (!e->next) 
			grib_context_log(c,GRIB_LOG_FATAL,"grib_concept_index_entry unable to allocate");

		e=e->next;
		keys=keys->next;
	}

	return entry;

}

static void index_entry_delete(grib_context* c,grib_concept_index_entry* entry) {
	grib_concept_index_entry* e;
	while (entry) {
		e=entry;
		entry=entry->next;	
		grib_context_free(c,e->name);
		grib_context_free(c,e->value);
		grib_context_free(c,e);
	}
}

static int index_insert_entry(grib_concept_index* index,grib_concept_index_entry* entry,void* object) {
	
		int err=0;
		int found=0;
		grib_conditions_tree* cur=index->conditions;
		grib_conditions_tree* prev=index->conditions;
		grib_concept_index_keys* keys=index->keys;

		while (keys->name) {

			if (!cur) {
				cur=grib_context_malloc_clear_persistent(index->context,sizeof(grib_conditions_tree));
				if (!cur)
					grib_context_log(index->context,GRIB_LOG_FATAL,"index_insert_entry unable to allocate");
				prev->next=cur;
			}
			value = entry->value ? entry->value : "*";
			while (cur && (!cur->value || (found=!strcmp(cur->value,value))==0) ) cur=cur->next; 

			if (!found) {
				cur->next=grib_context_malloc_clear_persistent(index->context,sizeof(grib_conditions_tree));
				if (!cur->next) 
					grib_context_log(index->context,GRIB_LOG_FATAL,"index_insert_entry unable to allocate");
				cur=cur->next;
			}

			cur->value=grib_context_strdup(index->context,value);
			entry=entry->next;
			keys=keys->next;
			prev=cur;
			cur=cur->next_key;
		}

		while (cur) {
			prev=cur;
			cur=cur->next_key;
		}

		prev->object=object;

		return err;
}

static void index_add_conditions(grib_concept_index* index,grib_concept_condition* condition) {
	grib_concept_condition* c=condition;
	char s[512]={0,};
	grib_concept_index_entry* e;;
	grib_concept_index_entry* entry=index_entry_new(index->context,index->keys);;

	while (c) {
		size_t size=512;
		int type;
		e=entry;
		type = grib_expression_native_type(0,c->expression);
		switch(type)
		{
			case GRIB_TYPE_LONG:
			  grib_expression_evaluate_long(0,c->expression,&lres);
			  sprintf(s,"%ld",lres);
			  break;

			case GRIB_TYPE_DOUBLE:
			  grib_expression_evaluate_double(0,c->expression,&dres);
			  sprintf(s,"%g",dres);
			  break;

			case GRIB_TYPE_STRING:
			  grib_expression_evaluate_string(0,c->expression,s,&size,&err);
			  break;

			default:
			  Assert(0);
			  break;
		}

		while (e->name && strcmp(e->name,c->name)) 
			e=e->next;

		e->type=type;
		e->value=grib_context_strdup(index->context,s);
		if (!e->name) {
			e->name=grib_context_strdup(index->context,c->name);
			e->next=grib_context_malloc_clear_persistent(index->context,sizeof(grib_concept_index_entry));
			if (!e->next) 
				grib_context_log(index->context,GRIB_LOG_FATAL,"index_add_conditions unable to allocate");
		} 

		c=c->next;
	}

	index_insert_entry(index,entry,condition->name);

	index_entry_delete(index->context,entry);
}

grib_concept_index* grib_concept_index_new_from_concept(grib_context* c,grib_concept_value* concept,int *err) {
	grib_concept_index* index;

	if (!c) c=grib_context_get_default();

	index=grib_context_malloc_clear_persistent(c,sizeof(grib_concept_index));
	index->keys=grib_context_malloc_clear_persistent(c,sizeof(grib_concept_index_key));
	index->conditions=grib_context_malloc_clear_persistent(c,sizeof(grib_conditions_tree));
	index->conditions=grib_context_malloc_clear_persistent(c,sizeof(grib_conditions_tree));
	index->context=c;

	while (concept) {
		index_add_conditions(index,concept->conditions,err);
		concept=concept->next;
	}

	return index;
}
