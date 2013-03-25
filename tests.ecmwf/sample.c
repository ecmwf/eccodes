#include <stdio.h>
#include <unistd.h>
#include "grib_api.h"

char buffer[1024*1024*20];

static char* names[] = {
	"localDefinitionNumber",
	"identificationOfOriginatingGeneratingCenter",
	"subCenterIdentification",
	"stream",
	"type",
	"localFlag",
	"section1.flag",
	"resolutionAndComponentFlags",
	"section4.flag",
	"numberOfPointsAlongAParallel",
	"dataRepresentationType",
	"pvlLocation",
	"flagShowingPostAuxiliaryArrayInUse",
	"resolutionAndComponentFlags",
	"numberOfLocalDefinitions",
	"timeRangeIndicator",
	"indicatorOfUnitOfTimeRange",
	"indicatorOfTypeOfLevel",
	"flagShowingPostAuxiliaryArrayInUse",
	"spaceUnitFlag",
	"timeUnitFlag",
	"mixedCoordinateFieldFlag",
	"coordinate1Flag",
	"averagingFlag",
	"coordinate2Flag",
	"coordinate3Flag",
	"coordinate4Flag",
	"flagForIrregularGridCoordinateList",
	"flagForNormalOrStaggeredGrid",
	"flagForAnyFurtherInformation",
};

char *dir = "/vol/dataserv/grib_samples/incoming/";

main(int argc,char** argv)
{
	long length = 0;
	FILE *f;
	grib_handle  *g =  0;
	int e;
	size_t size;
	char ext[5] = {0,};

	f = fopen(argc > 1 ? argv[1]: "data" ,"r");
	if(!f)
	{
		perror(argc > 1 ? argv[1]: "data");
		exit(1);
	}

	length = sizeof(buffer);


	while( (e = readany(f,buffer,&length)) == 0)
	{
		char fname[1024];
		int i;

		g = grib_handle_new_from_message(0,buffer,length);

		strncpy(ext,buffer,4);
		ext[0] = tolower(ext[0]);
		ext[1] = tolower(ext[1]);
		ext[2] = tolower(ext[2]);
		ext[3] = tolower(ext[3]);


		strcpy(fname,"./");
		strcpy(fname,"/vol/dataserv/grib_samples/brute_force/");
#if 1
		for(i = 0; i < sizeof(names)/sizeof(names[0]); i++)
		{
			char val[1024];
			size_t size = sizeof(val);
			e = grib_get_string(g,names[i],val,&size);
			if(e)
				strcat(fname,"na");
			else
				strcat(fname,val);
			strcat(fname,"_");
		}

		strcat(fname,".");
		strcat(fname,ext);

		if(access(fname,F_OK) != 0)
		{
			FILE* out = fopen(fname,"w");
			if(!out) {
					perror(fname);
					exit(1);
			}

			if(fwrite(buffer,1,length,out) != length)
			{
				perror(fname);
				exit(1);
			}

			if(fclose(out))
			{
				perror(fname);
				exit(1);
			}
		}
#else

		for(i = 0; i < sizeof(names)/sizeof(names[0]); i++)
		{
			char val[1024];
			size_t size = sizeof(val);
			e = grib_get_string(g,names[i],val,&size);
			if(e == 0)
			{
				sprintf(fname,"%s/%s=%s.%s",
						dir,
						names[i],
						val,
						ext);

				if(access(fname,F_OK) != 0)
				{
					FILE* out = fopen(fname,"w");
					if(!out) {
						perror(fname);
						exit(1);
					}

					if(fwrite(buffer,1,length,out) != length)
					{
						perror(fname);
						exit(1);
					}

					if(fclose(out))
					{
						perror(fname);
						exit(1);
					}
				}

			}
		}
#endif

		/* grib_iterator_delete(i); */

		grib_handle_delete(g);


		length = sizeof(buffer);
	}

	if(e != -1)
	{
		fprintf(stderr,"readany error %d\n",e);
		exit(1);
	}


	exit(0);
}
