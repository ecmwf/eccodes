C Copyright 2005-2013 ECMWF.
C
C This software is licensed under the terms of the Apache Licence Version 2.0
C which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
C 
C In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
C virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
C
C  Fortran Implementation: fieldset
C 
C  Description: how to use a fieldset.
C 
C 
C 
C 
C  
     program nearest 
     implicit none
     include 'grib_api_f77.h'
     integer err
     integer step
     integer nfiles
     integer i
     integer iset
     integer ig
     character*20 param
     integer len
     real*8 lats
     real*8 lons
     real*8 values
     real*8 distances
     integer indexes
     integer size
     real*8 lat,lon
     integer mode=0
     integer count
     character*200 filenames
     dimension filenames(5)
     integer in

  nfiles=argc-1;
  filenames=(char**)malloc(sizeof(char*)*nfiles);
  for (i=0;i<nfiles;i++)
    filenames[i]=(char*)strdup(argv[i+1]);

  set=grib_fieldset_new_from_files(0,filenames,nfiles,0,0,0,order_by,&err);
  GRIB_CHECK(err,0);

  printf("\nordering by %s\n",order_by);
  printf("\n%d fields in the fieldset\n",grib_fieldset_count(set));
  printf("n,step,param\n");

  mode=GRIB_NEAREST_SAME_GRID |  GRIB_NEAREST_SAME_POINT;
  count=1;
  while ((h=grib_fieldset_next_handle(set,&err))!=NULL) {
    GRIB_CHECK(grib_get_long(h,"step",&step),0);
    GRIB_CHECK(grib_get_string(h,"param",param,&len),0);

    printf("%d %ld %s  ",count,step,param);
    if (!nearest) nearest=grib_nearest_new(h,&err);
    GRIB_CHECK(err,0);
    GRIB_CHECK(grib_nearest_find(nearest,h,lat,lon,mode,lats,lons,values,distances,indexes,&size),0);
    for (i=0;i<4;i++) printf("%d %.2f %.2f %g %g - ",
         (int)indexes[i],lats[i],lons[i],distances[i],values[i]);
    printf("\n");

    grib_handle_delete(h);
    count++;
  }

  if (nearest) grib_nearest_delete(nearest);

  if (set) grib_fieldset_delete(set);

  return 0;
}

