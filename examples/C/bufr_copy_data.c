#include "eccodes.h"

static void usage(char* progname);

int main(int argc, char *argv[])
{
  FILE* f = NULL;
  codes_handle* h=NULL;
  codes_handle* ho=NULL;
  long size;
  char** keys;
  size_t nkeys=0,i;
  int err=0;
  char*    outfile;
  const char*    sampleName = "BUFR3";
  long ibitmap[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                  0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,
                  1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,
                  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,1,1,1,0};

  long ud[]={307011,7006,10004,222000,101023,31031,1031,1032,101023,33007,
              225000,236000,101023,31031,1031,1032,8024,101001,225255,225000,
              236000,101023,31031,1031,1032,8024,101001,225255,
              1063,2001,4001,4002,4003,4004,4005,5002,
              6002,7001,7006,11001,11016,11017,11002};

  if (argc != 3) usage(argv[0]);

  outfile=argv[2];

  f = fopen(argv[1],"r");
  if(!f) {
    perror(argv[1]);
    exit(1);
  }
  ho = codes_bufr_handle_new_from_samples(NULL, sampleName);
  if (ho == NULL) {
    fprintf(stderr, "ERROR creating BUFR from %s\n", sampleName);
    return 1;
  }
  size = 69;
  CODES_CHECK(codes_set_long_array(ho, "inputDataPresentIndicator", ibitmap, size), 0);
  CODES_CHECK(codes_set_long(ho, "bufrHeaderCentre", 98), 0);
  CODES_CHECK(codes_set_long(ho, "updateSequenceNumber", 1), 0);
  CODES_CHECK(codes_set_long(ho, "dataCategory", 0), 0);
  CODES_CHECK(codes_set_long(ho, "dataSubCategory", 140), 0);
  CODES_CHECK(codes_set_long(ho, "masterTablesVersionNumber", 13), 0);
  CODES_CHECK(codes_set_long(ho, "localTablesVersionNumber", 1), 0);
  CODES_CHECK(codes_set_long(ho, "typicalYearOfCentury", 15), 0);
  CODES_CHECK(codes_set_long(ho, "typicalMonth", 5), 0);
  CODES_CHECK(codes_set_long(ho, "typicalDay", 4), 0);
  CODES_CHECK(codes_set_long(ho, "typicalHour", 9), 0);
  CODES_CHECK(codes_set_long(ho, "typicalMinute", 30), 0);
  CODES_CHECK(codes_set_long(ho, "numberOfSubsets", 1), 0);
  CODES_CHECK(codes_set_long(ho, "observedData", 1), 0);
  CODES_CHECK(codes_set_long(ho, "compressedData", 0), 0);

  size=43;
  codes_set_long_array(ho,"unexpandedDescriptors",ud,size);

  while((h = codes_handle_new_from_file(0,f,PRODUCT_BUFR,&err)) != NULL)
  {
    if(!h) {
      printf("ERROR: Unable to create grib handle\n");
      exit(1);
    }
    /* codes_copy_key(h,ho,"unexpandedDescriptors",0); */
    codes_set_long(h,"unpack",1);

    /* err=codes_bufr_copy_data(h,ho); */
    keys=codes_bufr_copy_data_return_copied_keys(h,ho,&nkeys,&err); 
    for (i=0;i<nkeys;i++) {
      printf("%s\n",keys[i]);
      free(keys[i]);
    }
    free(keys);

    codes_handle_delete(h);
  }
  fclose(f);
  grib_write_message(ho,outfile,"w");
  codes_handle_delete(ho);

  return err;
}

static void usage(char* progname)
{
  printf("\nUsage: %s bufr_in bufr_out\n",progname);
  exit(1);
}
