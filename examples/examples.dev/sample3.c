#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "grib_api_internal.h"

int main(int argc, char *argv[])
{
  grib_handle* h=NULL;
  grib_keyset* keyset=NULL;

  char name[200];
  size_t klen=sizeof(name);

  h = grib_handle_new_from_template(0,"GRIB1");

  if(!h) {
    printf("ERROR: Unable to create grib handle\n");
    exit(1);
  }
  keyset  = grib_new_keyset(h,"*");
  klen = sizeof(name);
  while(grib_keyset_next(keyset,name,&klen)) {
      printf("MARS -> %s ...\n",name);
      klen = sizeof(name);
  }

  /* print_one_request(r); */

  grib_keyset_delete(keyset);

  return 0;

}
