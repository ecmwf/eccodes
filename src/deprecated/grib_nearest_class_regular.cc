static int find(grib_nearest* nearest, grib_handle* h,
        double inlat, double inlon,unsigned long flags,
        double* outlats,double* outlons,
        double *values,double *distances,int* indexes, size_t *len) {
    grib_nearest_regular* self = (grib_nearest_regular*) nearest;
    int ret=0,kk=0,ii=0,jj=0;
    size_t nvalues=0;
    double radiusInKm;

    if ((ret = grib_nearest_get_radius(h, &radiusInKm)) != GRIB_SUCCESS)
        return ret;

    if (!nearest->h || (flags & GRIB_NEAREST_SAME_DATA)==0 || nearest->h!=h) {
        grib_iterator* iter=NULL;
        double lat=0,lon=0;

        if( (ret =  grib_get_size(h,self->values_key,&nvalues))!= GRIB_SUCCESS)
            return ret;
        nearest->values_count = nvalues;
        if (nearest->values) grib_context_free(nearest->context,nearest->values);
        nearest->values = grib_context_malloc(h->context,nvalues*sizeof(double));
        if (!nearest->values) return GRIB_OUT_OF_MEMORY;

        ret=grib_get_double_array_internal( h,self->values_key,
                nearest->values,&(nearest->values_count));
        if (ret!=GRIB_SUCCESS) grib_context_log(nearest->context,GRIB_LOG_ERROR,
                "nearest: unable to get values array");

        if (!nearest->h || (flags & GRIB_NEAREST_SAME_GRID)==0) {
            double dummy=0;
            double olat=1.e10, olon=1.e10;
            int ilat=0,ilon=0;
            long n=0;

            if( (ret =  grib_get_long(h,self->Ni,&n))!= GRIB_SUCCESS)
                return ret;
            self->lons_count=n;

            if( (ret =  grib_get_long(h,self->Nj,&n))!= GRIB_SUCCESS)
                return ret;
            self->lats_count=n;

            if (self->lats) grib_context_free(nearest->context,self->lats);
            self->lats=grib_context_malloc( nearest->context,
                    self->lats_count* sizeof(double));
            if (!self->lats) return GRIB_OUT_OF_MEMORY;

            if (self->lons) grib_context_free(nearest->context,self->lons);
            self->lons=grib_context_malloc( nearest->context,
                    self->lons_count*sizeof(double));
            if (!self->lons) return GRIB_OUT_OF_MEMORY;

            iter=grib_iterator_new(h,0,&ret);
            if (ret) {
                grib_context_log(nearest->context,GRIB_LOG_ERROR,"unable to create iterator");
                return ret;
            }
            while(grib_iterator_next(iter,&lat,&lon,&dummy)) {
                if (olat != lat) {
                    self->lats[ilat++]=lat;
                    olat=lat;
                }
                if (ilon<self->lons_count && olon != lon) {
                    self->lons[ilon++]=lon;
                    olon=lon;
                }
            }
            grib_iterator_delete(iter);

        }
        nearest->h=h;
    }

    if (!self->distances || (flags & GRIB_NEAREST_SAME_POINT)==0
            || (flags & GRIB_NEAREST_SAME_GRID)==0) {

        grib_binary_search(self->lats,self->lats_count-1,inlat,
                &(self->j[0]),&(self->j[1]));
        grib_binary_search(self->lons,self->lons_count-1,inlon,
                &(self->i[0]),&(self->i[1]));
        if (!self->distances)
            self->distances=(double*)grib_context_malloc( nearest->context,NUM_NEIGHBOURS*sizeof(double));
        if (!self->k)
            self->k=(int*)grib_context_malloc( nearest->context,NUM_NEIGHBOURS*sizeof(int));
        kk=0;
        for (ii=0;ii<2;ii++) {
            for (jj=0;jj<2;jj++) {
                self->k[kk]=self->i[ii]+self->lons_count*self->j[jj]-1;
                self->distances[kk]=geographic_distance_spherical(radius,inlon,inlat,
                        self->lons[self->i[ii]],self->lats[self->j[jj]]);
                kk++;
            }
        }
    }

    kk=0;
    for (ii=0;ii<2;ii++) {
        for (jj=0;jj<2;jj++) {
            distances[kk]=self->distances[kk];
            outlats[kk]=self->lats[self->j[jj]];
            outlons[kk]=self->lons[self->i[ii]];
            values[kk]=nearest->values[self->k[kk]];
            indexes[kk]=self->k[kk];
            kk++;
        }
    }

    return GRIB_SUCCESS;
}
