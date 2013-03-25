#include "test.h"

#define READ_ANY


int main(int argc,char** argv)
{
#ifdef READ_ANY
  char buffer[1024*1024*80]; 
  long length = sizeof(buffer); 
#endif
#ifdef GRIB_READ_ANY
  char buffer[1024*1024*80]; 
  size_t length= sizeof(buffer);
  grib_context *gc=0;
#endif
#ifdef GRIB_READ_ANY_ALLOC
  size_t length = 0;
  unsigned char* buffer=0;
  grib_context *gc=0;
#endif
  
  FILE *f;
  grib_handle  *g =  0;
        
  int e;

  f = fopen(argc > 1 ? argv[1]: "data" ,"r");
  if(!f)
  {
    perror(argc > 1 ? argv[1]: "data");
    exit(1);
  }

#ifdef READ_ANY
  printf("#---- readany used ----\n");
  while((e = readany(f,buffer,&length)) != -1) 
#endif
#ifdef GRIB_READ_ANY
  printf("#---- grib_read_any used ----\n");
  while((e = grib_read_any(gc,f,buffer,&length)) != GRIB_END_OF_FILE)
#endif
#ifdef GRIB_READ_ANY_ALLOC
  printf("#---- grib_read_any_alloc used ----\n");
  while((e = grib_read_any_alloc(gc,f,&buffer,&length)) != GRIB_END_OF_FILE)
#endif
  {

    if(e != 0 ) 
    {
      printf("grib_read_any error %d\n",e);
      exit(e);
    }


    g = grib_handle_new_from_message(0,buffer,length);

    grib_dump_content(g,stdout,"serialize",0,NULL);

    grib_handle_delete(g);

  }


  exit(0);
}
