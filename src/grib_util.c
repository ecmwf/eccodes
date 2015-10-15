/*
 * Copyright 2005-2015 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

#ifdef ECCODES_ON_WINDOWS
 /* Replace C99/Unix rint() for Windows Visual C++ (only before VC++ 2013 versions) */
 #if defined _MSC_VER && _MSC_VER < 1800
 double rint(double x)
 {
     char * buf = 0;
     int decimal=0, sign=0, err = 0;
     double result = 0;
     buf = (char*) malloc(_CVTBUFSIZE);
     err = _fcvt_s(buf, _CVTBUFSIZE, x, 0, &decimal, &sign);
     Assert(err == 0);
     result = atof(buf);
     if(sign == 1) {
         result = result * -1;
     }
     free(buf);
     return result;
 }
 #endif
#endif

static void set_total_length(unsigned char* buffer,long *section_length,long *section_offset,int edition,size_t totalLength) {
    long off;
    switch (edition) {
    case 1:
        if(totalLength < 0x800000 ) {
            off=32;
            grib_encode_unsigned_long(buffer, (unsigned long)totalLength ,  &off, 24);
        } else {
            long s4len,t120;
            totalLength -= 4;
            t120  = (totalLength+119)/120;
            s4len  = t120*120 - totalLength;
            totalLength  = 0x800000 | t120;
            off=32;
            grib_encode_unsigned_long(buffer, (unsigned long)totalLength ,  &off, 24);
            off=section_offset[4]*8;
            grib_encode_unsigned_long(buffer, (unsigned long)s4len ,  &off, 24);
        }
        break;
    case 2:
        off=64;
        grib_encode_unsigned_long(buffer, (unsigned long)totalLength ,  &off, 64);
        break;
    }

}

static grib_handle* grib_sections_copy_internal(grib_handle* hfrom,grib_handle* hto,int sections[],int *err) {
    int i;
    size_t totalLength=0;
    unsigned char* buffer;
    unsigned char *p;
    long edition=0;
    long section_length[MAX_NUM_SECTIONS]={0,};
    long section_offset[MAX_NUM_SECTIONS]={0,};
    long off=0;
    grib_handle* h;
    char section_length_str[]="section0Length";
    char section_offset_str[]="offsetSection0";
    long length,offset;

    *err=grib_get_long(hfrom,"edition",&edition);
    if (*err) return NULL;

    for (i=0;i<=hfrom->sections_count;i++) {

        if (sections[i]) {h=hfrom;}
        else {h=hto;}

        sprintf(section_length_str,"section%dLength",i);
        if (grib_get_long(h,section_length_str,&length)) continue;
        section_length[i]=length;

        sprintf(section_offset_str,"offsetSection%d",i);
        if (grib_get_long(h,section_offset_str,&offset)) continue;
        section_offset[i]=offset;

        totalLength+=section_length[i];

    }

    buffer=(unsigned char*)grib_context_malloc_clear(hfrom->context,totalLength*sizeof(char));

    p=buffer;
    off=0;
    for (i=0;i<=hfrom->sections_count;i++) {
        grib_handle* h;
        if (sections[i]) h=hfrom;
        else h=hto;
        p=(unsigned char*)memcpy(p,h->buffer->data+section_offset[i],section_length[i]);
        section_offset[i]=off;
        off+=section_length[i];
        p+=section_length[i];
    }

    /* copy section 3 present flag*/
    if (edition==1) {
        const void* buffer_to=NULL;
        size_t size_to=0;
        grib_get_message(hto,&buffer_to,&size_to);
        memcpy(buffer+15,((unsigned char*)buffer_to)+15,1);
    }

    set_total_length(buffer,section_length,section_offset,edition,totalLength);

    h=grib_handle_new_from_message(hfrom->context,buffer,totalLength);

    /*to allow free of buffer*/
    h->buffer->property = GRIB_MY_BUFFER;

    switch (edition) {
    case 1:
        if (sections[1] && sections[2]) break;

        if (sections[1]) {
            long PVPresent;
            grib_get_long(hfrom,"PVPresent",&PVPresent);
            if (PVPresent) {
                double *pv;
                long numberOfVerticalCoordinateValues;
                size_t size=0;

                grib_get_long(hfrom,"numberOfVerticalCoordinateValues",&numberOfVerticalCoordinateValues);
                size=numberOfVerticalCoordinateValues;
                pv=(double*)grib_context_malloc_clear(hfrom->context,numberOfVerticalCoordinateValues*sizeof(double));
                grib_get_double_array(hfrom,"pv",pv,&size);
                grib_set_long(h,"PVPresent",1);
                grib_set_double_array(h,"pv",pv,size);

                grib_context_free(hfrom->context,pv);

            } else {
                grib_set_long(h,"PVPresent",0);
            }
        }
        if (sections[2]) {
            long PVPresent;
            grib_get_long(hto,"PVPresent",&PVPresent);
            if (PVPresent) {
                double *pv;
                long numberOfVerticalCoordinateValues;
                size_t size=0;

                grib_get_long(hto,"numberOfVerticalCoordinateValues",&numberOfVerticalCoordinateValues);
                size=numberOfVerticalCoordinateValues;
                pv=(double*)grib_context_malloc_clear(hto->context,numberOfVerticalCoordinateValues*sizeof(double));
                grib_get_double_array(hto,"pv",pv,&size);
                grib_set_long(h,"PVPresent",1);
                grib_set_double_array(h,"pv",pv,size);

                grib_context_free(hto->context,pv);

            } else {
                grib_set_long(h,"PVPresent",0);
            }
        }
        break;
    case 2:
        if (sections[1]) {
            long discipline;
            grib_get_long(hfrom,"discipline",&discipline);
            grib_set_long(h,"discipline",discipline);
        }
        break;
    }

    return h;
}

grib_handle* grib_util_sections_copy(grib_handle* hfrom,grib_handle* hto,int what,int *err) {
    long edition_from=0;
    long edition_to=0;
    long localDefinitionNumber=-1;
    int sections_to_copy[MAX_NUM_SECTIONS]={0,};

    *err=grib_get_long(hfrom,"edition",&edition_from);
    if (*err) return NULL;
    *err=grib_get_long(hto,"edition",&edition_to);
    if (*err) return NULL;

    if (edition_to != 1 && edition_to != 2 ) {
        *err=GRIB_NOT_IMPLEMENTED;
        return NULL;
    }

    if (edition_from!=edition_to) {
        *err=GRIB_DIFFERENT_EDITION;
        return NULL;
    }

    if (what & GRIB_SECTION_GRID) {
        switch (edition_from) {
        case 1:
            sections_to_copy[2]=1;
            break;
        case 2:
            sections_to_copy[3]=1;
            break;
        }
    }

    if (what & GRIB_SECTION_DATA) {
        switch (edition_from) {
        case 1:
            sections_to_copy[3]=1;
            sections_to_copy[4]=1;
            break;
        case 2:
            sections_to_copy[5]=1;
            sections_to_copy[6]=1;
            sections_to_copy[7]=1;
            break;
        }
    }

    if (what & GRIB_SECTION_LOCAL) {
        switch (edition_from) {
        case 1:
            sections_to_copy[1]=1;
            break;
        case 2:
            sections_to_copy[2]=1;
            break;
        }
    }

    if (what & GRIB_SECTION_PRODUCT) {
        switch (edition_from) {
        case 1:
            grib_get_long(hfrom,"localDefinitionNumber",&localDefinitionNumber);
            if (localDefinitionNumber==13) {
                sections_to_copy[4]=1;
            }
            sections_to_copy[1]=1;
            break;
        case 2:
            sections_to_copy[1]=1;
            sections_to_copy[4]=1;
            break;
        }
    }

    if (what & GRIB_SECTION_BITMAP) {
        switch (edition_from) {
        case 1:
            sections_to_copy[3]=1;
            break;
        case 2:
            sections_to_copy[6]=1;
            break;
        }
    }

    return grib_sections_copy_internal(hfrom,hto,sections_to_copy,err);

}

static grib_trie* init_list(const char* name);
static grib_trie* param_id_list = NULL;
static grib_trie* mars_param_list = NULL;
/* TODO thread safe */
grib_string_list* grib_util_get_param_id(const char* mars_param) {
    if (!mars_param_list && (mars_param_list=init_list("mars_param.table"))==NULL) return NULL;
    return (grib_string_list*)grib_trie_get(mars_param_list,mars_param);
}

grib_string_list* grib_util_get_mars_param(const char* param_id) {
    if (!param_id_list && (param_id_list=init_list("param_id.table"))==NULL) return NULL;
    return (grib_string_list*)grib_trie_get(param_id_list,param_id);
}

static grib_trie* init_list(const char* name) {
    char *full_path=0;
    FILE* fh;
    char s[101];
    char param[101];
    grib_string_list* list=0;
    grib_string_list* next=0;
    grib_trie* trie_list;
    grib_context* c=grib_context_get_default();
    full_path=grib_context_full_defs_path(c,name);

    fh=fopen(full_path,"r");
    if (!fh) {
        grib_context_log(c,GRIB_LOG_PERROR,"unable to read %s",full_path);
        return NULL;
    }

    list=(grib_string_list*)grib_context_malloc_clear(c,sizeof(grib_string_list));
    trie_list=grib_trie_new(c);
    if (fscanf(fh,"%100s",param)==EOF) {
        fclose(fh);
        return NULL;
    }
    while (fscanf(fh,"%100s",s)!=EOF) {
        if (!strcmp(s,"|")) {
            grib_trie_insert(trie_list, param,list);
            if (fscanf(fh,"%100s",param)==EOF) {
                fclose(fh);
                return trie_list;
            }
            list=NULL;
        } else {
            if (!list) {
                list=(grib_string_list*)grib_context_malloc_clear(c,sizeof(grib_string_list));
                list->value=grib_context_strdup(c,s);
            } else {
                next=list;
                while(next->next) next=next->next;
                next->next=(grib_string_list*)grib_context_malloc_clear(c,sizeof(grib_string_list));
                next->next->value=grib_context_strdup(c,s);
            }
        }
    }

    fclose(fh);
    return 0;
}

static void print_values(grib_context* c, const grib_util_grid_spec* spec, const double* data_values,size_t data_values_count,const grib_values *values,int count)
{
    int i;
    printf("ECCODES DEBUG grib_util grib_set_values: setting %d values \n",count);

    for(i = 0; i < count ; i++)
    {
        switch(values[i].type)
        {
        case GRIB_TYPE_LONG: printf("ECCODES DEBUG grib_util: => %s =  %ld;\n"
                ,values[i].name,(long)values[i].long_value); break;
        case GRIB_TYPE_DOUBLE: printf("ECCODES DEBUG grib_util: => %s = %.16e;\n"
                ,values[i].name,values[i].double_value); break;
        case GRIB_TYPE_STRING: printf("ECCODES DEBUG grib_util: => %s = \"%s\";\n"
                ,values[i].name,values[i].string_value); break;
        }
    }

    if(spec->bitmapPresent) {
        int missing = 0;
        double min = 1e100;
        for(i = 0; i < data_values_count ; i++)
        {
            double d = data_values[i] - spec->missingValue;
            if(d < 0) d = -d;

            if(d < min) {
                min = d;
            }

            if(data_values[i] == spec->missingValue)
                missing++;
        }
    }
}

grib_handle* grib_util_set_spec(grib_handle* h,
        const grib_util_grid_spec    *spec,
        const grib_util_packing_spec *packing_spec,
        int                           flags,
        const double*                 data_values,
        size_t                        data_values_count,
        int*                          err)
{

#define SET_LONG_VALUE(n,v)   do { Assert(count<1024); values[count].name = n; values[count].type = GRIB_TYPE_LONG;   values[count].long_value = v; count++; } while(0)
#define SET_DOUBLE_VALUE(n,v) do { Assert(count<1024); values[count].name = n; values[count].type = GRIB_TYPE_DOUBLE; values[count].double_value = v; count++; } while(0)
#define SET_STRING_VALUE(n,v) do { Assert(count<1024); values[count].name = n; values[count].type = GRIB_TYPE_STRING;   values[count].string_value = v; count++; } while(0)

#define COPY_SPEC_LONG(x)     do { Assert(count<1024); values[count].name = #x; values[count].type = GRIB_TYPE_LONG;   values[count].long_value = spec->x; count++; } while(0)
#define COPY_SPEC_DOUBLE(x)   do { Assert(count<1024); values[count].name = #x; values[count].type = GRIB_TYPE_DOUBLE; values[count].double_value = spec->x; count++; } while(0)

    grib_values  values[1024]={{0,},};
    size_t       count = 0;
    int i;
    long editionNumber;
    grib_handle* outh = NULL;
    grib_handle* tmp = NULL;
    const char*  grid_type = NULL;
    char name[1024];
    char input_grid_type[100];
    char input_packing_type[100];
    long input_bits_per_value=0;
    long input_decimal_scale_factor=0;
    size_t len=100;
    size_t input_grid_type_len=100;
    double laplacianOperator;
    int packingTypeIsSet=0;
    int setSecondOrder=0;
    size_t slen=17;

    static grib_util_packing_spec default_packing_spec = {0, };
    Assert(h);

    if(!packing_spec) {
        packing_spec = &default_packing_spec;
    }

    /* Get edition number from input handle */
    if((*err = grib_get_long(h,"edition",&editionNumber)) != 0)
    {
        grib_context* c=grib_context_get_default();
        if (c->write_on_fail)
            grib_write_message(h,"error.grib","w");
        return NULL;
    }

    if (packing_spec->deleteLocalDefinition) {
        /* TODO: not working for grib2 */
        SET_LONG_VALUE("deleteLocalDefinition",1);
    }

    len=100;
    grib_get_string(h,"packingType",input_packing_type,&len);
    grib_get_long(h,"bitsPerValue",&input_bits_per_value);
    grib_get_long(h,"decimalScaleFactor",&input_decimal_scale_factor);
    if (h->context->debug==-1) {
        printf("ECCODES DEBUG grib_util: input_packing_type = %s\n",input_packing_type);
        printf("ECCODES DEBUG grib_util: input_bits_per_value = %ld\n",input_bits_per_value);
        printf("ECCODES DEBUG grib_util: input_decimal_scale_factor = %ld\n",input_decimal_scale_factor);
    }

    if (flags & GRIB_UTIL_SET_SPEC_FLAGS_ONLY_PACKING) {
        if (packing_spec->packing == GRIB_UTIL_PACKING_USE_PROVIDED &&
                strcmp(input_packing_type,"grid_simple_matrix")) {
            switch (packing_spec->packing_type) {
            case GRIB_UTIL_PACKING_TYPE_SPECTRAL_COMPLEX:
                if (strcmp(input_packing_type,"spectral_complex") && !strcmp(input_packing_type,"spectral_simple"))
                    SET_STRING_VALUE("packingType","spectral_complex");
                break;
            case GRIB_UTIL_PACKING_TYPE_SPECTRAL_SIMPLE:
                if (strcmp(input_packing_type,"spectral_simple") && !strcmp(input_packing_type,"spectral_complex"))
                    SET_STRING_VALUE("packingType","spectral_simple");
                break;
            case GRIB_UTIL_PACKING_TYPE_GRID_SIMPLE:
                if (strcmp(input_packing_type,"grid_simple") && !strcmp(input_packing_type,"grid_complex") )
                    SET_STRING_VALUE("packingType","grid_simple");
                break;
            case GRIB_UTIL_PACKING_TYPE_GRID_SIMPLE_MATRIX:
                SET_STRING_VALUE("packingType","grid_simple_matrix");
                break;
            case GRIB_UTIL_PACKING_TYPE_GRID_COMPLEX:
                if (strcmp(input_packing_type,"grid_complex") && !strcmp(input_packing_type,"grid_simple"))
                    SET_STRING_VALUE("packingType","grid_complex");
                break;
            case GRIB_UTIL_PACKING_TYPE_JPEG:
                if (strcmp(input_packing_type,"grid_jpeg") && !strcmp(input_packing_type,"grid_simple"))
                    SET_STRING_VALUE("packingType","grid_jpeg");
                break;
            case GRIB_UTIL_PACKING_TYPE_GRID_SECOND_ORDER:
                /* we delay the set of grid_second_order because we don't want
                   to do it on a field with bitsPerValue=0 */
                setSecondOrder=1;
                break;
            default :
                printf("invalid packing_spec->packing_type = %ld\n",(long)packing_spec->packing_type);
                abort();
                break;
            }
            packingTypeIsSet=1;
        }
        switch(packing_spec->accuracy) {

        case GRIB_UTIL_ACCURACY_SAME_BITS_PER_VALUES_AS_INPUT:
            break;

        case GRIB_UTIL_ACCURACY_USE_PROVIDED_BITS_PER_VALUES:
            if (input_bits_per_value!=packing_spec->bitsPerValue)
                SET_LONG_VALUE("bitsPerValue", packing_spec->bitsPerValue);
            break;

        case GRIB_UTIL_ACCURACY_SAME_DECIMAL_SCALE_FACTOR_AS_INPUT:
            break;

        case GRIB_UTIL_ACCURACY_USE_PROVIDED_DECIMAL_SCALE_FACTOR:
            if (input_decimal_scale_factor!=packing_spec->decimalScaleFactor)
                SET_LONG_VALUE("decimalScaleFactor", packing_spec->decimalScaleFactor);
            break;

        default:
            printf("invalid packing_spec->accuracy = %ld\n",(long)packing_spec->accuracy);
            abort();
            break;
        }

        /*nothing to be changed*/
        if (count==0) {
            *err = -1;
            return h;
        }

        if (h->context->debug==-1)
            print_values(h->context,spec,data_values,data_values_count,values,count);

        if((*err = grib_set_values(h,values,count)) != 0)
        {
            fprintf(stderr,"GRIB_UTIL_SET_SPEC: Cannot set values  %s\n",grib_get_error_message(*err));

            for(i = 0; i < count; i++)
                if(values[i].error)
                    fprintf(stderr," %s %s\n",values[i].name,grib_get_error_message(values[i].error));
            goto cleanup;
        }
        if (h->context->debug==-1) {
            int j=0;
            printf("ECCODES DEBUG grib_util: grib_set_double_array\n");
            for (j=0;j<20;j++) printf("ECCODES DEBUG grib_util %g\n",data_values[j]);
            printf("ECCODES DEBUG grib_util: data_values_count=%d \n",(int)data_values_count);
        }

        if((*err = grib_set_double_array(h,"values",data_values,data_values_count)) != 0)
        {
            goto cleanup;
        }
        if (h->context->debug==-1)
            printf("ECCODES DEBUG grib_util: done grib_set_double_array \n");


        /* convert to second_order if not constant field */
        if (setSecondOrder ) {
            size_t slen=17;
            int ii=0;
            int constant=1;
            double missingValue=0;
            double value=missingValue;

            grib_get_double(h,"missingValue",&missingValue);
            for (ii=0;ii<data_values_count;ii++) {
                if (data_values[ii]!=missingValue) {
                    if (value==missingValue) {
                        value=data_values[ii];
                    } else {
                        if (value!=data_values[ii]) {
                            constant=0;
                            break;
                        }
                    }
                }
            }
            if (!constant) {
                if (editionNumber == 1 ) {
                    long numberOfGroups;
                    grib_handle* htmp=grib_handle_clone(h);

                    slen=17;
                    grib_set_string(htmp,"packingType","grid_second_order",&slen);
                    grib_get_long(htmp,"numberOfGroups",&numberOfGroups);
                    /* GRIBEX is not able to decode overflown numberOfGroups with SPD */
                    if (numberOfGroups>65534 && h->context->no_spd ) {
                        slen=24;
                        grib_set_string(h,"packingType","grid_second_order_no_SPD",&slen);
                        grib_handle_delete(htmp);
                    } else {
                        grib_handle_delete(h);
                        h=htmp;
                    }
                } else {
                    slen=17;
                    grib_set_string(h,"packingType","grid_second_order",&slen);
                    grib_set_double_array(h,"values",data_values,data_values_count);
                }
            } else {
                if (h->context->gribex_mode_on) {
                    h->context->gribex_mode_on=0;
                    grib_set_double_array(h,"values",data_values,data_values_count);
                    h->context->gribex_mode_on=1;
                }
            }
        }

        return h;
    }

    switch(spec->grid_type) {

    case GRIB_UTIL_GRID_SPEC_REGULAR_LL:
        grid_type = "regular_ll";
        break;

    case GRIB_UTIL_GRID_SPEC_ROTATED_LL:
        grid_type = "rotated_ll";
        break;

    case GRIB_UTIL_GRID_SPEC_REGULAR_GG:
        grid_type = "regular_gg";
        break;

    case GRIB_UTIL_GRID_SPEC_ROTATED_GG:
        grid_type = "rotated_gg";
        break;

    case GRIB_UTIL_GRID_SPEC_REDUCED_LL:
        grid_type = "reduced_ll";
        break;

    case GRIB_UTIL_GRID_SPEC_POLAR_STEREOGRAPHIC:
        grid_type = "polar_stereographic";
        break;

    case GRIB_UTIL_GRID_SPEC_REDUCED_GG:
        grid_type = "reduced_gg";
        break;

    case GRIB_UTIL_GRID_SPEC_SH:
        grid_type = "sh";
        break;

    case GRIB_UTIL_GRID_SPEC_REDUCED_ROTATED_GG:
        grid_type = "reduced_rotated_gg";
        break;

    default:
        *err = GRIB_NOT_IMPLEMENTED;
        return NULL;

    }

    SET_STRING_VALUE("gridType", grid_type);

    /* The "pl" is given from the template, but "section_copy" will take care of setting the right headers */

    {
        char levtype[80];
        size_t n = sizeof(levtype);
        Assert(grib_get_string(h,"levelType",levtype,&n) == 0);
        switch (spec->grid_type) {
        case GRIB_UTIL_GRID_SPEC_REDUCED_GG:
        case GRIB_UTIL_GRID_SPEC_REDUCED_ROTATED_GG:
            sprintf(name, "%s_pl_%ld_grib%ld", grid_type,spec->N, editionNumber);
            if (spec->pl && spec->pl_size) {
                /* GRIB-834: pl is given so can use any of the reduced_gg_pl samples */
                sprintf(name, "%s_pl_grib%ld", grid_type, editionNumber);
            }
            break;
        default :
            sprintf(name, "%s_pl_grib%ld", grid_type, editionNumber);
        }
    }

    /* TODO: recycle tmp handle */
    tmp = grib_handle_new_from_samples(NULL, name);
    if(!tmp) {
        *err = GRIB_INVALID_FILE;
        return NULL;
    }

    /* Set grid  */
    switch(spec->grid_type) {
    case GRIB_UTIL_GRID_SPEC_REGULAR_LL:
    case GRIB_UTIL_GRID_SPEC_ROTATED_LL:

        COPY_SPEC_LONG  (bitmapPresent);
        if (spec->missingValue) COPY_SPEC_DOUBLE(missingValue);

        SET_LONG_VALUE  ("ijDirectionIncrementGiven",    1);

        /* default iScansNegatively=0 jScansPositively=0 is ok */
        COPY_SPEC_LONG(iScansNegatively);
        COPY_SPEC_LONG(jScansPositively);

        COPY_SPEC_LONG(Ni);
        COPY_SPEC_LONG(Nj);

        COPY_SPEC_DOUBLE(iDirectionIncrementInDegrees);
        COPY_SPEC_DOUBLE(jDirectionIncrementInDegrees);

        COPY_SPEC_DOUBLE(longitudeOfFirstGridPointInDegrees);
        COPY_SPEC_DOUBLE(longitudeOfLastGridPointInDegrees);

        COPY_SPEC_DOUBLE(latitudeOfFirstGridPointInDegrees);
        COPY_SPEC_DOUBLE(latitudeOfLastGridPointInDegrees);

        break;

    case GRIB_UTIL_GRID_SPEC_REGULAR_GG:
    case GRIB_UTIL_GRID_SPEC_ROTATED_GG:

        COPY_SPEC_LONG  (bitmapPresent);
        if (spec->missingValue) COPY_SPEC_DOUBLE(missingValue);
        SET_LONG_VALUE("ijDirectionIncrementGiven", 1);

        /* TODO: add Assert */

        COPY_SPEC_LONG(Ni);
        COPY_SPEC_DOUBLE(iDirectionIncrementInDegrees);

        COPY_SPEC_LONG(Nj);
        COPY_SPEC_LONG(N);

        /* TODO: Compute here ... */
        COPY_SPEC_DOUBLE(longitudeOfFirstGridPointInDegrees);
        COPY_SPEC_DOUBLE(longitudeOfLastGridPointInDegrees);

        COPY_SPEC_DOUBLE(latitudeOfFirstGridPointInDegrees);
        COPY_SPEC_DOUBLE(latitudeOfLastGridPointInDegrees);
        break;


    case GRIB_UTIL_GRID_SPEC_REDUCED_LL:
        COPY_SPEC_LONG  (bitmapPresent);
        if (spec->missingValue) COPY_SPEC_DOUBLE(missingValue);
        SET_LONG_VALUE("ijDirectionIncrementGiven", 0);

        COPY_SPEC_LONG(Nj);

        COPY_SPEC_DOUBLE(longitudeOfFirstGridPointInDegrees);
        COPY_SPEC_DOUBLE(longitudeOfLastGridPointInDegrees);

        COPY_SPEC_DOUBLE(latitudeOfFirstGridPointInDegrees);
        COPY_SPEC_DOUBLE(latitudeOfLastGridPointInDegrees);
        break;

    case GRIB_UTIL_GRID_SPEC_POLAR_STEREOGRAPHIC:
        COPY_SPEC_LONG  (bitmapPresent);
        if (spec->missingValue) COPY_SPEC_DOUBLE(missingValue);

        COPY_SPEC_DOUBLE(longitudeOfFirstGridPointInDegrees);
        COPY_SPEC_DOUBLE(latitudeOfFirstGridPointInDegrees);
        COPY_SPEC_LONG(Ni);
        COPY_SPEC_LONG(Nj);

        /* default iScansNegatively=0 jScansPositively=0 is ok */
        COPY_SPEC_LONG(iScansNegatively);
        COPY_SPEC_LONG(jScansPositively);

        COPY_SPEC_DOUBLE(orientationOfTheGridInDegrees);

        COPY_SPEC_LONG(DxInMetres);
        COPY_SPEC_LONG(DyInMetres);

        break;

    case GRIB_UTIL_GRID_SPEC_REDUCED_GG:
    case GRIB_UTIL_GRID_SPEC_REDUCED_ROTATED_GG:

        COPY_SPEC_LONG  (bitmapPresent);
        if (spec->missingValue) COPY_SPEC_DOUBLE(missingValue);
        SET_LONG_VALUE("ijDirectionIncrementGiven", 0);

        COPY_SPEC_LONG(Nj);
        COPY_SPEC_LONG(N);

        COPY_SPEC_DOUBLE(longitudeOfFirstGridPointInDegrees);
        COPY_SPEC_DOUBLE(longitudeOfLastGridPointInDegrees);

        COPY_SPEC_DOUBLE(latitudeOfFirstGridPointInDegrees);
        COPY_SPEC_DOUBLE(latitudeOfLastGridPointInDegrees);

        break;

    case GRIB_UTIL_GRID_SPEC_SH:
        *err=grib_get_string(h,"gridType",input_grid_type,&input_grid_type_len);

        SET_LONG_VALUE("J", spec->truncation);
        SET_LONG_VALUE("K", spec->truncation);
        SET_LONG_VALUE("M", spec->truncation);

        if(packing_spec->packing_type == GRIB_UTIL_PACKING_TYPE_SPECTRAL_COMPLEX)
        {
            SET_STRING_VALUE("packingType", "spectral_complex");
            packingTypeIsSet=1;
            SET_LONG_VALUE("JS", 20);
            SET_LONG_VALUE("KS", 20);
            SET_LONG_VALUE("MS", 20);
            if (packing_spec->packing == GRIB_UTIL_PACKING_USE_PROVIDED && editionNumber==2 ) {
                SET_LONG_VALUE("computeLaplacianOperator", 1);
            } else if ((!(*err) && strcmp(input_grid_type,"sh")) || packing_spec->computeLaplacianOperator ) {
                SET_LONG_VALUE("computeLaplacianOperator", 1);
                if (packing_spec->truncateLaplacian)
                    SET_LONG_VALUE("truncateLaplacian",1);
            } else {
                SET_LONG_VALUE("computeLaplacianOperator", 0);
                *err=grib_get_double(h,"laplacianOperator",&laplacianOperator);
                if (packing_spec->truncateLaplacian) SET_LONG_VALUE("truncateLaplacian",1);
                SET_DOUBLE_VALUE("laplacianOperator", packing_spec->laplacianOperator);
                if (laplacianOperator) {
                    SET_DOUBLE_VALUE("laplacianOperator", laplacianOperator);
                }
            }
        }

        break;
    }

    /* Set rotation */
    /* FIXME: Missing angleOfRotationInDegrees */

    switch(spec->grid_type) {
    case GRIB_UTIL_GRID_SPEC_ROTATED_LL:
    case GRIB_UTIL_GRID_SPEC_ROTATED_GG:
    case GRIB_UTIL_GRID_SPEC_REDUCED_ROTATED_GG:
        COPY_SPEC_LONG(uvRelativeToGrid);
        COPY_SPEC_DOUBLE(latitudeOfSouthernPoleInDegrees);
        COPY_SPEC_DOUBLE(longitudeOfSouthernPoleInDegrees);
        COPY_SPEC_DOUBLE(angleOfRotationInDegrees);
        break;
    }

    /* process packing options */
    if (!packingTypeIsSet &&
            packing_spec->packing == GRIB_UTIL_PACKING_USE_PROVIDED &&
            strcmp(input_packing_type,"grid_simple_matrix")) {

        switch (packing_spec->packing_type) {
        case GRIB_UTIL_PACKING_TYPE_SPECTRAL_COMPLEX:
            if (strcmp(input_packing_type,"spectral_complex") && !strcmp(input_packing_type,"spectral_simple"))
                SET_STRING_VALUE("packingType","spectral_complex");
            break;
        case GRIB_UTIL_PACKING_TYPE_SPECTRAL_SIMPLE:
            if (strcmp(input_packing_type,"spectral_simple") && !strcmp(input_packing_type,"spectral_complex"))
                SET_STRING_VALUE("packingType","spectral_simple");
            break;
        case GRIB_UTIL_PACKING_TYPE_GRID_SIMPLE:
            if (strcmp(input_packing_type,"grid_simple") && !strcmp(input_packing_type,"grid_complex"))
                SET_STRING_VALUE("packingType","grid_simple");
            break;
        case GRIB_UTIL_PACKING_TYPE_GRID_COMPLEX:
            if (strcmp(input_packing_type,"grid_complex") && !strcmp(input_packing_type,"grid_simple"))
                SET_STRING_VALUE("packingType","grid_complex");
            break;
        case GRIB_UTIL_PACKING_TYPE_JPEG:
            if (strcmp(input_packing_type,"grid_jpeg") && !strcmp(input_packing_type,"grid_simple"))
                SET_STRING_VALUE("packingType","grid_jpeg");
            break;
        case GRIB_UTIL_PACKING_TYPE_GRID_SECOND_ORDER:
            /* we delay the set of grid_second_order because we don't want
               to do it on a field with bitsPerValue=0 */
            setSecondOrder=1;
            break;
        default :
            printf("invalid packing_spec->packing_type = %ld\n",(long)packing_spec->packing_type);
            abort();
            break;
        }
    }
    if (!strcmp(input_packing_type,"grid_simple_matrix")) {
        long numberOfDirections,numberOfFrequencies;
        if (h->context->keep_matrix) {
            int ret;
            SET_STRING_VALUE("packingType","grid_simple_matrix");
            ret=grib_get_long(h,"numberOfDirections",&numberOfDirections);
            if (!ret) {
                ret=grib_get_long(h,"numberOfDirections",&numberOfDirections);
                SET_LONG_VALUE("NC1",numberOfDirections);
                ret=grib_get_long(h,"numberOfFrequencies",&numberOfFrequencies);
                SET_LONG_VALUE("NC2",numberOfFrequencies);
                SET_LONG_VALUE("physicalFlag1",1);
                SET_LONG_VALUE("physicalFlag2",2);
                SET_LONG_VALUE("NR",1);
                SET_LONG_VALUE("NC",1);
            }
        } else {
            SET_STRING_VALUE("packingType","grid_simple");
        }
    }


    switch(packing_spec->accuracy) {

    case GRIB_UTIL_ACCURACY_SAME_BITS_PER_VALUES_AS_INPUT:
    {
        long bitsPerValue = 0;
        Assert(grib_get_long(h, "bitsPerValue", &bitsPerValue) == 0);
        SET_LONG_VALUE("bitsPerValue", bitsPerValue);
    }
    break;

    case GRIB_UTIL_ACCURACY_USE_PROVIDED_BITS_PER_VALUES:
        SET_LONG_VALUE("bitsPerValue", packing_spec->bitsPerValue);
        break;

    case GRIB_UTIL_ACCURACY_SAME_DECIMAL_SCALE_FACTOR_AS_INPUT:
    {
        long decimalScaleFactor = 0;
        Assert(grib_get_long(h, "decimalScaleFactor", &decimalScaleFactor) == 0);
        SET_LONG_VALUE("decimalScaleFactor", decimalScaleFactor);
    }
    break;

    case GRIB_UTIL_ACCURACY_USE_PROVIDED_DECIMAL_SCALE_FACTOR:
        SET_LONG_VALUE("decimalScaleFactor", packing_spec->decimalScaleFactor);
        break;

    default:
        printf("invalid packing_spec->accuracy = %ld\n",(long)packing_spec->accuracy);
        abort();
        break;
    }

    if(packing_spec->extra_settings_count) {
        for(i = 0; i < packing_spec->extra_settings_count; i++) {
            Assert(count < 1024);
            values[count++] = packing_spec->extra_settings[i];
        }
    }
    /* grib_write_message(h,"input.grib","w"); */
    /* grib_write_message(tmp,"geo.grib","w"); */
    if((outh = grib_util_sections_copy(h, tmp, GRIB_SECTION_PRODUCT | GRIB_SECTION_LOCAL,err)) == NULL)
    {
        goto cleanup;
    }

    grib_handle_delete(tmp);
    Assert(*err == 0);

    if (h->context->debug==-1)
        print_values(h->context,spec,data_values,data_values_count,values,count);

    if((*err = grib_set_values(outh,values,count)) != 0)
    {
        fprintf(stderr,"SET_GRID_DATA_DESCRIPTION: Cannot set values  %s\n",grib_get_error_message(*err));

        for(i = 0; i < count; i++)
            if(values[i].error)
                fprintf(stderr," %s %s\n",values[i].name,grib_get_error_message(values[i].error));
        goto cleanup;
    }

    Assert(spec->pl_size >= 0);
    if (spec->pl && spec->pl_size == 0) {
        fprintf(stderr, "pl array not NULL but pl_size == 0!\n");
        goto cleanup;
    }
    if (spec->pl_size > 0 && spec->pl == NULL) {
        fprintf(stderr, "pl_size not zero but pl array == NULL!\n");
        goto cleanup;
    }

    if (spec->pl_size!=0 && (spec->grid_type==GRIB_UTIL_GRID_SPEC_REDUCED_GG || spec->grid_type==GRIB_UTIL_GRID_SPEC_REDUCED_ROTATED_GG))
    {
        *err=grib_set_long_array(outh,"pl",spec->pl,spec->pl_size);
        if (*err) {
            fprintf(stderr,"SET_GRID_DATA_DESCRIPTION: Cannot set pl  %s\n",grib_get_error_message(*err));
            goto cleanup;
        }
    }

    if((*err = grib_set_double_array(outh,"values",data_values,data_values_count)) != 0)
    {
        FILE* ferror;
        long i,count;
        grib_context* c=grib_context_get_default();

        ferror=fopen("error.data","w");
        count=0;
        fprintf(ferror,"# data_values_count=%ld\n",(long)data_values_count);
        fprintf(ferror,"set values={ ");
        for (i=0;i<data_values_count-1;i++) {
            fprintf(ferror,"%g, ",data_values[i]);
            if (count>10) {fprintf(ferror,"\n");count=0;}
            count++;
        }
        fprintf(ferror,"%g }",data_values[data_values_count-1]);
        fclose(ferror);
        if (c->write_on_fail)
            grib_write_message(outh,"error.grib","w");
        goto cleanup;
    }
    /* grib_write_message(outh,"h.grib","w"); */
    /* if the field is empty GRIBEX is packing as simple*/
    /*
	if (!strcmp(input_packing_type,"grid_simple_matrix")) {
		long numberOfValues;
		grib_get_long(outh,"numberOfValues",&numberOfValues);
		if (numberOfValues==0)  {
			slen=11;
			grib_set_string(outh,"packingType","grid_simple",&slen);
		}
	}
     */

    /* convert to second_order if not constant field */
    if (setSecondOrder ) {
        int ii=0;
        int constant=1;
        double missingValue=0;
        double value=missingValue;

        grib_get_double(outh,"missingValue",&missingValue);
        for (ii=0;ii<data_values_count;ii++) {
            if (data_values[ii]!=missingValue) {
                if (value==missingValue) {
                    value=data_values[ii];
                } else {
                    if (value!=data_values[ii]) {
                        constant=0;
                        break;
                    }
                }
            }
        }
        if (!constant) {
            if (editionNumber == 1 ) {
                long numberOfGroups;
                grib_handle* htmp=grib_handle_clone(outh);

                slen=17;
                grib_set_string(htmp,"packingType","grid_second_order",&slen);
                grib_get_long(htmp,"numberOfGroups",&numberOfGroups);
                /* GRIBEX is not able to decode overflown numberOfGroups with SPD */
                if (numberOfGroups>65534 && outh->context->no_spd) {
                    slen=24;
                    grib_set_string(outh,"packingType","grid_second_order_no_SPD",&slen);
                    grib_handle_delete(htmp);
                } else {
                    grib_handle_delete(outh);
                    outh=htmp;
                }
            } else {
                slen=17;
                grib_set_string(outh,"packingType","grid_second_order",&slen);
                grib_set_double_array(outh,"values",data_values,data_values_count);
            }
        } else {
            if (outh->context->gribex_mode_on) {
                outh->context->gribex_mode_on=0;
                grib_set_double_array(outh,"values",data_values,data_values_count);
                outh->context->gribex_mode_on=1;
            }
        }
    }

    if(packing_spec->editionNumber && packing_spec->editionNumber!=editionNumber)
        grib_set_long(outh,"edition", packing_spec->editionNumber);

    if (h->context->debug==-1)
        printf("ECCODES DEBUG: grib_util_set_spec end\n");

    return outh;

    cleanup:
    if(outh)  grib_handle_delete(outh);
    return NULL;
}

int grib_moments(grib_handle* h,double east,double north,double west,double south,int order,double* moments,long *count) {
    grib_iterator* iter=NULL;
    int ret=0,i,j,l;
    size_t n=0,numberOfPoints=0;
    double *lat,*lon,*values;
    double vlat,vlon,val;
    double dx,dy,ddx,ddy;
    double mass,centroidX,centroidY;
    double missingValue;
    grib_context* c=grib_context_get_default();

    ret=grib_get_size(h,"values",&n);
    if (ret) return ret;

    lat=(double*)grib_context_malloc_clear(c,sizeof(double)*n);
    lon=(double*)grib_context_malloc_clear(c,sizeof(double)*n);
    values=(double*)grib_context_malloc_clear(c,sizeof(double)*n);

    iter=grib_iterator_new(h,0,&ret);
    numberOfPoints=0;
    while(grib_iterator_next(iter,&vlat,&vlon,&val)) {
        if (vlon>=east && vlon<=west && vlat>=south && vlat<=north) {
            lat[numberOfPoints]=vlat;
            lon[numberOfPoints]=vlon;
            values[numberOfPoints]=val;
            numberOfPoints++;
        }
    }
    grib_iterator_delete(iter);

    ret=grib_get_double(h,"missingValue",&missingValue);

    centroidX=0;
    centroidY=0;
    mass=0;
    *count=0;
    for (i=0;i<numberOfPoints;i++) {
        if (values[i]!=missingValue) {
            centroidX+=lon[i]*values[i];
            centroidY+=lat[i]*values[i];
            mass+=values[i];
            (*count)++;
        }
    }
    centroidX/=mass;
    centroidY/=mass;
    mass/=*count;

    for (j=0;j<order*order;j++) moments[j]=0;

    for (i=0;i<numberOfPoints;i++) {
        if (values[i]!=missingValue) {
            dx=(lon[i]-centroidX);
            dy=(lat[i]-centroidY);
            ddx=1;
            for (j=0;j<order;j++) {
                ddy=1;
                for (l=0;l<order;l++) {
                    moments[j*order+l]+=ddx*ddy*values[i];
                    ddy*=dy;
                }
                ddx*=dx;
            }
        }
    }
    for (j=0;j<order;j++) {
        for (l=0;l<order;l++) {
            if (j+l>1) {
                moments[j*order+l]=pow(fabs(moments[j*order+l]),1.0/(j+l)) / *count ;
            } else {
                moments[j*order+l]/=*count ;
            }
        }
    }

    grib_context_free(c,lat);
    grib_context_free(c,lon);
    grib_context_free(c,values);

    return ret;
}

/* Helper function for 'parse_keyval_string' */
static void set_value(grib_values* value,char* str,int equal)
{
    char *p=0,*q=0,*s=0;
    char buf[1000]={0,};
    grib_context* c=grib_context_get_default();

    value->equal=equal;
    q=str;

    while (*q != '/' && *q!=0 ) q++;
    if (*q=='/') {
        s=grib_context_strdup(c,q+1);
        value->next=(grib_values*)grib_context_malloc_clear(c,sizeof(grib_values));
        value->next->type=value->type;
        value->next->name=grib_context_strdup(c,value->name);
        set_value(value->next,s,equal);
        grib_context_free(c,s);
    }

    memcpy(buf,str,q-str);

    switch (value->type) {
    case GRIB_TYPE_DOUBLE:
        value->double_value = strtod(buf,&p);
        if(*p != 0) value->has_value=1;
        else if (!strcmp(str,"missing") ||
                !strcmp(str,"MISSING") ||
                !strcmp(str,"Missing")  ) {
            value->type=GRIB_TYPE_MISSING;
            value->has_value=1;
        }
        break;
    case GRIB_TYPE_LONG:
        value->long_value = strtol(buf,&p,10);
        if(*p != 0) value->has_value=1;
        else if (!strcmp(buf,"missing") ||
                !strcmp(buf,"MISSING") ||
                !strcmp(buf,"Missing")  ) {
            value->type=GRIB_TYPE_MISSING;
            value->has_value=1;
        }
        break;
    case GRIB_TYPE_STRING:
        if ( !strcmp(buf,"missing") ||
                !strcmp(buf,"MISSING") ||
                !strcmp(buf,"Missing")  ) {
            value->type=GRIB_TYPE_MISSING;
            value->has_value=1;
        } else {
            value->string_value = grib_context_strdup(c,buf);
            value->has_value=1;
        }
        break;
    case GRIB_TYPE_UNDEFINED:
        value->long_value = strtol(buf,&p,10);
        if(*p == 0) {
            value->type = GRIB_TYPE_LONG;
            value->has_value=1;
        } else {
            value->double_value = strtod(buf,&p);
            if(*p == 0) {
                value->type = GRIB_TYPE_DOUBLE;
                value->has_value=1;
            } else if ( !strcmp(buf,"missing") ||
                    !strcmp(buf,"MISSING") ||
                    !strcmp(buf,"Missing")  ) {
                value->type=GRIB_TYPE_MISSING;
                value->has_value=1;
            } else {
                value->string_value = grib_context_strdup(c,buf);
                value->type         = GRIB_TYPE_STRING;
                value->has_value=1;
            }
        }
        break;
    }
}

/*
 'grib_tool'        Optional tool name which is printed on error. Can be NULL
 'arg'              The string to be parsed e.g. key1=value1,key2!=value2 etc
 'values_required'  If true then each key must have a value after it
 'default_type'     The default type e.g. GRIB_TYPE_UNDEFINED or GRIB_TYPE_DOUBLE
 'values'           The array we populate and return
 'count'            The number of elements
 */
int parse_keyval_string(char* grib_tool, char* arg, int values_required, int default_type, grib_values values[], int* count)
{
    char* p;
    int i=0;
    if (arg == NULL) {
        *count=0;
        return GRIB_SUCCESS;
    }
    p=strtok(arg,",");
    while (p != NULL) {
        values[i].name=(char*)calloc(1,strlen(p)+1);
        strcpy((char*)values[i].name,p);
        p=strtok(NULL,",");
        i++;
        if (i > *count) return GRIB_ARRAY_TOO_SMALL;
    }
    *count=i;

    for (i=0;i<*count;i++) {
        int equal=1;
        char* value=NULL;
        if (values_required) {
            /* Can be either k=v or k!=v */
            p=(char*)values[i].name;
            while( *p != '=' && *p != '!' && *p != '\0') p++;
            if (*p == '=') {
                *p='\0';
                p++;
                value=p;
                equal=1;
            } else if (*p == '!' && *(++p) == '=' ) {
                *p='\0';
                *(p-1)='\0';
                p++;
                value=p;
                equal=0;
            } else {
                return GRIB_INVALID_ARGUMENT;
            }
        }
        p=(char*)values[i].name;
        while(*p != ':' && *p != '\0') p++;
        if ( *p == ':' ) {
            values[i].type = grib_type_to_int(*(p+1));
            if (*(p+1)=='n') values[i].type = GRIB_NAMESPACE;
            *p='\0';
            p++;
        } else {
            values[i].type = default_type;
        }
        if (values_required) {
            if (strlen(value)==0) {
                if (grib_tool)
                    printf("%s error: no value provided for key \"%s\"\n", grib_tool, values[i].name);
                else
                    printf("Error: no value provided for key \"%s\"\n", values[i].name);
                exit(GRIB_INVALID_ARGUMENT);
            }
            set_value(&values[i],value,equal);
        }
    }
    return GRIB_SUCCESS;
}

/* Return 1 if the productDefinitionTemplateNumber (grib edition 2) is related to EPS */
int is_productDefinitionTemplateNumber_EPS(long productDefinitionTemplateNumber)
{
    return (
            productDefinitionTemplateNumber == 1 || productDefinitionTemplateNumber == 11 ||
            productDefinitionTemplateNumber ==33 || productDefinitionTemplateNumber == 34 || /*simulated (synthetic) satellite data*/
            productDefinitionTemplateNumber ==41 || productDefinitionTemplateNumber == 43 || /*atmospheric chemical constituents*/
            productDefinitionTemplateNumber ==45 || productDefinitionTemplateNumber == 47    /*aerosols*/
    );
}

/* Return 1 if the productDefinitionTemplateNumber (grib edition 2) is related to atmospheric chemical constituents */
int is_productDefinitionTemplateNumber_Chemical(long productDefinitionTemplateNumber)
{
    return (
            productDefinitionTemplateNumber == 40 ||
            productDefinitionTemplateNumber == 41 ||
            productDefinitionTemplateNumber == 42 ||
            productDefinitionTemplateNumber == 43);
}

/* Return 1 if the productDefinitionTemplateNumber (grib edition 2) is related to aerosols */
int is_productDefinitionTemplateNumber_Aerosol(long productDefinitionTemplateNumber)
{
    return (
            productDefinitionTemplateNumber == 44 ||
            productDefinitionTemplateNumber == 45 ||
            productDefinitionTemplateNumber == 46 ||
            productDefinitionTemplateNumber == 47);
}

int is_index_file(const char* filename)
{
    FILE* fh;
    char buf[8]={0,};
    char* str="GRBIDX";
    int ret=0;

    fh=fopen(filename,"r");
    if (!fh) return 0;

    fread(buf,1,1,fh);
    fread(buf,6,1,fh);

    ret=!strcmp(buf,str);

    fclose(fh);

    return ret;
}

char get_dir_separator_char(void)
{
#ifdef ECCODES_ON_WINDOWS
#   define DIR_SEPARATOR_CHAR '\\'
#else
#   define DIR_SEPARATOR_CHAR '/'
#endif
    return DIR_SEPARATOR_CHAR;
}

/* Return the component after final slash */
/*  "/tmp/x"  -> "x"  */
/*  "/tmp/"   -> ""   */
const char* extract_filename(const char* filepath)
{
    char* s = strrchr(filepath, get_dir_separator_char());
    if (!s) return filepath;
    else    return s + 1;
}
