/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * C Implementation: grib_merge
 *
 * Author: Enrico Fucile <enrico.fucile@ecmwf.int>
 * Description:
 * In Observations team we need a tool to merge the GRIB messages coming from GTS.
 * They come with compatible grid (lat/lon) but they are split in different quadrants.
 * This tool should be able to merge different quadrants in one bigger grid,
 * automatically recognising when this can be done by checking that the fields have the same
 * parameter, step, ... and compatible grid.
 * The current solution is to have a grib_merge working on an input file and writing in output the
 * merged fields working out automatically what has to be merged.
 */

#include "grib_tools.h"

#define MAX_KEY_VALUES 100

grib_handle *hh=0;
grib_values key_values[MAX_KEY_VALUES];
int key_values_size=MAX_KEY_VALUES;

/* This key was added to provide a unique ID for the product description used by the merge
 * tool to identify the fields to be merged */
static const char* md5Key = "md5Product";

char* grib_tool_description="Merge two fields with identical parameters and different geographical area";
char* grib_tool_name="grib_merge";
char* grib_tool_usage="[options] file file ... output_file";

grib_option grib_options[]={
        /*  {id, args, help}, on, command_line, value */
        {"f",0,0,0,1,0},
        {"c",0,0,1,0,0},
        {"r",0,0,0,1,0},
        {"q",0,0,1,0,0},
        {"p:",0,0,1,1,0},
        {"P:",0,0,0,1,0},
        {"B:",0,0,1,1,"md5Product"},
        {"V",0,0,0,1,0},
        {"W:",0,0,0,1,0},
        {"M",0,0,0,1,0},
        {"U",0,0,1,0,0},
        {"H",0,0,1,0,0},
        {"T:",0,0,1,0,"G"},
        {"S",0,0,1,0,0},
        {"g",0,0,0,1,0},
        {"G",0,0,0,1,0},
        {"7",0,0,0,1,0},
        {"v",0,0,0,1,0}
};

int grib_options_count=sizeof(grib_options)/sizeof(grib_option);

int main(int argc, char *argv[])
{
    int ret=grib_tool(argc,argv);
    return ret;
}

int grib_tool_before_getopt(grib_runtime_options* options)
{
    return 0;
}

int grib_tool_init(grib_runtime_options* options)
{
    return 0;
}

int grib_tool_new_filename_action(grib_runtime_options* options,const char* file)
{
    return 0;
}

int grib_tool_new_file_action(grib_runtime_options* options,grib_tools_file* file)
{
    return 0;
}

static int idx(double lat,double lon,double latFirst,double lonFirst,double latLast,double lonLast,
        long Ni,double di,double dj)
{
    long ilon,ilat;
    if ((ilon=(lon-lonFirst)/di) < 0 ) return -1;
    if ((ilat=(latFirst-lat)/dj) < 0 ) return -1;
    if (lon>lonLast) {
        if (lonLast==180) {
            lon-=360;
            ilon=(lon-lonFirst)/di;
        } else
            return -1;
    }
    if (lat<latLast) return -1;
    if ((ilat=(latFirst-lat)/dj) < 0 ) return -1;
    return ilon+ilat*Ni;
}

static grib_handle* merge(grib_handle* h1,grib_handle* h2)
{
    char s1[100]={0,};
    size_t len1;
    char s2[100]={0,};
    size_t len2;
    size_t sn;
    long l1,l2;
    double di,dj,di1,dj1,di2,dj2;
    long bitmapPresent;
    long n=0,n1,n2;
    double latFirst1,latFirst2,latFirst;
    double latLast1,latLast2,latLast;
    double lonFirst1,lonFirst2,lonFirst;
    double lonLast1,lonLast2,lonLast;
    double missingValue;
    double *v=0,*v1=0,*v2=0,*lat1=0,*lat2=0,*lon1=0,*lon2=0,*lat=0,*lon=0;
    long i,j,iscan,jscan,Ni,Nj,idj,idi;
    long Ni1,Nj1,Ni2,Nj2;
    grib_handle* h=NULL;
    int err=0;
    /*
    int dump_flags=   GRIB_DUMP_FLAG_CODED 
                    | GRIB_DUMP_FLAG_OCTECT 
                    | GRIB_DUMP_FLAG_VALUES 
                    | GRIB_DUMP_FLAG_READ_ONLY;

     */

    /* same products? */
    if (grib_key_equal(h1,h2,md5Key,GRIB_TYPE_STRING,&err)==0 && err==0) {
        return NULL;
    }

    /* can we do it?*/

    len2=sizeof(s2)/sizeof(*s2);
    err=grib_get_string(h2,"gridType",s2,&len2);
    if (strcmp(s2,"regular_ll")) {
        grib_context_log(h1->context,GRIB_LOG_WARNING,"gridType=%s not supported",s2);
        return NULL;
    }

    len1=sizeof(s1)/sizeof(*s1);
    err=grib_get_string(h1,"gridType",s1,&len1);
    if (strcmp(s1,"regular_ll")) {
        grib_context_log(h1->context,GRIB_LOG_WARNING,"gridType=%s not supported",s1);
        return NULL;
    }

    if (!grib_key_equal(h1,h2,"iDirectionIncrementInDegrees",GRIB_TYPE_DOUBLE,&err) ) {
        grib_context_log(h1->context,GRIB_LOG_WARNING,
                "unable to merge: different iDirectionIncrementInDegrees");
        return NULL;
    }
    if (!grib_key_equal(h1,h2,"jDirectionIncrementInDegrees",GRIB_TYPE_DOUBLE,&err) ) {
        grib_context_log(h1->context,GRIB_LOG_WARNING,
                "unable to merge: different jDirectionIncrementInDegrees");
        return NULL;
    }

    grib_get_long(h1,"latitudeOfFirstGridPoint",&l1);
    grib_get_long(h2,"latitudeOfFirstGridPoint",&l2);
    grib_get_long(h2,"jDirectionIncrement",&idj);
    if ( (l1-l2) % idj ) {
        grib_context_log(h1->context,GRIB_LOG_WARNING, "unable to merge: incompatible grid");
        return NULL;
    }

    grib_get_long(h1,"longitudeOfFirstGridPoint",&l1);
    grib_get_long(h2,"longitudeOfFirstGridPoint",&l2);
    grib_get_long(h2,"iDirectionIncrement",&idi);
    if ( (l1-l2) % idi ) {
        grib_context_log(h1->context,GRIB_LOG_WARNING, "unable to merge: incompatible grid");
        return NULL;
    }

    /* yes we can!*/

    /* do we have something to do?*/

    if ( grib_key_equal(h1,h2,"latitudeOfFirstGridPointInDegrees",GRIB_TYPE_DOUBLE,&err) &&
            grib_key_equal(h1,h2,"latitudeOfLastGridPointInDegrees",GRIB_TYPE_DOUBLE,&err)  &&
            grib_key_equal(h1,h2,"longitudeOfFirstGridPointInDegrees",GRIB_TYPE_DOUBLE,&err) &&
            grib_key_equal(h1,h2,"longitudeOfLastGridPointInDegrees",GRIB_TYPE_DOUBLE,&err)
    ) {
        /* no we don't */
        return NULL;
    }

    /* yes we do! */

    /* check scanning mode */
    grib_get_long(h1,"iScansNegatively",&iscan);
    if (iscan) grib_set_long(h1,"swapScanningLon",1);
    grib_get_long(h2,"iScansNegatively",&iscan);
    if (iscan) grib_set_long(h2,"swapScanningLon",1);

    grib_get_long(h1,"jScansPositively",&jscan);
    if (jscan) grib_set_long(h1,"swapScanningLat",1);
    grib_get_long(h2,"jScansPositively",&jscan);
    if (jscan) grib_set_long(h2,"swapScanningLat",1);

    grib_get_double(h1,"latitudeOfFirstGridPointInDegrees",&latFirst1);
    grib_get_double(h2,"latitudeOfFirstGridPointInDegrees",&latFirst2);
    grib_get_double(h1,"longitudeOfFirstGridPointInDegrees",&lonFirst1);
    grib_get_double(h2,"longitudeOfFirstGridPointInDegrees",&lonFirst2);

    latFirst = latFirst1>latFirst2 ? latFirst1 : latFirst2;
    lonFirst = lonFirst1<lonFirst2 ? lonFirst1 : lonFirst2;

    grib_get_double(h1,"latitudeOfLastGridPointInDegrees",&latLast1);
    grib_get_double(h2,"latitudeOfLastGridPointInDegrees",&latLast2);
    grib_get_double(h1,"longitudeOfLastGridPointInDegrees",&lonLast1);
    grib_get_double(h2,"longitudeOfLastGridPointInDegrees",&lonLast2);

    latLast = latLast1<latLast2 ? latLast1 : latLast2;
    lonLast = lonLast1>lonLast2 ? lonLast1 : lonLast2;

    grib_get_double(h1,"iDirectionIncrementInDegrees",&di1);
    grib_get_double(h1,"jDirectionIncrementInDegrees",&dj1);
    grib_get_double(h2,"iDirectionIncrementInDegrees",&di2);
    grib_get_double(h2,"jDirectionIncrementInDegrees",&dj2);
    grib_get_double(h1,"iDirectionIncrementInDegrees",&di);
    grib_get_double(h1,"jDirectionIncrementInDegrees",&dj);
    grib_get_long(h1,"Ni",&Ni1);
    grib_get_long(h1,"Nj",&Nj1);
    grib_get_long(h2,"Ni",&Ni2);
    grib_get_long(h2,"Nj",&Nj2);

    if (lonFirst==0 && lonLast==360) lonLast-=di;
    if (lonFirst==-180 && lonLast==180) { 
        lonFirst=0;
        lonLast=360-di;
    }

    /* create new grib for bigger area*/
    h=grib_handle_clone(h1);
    grib_set_double(h,"latitudeOfFirstGridPointInDegrees",latFirst);
    grib_set_double(h,"longitudeOfFirstGridPointInDegrees",lonFirst);
    grib_set_double(h,"latitudeOfLastGridPointInDegrees",latLast);
    grib_set_double(h,"longitudeOfLastGridPointInDegrees",lonLast);
    Ni=fabs(lonLast-lonFirst)/di+1;
    Nj=fabs(latLast-latFirst)/dj+1;

    grib_set_long(h,"Ni",Ni);
    grib_set_long(h,"Nj",Nj);

    grib_get_long(h,"bitmapPresent",&bitmapPresent);
    if (!bitmapPresent) grib_set_long(h,"bitmapPresent",1);

    grib_get_long(h,"numberOfPoints",&n);
    grib_get_double(h,"missingValue",&missingValue);
    v=(double*)grib_context_malloc_clear(h->context,sizeof(double)*n);
    for (i=0;i<n;i++) v[i]=1.0;
    sn=n;
    grib_set_double_array(h,"values",v,sn);
    for (i=0;i<n;i++) v[i]=missingValue;
    lat=(double*)grib_context_malloc_clear(h->context,sizeof(double)*n);
    lon=(double*)grib_context_malloc_clear(h->context,sizeof(double)*n);
    sn=n;
    grib_get_double_array(h,"latitudes",lat,&sn);
    grib_get_double_array(h,"longitudes",lon,&sn);

    grib_get_long(h1,"numberOfPoints",&n1);
    v1=(double*)grib_context_malloc_clear(h->context,sizeof(double)*n1);
    lat1=(double*)grib_context_malloc_clear(h->context,sizeof(double)*n1);
    lon1=(double*)grib_context_malloc_clear(h->context,sizeof(double)*n1);
    sn=n1;
    grib_get_double_array(h1,"latitudes",lat1,&sn);
    grib_get_double_array(h1,"longitudes",lon1,&sn);
    grib_get_double_array(h1,"values",v1,&sn);

    grib_get_long(h2,"numberOfPoints",&n2);
    v2=(double*)grib_context_malloc_clear(h->context,sizeof(double)*n2);
    lat2=(double*)grib_context_malloc_clear(h->context,sizeof(double)*n2);
    lon2=(double*)grib_context_malloc_clear(h->context,sizeof(double)*n2);
    sn=n2;
    grib_get_double_array(h2,"latitudes",lat2,&sn);
    grib_get_double_array(h2,"longitudes",lon2,&sn);
    grib_get_double_array(h2,"values",v2,&sn);

    for (i=0;i<n;i++) {
        if ((j=idx(lat[i],lon[i],latFirst1,lonFirst1,latLast1,lonLast1,Ni1,di1,dj1)) >=0 ) {
            v[i]=v1[j];
        } else if ( (j=idx(lat[i],lon[i],latFirst2,lonFirst2,latLast2,lonLast2,Ni2,di2,dj2))>=0) {
            v[i]=v2[j];
        }
    }

    grib_set_double_array(h,"values",v,n);

    return h;
}

int grib_tool_new_handle_action(grib_runtime_options* options, grib_handle* h)
{
    int err=0;
    grib_handle* hm=0;
    char md5[200]={0,};
    char fname[210]={0,};
    size_t lmd5=32;

    if (!hh) { hh=grib_handle_clone(h); return 0; }
    err = grib_get_string(h,md5Key,md5,&lmd5);
    if (err) {
        fprintf(stderr, "grib_merge error getting key %s: %s\n", 
                md5Key, grib_get_error_message(err));
        exit(err);
    }
    sprintf(fname,"_%s.orig.grib",md5);
    err = grib_write_message(h,fname,"a");

    if ((hm=merge(h,hh))==NULL ) {
        grib_tools_write_message(options,hh);
        lmd5=sizeof(md5)/sizeof(*md5);
        err = grib_get_string(hh,md5Key,md5,&lmd5);
        if (err) {
            fprintf(stderr, "grib_merge error getting key %s: %s\n", 
                    md5Key, grib_get_error_message(err));
            exit(err);
        }
        sprintf(fname,"_%s.merge.grib",md5);
        grib_write_message(hh,fname,"a");
    }
    grib_handle_delete(hh);
    hh = hm!=NULL ? hm : grib_handle_clone(h) ;

    return err;
}

int grib_tool_skip_handle(grib_runtime_options* options, grib_handle* h)
{
    grib_handle_delete(h);
    return 0;
}

void grib_tool_print_key_values(grib_runtime_options* options,grib_handle* h)
{
    grib_print_key_values(options,h);
}

int grib_tool_finalise_action(grib_runtime_options* options)
{
    grib_tools_write_message(options,hh);
    if (options->outfile->file) {
        fclose(options->outfile->file);
    }
    return 0;
}

int grib_no_handle_action(grib_runtime_options* options, int err)
{
    fprintf(dump_file,"\t\t\"ERROR: unreadable message\"\n");
    return 0;
}
