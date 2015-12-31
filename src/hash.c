/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include <stdio.h>
#include "mars.h"

#define SIZE 	((int)4097)

typedef struct hashrec {
	struct hashrec  *next;
	char   *name;
	int    cnt;
} hashrec;

static hashrec *table[SIZE] = { 
	NULL, };


static int hash(const char *name)
{
	int n = 0;

	while(*name)
		n +=  (*name++ - 'A') + (n << 5);

#ifdef CRAY
	if(n<0) n = -n;
#else
	if(n<0)  {
		int m = -n/SIZE;
		n += (m+1)*SIZE;
	}
#endif
	return n % SIZE;
}

void hash_stat()
{
	int i;
	int a=0,b=0,c=0;
	hashrec *h;

	printf("Table size is %d. ",SIZE);

	for(i=0;i<SIZE;i++)
		if(h = table[i])
		{
			a++;
			while(h) { b++; c += h->cnt; h = h->next; };
		}

	printf("Used: %d, Total: %d, Strings: %d\n",a,b,c);
}

char *strcache(const char *name)
{
	hashrec *h;
	int      n;

	if(name == NULL) return NULL;

	n = hash(name);
	h = table[n];

	while(h)
	{
		if(EQ(h->name,name))
		{
			h->cnt++;
			return h->name;
		}
		h = h->next;
	}

	h        = NEW(hashrec);
	h->name  = NEW_STRING(name);
	h->next  = table[n];
	h->cnt   = 1;
	table[n] = h;

	return h->name;
}

void strfree(char *name)
{

	hashrec *h;
	hashrec *p = NULL;
	int      n;

	if(name == NULL) return;

	n = hash(name);
	h = table[n];

	while(h)
	{
		if(h->name == name)
		{
			h->cnt--;
			break;
		}
		p = h;
		h = h->next;
	}

	if(h == NULL)
	{
		marslog(LOG_WARN,"%s was not in hash table",name);
		abort();
		printf("n=%d\n",n);
		h = table[n];
		while(h)
		{
			printf("%s %d\n",h->name,h->cnt);
			h = h->next;
		}
		return;
	}

	if(h->cnt == 0)
	{
		if(p) p->next = h->next;
		else table[n] = h->next;
		FREE(h->name);
		FREE(h);
	}
}


