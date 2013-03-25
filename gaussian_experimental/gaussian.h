#include "grib_rgauss.h"
typedef struct gaussian_grid gaussian_grid;

struct gaussian_grid {
  int reduced;  
  int order;
  int *points;
  int *latitudes;
};

void usage(char*);

