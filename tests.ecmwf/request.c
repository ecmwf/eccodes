#include "test.h"
/* #include <mars.h> */

char buffer[1024*1024*10];

char* check[] = {

	"class",
	"type",
	"stream",

	"expver",
	"date",
	"time",
	"levelist",
	"channel",

	"diagnostic",
	"iteration",
	"method",
	"system",
	"number",
	"frequency",
	"direction",
	"origin",
	"refdate",
	"reference",
	"fcmonth",
	"leadtime",
	"opttime",
	"ident",
	"instrument",
	"obstype",
	"levtype",

	"fcperiod",
	"param",
	"domain",
	"quantile",

	/* complicated: ocean data, def 4 */
	"product",
	"section",
	"latitude",
	"longitude",
	"range",
	"step",

};

char* warn[] = {
	"warn",
};

int main(int argc,char** argv)
{
	size_t length = 0;
	FILE *f;
	grib_handle  *g =  0;
	int e;
	grib_keyset* ks;
	char name[80];
	size_t len = sizeof(name);


	/* marsinit(&argc,argv,0,0,0); */

	f = fopen(argc > 1 ? argv[1]: "data" ,"r");
	if(!f)
	{
		perror(argc > 1 ? argv[1]: "data");
		exit(1);
	}

	length = sizeof(buffer);


	while((e = grib_read_any(0,f,buffer,&length)) == 0)
	{
		/* request *r = empty_request(0); */
		/* parameter *p; */
		/* grib_to_request(r,buffer,length); */
		/* print_all_requests(r); */

		g = grib_handle_new_from_message(0,buffer,length);

		printf("------------\n");
		ks  = grib_new_keyset(g,"mars.*");

		while(grib_keyset_next(ks,name,&len))
		{
			char c[1024];
			len = sizeof(c);


			if((e = grib_get_string(g,name,c,&len)))
				fprintf(stderr,"Cannot get %s as string %d (%s)\n",name,e,
						grib_get_error_message(e));

			printf("MARS -> %s ... %s\n",name,c);

			len = sizeof(name);
		}

		/* print_one_request(r); */

		grib_keyset_delete(ks);



#if 0
		p = r->params;
		while(p)
		{
			if(*p->name != '_')
			{
				char name[80];
				char value[80];
				char *q = name;
				char gvalue[80] = {0,};
				char nam[80] = {0,};
				size_t s = sizeof(gvalue);
				int e;
				int j;
				int nerr = 0;
				strcpy(name,p->name);
				while(*q) { if(isupper(*q)) *q = tolower(*q); q++; }

				strcpy(value,p->values->name);
				q = value;
				while(*q) { if(isupper(*q)) *q = tolower(*q); q++; }

				if(strcmp(name,"stream") == 0 && strcmp(value,"da") == 0)
					strcpy(value,"oper");

				if(strcmp(name,"stream") == 0 && strcmp(value,"ef") == 0)
					strcpy(value,"enfo");

				if(strcmp(name,"stream") == 0 && strcmp(value,"sd") == 0)
					strcpy(value,"supd");

				if(strcmp(name,"stream") == 0 && strcmp(value,"wv") == 0)
					strcpy(value,"wave");

				if(strcmp(name,"stream") == 0 && strcmp(value,"mo") == 0)
					strcpy(value,"mnth");

				if(strcmp(name,"stream") == 0 && strcmp(value,"ch") == 0)
					strcpy(value,"cher");

				if(strcmp(name,"stream") == 0 && strcmp(value,"sf") == 0)
					strcpy(value,"sens");

				if(strcmp(name,"stream") == 0 && strcmp(value,"tg") == 0)
					strcpy(value,"toga");

				if(strcmp(name,"stream") == 0 && strcmp(value,"mv") == 0)
					strcpy(value,"msdc");

				for(j = 0; j < NUMBER(check) ; j++)
				{

					if(EQ(name,check[j]))
					{
						sprintf(nam,"mars.%s",name);
						e = grib_get_string(g,nam,gvalue,&s); 
						printf("%s api=[%s] mars=[%s]\n",nam,gvalue,value);


						if(!EQ(value,gvalue))
						{
							fprintf(stderr,"mismatch %s api=[%s] mars=[%s]\n",name,gvalue,value);
							;
							nerr++;
						}
					}
				}

				if(nerr)
					exit(1);

				for(j = 0; j < NUMBER(warn) ; j++)
				{

					if(EQ(name,warn[j]))
					{
						sprintf(nam,"mars.%s",name);
						e = grib_get_string(g,nam,gvalue,&s); 
						printf("%s api=[%s] mars=[%s]\n",nam,gvalue,value);


						if(!EQ(value,gvalue))
						{
							fprintf(stderr,"mismatch %s api=[%s] mars=[%s]\n",name,gvalue,value);
						}
					}
				}


			}
			p = p->next;
		}


		free_all_requests(r);
#endif



		grib_handle_delete(g);


		length = sizeof(buffer);
	}

	if(e != -1)
	{
		fprintf(stderr,"readany error %d\n",e);
		exit(1);
	}

	exit(0);
}
