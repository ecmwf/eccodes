#include <stdio.h>

#include "grib_api_internal.h"
#include "netcdf.h"
#include "test.h"

void handle_error(int err,const char* caller);
void preproc(double* values,double bias,int lenght,int mode);

int main(int argc, char *argv[])
{
	FILE *f;
	FILE *fl;
	int i,ii,jj;
	grib_context* gc;
	grib_handle* gh;
	double* grib_values;
	double* nc_values;
	size_t *pstart,*pcount;
	int dimids[NC_MAX_VAR_DIMS];
	int varid,ndims;
	char varname[NC_MAX_NAME+1];
	int dimlen,length;
	int* dimlens;
	int err,ncid,natts;
	nc_type vartype;
	double nc_vmax,nc_vmin,val;
	int nc_imax,nc_imin;
	double grib_vmax,grib_vmin;
	int grib_imax,grib_imin;
	int latid,lonid;
	size_t size        = 0;
  const void* buffer = NULL;
	int ret=0;
	double max_error=0;
	int id_max_error=0;
#if 0
	double R,xscale,scaled_vmax;
	int D;
#endif
	int numberOfFigures;
	double referenceValue;
	long decimalScaleFactor,numberOfBitsContainingEachPackedValue,binaryScaleFactor;
	long typeOfPreProcessing;
	double preProcessingParameter;
		
	/* ---------- Variables ------------*/
	varid=5;
	latid=2;
	lonid=3;
	numberOfFigures=4;
	/*---------------------------------*/
	
	gc = grib_context_get_default();
	
	gh=grib_handle_new_from_samples(NULL,"GRIB1");
	
	if (!gh) {
		printf("Error: unable to create grib_handle\n");
		exit(1);
	}
	 
	grib_set_long(gh,"editionNumber",2);
	grib_set_long(gh,"gridDefinitionTemplateNumber",61);
	
  err = nc_open("nc_data", NC_NOWRITE, &ncid);
	handle_error(err,"nc_open");
	
	err=nc_inq_var(ncid,varid,varname,&vartype,&ndims,dimids,&natts);
  handle_error(err,"nc_inq_var");
	
	pstart=(size_t*)malloc(ndims*sizeof(size_t));
	pcount=(size_t*)malloc(ndims*sizeof(size_t));
	dimlens=(int*)malloc(ndims*sizeof(int));
	
	length=1;
	for (i=0;i<ndims;i++) {
		err=nc_inq_dimlen(ncid,dimids[i],&dimlen);
		handle_error(err,"nc_inq_dimlen");
		*(dimlens+i)=dimlen;
		printf("dimlens[%d]=%d\n",i,*(dimlens+i));
		if ( i == latid || i == lonid) {
			length*=dimlen;
			(*(pcount+i))=dimlen;
		} else (*(pcount+i))=1;
		(*(pstart+i))=0;
		printf("pcount[%d]=%d\n",i,*(pcount+i));
	}
	
	nc_values=(double*) malloc(length*sizeof(double));
	if (!nc_values) printf("malloc unable to allocate %d doubles\n",length);
	
	grib_values=(double*) malloc(length*sizeof(double));
	if (!grib_values) printf("malloc unable to allocate %d doubles\n",length);
	
	
	f = fopen("out.grb","w");
  if(!f) {
      perror("out.grb");
      exit(1);
  }
	
	fl = fopen("chemical.log","w");
  if(!f) {
      perror("chemical.log");
      exit(1);
  }
	
	printf("varname=%s\n",varname);
	fprintf(fl,"varname=%s\n",varname);
	fprintf(fl,"npoints,nc_vmax,nc_vmin,max_relative_error,nc_values[id_max_error],grib_values[id_max_error],id_max_error\n");
	
	grib_get_long(gh,"typeOfPreProcessing",&typeOfPreProcessing);
	grib_get_long(gh,"numberOfBitsContainingEachPackedValue",&numberOfBitsContainingEachPackedValue);
	fprintf(fl,"typeOfPreProcessing=%ld numberOfBitsContainingEachPackedValue=%ld\n",typeOfPreProcessing,numberOfBitsContainingEachPackedValue);
	
	for (ii=0;ii<dimlens[0];ii++) {
		(*(pstart))=ii;

		for (jj=0;jj<dimlens[1];jj++) {
			(*(pstart+1))=jj;
			
			err=nc_get_vara_double(ncid,varid,pstart,pcount,nc_values);
			handle_error(err,"nc_get_vara_double");
			
			
			nc_vmax=nc_values[0];
			nc_vmin=nc_values[0];
			nc_imin=0;
			nc_imin=0;
			
			for (i=0;i<length;i++) {
				val=nc_values[i];
				if (val>nc_vmax) {
					nc_vmax=val;
					nc_imax=i;
				}
				if (val<nc_vmin) {
					nc_vmin=val;
					nc_imin=i;
				}
			}
#if 0
			nc_vmin=0;
			nc_values[nc_imin]=0;
#endif
#if 0
			R=nc_vmin;
			xscale=1;
			for (i=0;i<numberOfFigures;i++) xscale*= 10.0;
			D=0;
			if (R !=0) {
				while ( R < xscale ) {
					R*=10.0;
					D++;
				}
			}
			scaled_vmax=1;
			for (i=0;i<D;i++) scaled_vmax*= 10.0;
			scaled_vmax=nc_vmax*scaled_vmax-R;
#endif
			for (i=0;i<length;i++) grib_values[i]=nc_values[i];
			grib_set_double_array(gh,"values",grib_values,length);
			
			for (i=0;i<length;i++) grib_values[i]=0.0;
			grib_get_double_array(gh,"values",grib_values,&length);
			
			grib_vmax=grib_values[0];
			grib_vmin=grib_values[0];
			grib_imax=0;
			grib_imin=0;
			
			for (i=0;i<length;i++) {
				val=grib_values[i];
				if (val>grib_vmax) {
					grib_vmax=val;
					grib_imax=i;
				}
				if (val<grib_vmin) {
					grib_vmin=val;
					grib_imin=i;
				}
			}
			
			printf("---- idim[1]=%d  idim[2]=%d -----\n",ii+1,jj+1);
#if 0
	    printf("-- D=%d  R=%f scaled_vmax=%.30e\n",D,R,scaled_vmax);
#endif
			printf("-- nc   -- npoints=%d   nc_vmax=%.30e   nc_imax=%d   nc_vmin=%.30e   nc_imin=%d\n",
				length,nc_vmax,nc_imax,nc_vmin,nc_imin);
			printf("-- grib -- npoints=%d grib_vmax=%.30e   nc_imax=%d grib_vmin=%.30e   nc_imin=%d\n",
				length,grib_values[nc_imax],nc_imax,grib_values[nc_imin],nc_imin);
			printf("-- nc   -- npoints=%d   nc_vmax=%.30e grib_imax=%d   nc_vmin=%.30e grib_imin=%d\n",
				length,nc_values[grib_imax],grib_imax,nc_values[grib_imin],grib_imin);
			printf("-- grib -- npoints=%d grib_vmax=%.30e grib_imax=%d grib_vmin=%.30e grib_imin=%d\n",
				length,grib_values[grib_imax],grib_imax,grib_values[grib_imin],grib_imin);
			
			grib_get_double(gh,"referenceValue",&referenceValue);
			grib_get_long(gh,"numberOfBitsContainingEachPackedValue",&numberOfBitsContainingEachPackedValue);
			grib_get_long(gh,"binaryScaleFactor",&binaryScaleFactor);
			grib_get_long(gh,"decimalScaleFactor",&decimalScaleFactor);
			grib_get_long(gh,"typeOfPreProcessing",&typeOfPreProcessing);
			grib_get_double(gh,"preProcessingParameter",&preProcessingParameter);
			
			printf("decimalScaleFactor=%ld referenceValue=%.10e binaryScaleFactor=%ld numberOfBitsContainingEachPackedValue=%ld \n",decimalScaleFactor,referenceValue,binaryScaleFactor,numberOfBitsContainingEachPackedValue);
			printf("typeOfPreProcessing=%ld  preProcessingParameter=%g\n",typeOfPreProcessing,preProcessingParameter);
			
			printf("bucket_size=%g\n",bucket_size( gh));
			
			max_error=0;
			id_max_error=0;
			get_max_relative_error(grib_values,nc_values,length,&max_error,&id_max_error);
			fprintf(fl,"%d \t%.16e\t%.16e\t%.16e\t%.16e\t%.16e\t%d\n",
				length,nc_vmax,nc_vmin,max_error,nc_values[id_max_error],grib_values[id_max_error],id_max_error);
			compare_values( grib_values,nc_values,length,"");
#if 0
			if (ii==dimlens[0]-1 && jj==dimlens[1]-1) {
				for (i=0;i<length;i++) {
					printf("i           %d\n",i);
					printf("nc      %.40e  \n",nc_values[i]);
					printf("grib    %.40e\n",grib_values[i]);
				}
			}
#endif
			grib_get_message(gh,&buffer,&size);

  		if(fwrite(buffer,1,size,f) != size) {
      	perror(argv[1]);
      	exit(1);
  		}
		}
	}
	
	free(nc_values);
	free(grib_values);
	
  if(fclose(f)) {
     perror(argv[1]);
     exit(1);
  }
	
	fclose(fl);

  grib_dump_content(gh,stdout,NULL,0,NULL);
	grib_handle_delete(gh);
	
	return ret;

}

void handle_error(int err,const char* caller) {
      if (err != NC_NOERR) {
        printf("%s error: %d\n",caller,err);
        exit(err);
      }
}

