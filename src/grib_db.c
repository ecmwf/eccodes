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
 *
 * Description: grib database routines
 *
 */
#include "grib_api_internal.h"
#define GRIB_START_ARRAY_SIZE 5000
#define GRIB_ARRAY_INCREMENT 1000

#define SWAP(a,b) temp=(a);(a)=(b);(b)=temp;

#define GRIB_ORDER_BY_ASC    1
#define GRIB_ORDER_BY_DESC   -1

GRIB_INLINE static int grib_inline_strcmp(const char* a,const char* b) {
  if (*a != *b) return 1;
  while((*a!=0 && *b!=0) &&  *(a) == *(b) ) {a++;b++;}
  return (*a==0 && *b==0) ? 0 : 1;
}

static int grib_db_new_column(grib_db* db,int id,char* key,int type);
static void grib_db_delete_columns(grib_db* db);
static int grib_db_columns_resize(grib_db* db,size_t newsize);
static int grib_db_column_copy_from_handle(grib_handle* h,grib_db* db,int i);
static grib_db* grib_db_create_from_keys(grib_context* c,char** keys,int nkeys,int* err);
static grib_db* grib_db_create_from_keys(grib_context* c,char** keys,int nkeys,int* err);
static void grib_fieldset* grib_db_fieldset_create(grib_db* db, int* err);
static int grib_fieldset_compare(grib_fieldset* set,int* i,int* j);
static void grib_db_sort(grib_set* set, int beg, int theEnd);
static grib_order_by* grib_db_new_order_by(grib_context* c,char* obstr);
static void grib_db_delete_order_by(grib_context* c,grib_order_by* order_by);
static int grib_db_resize(grib_db* db,size_t newsize);
static int grib_db_resize_fields(grib_db* db,size_t newsize);


/* --------------- grib_column functions ------------------*/
static int grib_db_new_column(grib_db* db,int id,char* key,int type) {
  grib_column* column=0;
  grib_context* c;
  int err=0;

  if (!db) return GRIB_INVALID_ARGUMENT;

  c=db->context;

  db->columns[id].errors=(int*)grib_context_malloc(c,
             sizeof(int)*GRIB_START_ARRAY_SIZE);

  switch (type) {
    case GRIB_TYPE_LONG:
      db->columns[id].long_values=(long*)grib_context_malloc(c,
             sizeof(long)*GRIB_START_ARRAY_SIZE);
      if (!db->columns[id].long_values) {
        grib_context_log(c, GRIB_LOG_ERROR,
          "grib_db_new_column : Cannot malloc %d bytes",
          sizeof(long)*GRIB_START_ARRAY_SIZE);
        err=GRIB_OUT_OF_MEMORY;
        return err;
      }
      break;
    case GRIB_TYPE_DOUBLE:
      db->columns[id].double_values=(double*)grib_context_malloc(c,
             sizeof(double)*GRIB_START_ARRAY_SIZE);
      if (!db->columns[id].double_values) {
        grib_context_log(c, GRIB_LOG_ERROR,
          "grib_db_new_column : Cannot malloc %d bytes",
          sizeof(double)*GRIB_START_ARRAY_SIZE);
        err=GRIB_OUT_OF_MEMORY;
        return err;
      }
      break;
    case GRIB_TYPE_STRING:
      db->columns[id].string_values=(char**)grib_context_malloc(c,
             sizeof(char*)*GRIB_START_ARRAY_SIZE);
      if (!db->columns[id].string_values) {
        grib_context_log(c, GRIB_LOG_ERROR,
          "grib_db_new_column : Cannot malloc %d bytes",
          sizeof(char*)*GRIB_START_ARRAY_SIZE);
        err=GRIB_OUT_OF_MEMORY;
        return err;
      }
      break;
    default:
      grib_context_log(c, GRIB_LOG_ERROR,
        "grib_db_new_column : unknown column type %d",type);
      grib_context_free(c,column);
      return err;
  }

  db->columns[id].context=c;
  db->columns[id].name=strdup(key);
  db->columns[id].type=type;
  db->columns[id].values_array_size=GRIB_START_ARRAY_SIZE;
  db->columns[id].size=0;
  return err;
}

static void grib_db_delete_columns(grib_db* db) {
  int i=0;
  grib_context* c;

  if (!set) return;
  c=db->context;

  for (i=0;i<db->columns_size;i++) {
    switch (db->columns[i].type) {
      case GRIB_TYPE_LONG:
        grib_context_free(c,db->columns[i].long_values);
        break;
      case GRIB_TYPE_DOUBLE:
        grib_context_free(c,db->columns[i].double_values);
        break;
      case GRIB_TYPE_STRING:
        for (i=0;i<db->columns[i].size;i++)
          grib_context_free(c,db->columns[i].string_values[i]);
        grib_context_free(c,db->columns[i].string_values);
        break;
      default:
        grib_context_log(c, GRIB_LOG_ERROR,
          "grib_db_new_column : unknown column type %d",db->columns[i].type);
    }
    grib_context_free(c,db->columns[i].errors);
    free(db->columns[i].name);
  }
  grib_context_free(c,db->columns);
}

static int grib_db_columns_resize(grib_db* db,size_t newsize) {
  double* newdoubles;
  long* newlongs;
  char** newstrings;
  int* newerrors;
  int i=0;
  grib_context* c;

  if (!db || !db->columns) return GRIB_INVALID_ARGUMENT;

  c=db->context;

  if (newsize <= db->columns[0].values_array_size) return 0;

  for (i=0;i<db->columns_size;i++) {

    switch (db->columns[i].type) {
      case GRIB_TYPE_LONG:
        newlongs=(long*)grib_context_realloc(c,db->columns[i].long_values,
              newsize*sizeof(long));
        if (!newlongs ) {
          grib_context_log(c, GRIB_LOG_ERROR,
              "grib_db_columns_resize : Cannot malloc %d bytes",newsize-db->columns[i].values_array_size);
          return GRIB_OUT_OF_MEMORY;
        } else db->columns[i].long_values=newlongs;
        break;
      case GRIB_TYPE_DOUBLE:
        newdoubles=(double*)grib_context_realloc(c,db->columns[i].double_values,
             newsize*sizeof(double));
        if (!newdoubles) {
          grib_context_log(c, GRIB_LOG_ERROR,
              "grib_db_columns_resize : Cannot malloc %d bytes",newsize-db->columns[i].values_array_size);
          return GRIB_OUT_OF_MEMORY;
        } else db->columns[i].double_values=newdoubles;
        break;
      case GRIB_TYPE_STRING:
        newstrings=(char**)grib_context_realloc(c,db->columns[i].string_values,
              newsize*sizeof(char*));
        if (!newstrings) {
          grib_context_log(c, GRIB_LOG_ERROR,
              "grib_db_columns_resize : Cannot malloc %d bytes",newsize-db->columns[i].values_array_size);
          return GRIB_OUT_OF_MEMORY;
        } else db->columns[i].string_values=newstrings;
        break;
    }
    newerrors=(int*)grib_context_realloc(c,db->columns[i].errors,newsize*sizeof(int));
    if (!newerrors) {
      grib_context_log(c, GRIB_LOG_ERROR,
          "grib_db_columns_resize : Cannot malloc %d bytes",
          db->columns[i].errors,newsize*sizeof(int));
      return GRIB_OUT_OF_MEMORY;
    } else db->columns[i].errors=newerrors;

    db->columns[i].values_array_size=newsize;

  }

  return GRIB_SUCCESS;
}

static int grib_db_column_copy_from_handle(grib_handle* h,grib_db* db,int i) {
  int err=0;
  long lval=0;
  double dval=0;
  char sval[1024];
  size_t slen=1024;
  if (!db || !h || db->columns[i].type == 0)
    return GRIB_INVALID_ARGUMENT;

  if (db->columns[i].size >= db->columns[i].values_array_size)
    grib_db_columns_resize(db,db->columns[i].values_array_size+GRIB_ARRAY_INCREMENT);

  switch (db->columns[i].type) {
    case GRIB_TYPE_LONG:
      err=grib_get_long(h,db->columns[i].name,&lval);
      db->columns[i].long_values[db->columns[i].size]=lval;
      break;
    case GRIB_TYPE_DOUBLE:
      err=grib_get_double(h,db->columns[i].name,&dval);
      db->columns[i].double_values[db->columns[i].size]=dval;
      break;
    case GRIB_TYPE_STRING:
      err=grib_get_string(h,db->columns[i].name,sval,&slen);
      db->columns[i].string_values[db->columns[i].size]=strdup(sval);
      break;
  }

  db->columns[i].errors[db->columns[i].size]=err;
  db->columns[i].size++;

  return err;
}

/* --------------- grib_db functions ------------------*/
grib_db* grib_db_new_from_files(grib_context* c,char* filenames[],
int nfiles, char** keys,int nkeys,int* err) {
   int i=0;
   int ret=GRIB_SUCCESS;

   grib_db* db=0;

   if (!c) c=grib_context_get_default( );

   if (( (!keys || nkeys==0) )   || !filenames ) {
     *err=GRIB_INVALID_ARGUMENT;
     return NULL;
   }

   db=grib_db_new_from_file(c,filenames[0],keys,nkeys,err);
   if (!db || err != GRIB_SUCCESS) return db;

   *err=GRIB_SUCCESS;
   for (i=1;i<nfiles;i++) {
     ret=grib_db_load(db,filenames[i]);
     if (ret != GRIB_SUCCESS) *err=ret;
   }

   return db;
}

grib_db* grib_db_new_from_file(grib_context* c,char* filename,
            char** keys,int nkeys,int* err) {
   int i=0;
   int ret=GRIB_SUCCESS;

   grib_db* db=0;

   if (!c) c=grib_context_get_default( );

   if (( (!keys || nkeys==0) )
        || !filename ) {
     *err=GRIB_INVALID_ARGUMENT;
     return NULL;
   }

   db=grib_db_create_from_keys(c,keys,nkeys,err);

   *err=GRIB_SUCCESS;
   ret=grib_db_load(db,filename);
   if (ret != GRIB_SUCCESS) *err=ret;

   return db;
}

static grib_db* grib_db_create_from_keys(grib_context* c,char** keys,int nkeys,int* err) {
   grib_db* db=0;
   size_t msize=0,size=0;
   int i=0;
   int type=0;
   grib_context* c=NULL;
   int default_type=GRIB_TYPE_DOUBLE;

   if (!c) c=grib_context_get_default( );

   size=GRIB_START_ARRAY_SIZE;

   msize=sizeof(grib_db);
   db=(grib_db*)grib_context_malloc(c,msize);
   if (!db) {
     grib_context_log(c, GRIB_LOG_ERROR,
        "grib_db_create : Cannot malloc %d bytes",msize);
      return NULL;
   }

   db->context=c;
   db->size=0;
   db->fields_array_size=size;
   db->fields=0;
   db->columns=0;

   db->fields=grib_db_create_fields(db->context,size);

   db->columns=(grib_column*)grib_context_malloc(c,sizeof(grib_column)*nkeys);
   if (!set->columns) {
       grib_context_log(c,GRIB_LOG_ERROR,"grib_db_new_query: memory allocation error");
       *err=GRIB_OUT_OF_MEMORY;
       return NULL;
   }
   for (i=0;i<nkeys;i++) {
     char* key=strdup(keys[i]);
     char* p=key;
     while(*p != ':' && *p != '\0') p++;
     if (*p==':') {
       type = grib_type_to_int(*(p+1));
       *p='\0';
     } else {
       type = default_type;
     }
     *err=grib_db_new_column(db,i,key,type);
     free(key);
   }

   db->columns_size=nkeys;

   return db;
}

void grib_db_delete(grib_db* db) {
  grib_context* c=0;
  if (!db) return;

  c=set->context;

  grib_db_delete_columns(db);

  grib_db_delete_fields(db);

  grib_context_free( c, db);
}

grib_query* grib_db_new_query(grib_context* c,const char* where_string,
                              const char* order_by_string) {
  grib_query* q=0;

  if (!c) grib_context_get_default( );

  q=(grib_query*)grib_context_malloc(c,sizeof(grib_query));

  q->where_string=0;
  q->order_by=0;

  if (where_string) q->where_string=strdup(where_string);

  if (order_by_string) {
    q->order_by=grib_db_new_order_by(set->context,(char*)order_by_string);
    if ((err=grib_db_set_order_by(set,ob)) != GRIB_SUCCESS)
      return q;
  }

  return q;
}

grib_fieldset* grib_db_execute(grib_db* db,grib_query* query,int* err) {
  grib_fieldset* set=NULL;

  if (!db) {
     *err=GRIB_INVALID_ARGUMENT;
     return NULL;
  }
  set= grib_db_fieldset_create(db,err);
  if (*err!=GRIB_SUCCESS) return set;

  set->query=query;

  *err=grib_db_apply_where(set,query->where_string);
  if (*err!=GRIB_SUCCESS) return set;

  *err=grib_db_apply_order_by(set,query->order_by);
  if (*err!=GRIB_SUCCESS) return set;

  return set;

}

static void grib_fieldset* grib_db_fieldset_create(grib_db* db, int* err) {
  grib_fieldset* set=(grib_fieldset*)grib_context_malloc(db->context,
                              sizeof(grib_fieldset));

  if (!set) {
    *err=GRIB_OUT_OF_MEMORY;
    return NULL;
  }

  set->db=db;
  set->context=db->context;
  set->grib_query=NULL;
  set->size=0;
  set->filter=NULL;
  set->order=NULL;

  return set;
}

int grib_db_apply_where(grib_fieldset* set) {
  int err=GRIB_SUCCESS;
  grib_math* m=0;

  if (!set) return GRIB_INVALID_ARGUMENT;

  /*
  m=grib_math_new(set->context,where_string,&err);

  print_math(m);
  printf("\n");
  */

  if (set->filter) grib_db_delete_int_array(set->filter);
  set->filter=grib_db_create_int_array(set->context,db->size);

  if (set->order) grib_db_delete_int_array(set->order);
  set->order=grib_db_create_int_array(set->context,db->size);

  for (i=0;i<db->size;i++) set->filter[i]=i;

  return err;
}

int grib_db_apply_order_by(grib_fieldset* set) {
  int err=0;
  grib_order_by* ob=NULL;

  if (!set || !set->query) return GRIB_INVALID_ARGUMENT;

  if (set->query->order_by) grib_db_sort(set,0,set->size-1);

  grib_db_rewind(set);

  return err;
}

static int grib_fieldset_compare(grib_fieldset* set,int* i,int* j) {
  int ret=0;
  double d=0;
  int idkey=0;
  grib_order_by* ob=0;
  int ii=0,jj=0;
  int *order=0,*filter=0;

  if (!set || !set->order_by) return GRIB_INVALID_ARGUMENT;
  ob=set->query->order_by;
  order=set->order->el;
  filter=set->filter->el;

  ii=*(set->filter->el+*(order+*i));
  jj=*(set->filter->el+*(order+*j));

  while (ob) {
    idkey=ob->idkey;
    switch (set->db->columns[idkey].type) {
      case GRIB_TYPE_STRING:
        ret=grib_inline_strcmp(set->db->columns[idkey].string_values[ii],
                   set->db->columns[idkey].string_values[jj]);
        break;

      case GRIB_TYPE_DOUBLE:
        d=set->db->columns[idkey].double_values[ii]-
          set->db->columns[idkey].double_values[jj];
        if (d > 0 ) ret=1;
        else if ( d == 0) ret=0;
        else ret=-1;
        break;

      case GRIB_TYPE_LONG:
        ret=set->db->columns[idkey].long_values[ii]-
            set->db->columns[idkey].long_values[jj];
        break;
      default:
        return GRIB_INVALID_TYPE;
    }
    if (ret!=0) {
      ret*=ob->mode;
      break;
    }
    ob=ob->next;
  }

  return ret;
}

static void grib_db_sort(grib_set* set, int beg, int theEnd) {
   double temp;
   int l=0,r=0;
   if (theEnd > beg) {
      l = beg + 1;
      r = theEnd;
      while (l < r) {
         if ( grib_db_compare(set,&l,&beg) <= 0 ) {
           l++;
         } else if(grib_db_compare(set,&r,&beg) >= 0 ) {
           r--;
         } else {
           SWAP(set->order->el[l],set->order->el[r])
         }
      }

      if (grib_db_compare(set,&l,&beg) < 0) {
        SWAP(set->order->el[l],set->order->el[beg])
        l--;
      } else {
        l--;
        SWAP(set->order->el[l],set->order->el[beg])
      }

      grib_db_sort(set, beg, l);
      grib_db_sort(set, r, theEnd);
   }
}

static void grib_db_delete_order_by(grib_context* c,grib_order_by* order_by) {
  grib_order_by* ob=order_by;

  if (!c) c=grib_context_get_default();

  while (order_by) {
    if (order_by->key) free(order_by->key);
    ob=order_by;
    order_by=order_by->next;
    grib_context_free( c,ob);
  }

  return;
}

static grib_order_by* grib_db_new_order_by(grib_context* c,char* obstr) {
  char *t1=0,*t2=0,*p=0;
  int id=0;
  char *z=0,*zs=0;
  int mode,mode_default=GRIB_ORDER_BY_ASC;
  grib_order_by *ob,*sob;

  if (!obstr) return NULL;

  z=strdup(obstr);
  zs=z;
  grib_trim(&z);

  if (strlen(z)==0) {return 0;}

  ob=(grib_order_by*)grib_context_malloc(c,sizeof(grib_order_by));
  sob=ob;
  ob->key=0;
  ob->idkey=0;
  ob->mode=0;
  ob->next=0;

  if (z) t1=strtok(z,",");

  while (t1) {
    grib_trim(&t1);
    t2=strdup(t1);
    p=t2;
    while ( *p != ' ' && *p != '\0' ) p++;
    mode=mode_default;
    if (p != t2) {
      while ( *p == ' ' && *p != '\0' ) p++;
      if (*p != '\0') {
        *(p-1)='\0';
        if (!grib_inline_strcmp(p,"asc")) mode=GRIB_ORDER_BY_ASC;
        if (!grib_inline_strcmp(p,"desc")) mode=GRIB_ORDER_BY_DESC;
      }
      grib_trim(&p);
    }
    grib_trim(&t2);
    id=-1;
    t1=strtok(NULL,",");

    if (ob->key) {
      ob->next=(grib_order_by*)grib_context_malloc(c,sizeof(grib_order_by));
      ob=ob->next;
      ob->key=0;
      ob->next=0;
    }
    ob->mode=mode;
    ob->key=t2;
    ob->idkey=id;
  }

  free(zs);
  return sob;
}

int grib_db_load(grib_db* db,char* filename) {
   int ret=GRIB_SUCCESS;
   int err=0;
   int i=0;
   grib_handle* h=0;
   int nkeys;
   grib_file* file;
   double offset=0;
   long length=0;
   grib_context* c=0;

   if (!db || !filename ) return GRIB_INVALID_ARGUMENT;
   c=db->context;

   nkeys=db->columns_size;

   file=grib_file_open(filename,"r",&err);
   if (!file || !file->handle) return err;

   while((h = grib_handle_new_from_file(c,file->handle,&err))
         != NULL || ret != GRIB_SUCCESS ) {
     if (!h) return ret;

     err=GRIB_SUCCESS;
     for (i=0;i<db->columns_size;i++) {
       err=grib_db_column_copy_from_handle(h,db,i);
       if (err != GRIB_SUCCESS) ret=err;
     }
     if (err==GRIB_SUCCESS || err==GRIB_NOT_FOUND) {
       if (db->fields_array_size < db->columns[0].values_array_size) {
         ret=grib_db_resize(db,db->columns[0].values_array_size);
         if (ret!=GRIB_SUCCESS) return ret;
       }
       offset=0;
       ret=grib_get_double(h,"offset",&offset);
       db->fields[db->size]=(grib_field*)grib_context_malloc(c,sizeof(grib_field));
       db->fields[db->size]->file=file;
       file->refcount++;
       db->fields[db->size]->offset=(off_t)offset;
       ret=grib_get_long(h,"totalLength",&length);
       db->fields[db->size]->length=length;
       db->size=set->columns[0].size;
     }
     grib_handle_delete(h);
   }
   if (h) grib_handle_delete(h);

   grib_file_close(file->name, 0, &err);

   return ret;
}

static int grib_db_resize(grib_db* db,size_t newsize) {
  int err=0;

  err=grib_db_resize_fields(db,newsize);
  if (err != 0) return err;

  set->fields_array_size=newsize;

  return GRIB_SUCCESS;
}

void grib_db_rewind(grib_fieldset* set) {
  if (set) set->current=0;
}

grib_handle* grib_db_next_handle(grib_fieldset* set,int* err) {
  grib_handle* h;
  *err=GRIB_SUCCESS;
  h=grib_db_retrieve(set,set->current,err);
  if (*err==GRIB_SUCCESS) {
    set->current++;
  }
  return h;
}

int grib_db_count(grib_fieldset* set) {
  return set->size;
}

grib_handle* grib_db_retrieve(grib_fieldset* set,int i,int* err) {
   grib_handle* h=0;
   grib_field* field=0;
   *err=GRIB_SUCCESS;
   if ( !set  ) {
    *err=GRIB_INVALID_ARGUMENT;
    return NULL;
   }
   if (i >= set->size) return NULL;

   field=set->db->fields[set->filter->el[set->order->el[i]]];
   grib_file_open(field->file->name,"r",err);
   if (*err!=GRIB_SUCCESS) return NULL;

   fseeko(field->file->handle,field->offset,SEEK_SET);
   h=grib_handle_new_from_file(set->context,field->file->handle,err);
   if (*err!=GRIB_SUCCESS) return NULL;

   grib_file_close(field->file->name, 0, err);

   return h;
}

static grib_int_array* grib_db_create_int_array(grib_context* c,size_t size) {
  grib_int_array* a;
  int i=0;

  if (!c) c=grib_context_get_default();

  a=(grib_int_array*)grib_context_malloc(c,sizeof(grib_int_array));

  if (!a) {
    grib_context_log(c, GRIB_LOG_ERROR,
        "grib_db_create_int_array : Cannot malloc %d bytes",
        sizeof(grib_int_array));
    return NULL;
  }

  a->el=(int*)grib_context_malloc(c,sizeof(int)*size);
  if (!a->el) {
    grib_context_log(c, GRIB_LOG_ERROR,
        "grib_db_create_int_array : Cannot malloc %d bytes",
        sizeof(int)*size);
    return NULL;
  }

  a->size=size;
  a->context=c;
  for (i=0;i<size;i++) a->el[i]=i;

  return a;
}

static int grib_db_resize_int_array(grib_int_array* a,size_t newsize) {
  int* el;
  int err=0;
  if (!a) return GRIB_INVALID_ARGUMENT;

  newsize=newsize*sizeof(int);

  el=(int*)grib_context_realloc(a->context,a->el,newsize);
  if (!el) {
    grib_context_log(a->context, GRIB_LOG_ERROR,
        "grib_db_resize_int_array : Cannot malloc %d bytes",
        newsize);
    return GRIB_OUT_OF_MEMORY;
  } else a->el=el;
  a->size=newsize;
  return err;
}

static void grib_db_delete_int_array(grib_int_array* f)
{
  grib_context* c = NULL;

  if (!f) return;
  c = f->context;

  grib_context_free(c,f->el);
  grib_context_free(c,f);

}

static grib_field** grib_db_create_fields(grib_context* c,size_t size) {
  int i=0;
  grib_field** fields=(grib_field**)grib_context_malloc(c,size*sizeof(grib_field*));
  if (!fields) return NULL;
  for (i=0;i<size;i++) fields[i]=0;
  return fields;
}

static int grib_db_resize_fields(grib_db* db,size_t newsize) {
  int err=0;
  int i;
  grib_field** fields;
  if (!db) return GRIB_INVALID_ARGUMENT;

  fields=(grib_field**)grib_context_realloc(set->context,set->fields,newsize*sizeof(grib_field*));
  if (!fields) {
    grib_context_log(set->context, GRIB_LOG_ERROR,
        "grib_db_resize_fields : Cannot malloc %d bytes",
        newsize*sizeof(grib_field*));
    return GRIB_OUT_OF_MEMORY;
  } else db->fields=fields;

  for (i=set->fields_array_size;i<newsize;i++)
    db->fields[i]=0;

  db->fields_array_size=newsize;
  return err;
}

static void grib_db_delete_fields(grib_fieldset* set) {
  int i;
  for (i=0;i<set->size;i++) {
   if (!set->fields[i]) continue;
   set->fields[i]->file->refcount--;
   grib_context_free(set->context,set->fields[i]);
 }
 grib_context_free(set->context,set->fields);

}

static void grib_trim(char** x) {
  char* p=0;
  while (**x == ' ' && **x != '\0' ) (*x)++;
  if (**x == '\0') return;
  p=(*x)+strlen(*x)-1;
  while ( *p == ' ' ) {*p='\0';p--;}
  if ( *p == ' ' ) *p='\0';
}

static int grib_db_set_order_by(grib_fieldset* set,grib_order_by* ob) {
  grib_order_by* next=ob;
  int i=0;

  while(next) {
    next->idkey=-1;
    for (i=0;i<set->columns_size; i++) {
      if (!grib_inline_strcmp(next->key,set->columns[i].name)) {
        next->idkey=i;
        break;
      }
    }
    if (next->idkey == -1) {
      grib_context_log(set->context,GRIB_LOG_ERROR,
        "Unable to apply the order by. Key missing from the fieldset.\n");
      return GRIB_MISSING_KEY;
    }
    next=next->next;
  }

  set->order_by=ob;

  return GRIB_SUCCESS;

}

