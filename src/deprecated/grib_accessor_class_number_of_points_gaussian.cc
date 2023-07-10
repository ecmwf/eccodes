#if 0
/*Legacy mode*/
static long num_points_reduced_gauss_old(grib_handle* h, long nj, long pl[],
                                         long max_pl, double lats[],
                                         double angular_precision,
                                         double lat_first, double lat_last,
                                         double lon_first, double lon_last)
{
    long result=0;
    int is_global=0;
    size_t plsize=0;
    long ilon_first=0,ilon_last=0;
    double lon_first_row=0,lon_last_row=0;
    float d = 0;
    is_global=is_gaussian_global(lat_first,lat_last,lon_first,lon_last,max_pl,lats,angular_precision);
    d=fabs(lats[0]-lats[1]);
    if ( !is_global ) {
        long j = 0;
        /*sub area*/
        (void)d;
#if EFDEBUG
        printf("-------- subarea fabs(lat_first-lats[0])=%g d=%g\n",fabs(lat_first-lats[0]),d);
        printf("-------- subarea fabs(lat_last+lats[0])=%g d=%g\n",fabs(lat_last+lats[0]),d);
        printf("-------- subarea lon_last=%g order=%ld 360.0-90.0/order=%g\n",
                lon_last,order,360.0-90.0/order);
        printf("-------- subarea lon_first=%g fabs(lon_last  -( 360.0-90.0/order))=%g 90.0/order=%g\n",
                lon_first,fabs(lon_last  - (360.0-90.0/order)),90.0/order);
#endif
        for (j=0;j<nj;j++) {
            long row_count=0;
#if EFDEBUG
            printf("--  %d ",j);
#endif
            grib_get_reduced_row(pl[j],lon_first,lon_last,&row_count,&ilon_first,&ilon_last);
            lon_first_row=((ilon_first)*360.0)/pl[j];
            lon_last_row=((ilon_last)*360.0)/pl[j];
            result += row_count;
            (void)lon_last_row;
            (void)lon_first_row;
#if EFDEBUG
            printf("        ilon_first=%ld lon_first=%.10e ilon_last=%ld lon_last=%.10e count=%ld row_count=%ld\n",
                    ilon_first,lon_first_row,ilon_last,lon_last_row,result,row_count);
#endif
        }
    } else {
        int i = 0;
        result=0;
        for (i=0;i<plsize;i++) result += pl[i];
    }
    return result;
}

/* New MIR compatible way */
static long num_points_reduced_gauss_new(grib_handle* h, long nj, long pl[], double lon_first, double lon_last)
{
    long result = 0;
    long j;
    /* Always assume sub area */
    for (j=0;j<nj;j++) {
        long row_count=0;
        long ilon_first=0,ilon_last=0;
        grib_get_reduced_row2(pl[j], lon_first, lon_last, &row_count, &ilon_first, &ilon_last);
        result += row_count;
    }
    return result;
}
#endif
