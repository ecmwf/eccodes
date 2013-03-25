#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"

#include <grib_api.h>
#include <stdlib.h>
/*
http://world.std.com/~swmcd/steven/perl/pm/xs/modules/Set/Bit/
*/

static int _fail_on_error = 0;

static int error(int e)
{
	if(e && _fail_on_error) croak(grib_get_error_message(e));
	return e;
}

static int fail(int e)
{
	if(e) croak(grib_get_error_message(e));
	return e;
}

static SV* get_long(grib_handle* h,const char* what)
{
	long value;

	if(error(grib_get_long(h,what,&value)) == 0)
		return sv_2mortal(newSViv(value));

	return &PL_sv_undef;
}

static SV* get_double(grib_handle* h,const char* what)
{
	double value;

	if(error(grib_get_double(h,what,&value)) == 0)
		return sv_2mortal(newSVnv(value));

	return &PL_sv_undef;
}

static SV* get_string(grib_handle* h,const char* what)
{
	char buf[1024];
	size_t size = sizeof(buf);

	if(error(grib_get_string(h,what,buf,&size)) == 0)
	{
		while(size>0 && buf[size-1] == 0) size--;
		return sv_2mortal(newSVpv(buf,size));
	}

	return &PL_sv_undef;
}

static SV* get_bytes(grib_handle* h,const char* what)
{
	size_t size;
	SV*   sv;
	char *buffer = 0;

	if(error(grib_get_size(h,what,&size)) != 0)
		return &PL_sv_undef;

	sv = newSVpv("",0);
	buffer = SvGROW(sv,size);

	if(error(grib_get_bytes(h,what,buffer,&size)) == 0)
	{
		SvCUR_set(sv, size);
		SvPOK_only(sv);
		return sv_2mortal(sv);
	}
	else
	{
		/* Memory leek ... */
		return &PL_sv_undef;
	}

}

static SV* get_double_array(grib_handle* h,const char* what)
{

	size_t size = 0;
	double *values = 0;
	SV* result  = &PL_sv_undef;


	if(error(grib_get_size(h,what,&size)) != 0)
		return &PL_sv_undef;

	Newz(0,values,size,double);

	if(error(grib_get_double_array(h,what,values,&size)) == 0)
	{
			AV* av      = newAV();
			int i;

			for(i = 0; i < size; i++)
				av_push(av,newSVnv(values[i]));

			result = sv_2mortal(newRV_noinc((SV*)av));
			/* sv_bless(result,gv_stashpv("GRIB::API::Debug",1)); */
	}
	Safefree(values);

	return result;

}

static SV* get_long_array(grib_handle* h,const char* what)
{

	size_t size = 0;
	long *values = 0;
	SV* result  = &PL_sv_undef;


	if(error(grib_get_size(h,what,&size)) != 0)
		return &PL_sv_undef;

	Newz(0,values,size,long);

	if(error(grib_get_long_array(h,what,values,&size)) == 0)
	{
			AV* av      = newAV();
			int i;

			for(i = 0; i < size; i++)
				av_push(av,newSVnv(values[i]));

			result = sv_2mortal(newRV_noinc((SV*)av));
	}
	Safefree(values);

	return result;

}

static int compar(const void *a, const void *b)
{
	double da = *(double*)a;
	double db = *(double*)b;

	if(da == db)
		return 0;

	if(da < db)
		return -1;

	return 1;
}


typedef grib_handle   *GRIB__API;
typedef grib_iterator *GRIB__API__Iterator;


MODULE = GRIB::API		PACKAGE = GRIB::API		

PROTOTYPES: ENABLE

void debug()
	CODE:
		grib_context_set_debug(0,1);

GRIB::API
Read(file)
	SV*   file

	PREINIT:
		FILE* f = PerlIO_findFILE(IoIFP(sv_2io(file)));
		int err;
	CODE:
		RETVAL = grib_handle_new_from_file(0,f,&err);
		fail(err);
	OUTPUT:
		RETVAL

GRIB::API
create(bufsv)
	SV*   bufsv

	PREINIT:
		size_t size = 0;
		char   *buffer;
	CODE:

		RETVAL = 0;

		if(SvOK(bufsv))
		{
			size   = SvCUR(bufsv);
			buffer = SvPV(bufsv,PL_na);
			RETVAL = grib_handle_new_from_message(0,buffer,size);
		}
	OUTPUT:
		RETVAL

GRIB::API
template(name)
	char* name

	PREINIT:
	CODE:
		RETVAL = grib_handle_new_from_template(0,name);
	OUTPUT:
		RETVAL


GRIB::API
clone(h)
	GRIB::API h
	CODE:
		RETVAL = grib_handle_clone(h);
	OUTPUT:
		RETVAL
		

void
DESTROY(h)
	GRIB::API h
	CODE:
		grib_handle_delete(h);

void
Write(h,file)
	GRIB::API h
	SV*   file

	PREINIT:
		FILE* f = PerlIO_findFILE(IoIFP(sv_2io(file)));
		size_t size = 0;
		const void *message = 0;
	PPCODE:

		if( fail(grib_get_message(h,&message,&size) ) == 0)
		{
			/* printf("write %ld\n",size); */
			if(fwrite(message,1,size,f) == size)
				XPUSHs(&PL_sv_yes);
			else
				XPUSHs(&PL_sv_undef);
		}
		else
				XPUSHs(&PL_sv_undef);

void fail_on_error(f = 1)
	long f

	PPCODE: 
		_fail_on_error = f;
		XPUSHs(&PL_sv_yes);

void
Dump(h,file,mode = "debug",flags = ~0,format = NULL)
	GRIB::API h
	SV*   file
	char *mode
	long flags
	char *format

	PREINIT:
		FILE* f = PerlIO_findFILE(IoIFP(sv_2io(file)));
	PPCODE:
		grib_dump_content(h,f,mode,flags,format);
		XPUSHs(&PL_sv_yes);


void
get_long(h,what)
	GRIB::API h
	char *what
	PPCODE:
		XPUSHs(get_long(h,what));

	
void
get_double(h,what)
	GRIB::API h
	char *what
	PPCODE:
		XPUSHs(get_double(h,what));

void
get_string(h,what)
	GRIB::API h
	char *what
	PPCODE:
		XPUSHs(get_string(h,what));

void
get_bytes(h,what)
	GRIB::API h
	char *what
	PPCODE:
		XPUSHs(get_bytes(h,what));


void
set_bytes(h,what,bufsv)
	GRIB::API h
	char *what
	SV*   bufsv

	PREINIT:
		size_t size = 0;
		char   *buffer;
	PPCODE:

		if(!SvOK(bufsv))
			XPUSHs(&PL_sv_undef);
		else {

			size   = SvCUR(bufsv);
			buffer = SvPV(bufsv,PL_na);

			if(error(grib_set_bytes(h,what,buffer,&size)) == 0)
				XPUSHs(&PL_sv_yes);
			else
				XPUSHs(&PL_sv_undef);
		}

void
set_values(h,ref)
	GRIB::API h
	SV*   ref

	PREINIT:
		size_t size = 0;
		grib_values vals[1024];
		HV* hash;
		SV *value;
		I32 ret;
		char* key;
		int i;
	PPCODE:
		{
			if(SvTYPE(SvRV(ref)) != SVt_PVHV)
			{
				croak("not an HASH reference!");
				XPUSHs(&PL_sv_undef);
			}
			else
			{
				hash = (HV*)SvRV(ref);
				hv_iterinit(hash);
				size = 0;
				while((value = hv_iternextsv(hash, &key, &ret)) != 0)
				{
					/* printf("key=%s\n",key); */
					vals[size].name = strdup(key);
					if(SvIOK(value))
					{
						vals[size].type       = GRIB_TYPE_LONG;
						vals[size].long_value = SvIV(value);
						/* printf("-- %d %d %ld\n",size,vals[size].type,vals[size].long_value); */
					}
					else if(SvNOK(value))
					{
						vals[size].type         = GRIB_TYPE_DOUBLE;
						vals[size].double_value = SvNV(value);
						/* printf("-- %d %d %g\n",size,vals[size].type,vals[size].double_value); */
					}
					else if(SvPOK(value))
					{
						size_t len;
						char *buffer = SvPV(value,len);
						/* e =  grib_set_string(h,what,buffer,&size); */
						vals[size].type         = GRIB_TYPE_STRING;
						vals[size].string_value = buffer;
						/* printf("-- %d %d %s\n",size,vals[size].type,vals[size].string_value); */
					}
					else if(!SvOK(value) || (value == &PL_sv_undef)) 
					{
						/* TODO: support other missing */
						vals[size].type       = GRIB_TYPE_LONG;
						vals[size].long_value = GRIB_MISSING_LONG;
					}
					else
					{
						char buf[1024];
						sprintf(buf,"Invalid type %s",key);
						croak(buf);
						XPUSHs(&PL_sv_undef);
						break;
					}

					/* printf("%d %d %d\n",SvIOK(value),SvNOK(value),SvPOK(value)); */

					size++;
					if(size > 1024)
					{
						croak("Too many values");
						XPUSHs(&PL_sv_undef);
						break;
					}
				}

				/* printf("%s %d\n",vals[0].name,vals[0].type); */
				/* printf("SIZE %d\n",size); */

				if(error(grib_set_values(h,vals,size)) == 0)
					XPUSHs(&PL_sv_yes);
				else
					XPUSHs(&PL_sv_undef);

				for(i = 0; i < size; i++)
					free((char*)vals[i].name);

				XPUSHs(&PL_sv_yes);
			}
		}

void
get_double_array(h,what)
	GRIB::API h
	char *what

	PREINIT:
		size_t size = 0;
	PPCODE:

		if(error(grib_get_size(h,what,&size)) != 0)
			XPUSHs(&PL_sv_undef);
		else {

			double *values = 0;
			
			Newz(0,values,size,double);

			if(values)
			{

				if(error(grib_get_double_array(h,what,values,&size)) == 0)
				{
					int i;
					if(GIMME_V == G_ARRAY)
					{
						EXTEND(SP,size);
						for(i = 0; i < size; i++)
							XPUSHs(sv_2mortal(newSVnv(values[i])));
					}
					else
					{
						AV* av      = newAV();

						for(i = 0; i < size; i++)
							av_push(av,newSVnv(values[i]));

					/* XPUSHs(bufsv); */
					/* XPUSHs(sv_2mortal(newSViv(size)));*/
						XPUSHs(newRV_noinc((SV*)av));
					}
				}
				else
					XPUSHs(&PL_sv_undef);

			}
			else
				XPUSHs(&PL_sv_undef);

			Safefree(values);
		}

void
set_double_array(h,what,list)
	GRIB::API h
	char *what
	SV* list

	PREINIT:
		size_t size = 0;
		double *values = 0;
		int i;
		AV* av;

		if(!SvROK(list))
			croak("Argument 2 is not an ARRAY reference");
		
		if(SvTYPE(SvRV(list))!=SVt_PVAV)
			croak("Argument 2 is not an ARRAY reference");

		av  = (AV*)SvRV(list);
		if(av_len(av) < 0)
			croak("list has negative size ");

		size   = av_len(av) + 1;
		/* printf("set_double_array: %d\n",size); */
		

	PPCODE:

			
		Newz(0,values,size,double);

		if(!values)
			croak("cannot allocate values");

		for(i = 0; i < size; i++)
		{
			SV* sv = *av_fetch(av,i,1);
			if(SvNOK(sv))
				values[i] =  SvNV(sv);
			else if(SvIOK(sv))
				values[i] =  SvIV(sv);
			else if(SvPOK(sv))
			{
					char *buffer = SvPV(sv,PL_na);
					values[i] = atof(buffer);
			}
			else values[i] = 0;
		}


		/* printf("list %d\n",size); */

		if(error(grib_set_double_array(h,what,values,size)) == 0)
			XPUSHs(&PL_sv_yes);
		else
			XPUSHs(&PL_sv_undef);

		Safefree(values);

void
set_long_array(h,what,list)
	GRIB::API h
	char *what
	SV* list

	PREINIT:
		size_t size = 0;

		if(!SvROK(list))
			croak("Argument 2 is not an ARRAY reference");
		
		if(SvTYPE(SvRV(list))!=SVt_PVAV)
			croak("Argument 2 is not an ARRAY reference");

		AV* av  = (AV*)SvRV(list);
		if(av_len(av) < 0)
			croak("list has negative size ");

		size   = av_len(av);
		

	PPCODE:

		if(error(grib_get_size(h,what,&size)) != 0)
			XPUSHs(&PL_sv_undef);
		else {

			long *values = 0;
			
			Newz(0,values,size,long);

			if(values)
			{
				int i;
				AV* av = (AV*)SvRV(list);
				size = av_len(av);
				for(i = 0; i < size; i++)
				{
					SV* sv = *av_fetch(av,i,1);
					if(SvNOK(sv))
						values[i] =  SvNV(sv);
					else if(SvIOK(sv))
						values[i] =  SvIV(sv);
					else if(SvPOK(sv))
					{
						char *buffer = SvPV(sv,PL_na);
						values[i] = atof(buffer);
					}
					else
						values[i] = 0;

				}

				/* printf("list %d\n",size); */

				if(error(grib_set_long_array(h,what,values,size)) == 0)
					XPUSHs(&PL_sv_yes);
				else
					XPUSHs(&PL_sv_undef);

			}
			else
				XPUSHs(&PL_sv_undef);

			Safefree(values);
		}

void
get_long_array(h,what)
	GRIB::API h
	char *what

	PREINIT:
		size_t size = 0;
	PPCODE:

		if(error(grib_get_size(h,what,&size)) != 0)
			XPUSHs(&PL_sv_undef);
		else {

			long *values = 0;
			
			Newz(0,values,size,long);

			if(values)
			{

				if(error(grib_get_long_array(h,what,values,&size)) == 0)
				{
					int i;
					if(GIMME_V == G_ARRAY)
					{
						EXTEND(SP,size);
						for(i = 0; i < size; i++)
							XPUSHs(sv_2mortal(newSViv(values[i])));
					}
					else
					{
						AV* av      = newAV();

						for(i = 0; i < size; i++)
							av_push(av,newSViv(values[i]));

					/* XPUSHs(bufsv); */
					/*XPUSHs(sv_2mortal(newSViv(size)));*/
						XPUSHs(sv_2mortal(newRV_noinc((SV*)av)));
					}
				}
				else
					XPUSHs(&PL_sv_undef);

			}
			else
				XPUSHs(&PL_sv_undef);

			Safefree(values);
		}

char*
get_accessor_class(h,what)
	GRIB::API h
	char *what
	PREINIT:
	char *p;
	PPCODE:
		 p = (char*)grib_get_accessor_class_name(h,what);
		 if(p)
		 	XPUSHs(sv_2mortal(newSVpv(p,0)));
		else
			XPUSHs(&PL_sv_undef);

void
get_size(h,what)
	GRIB::API h
	char *what

	PREINIT:
		size_t size = 0;
	PPCODE:

		if( error(grib_get_size(h,what,&size)) != 0)
			XPUSHs(&PL_sv_undef);
		else 
			XPUSHs(sv_2mortal(newSViv(size)));

void
get_type(h,what)
	GRIB::API h
	char *what

	PREINIT:
		int type = 0;
	PPCODE:
		if( error(grib_get_native_type(h,what,&type)) != 0)
			XPUSHs(&PL_sv_undef);
		else 
			XPUSHs(sv_2mortal(newSViv(type)));

void
set(h,what,value)
	GRIB::API h
	char *what
	SV* value;
	PREINIT:
		int e = 1;
	PPCODE:
		if(SvIOK(value))
			e = error(grib_set_long(h,what,SvIV(value)));
		else if(SvNOK(value))
			e =  error(grib_set_double(h,what,SvNV(value)));
		else if(SvPOK(value))
		{
			size_t size;
			char *buffer = SvPV(value,size);
			e =  error(grib_set_string(h,what,buffer,&size));
		}

		if(e == 0)
			XPUSHs(&PL_sv_yes);
		else
			XPUSHs(&PL_sv_undef);

SV*
get(h,what)
	GRIB::API h
	char *what
	PREINIT:
		int e1,e2;
		int type;
		size_t size;
		long long_value;
		double double_value;
		SV* result = &PL_sv_undef;
		char buf[1024];

	PPCODE:
		e1 = error(grib_get_native_type(h,what,&type));
		e2 = error(grib_get_size(h,what,&size));

		if(e1 == 0 && e2 == 0)
		{
			/* printf("what=%s type=%d size=%d\n",what,type,size); */
			switch(type)
			{
				case GRIB_TYPE_LONG:
					if(size > 1)
						result = get_long_array(h,what);
					else
						result = get_long(h,what);
					break;

				case GRIB_TYPE_DOUBLE:
					if(size > 1)
						result = get_double_array(h,what);
					else
						result = get_double(h,what);
					break;

				case GRIB_TYPE_STRING:
						result = get_string(h,what);
					break;

				case GRIB_TYPE_BYTES:
						result = get_bytes(h,what);
					break;

				default:
						/* result = get_bytes(h,what); */
					break;
					
			}
		}
		XPUSHs(result);


SV* 
get_gaussian_latitudes(n)
	int n
	PREINIT:
		double *values = 0;
		SV* result  = &PL_sv_undef;

	PPCODE:
		Newz(0,values,n*2,double);

		if( error(grib_get_gaussian_latitudes(n,values)) == 0)
		{
				AV* av      = newAV();
				int i;

				for(i = 0; i < n*2; i++)
					av_push(av,newSVnv(values[i]));

				result = sv_2mortal(newRV_noinc((SV*)av));
				/* sv_bless(result,gv_stashpv("GRIB::API::Debug",1)); */
		}
		Safefree(values);
		XPUSHs(result);


void
set_long(h,what,value)
	GRIB::API h
	char *what
	long value;
	PREINIT:
	PPCODE:
		if(error(grib_set_long(h,what,value)) == 0)
			XPUSHs(&PL_sv_yes);
		else
			XPUSHs(&PL_sv_undef);

void
set_long_internal(h,what,value)
	GRIB::API h
	char *what
	long value;
	PREINIT:
	PPCODE:
		if(error(grib_set_long_internal(h,what,value)) == 0)
			XPUSHs(&PL_sv_yes);
		else
			XPUSHs(&PL_sv_undef);

void
set_double(h,what,value)
	GRIB::API h
	char *what
	double value;
	PREINIT:
	PPCODE:
		if(error(grib_set_double(h,what,value)) == 0)
			XPUSHs(&PL_sv_yes);
		else
			XPUSHs(&PL_sv_undef);

void
set_double_internal(h,what,value)
	GRIB::API h
	char *what
	double value;
	PREINIT:
	PPCODE:
		if(error(grib_set_double_internal(h,what,value)) == 0)
			XPUSHs(&PL_sv_yes);
		else
			XPUSHs(&PL_sv_undef);

void
set_string(h,what,value)
	GRIB::API h
	char *what
	char *value;
	PREINIT:
		size_t size = strlen(value);
	PPCODE:
		if(error(grib_set_string(h,what,value,&size)) == 0)
			XPUSHs(&PL_sv_yes);
		else
			XPUSHs(&PL_sv_undef);

void
set_string_internal(h,what,value)
	GRIB::API h
	char *what
	char *value;
	PREINIT:
		size_t size = strlen(value);
	PPCODE:
		if(error(grib_set_string_internal(h,what,value,&size)) == 0)
			XPUSHs(&PL_sv_yes);
		else
			XPUSHs(&PL_sv_undef);

void
update_sections_lengths(h)
	GRIB::API h
	PPCODE:
		grib_update_sections_lengths(h);

void
obsfucate(h,what = "values")
	GRIB::API h
	char *what
	PREINIT:
		int e;
		size_t size = 0;
		double *values = 0;
	PPCODE:


	if(error(grib_get_size(h,what,&size)) != 0)
	{
		croak(grib_get_error_message(e));
		return;
	}

	Newz(0,values,size,double);


	if(error(grib_get_double_array(h,what,values,&size)) != 0)
	{
		Safefree(values);
		croak(grib_get_error_message(e));
		return;
	}

	qsort(values,size,sizeof(double),&compar);

	if(error(grib_set_double_array(h,what,values,size)) != 0)
	{
		Safefree(values);
		croak(grib_get_error_message(e));
		return;
	}

	Safefree(values);

void
zero(h,what = "values")
	GRIB::API h
	char *what
	PREINIT:
	PPCODE:

	fail(grib_set_double_array(h,what,NULL,0));




void
get_keys(h,name_space = NULL)
	GRIB::API h
	char *name_space

	PREINIT:
		grib_keys_iterator* ks;

	PPCODE:

		ks = grib_keys_iterator_new(h,GRIB_KEYS_ITERATOR_ALL_KEYS,name_space);

		if(GIMME_V == G_ARRAY)
		{
			EXTEND(SP,800);
			while(grib_keys_iterator_next(ks))
			{
				const char* name = grib_keys_iterator_get_name(ks);
				XPUSHs(sv_2mortal(newSVpvn(name,strlen(name))));
			}
		}
		else
		{
			AV* av  = newAV();
			while(grib_keys_iterator_next(ks))
			{
				const char* name = grib_keys_iterator_get_name(ks);
				av_push(av,newSVpvn(name,strlen(name)));
			}

			XPUSHs(newRV_noinc((SV*)av));
		}
		grib_keys_iterator_delete(ks); 


GRIB::API::Iterator
iterator(h)
	GRIB::API h
	CODE:
		/* FIXME: RETVAL = grib_iterator_new(h); */
		abort();

		/* printf("ITERATOR %p\n",RETVAL); */
	OUTPUT:
		RETVAL

MODULE = GRIB::API		PACKAGE = GRIB::API::Iterator

void
next(i)
	GRIB::API::Iterator i
	PREINIT:
		double lat,lon,value;
	PPCODE:
		if(grib_iterator_next(i,&lat,&lon,&value))
		{
			
			XPUSHs(sv_2mortal(newSVnv(lat)));
			XPUSHs(sv_2mortal(newSVnv(lon)));
			XPUSHs(sv_2mortal(newSVnv(value)));
		}
		/* else */
			/* XPUSHs(&PL_sv_undef); */
			
		
	
