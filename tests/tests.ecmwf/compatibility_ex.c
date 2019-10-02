#include "test.h"

char buffer[1024*1024*80];

int main(int argc,char** argv)
{
  FILE *f;
  FILE* of;
  grib_handle  *h =  0;
  grib_context *gc = 0;
  int err;
  int option_flags =0;
  char* ofname;
  char* fname;
  char* mode="serialize";
  char* values_format="10%g";
  option_flags |= GRIB_DUMP_FLAG_VALUES;

  fname = argc > 1 ? argv[1]: "data" ;
  ofname=(char*)malloc(sizeof(char)*(strlen(fname)+6));
  sprintf(ofname,"%s.dump",fname);

  f = fopen(fname ,"r");
  if(!f) {
    perror(fname);
    exit(10);
  }

  if ((h = grib_handle_new_from_file(0,f,&err)) == NULL) {
   fclose( f);
   exit(1);
  }

  of = fopen(ofname ,"w");
  if(!of) {
    perror(ofname);
    exit(10);
  }

  grib_dump_content(h,of,mode,option_flags,values_format);

  grib_handle_delete(h);

  if(fclose(of)) {
    perror(ofname);
    exit(1);
  }

  of = fopen(ofname ,"r");
  if(!of) {
    perror(ofname);
    exit(10);
  }

  gc = grib_context_get_default();
  h=grib_handle_new(gc);
  if (!h) {
   printf("Error: unable to create handle\n");
   exit(1);
  }

  GRIB_CHECK(grib_load_from_text_file(h,of),ofname);

  if(fclose(of)) {
    perror(ofname);
    exit(1);
  }

  /* unlink(ofname);*/

  if(compare_with_gribex(h))
    exit(1);

  grib_handle_delete(h);

  fclose( f);
  exit(0);
}
