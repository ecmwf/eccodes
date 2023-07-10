/* Old implementation. Now superseded. See ECC-441 and ECC-261 */
static int pack_double_old(grib_accessor* a, const double* val, size_t *len)
{
    grib_accessor_data_g1second_order_general_extended_packing* self =  (grib_accessor_data_g1second_order_general_extended_packing*)a;
    int ret=0;
    int grib2=0;
    long bits_per_value,orderOfSPD,binary_scale_factor;
    long numberOfValues;
    double max,min;
    double decimal,divisor;
    double reference_value;
    size_t size,sizebits;
    long half_byte;
    long* X;
    long* Xp;
    long i;
    long incrementGroupLengthA,groupWidthA,prevGroupLength,offsetD,remainingValuesB,groupLengthB;
    long maxB,minB,maxAB,minAB;
    long offsetBeforeData,offsetSection4;
    unsigned char*  buffer = NULL;
    long maxWidth,maxLength,widthOfWidths,NL,widthOfLengths,N1,N2,extraValues,codedNumberOfGroups,numberOfSecondOrderPackedValues;
    long pos;

    long numberOfGroups;
    long groupLengthC,groupLengthA,remainingValues,count;
    long maxA=0,minA=0;
    long maxC,minC,offsetC;
    long maxAC,minAC;
    long range,bias=0,maxSPD;
    long firstOrderValuesMax,offset,groupLength,j,groupWidth,firstOrderValue,lengthOfSecondOrderValues;
    long *groupLengths,*groupWidths,*firstOrderValues;
    /* long groupLengths[MAX_NUMBER_OF_GROUPS],groupWidths[MAX_NUMBER_OF_GROUPS],firstOrderValues[MAX_NUMBER_OF_GROUPS]; */

    /* TODO put these parameters in def file */
    long startGroupLength=15;
    long incrementGroupLength=3;
    long minGroupLength=3;
    long widthOfSPD=0,widthOfBias=0;
    long *offsets;
    long widthOfFirstOrderValues;
    int computeGroupA=1;
    long dataHeadersLength,widthsLength,lengthsLength,firstOrderValuesLength;
    long decimal_scale_factor;
    grib_handle* handle = grib_handle_of_accessor(a);

    self->dirty=1;

    numberOfValues=*len;

    max = val[0];
    min = max;
    for(i=1;i< numberOfValues;i++) {
        if      (val[i] > max ) max = val[i];
        else if (val[i] < min ) min = val[i];
    }

    /* For constant fields set decimal scale factor to 0 (See GRIB-165) */
    if (min==max) {
        grib_set_long_internal(handle,self->decimal_scale_factor, 0);
    }

    if((ret = grib_get_long_internal(handle,self->decimal_scale_factor, &decimal_scale_factor))
            != GRIB_SUCCESS)
        return ret;
    decimal = codes_power<double>(decimal_scale_factor,10);

    max*=decimal;
    min*=decimal;

    if (grib_get_nearest_smaller_value(handle,self->reference_value,min,&reference_value)
            !=GRIB_SUCCESS) {
        grib_context_log(a->context,GRIB_LOG_ERROR,
                "unable to find nearest_smaller_value of %g for %s",min,self->reference_value);
        return GRIB_INTERNAL_ERROR;
    }
    if((ret = grib_set_double_internal(handle,self->reference_value, reference_value)) !=
            GRIB_SUCCESS)
        return ret;

    if((ret=grib_get_long_internal(handle,self->bits_per_value,&bits_per_value)) != GRIB_SUCCESS)
        return ret;

    if((ret=grib_get_long_internal(handle,self->offsetdata,&offsetBeforeData)) != GRIB_SUCCESS)
        return ret;

    if((ret=grib_get_long_internal(handle,self->offsetsection,&offsetSection4)) != GRIB_SUCCESS)
        return ret;

    if((ret=grib_get_long_internal(handle,self->orderOfSPD,&orderOfSPD)) != GRIB_SUCCESS)
        return ret;

    binary_scale_factor = grib_get_binary_scale_fact(max,reference_value,bits_per_value,&ret);
    if (ret != GRIB_SUCCESS) return ret;

    if((ret = grib_set_long_internal(handle,self->binary_scale_factor, binary_scale_factor)) !=
            GRIB_SUCCESS)
        return ret;

    divisor = codes_power<double>(-binary_scale_factor,2);
    X=(long*)grib_context_malloc_clear(a->context,sizeof(long)*numberOfValues);
    for(i=0;i< numberOfValues;i++){
        X[i] = (((val[i]*decimal)-reference_value)*divisor)+0.5;
    }

    groupLengths=(long*)grib_context_malloc_clear(a->context,sizeof(long)*numberOfValues);
    groupWidths=(long*)grib_context_malloc_clear(a->context,sizeof(long)*numberOfValues);
    firstOrderValues=(long*)grib_context_malloc_clear(a->context,sizeof(long)*numberOfValues);

    /* spatial differencing */
    switch (orderOfSPD) {
    case 1:
        for (i=numberOfValues-1;i>0;i--) {
            X[i]-=X[i-1];
        }
        break;
    case 2:
        for (i=numberOfValues-1;i>1;i--) {
            X[i]-=2*X[i-1]-X[i-2];
        }
        break;
    case 3:
        for (i=numberOfValues-1;i>2;i--) {
            X[i]-=3*(X[i-1]-X[i-2])+X[i-3];
        }
        break;
    }
    if (orderOfSPD) {
        Assert(orderOfSPD >=0 && orderOfSPD < numberOfValues);
        bias=X[orderOfSPD];
        for (i=orderOfSPD+1;i<numberOfValues;i++) {
            if ( bias > X[i] ) bias=X[i];
        }
        for (i=orderOfSPD;i<numberOfValues;i++) {
            X[i]-=bias;
        }
        maxSPD=X[0];
        for (i=1;i<orderOfSPD;i++) {
            if ( maxSPD < X[i] ) maxSPD=X[i];
        }
        /* widthOfSPD=(long)ceil(log((double)(maxSPD+1))/log(2.0)); */
        widthOfSPD=number_of_bits(handle, maxSPD);
        widthOfBias=number_of_bits(handle, labs(bias))+1;

        if ( widthOfSPD < widthOfBias  ) widthOfSPD=widthOfBias;

    }
    /* end of spatial differencing */

    count=orderOfSPD;
    remainingValues=numberOfValues-count;
    numberOfGroups=0;
    incrementGroupLengthA=startGroupLength;

    computeGroupA=1;
    while (remainingValues) {
        /* group A created with length=incrementGroupLengthA (if enough values remain)
           incrementGroupLengthA=startGroupLength always except when coming from an A+C or A+B ok branch
         */
        groupLengthA= incrementGroupLengthA < remainingValues ? incrementGroupLengthA : remainingValues ;
        if (computeGroupA) {
            maxA=X[count];
            minA=X[count];
            for (i=1;i<groupLengthA;i++) {
                DebugAssertAccess(X, count+i, numberOfValues);
                if (maxA<X[count+i]) maxA=X[count+i];
                if (minA>X[count+i]) minA=X[count+i];
            }
        }
        groupWidthA=number_of_bits(handle, maxA-minA);
        range=(long)codes_power<double>(groupWidthA,2)-1;

        offsetC=count+groupLengthA;
        if (offsetC==numberOfValues) {
            /* no more values close group A and end loop */
            groupLengths[numberOfGroups]=groupLengthA;
            groupWidths[numberOfGroups]=groupWidthA;
            /* firstOrderValues[numberOfGroups]=minA; */
            /* to optimise the width of first order variable */
            firstOrderValues[numberOfGroups] = maxA-range > 0 ? maxA-range : 0;
            numberOfGroups++;
            break;
        }

        /* group C created with length=incrementGroupLength (fixed)
           or remaining values if close to end
         */
        groupLengthC=incrementGroupLength;
        if ( groupLengthC + offsetC > numberOfValues - startGroupLength/2) {
            groupLengthC=numberOfValues-offsetC;
        }
        maxC=X[offsetC];
        minC=X[offsetC];
        for (i=1;i<groupLengthC;i++) {
            DebugAssertAccess(X, offsetC+i, numberOfValues);
            if (maxC<X[offsetC+i]) maxC=X[offsetC+i];
            if (minC>X[offsetC+i]) minC=X[offsetC+i];
        }

        maxAC= maxA > maxC ? maxA : maxC;
        minAC= minA < minC ? minA : minC;

        /* check if A+C can be represented with the same width as A*/
        if (maxAC-minAC > range) {
            /* A could not be expanded adding C. Check if A could be expanded taking
               some elements from preceding group. The condition is always that width of
               A doesn't increase.
             */
            if (numberOfGroups>0 && groupWidths[numberOfGroups-1] > groupWidthA ) {
                prevGroupLength=groupLengths[numberOfGroups-1]-incrementGroupLength;
                offsetC=count-incrementGroupLength;
                /* preceding group length cannot be less than a minimum value */
                while (prevGroupLength >= minGroupLength) {
                    maxAC=maxA;
                    minAC=minA;
                    for (i=0;i<incrementGroupLength;i++) {
                        if (maxAC<X[offsetC+i]) maxAC=X[offsetC+i];
                        if (minAC>X[offsetC+i]) minAC=X[offsetC+i];
                    }

                    /* no more elements can be transfered, exit loop*/
                    if (maxAC-minAC > range) break;

                    maxA=maxAC;
                    minA=minAC;
                    groupLengths[numberOfGroups-1]-=incrementGroupLength;
                    groupLengthA+=incrementGroupLength;
                    count-=incrementGroupLength;
                    remainingValues+=incrementGroupLength;

                    offsetC-=incrementGroupLength;
                    prevGroupLength-=incrementGroupLength;
                }
            }
            /* close group A*/
            groupLengths[numberOfGroups]=groupLengthA;
            groupWidths[numberOfGroups]=groupWidthA;
            /* firstOrderValues[numberOfGroups]=minA; */
            /* to optimise the width of first order variable */
            firstOrderValues[numberOfGroups] = maxA-range > 0 ? maxA-range : 0;
            count+=groupLengthA;
            remainingValues-=groupLengthA;
            numberOfGroups++;
            /* incrementGroupLengthA is reset to the fixed startGroupLength as it
               could have been changed after the A+C or A+B ok condition.
             */
            incrementGroupLengthA=startGroupLength;
            computeGroupA=1;
#if 0
            if (numberOfGroups==MAX_NUMBER_OF_GROUPS) {
                groupLengthA= remainingValues ;
                maxA=X[count];
                minA=X[count];
                for (i=1;i<groupLengthA;i++) {
                    if (maxA<X[count+i]) maxA=X[count+i];
                    if (minA>X[count+i]) minA=X[count+i];
                }
                groupWidthA=number_of_bits(maxA-minA);
                range=(long)codes_power<double>(groupWidthA,2)-1;
                groupLengths[numberOfGroups]=groupLengthA;
                groupWidths[numberOfGroups]=groupWidthA;
                firstOrderValues[numberOfGroups] = maxA-range > 0 ? maxA-range : 0;
                break;
            }
#endif
            continue;
        }

        /* A+C could be coded with the same width as A*/
        offsetD=offsetC+groupLengthC;
        if (offsetD==numberOfValues) {
            groupLengths[numberOfGroups]=groupLengthA+groupLengthC;
            groupWidths[numberOfGroups]=groupWidthA;

            /* range of AC is the same as A*/
            /* firstOrderValues[numberOfGroups]=minAC; */
            /* to optimise the width of first order variable */
            firstOrderValues[numberOfGroups] = maxAC-range > 0 ? maxAC-range : 0;
            numberOfGroups++;
            break;
        }

        /* group B is created with length startGroupLength, starting at the
           same offset as C.
         */
        remainingValuesB=numberOfValues-offsetC;
        groupLengthB= startGroupLength < remainingValuesB ? startGroupLength : remainingValuesB ;
        maxB=maxC;
        minB=minC;
        for (i=groupLengthC;i<groupLengthB;i++) {
            if (maxB<X[offsetC+i]) maxB=X[offsetC+i];
            if (minB>X[offsetC+i]) minB=X[offsetC+i];
        }

        /* check if group B can be coded with a smaller width than A */
        if (maxB-minB <= range/2 && range>0 ) {

            /* TODO Add code to try if A can be expanded taking some elements
               from the left (preceding) group.
                A possible variation is to do this left check (and the previous one)
                in the final loop when checking that the width of each group.
             */

            /* close group A and continue loop*/
            groupLengths[numberOfGroups]=groupLengthA;
            groupWidths[numberOfGroups]=groupWidthA;
            /* firstOrderValues[numberOfGroups]=minA; */
            /* to optimise the width of first order variable */
            firstOrderValues[numberOfGroups] = maxA-range > 0 ? maxA-range : 0;
            count+=groupLengthA;
            remainingValues-=groupLengthA;
            numberOfGroups++;
#if 0
            if (numberOfGroups==MAX_NUMBER_OF_GROUPS) {
                groupLengthA= remainingValues ;
                maxA=X[count];
                minA=X[count];
                for (i=1;i<groupLengthA;i++) {
                    if (maxA<X[count+i]) maxA=X[count+i];
                    if (minA>X[count+i]) minA=X[count+i];
                }
                groupWidthA=number_of_bits(maxA-minA);
                range=(long)codes_power<double>(groupWidthA,2)-1;
                groupLengths[numberOfGroups]=groupLengthA;
                groupWidths[numberOfGroups]=groupWidthA;
                firstOrderValues[numberOfGroups] = maxA-range > 0 ? maxA-range : 0;
                break;
            }
#endif
            incrementGroupLengthA=startGroupLength;
            computeGroupA=1;
            continue;
        }

        /* check if A+B can be coded with same with as A */
        maxAB= maxA > maxB ? maxA : maxB;
        minAB= minA < minB ? minA : minB;
        if (maxAB-minAB <= range) {
            /* A+B can be merged. The increment used at the beginning of the loop to
               build group C is increased to the size of group B
             */
            incrementGroupLengthA+=groupLengthB;
            maxA=maxAB;
            minA=minAB;
            computeGroupA=0;
            continue;
        }

        /* A+B cannot be merged, A+C can be merged*/
        incrementGroupLengthA+=groupLengthC;
        computeGroupA=1;

    } /* end of the while*/

    /* computing bitsPerValue as the number of bits needed to represent
       the firstOrderValues.
     */
    max=firstOrderValues[0];
    min=firstOrderValues[0];
    for (i=1;i<numberOfGroups;i++) {
        if (max<firstOrderValues[i]) max=firstOrderValues[i];
        if (min>firstOrderValues[i]) min=firstOrderValues[i];
    }
    widthOfFirstOrderValues=number_of_bits(handle, max-min);
    firstOrderValuesMax=(long)codes_power<double>(widthOfFirstOrderValues,2)-1;

    if (numberOfGroups>2) {
        /* loop through all the groups except the last in reverse order to
           check if each group width is still appropriate for the group.
           Focus on groups which have been shrank as left groups of an A group taking
           some of their elements.
         */
        offsets=(long*)grib_context_malloc_clear(a->context,sizeof(long)*numberOfGroups);
        offsets[0]=orderOfSPD;
        for (i=1;i<numberOfGroups;i++) offsets[i]=offsets[i-1]+groupLengths[i-1];
        for (i=numberOfGroups-2;i>=0;i--) {
            offset=offsets[i];
            groupLength=groupLengths[i];

            if (groupLength >= startGroupLength) continue;

            max=X[offset];
            min=X[offset];
            for (j=1;j<groupLength;j++) {
                if (max<X[offset+j]) max=X[offset+j];
                if (min>X[offset+j]) min=X[offset+j];
            }
            groupWidth=number_of_bits(handle, max-min);
            range=(long)codes_power<double>(groupWidth,2)-1;

            /* width of first order values has to be unchanged.*/
            for (j=groupWidth;j<groupWidths[i];j++) {
                firstOrderValue= max>range ? max-range : 0;
                if (firstOrderValue <= firstOrderValuesMax ) {
                    groupWidths[i]=j;
                    firstOrderValues[i]=firstOrderValue;
                    break;
                }
            }

            offsetC=offset;
            /*  group width of the current group (i) can have been reduced
                and it is worth to try to expand the group to get some elements
                from the left group if it has bigger width.
             */
            if (i>0 && (groupWidths[i-1] > groupWidths[i]) ) {
                prevGroupLength=groupLengths[i-1]-incrementGroupLength;
                offsetC-=incrementGroupLength;
                while (prevGroupLength >= minGroupLength) {
                    for (j=0;j<incrementGroupLength;j++) {
                        if (max<X[offsetC+j]) max=X[offsetC+j];
                        if (min>X[offsetC+j]) min=X[offsetC+j];
                    }

                    /* width of first order values has to be unchanged*/
                    firstOrderValue=max>range ? max-range : 0;
                    if (max-min > range || firstOrderValue > firstOrderValuesMax ) break;

                    groupLengths[i-1]-=incrementGroupLength;
                    groupLengths[i]+=incrementGroupLength;
                    firstOrderValues[i]=firstOrderValue;

                    offsetC-=incrementGroupLength;
                    prevGroupLength-=incrementGroupLength;
                }
            }

        }
        grib_context_free(a->context,offsets);
    }

    maxWidth=groupWidths[0];
    maxLength=groupLengths[0];
    for (i=1;i<numberOfGroups;i++) {
        if (maxWidth<groupWidths[i]) maxWidth=groupWidths[i];
        if (maxLength<groupLengths[i]) maxLength=groupLengths[i];
    }

    if (maxWidth < 0 || maxLength < 0) {
        grib_context_log(a->parent->h->context, GRIB_LOG_ERROR, "Cannot compute parameters for second order packing.");
        return GRIB_ENCODING_ERROR;
    }
    widthOfWidths=number_of_bits(handle, maxWidth);
    widthOfLengths=number_of_bits(handle, maxLength);

    lengthOfSecondOrderValues=0;
    for ( i=0; i<numberOfGroups;i++) {
        lengthOfSecondOrderValues+=groupLengths[i]*groupWidths[i];
    }

    if (!a->context->no_big_group_split) {
        grib_split_long_groups(handle, a->context,&numberOfGroups,&lengthOfSecondOrderValues,
                groupLengths,&widthOfLengths,groupWidths,widthOfWidths,
                firstOrderValues,widthOfFirstOrderValues);
    }

    Xp=X+orderOfSPD;
    for ( i=0; i<numberOfGroups;i++) {
        for (j=0; j<groupLengths[i]; j++) {
            *(Xp++)-=firstOrderValues[i];
        }
    }

    /* start writing to message */

    /* writing SPD */
    if (orderOfSPD) {
        if((ret = grib_set_long_internal(handle,self->widthOfSPD, widthOfSPD))
                != GRIB_SUCCESS)
            return ret;
    }

    /* end writing SPD */
    if((ret = grib_set_long_internal(handle,self->widthOfFirstOrderValues, widthOfFirstOrderValues))
            != GRIB_SUCCESS)
        return ret;

    dataHeadersLength=25;
    if (orderOfSPD) dataHeadersLength+=1+((orderOfSPD+1)*widthOfSPD+7)/8;
    widthsLength=(widthOfWidths*numberOfGroups+7)/8;
    lengthsLength=(widthOfLengths*numberOfGroups+7)/8;
    firstOrderValuesLength=(widthOfFirstOrderValues*numberOfGroups+7)/8;

    NL=widthsLength+dataHeadersLength+1;
    N1=NL+lengthsLength;
    N2=N1+firstOrderValuesLength;

    NL= NL > 65535 ? 65535 : NL;
    N2= N2 > 65535 ? 65535 : N2;
    N1= N1 > 65535 ? 65535 : N1;

    grib_set_long(handle,self->NL, NL);
    grib_set_long(handle,self->N1, N1);
    grib_set_long(handle,self->N2, N2);

    if (numberOfGroups > 65535 ) {
        extraValues=numberOfGroups/65536;
        codedNumberOfGroups=numberOfGroups%65536;
    } else {
        extraValues=0;
        codedNumberOfGroups=numberOfGroups;
    }

    /* if no extraValues key present it is a GRIB2*/
    grib2=0;
    if((ret = grib_set_long(handle,self->extraValues, extraValues)) != GRIB_SUCCESS) {
        codedNumberOfGroups=numberOfGroups;
        grib2=1;
    }

    if((ret = grib_set_long_internal(handle,self->codedNumberOfGroups, codedNumberOfGroups)) != GRIB_SUCCESS)
        return ret;

    numberOfSecondOrderPackedValues=numberOfValues-orderOfSPD;
    if (!grib2 && numberOfSecondOrderPackedValues > 65535 )
        numberOfSecondOrderPackedValues= 65535;

    if((ret = grib_set_long_internal(handle,self->numberOfSecondOrderPackedValues, numberOfSecondOrderPackedValues))
            != GRIB_SUCCESS)
        return ret;

    if (grib2) {
        if((ret = grib_set_long_internal(handle,self->bits_per_value, bits_per_value)) != GRIB_SUCCESS)
            return ret;
    } else {
        if((ret = grib_set_long_internal(handle,self->bits_per_value, 0)) != GRIB_SUCCESS)
            return ret;
    }

    if((ret = grib_set_long_internal(handle,self->widthOfWidths, widthOfWidths)) != GRIB_SUCCESS)
        return ret;

    if((ret = grib_set_long_internal(handle,self->widthOfLengths, widthOfLengths)) != GRIB_SUCCESS)
        return ret;

    lengthOfSecondOrderValues=0;
    for ( i=0; i<numberOfGroups;i++) {
        lengthOfSecondOrderValues+=groupLengths[i]*groupWidths[i];
    }

    size=(lengthOfSecondOrderValues+7)/8;
    sizebits=lengthOfSecondOrderValues;

    /* padding section 4 to an even number of octets*/
    size = (size+offsetBeforeData-offsetSection4) % 2 ? size+1 : size;
    half_byte=8*size-sizebits;
    if((ret = grib_set_long_internal(handle,self->half_byte, half_byte)) != GRIB_SUCCESS)
        return ret;

    buffer=(unsigned char*)grib_context_malloc_clear(a->context,size);

    pos=0;
    if (orderOfSPD) {
        long SPD[4]={0,};
        size_t nSPD=orderOfSPD+1;
        Assert(orderOfSPD<=3);
        for (i=0;i<orderOfSPD;i++) SPD[i]=X[i];
        SPD[orderOfSPD]=bias;
        ret=grib_set_long_array_internal(handle,self->SPD,SPD,nSPD);
        if(ret) return ret;
    }

    ret=grib_set_long_array_internal(handle,self->groupWidths,groupWidths,(size_t)numberOfGroups);
    if(ret) return ret;

    ret=grib_set_long_array_internal(handle,self->groupLengths,groupLengths,(size_t)numberOfGroups);
    if(ret) return ret;

    ret=grib_set_long_array_internal(handle,self->firstOrderValues,firstOrderValues,(size_t)numberOfGroups);
    if(ret) return ret;

    Xp=X+orderOfSPD;
    pos=0;
    count=0;
    for (i=0;i<numberOfGroups;i++) {
        if (groupWidths[i]>0) {
            for (j=0;j<groupLengths[i];j++) {
#if EFDEBUG
                printf("CXXXXX %ld %ld %ld %ld\n",count,*Xp,groupWidths[i],groupLengths[i]);
                count++;
#endif
                grib_encode_unsigned_longb(buffer,*(Xp++),&pos,groupWidths[i]);
            }
        } else  {
            Xp+=groupLengths[i];
#if EFDEBUG
            count+=groupLengths[i];
#endif
        }
    }

    /* ECC-259: Set correct number of values */
    ret=grib_set_long_internal(a->parent->h,self->number_of_values, *len);
    if(ret) return ret;

    grib_buffer_replace(a, buffer, size,1,1);

    grib_context_free(a->context,buffer);
    grib_context_free(a->context,X);
    grib_context_free(a->context,groupLengths);
    grib_context_free(a->context,groupWidths);
    grib_context_free(a->context,firstOrderValues);

    return ret;
}
