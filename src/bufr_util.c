/*
 * Copyright 2005-2019 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"

/* Return the rank of the key using list of keys (For BUFR keys) */
/* The argument 'keys' is an input as well as output from each call */
int compute_bufr_key_rank(grib_handle* h, grib_string_list* keys, const char* key)
{
    grib_string_list* next=keys;
    grib_string_list* prev=keys;
    int theRank=0;
    size_t size=0;
    grib_context* c=h->context;
    DebugAssert(h->product_kind == PRODUCT_BUFR);

    while (next && next->value && strcmp(next->value, key)) {
        prev=next;
        next=next->next;
    }
    if (!next) {
        DebugAssert(prev);
        prev->next = (grib_string_list*)grib_context_malloc_clear(c, sizeof(grib_string_list));
        next=prev->next;
    }
    if (!next->value) {
        next->value=strdup(key);
        next->count=0;
    }

    next->count++;
    theRank=next->count;
    if (theRank==1) {
        /* If the count is 1 it could mean two things: */
        /*   This is the first instance of the key and there is another one */
        /*   This is the first and only instance of the key */
        /* So we check if there is a second one of this key, */
        /* If not, then rank is zero i.e. this is the only instance */
        char* s = (char*)grib_context_malloc_clear(c, strlen(key)+5);
        sprintf(s, "#2#%s", key);
        if (grib_get_size(h, s, &size) == GRIB_NOT_FOUND) theRank=0;
        grib_context_free(c, s);
    }

    return theRank;
}

char** codes_bufr_copy_data_return_copied_keys(grib_handle* hin, grib_handle* hout, size_t* nkeys, int* err)
{
    bufr_keys_iterator* kiter=NULL;
    char* name=0;
    char** keys=NULL;
    grib_sarray* k=0;

    if (hin==NULL || hout==NULL) {
        *err = GRIB_NULL_HANDLE;
        return NULL;
    }

    kiter=codes_bufr_data_section_keys_iterator_new(hin);
    if (!kiter) return NULL;
    k=grib_sarray_new(hin->context, 50, 10);

    while (codes_bufr_keys_iterator_next(kiter))
    {
        name = codes_bufr_keys_iterator_get_name(kiter);
        /* if the copy fails we want to keep copying without any errors.
           This is because the copy can be between structures that are not
           identical and we want to copy what can be copied and skip what
           cannot be copied because is not in the output handle
         */
        *err=codes_copy_key(hin, hout, name, 0);
        if (*err==0) {
            /* 'name' will be freed when we call codes_bufr_keys_iterator_delete so copy */
            char* copied_name = strdup(name);
            k=grib_sarray_push(hin->context, k, copied_name);
        }
    }
    *nkeys=grib_sarray_used_size(k);
    keys=grib_sarray_get_array(hin->context, k);
    grib_sarray_delete(hin->context, k);
    if (*nkeys > 0) {
        /* Do the pack if something was copied */
        *err=grib_set_long(hout, "pack", 1);
    }
    codes_bufr_keys_iterator_delete(kiter);
    return keys;
}

int codes_bufr_copy_data(grib_handle* hin, grib_handle* hout)
{
    bufr_keys_iterator* kiter=NULL;
    char* name=0;
    int err=0;
    int nkeys=0;

    if (hin==NULL || hout==NULL) {
        return GRIB_NULL_HANDLE;
    }

    kiter=codes_bufr_data_section_keys_iterator_new(hin);
    if (!kiter) return GRIB_INTERNAL_ERROR;

    while (codes_bufr_keys_iterator_next(kiter))
    {
        name = codes_bufr_keys_iterator_get_name(kiter);
        /* if the copy fails we want to keep copying without any error messages.
           This is because the copy can be between structures that are not
           identical and we want to copy what can be copied and skip what
           cannot be copied because is not in the output handle
         */
        err=codes_copy_key(hin, hout, name, GRIB_TYPE_UNDEFINED);
        if (err==0) nkeys++;
    }

    if (nkeys > 0) {
        /* Do the pack if something was copied */
        err=grib_set_long(hout, "pack", 1);
    }

    codes_bufr_keys_iterator_delete(kiter);
    return err;
}

#define BUFR_SECTION0_LEN 8 /* BUFR section 0 is always 8 bytes long */
static int bufr_extract_edition(const void* message, long* edition)
{
    const long nbits_edition = 8;
    long pos_edition = 7*8;

    *edition = (long)grib_decode_unsigned_long(message, &pos_edition, nbits_edition);
    return GRIB_SUCCESS;
}
/* The ECMWF BUFR local use section */
static int bufr_decode_rdb_keys(const void* message, long offset_section2, codes_bufr_header* hdr)
{
    long nbits_rdbType    = 1*8;
    long pos_rdbType      = (offset_section2+4)*8;
    long nbits_oldSubtype = 1*8;
    long pos_oldSubtype   = (offset_section2+5)*8;

    long nbits_qualityControl = 1*8;
    long pos_qualityControl   = (offset_section2+48)*8;
    long nbits_newSubtype     = 2*8;
    long pos_newSubtype       = (offset_section2+49)*8;
    long nbits_daLoop         = 1*8;
    long pos_daLoop           = (offset_section2+51)*8;

    long start = 0;
    const long offset_keyData = offset_section2 + 6;
    const long offset_rdbtime = offset_section2 + 38;
    const long offset_rectime = offset_section2 + 41;

    unsigned char* p = (unsigned char*)message + offset_keyData;

    DebugAssert(hdr->ecmwfLocalSectionPresent);

    hdr->rdbType = (long)grib_decode_unsigned_long(message, &pos_rdbType, nbits_rdbType);
    hdr->oldSubtype = (long)grib_decode_unsigned_long(message, &pos_oldSubtype, nbits_oldSubtype);

    start = 0;
    hdr->localYear   = (long)grib_decode_unsigned_long(p, &start, 12);
    hdr->localMonth  = (long)grib_decode_unsigned_long(p, &start, 4);
    hdr->localDay    = (long)grib_decode_unsigned_long(p, &start, 6);
    hdr->localHour   = (long)grib_decode_unsigned_long(p, &start, 5);
    hdr->localMinute = (long)grib_decode_unsigned_long(p, &start, 6);
    hdr->localSecond = (long)grib_decode_unsigned_long(p, &start, 6);

    /* rdbtime */
    p  = (unsigned char*)message + offset_rdbtime;
    start = 0;
    hdr->rdbtimeDay    = (long)grib_decode_unsigned_long(p, &start, 6);
    hdr->rdbtimeHour   = (long)grib_decode_unsigned_long(p, &start, 5);
    hdr->rdbtimeMinute = (long)grib_decode_unsigned_long(p, &start, 6);
    hdr->rdbtimeSecond = (long)grib_decode_unsigned_long(p, &start, 6);

    /* rectime */
    p  = (unsigned char*)message + offset_rectime;
    start = 0;
    hdr->rectimeDay    = (long)grib_decode_unsigned_long(p, &start, 6);
    hdr->rectimeHour   = (long)grib_decode_unsigned_long(p, &start, 5);
    hdr->rectimeMinute = (long)grib_decode_unsigned_long(p, &start, 6);
    hdr->rectimeSecond = (long)grib_decode_unsigned_long(p, &start, 6);

    hdr->qualityControl = (long)grib_decode_unsigned_long(message, &pos_qualityControl, nbits_qualityControl);
    hdr->newSubtype     = (long)grib_decode_unsigned_long(message, &pos_newSubtype, nbits_newSubtype);
    hdr->daLoop         = (long)grib_decode_unsigned_long(message, &pos_daLoop, nbits_daLoop);

    return GRIB_SUCCESS;
}

/* The ECMWF BUFR local use section */
static int bufr_decode_extra_rdb_keys(const void* message, long offset_section2, codes_bufr_header* hdr)
{
    size_t i = 0;
    int isSatelliteType = 0;
    long start = 0;
    const long offset_keyData = offset_section2 + 6;
    const long offset_keyMore = offset_section2 + 19; /* 8 bytes long */
    const long offset_keySat  = offset_section2 + 27; /* 9 bytes long */

    unsigned char* pKeyData = (unsigned char*)message + offset_keyData;
    char* pKeyMore = (char*)message + offset_keyMore;

    DebugAssert(hdr->ecmwfLocalSectionPresent);

    if ( hdr->rdbType == 2 || hdr->rdbType == 3 || hdr->rdbType == 8 || hdr->rdbType == 12 ) {
        isSatelliteType = 1;
    }
    if (isSatelliteType || hdr->numberOfSubsets > 1) {
        hdr->isSatellite = 1;
    } else {
        hdr->isSatellite = 0;
    }
    
    if (hdr->isSatellite) {
        unsigned char* pKeyMoreLong = (unsigned char*)message + offset_keyMore; /* as an integer */
        unsigned char* pKeySat      = (unsigned char*)message + offset_keySat;
        long lValue = 0;
        start = 40;
        lValue = (long)grib_decode_unsigned_long(pKeyData, &start, 26);
        hdr->localLongitude1 = (lValue - 18000000.0)/100000.0;
        start = 72;
        lValue = (long)grib_decode_unsigned_long(pKeyData, &start, 25);
        hdr->localLatitude1  = (lValue - 9000000.0)/100000.0;
        start = 0;
        lValue = (long)grib_decode_unsigned_long(pKeyMoreLong, &start, 26);
        hdr->localLongitude2 = (lValue -18000000.0)/100000.0;
        start = 32;
        lValue = (long)grib_decode_unsigned_long(pKeyMoreLong, &start, 25);
        hdr->localLatitude2  = (lValue - 9000000)/100000.0;
        
        if (hdr->oldSubtype == 255 || hdr->numberOfSubsets > 255 ||
            (hdr->oldSubtype >= 121 && hdr->oldSubtype <= 130)   ||
            hdr->oldSubtype==31)
        {
            start = 0;
            hdr->localNumberOfObservations = (long)grib_decode_unsigned_long(pKeySat, &start, 16);
            start = 16;
            hdr->satelliteID = (long)grib_decode_unsigned_long(pKeySat, &start, 16);
        }
        else
        {
            start = 0;
            hdr->localNumberOfObservations = (long)grib_decode_unsigned_long(pKeySat, &start, 8);
            start = 8;
            hdr->satelliteID = (long)grib_decode_unsigned_long(pKeySat, &start, 16);
        }
    }
    else {
        long lValue = 0;
        start = 72;
        lValue = (long)grib_decode_unsigned_long(pKeyData, &start, 25);
        hdr->localLatitude = (lValue -9000000.0)/100000.0;
        start = 40;
        lValue = (long)grib_decode_unsigned_long(pKeyData, &start, 26);
        hdr->localLongitude = (lValue - 18000000.0)/100000.0;

        /* interpret keyMore as a string */
        for (i=0; i<8; ++i) {
            hdr->ident[i] = *pKeyMore++;
        }
        hdr->ident[i] = '\0';
    }

    return GRIB_SUCCESS;
}

static int bufr_decode_edition3(const void* message, codes_bufr_header* hdr)
{
    int err = GRIB_SUCCESS;

    unsigned long totalLength = 0;
    const long nbits_totalLength = 3*8;
    long pos_totalLength = 4*8;

    unsigned long section1Length = 0;
    const long nbits_section1Length = 3*8;
    long pos_section1Length = 8*8;

    long nbits_masterTableNumber = 1*8;
    long pos_masterTableNumber   = 11*8;

    long nbits_bufrHeaderSubCentre = 1*8;
    long pos_bufrHeaderSubCentre   = 12*8;

    long nbits_bufrHeaderCentre = 1*8;
    long pos_bufrHeaderCentre   = 13*8;

    long nbits_updateSequenceNumber = 1*8;
    long pos_updateSequenceNumber   = 14*8;

    long section1Flags = 0;
    long nbits_section1Flags = 1*8;
    long pos_section1Flags   = 15*8;

    long nbits_dataCategory = 1*8;
    long pos_dataCategory   = 16*8;

    long nbits_dataSubCategory = 1*8;
    long pos_dataSubCategory   = 17*8;

    long nbits_masterTablesVersionNumber = 1*8;
    long pos_masterTablesVersionNumber   = 18*8;

    long nbits_localTablesVersionNumber = 1*8;
    long pos_localTablesVersionNumber   = 19*8;

    const long typicalCentury = 21; /* This century */
    long typicalYearOfCentury = 0;
    long nbits_typicalYearOfCentury = 1*8;
    long pos_typicalYearOfCentury   = 20*8;

    long nbits_typicalMonth = 1*8;
    long pos_typicalMonth = 21*8;

    long nbits_typicalDay = 1*8;
    long pos_typicalDay = 22*8;

    long nbits_typicalHour = 1*8;
    long pos_typicalHour = 23*8;

    long nbits_typicalMinute = 1*8;
    long pos_typicalMinute = 24*8;

    long section2Length = 0;
    long offset_section2 = 0;
    long offset_section3 = 0;
    long nbits_numberOfSubsets  = 2*8;
    long pos_numberOfSubsets = 0;  /*depends on offset_section3*/

    long section3Flags;
    long nbits_section3Flags = 1*8;
    long pos_section3Flags   = 0;  /*depends on offset_section3*/

    totalLength    = grib_decode_unsigned_long(message, &pos_totalLength, nbits_totalLength);
    if (totalLength != hdr->message_size) {
        return GRIB_WRONG_LENGTH;
    }
    section1Length      = grib_decode_unsigned_long(message, &pos_section1Length, nbits_section1Length);
    hdr->masterTableNumber   = (long)grib_decode_unsigned_long(message, &pos_masterTableNumber, nbits_masterTableNumber);
    hdr->bufrHeaderSubCentre = (long)grib_decode_unsigned_long(message, &pos_bufrHeaderSubCentre, nbits_bufrHeaderSubCentre);
    hdr->bufrHeaderCentre    = (long)grib_decode_unsigned_long(message, &pos_bufrHeaderCentre, nbits_bufrHeaderCentre);
    hdr->updateSequenceNumber = (long)grib_decode_unsigned_long(message, &pos_updateSequenceNumber, nbits_updateSequenceNumber);
    section1Flags       = (long)grib_decode_unsigned_long(message, &pos_section1Flags, nbits_section1Flags);
    hdr->dataCategory        = (long)grib_decode_unsigned_long(message, &pos_dataCategory, nbits_dataCategory);
    hdr->dataSubCategory     = (long)grib_decode_unsigned_long(message, &pos_dataSubCategory, nbits_dataSubCategory);
    hdr->masterTablesVersionNumber = (long)grib_decode_unsigned_long(
        message, &pos_masterTablesVersionNumber, nbits_masterTablesVersionNumber);
    hdr->localTablesVersionNumber = (long)grib_decode_unsigned_long(message, &pos_localTablesVersionNumber, nbits_localTablesVersionNumber);
    typicalYearOfCentury = (long)grib_decode_unsigned_long(message, &pos_typicalYearOfCentury, nbits_typicalYearOfCentury);
    hdr->typicalYear = (typicalCentury - 1) * 100  + typicalYearOfCentury;
    hdr->typicalMonth  = (long)grib_decode_unsigned_long(message, &pos_typicalMonth, nbits_typicalMonth);
    hdr->typicalDay    = (long)grib_decode_unsigned_long(message, &pos_typicalDay, nbits_typicalDay);
    hdr->typicalHour   = (long)grib_decode_unsigned_long(message, &pos_typicalHour, nbits_typicalHour);
    hdr->typicalMinute = (long)grib_decode_unsigned_long(message, &pos_typicalMinute, nbits_typicalMinute);
    hdr->typicalSecond = 0;
    hdr->typicalDate = hdr->typicalYear * 10000 + hdr->typicalMonth * 100 + hdr->typicalDay;
    hdr->typicalTime = hdr->typicalHour * 10000 + hdr->typicalMinute * 100 + hdr->typicalSecond;

    offset_section2 = BUFR_SECTION0_LEN + section1Length;  /*bytes*/
    section2Length = 0;
    hdr->ecmwfLocalSectionPresent = (section1Flags != 0);
    if (hdr->ecmwfLocalSectionPresent) {
        long pos_section2Length;
        const long nbits_section2Length = 3*8;
        pos_section2Length = offset_section2*8;

        section2Length = grib_decode_unsigned_long(message, &pos_section2Length, nbits_section2Length);

        if (hdr->bufrHeaderCentre == 98) {
            err = bufr_decode_rdb_keys(message, offset_section2, hdr);
        }
    }

    offset_section3 = BUFR_SECTION0_LEN + section1Length + section2Length;  /*bytes*/
    nbits_numberOfSubsets = 2*8;
    pos_numberOfSubsets   = (offset_section3+4)*8;
    hdr->numberOfSubsets  = grib_decode_unsigned_long(message, &pos_numberOfSubsets, nbits_numberOfSubsets);

    pos_section3Flags   = (offset_section3+6)*8;
    section3Flags       = (long)grib_decode_unsigned_long(message, &pos_section3Flags, nbits_section3Flags);
    hdr->observedData   = (section3Flags & 1<<7) ? 1 : 0;
    hdr->compressedData = (section3Flags & 1<<6) ? 1 : 0;

    if (hdr->ecmwfLocalSectionPresent && hdr->bufrHeaderCentre == 98 && section2Length == 52) {
        err = bufr_decode_extra_rdb_keys(message, offset_section2, hdr);
    }

    return err;
}

static int bufr_decode_edition4(const void* message, codes_bufr_header* hdr)
{
    int err = GRIB_SUCCESS;

    unsigned long totalLength = 0;
    const long nbits_totalLength = 3*8;
    long pos_totalLength = 4*8;

    unsigned long section1Length;
    const long nbits_section1Length = 3*8;
    long pos_section1Length = 8*8;

    long nbits_masterTableNumber = 1*8;
    long pos_masterTableNumber   = 11*8;

    long nbits_bufrHeaderCentre = 2*8;
    long pos_bufrHeaderCentre   = 12*8;

    long nbits_bufrHeaderSubCentre = 2*8;
    long pos_bufrHeaderSubCentre   = 14*8;

    long nbits_updateSequenceNumber = 1*8;
    long pos_updateSequenceNumber   = 16*8;

    long section1Flags = 0;
    long nbits_section1Flags = 1*8;
    long pos_section1Flags   = 17*8;

    long nbits_dataCategory = 1*8;
    long pos_dataCategory   = 18*8;

    long nbits_internationalDataSubCategory = 1*8;
    long pos_internationalDataSubCategory   = 19*8;

    long nbits_dataSubCategory = 1*8;
    long pos_dataSubCategory   = 20*8;

    long nbits_masterTablesVersionNumber = 1*8;
    long pos_masterTablesVersionNumber   = 21*8;

    long nbits_localTablesVersionNumber = 1*8;
    long pos_localTablesVersionNumber   = 22*8;

    long typicalYear2 = 0; // corrected
    long nbits_typicalYear = 2*8;
    long pos_typicalYear   = 23*8;

    long nbits_typicalMonth = 1*8;
    long pos_typicalMonth   = 25*8;

    long nbits_typicalDay = 1*8;
    long pos_typicalDay   = 26*8;

    long nbits_typicalHour = 1*8;
    long pos_typicalHour   = 27*8;

    long nbits_typicalMinute = 1*8;
    long pos_typicalMinute   = 28*8;

    long nbits_typicalSecond = 1*8;
    long pos_typicalSecond   = 29*8;

    long section2Length = 0;
    long offset_section2 = 0;
    long offset_section3 = 0;
    long nbits_numberOfSubsets  = 2*8;
    long pos_numberOfSubsets = 0; /*depends on offset_section3*/

    long section3Flags;
    long nbits_section3Flags = 1*8;
    long pos_section3Flags   = 0;  /*depends on offset_section3*/

    totalLength    = grib_decode_unsigned_long(message, &pos_totalLength, nbits_totalLength);
    if (totalLength != hdr->message_size) {
        return GRIB_WRONG_LENGTH;
    }
    section1Length      = grib_decode_unsigned_long(message, &pos_section1Length, nbits_section1Length);
    hdr->masterTableNumber   = (long)grib_decode_unsigned_long(message, &pos_masterTableNumber, nbits_masterTableNumber);
    hdr->bufrHeaderCentre    = (long)grib_decode_unsigned_long(message, &pos_bufrHeaderCentre, nbits_bufrHeaderCentre);
    hdr->bufrHeaderSubCentre = (long)grib_decode_unsigned_long(message, &pos_bufrHeaderSubCentre, nbits_bufrHeaderSubCentre);
    hdr->updateSequenceNumber = (long)grib_decode_unsigned_long(message, &pos_updateSequenceNumber, nbits_updateSequenceNumber);
    section1Flags       = (long)grib_decode_unsigned_long(message, &pos_section1Flags, nbits_section1Flags);
    hdr->dataCategory        = (long)grib_decode_unsigned_long(message, &pos_dataCategory, nbits_dataCategory);
    hdr->internationalDataSubCategory = (long)grib_decode_unsigned_long(message, &pos_internationalDataSubCategory, nbits_internationalDataSubCategory);
    hdr->dataSubCategory     = (long)grib_decode_unsigned_long(message, &pos_dataSubCategory, nbits_dataSubCategory);
    hdr->masterTablesVersionNumber = (long)grib_decode_unsigned_long(message, &pos_masterTablesVersionNumber, nbits_masterTablesVersionNumber);
    hdr->localTablesVersionNumber = (long)grib_decode_unsigned_long(message, &pos_localTablesVersionNumber, nbits_localTablesVersionNumber);

    hdr->typicalYear   = (long)grib_decode_unsigned_long(message, &pos_typicalYear, nbits_typicalYear);
    typicalYear2 = hdr->typicalYear < 100 ? 2000 + hdr->typicalYear : hdr->typicalYear; // ECC-556
    hdr->typicalMonth  = (long)grib_decode_unsigned_long(message, &pos_typicalMonth, nbits_typicalMonth);
    hdr->typicalDay    = (long)grib_decode_unsigned_long(message, &pos_typicalDay, nbits_typicalDay);
    hdr->typicalHour   = (long)grib_decode_unsigned_long(message, &pos_typicalHour, nbits_typicalHour);
    hdr->typicalMinute = (long)grib_decode_unsigned_long(message, &pos_typicalMinute, nbits_typicalMinute);
    hdr->typicalSecond = (long)grib_decode_unsigned_long(message, &pos_typicalSecond, nbits_typicalSecond);
    hdr->typicalDate = typicalYear2 * 10000 + hdr->typicalMonth * 100 + hdr->typicalDay;
    hdr->typicalTime = hdr->typicalHour * 10000 + hdr->typicalMinute * 100 + hdr->typicalSecond;

    offset_section2 = BUFR_SECTION0_LEN + section1Length;  /*bytes*/
    section2Length = 0;
    hdr->ecmwfLocalSectionPresent = (section1Flags != 0);
    if (hdr->ecmwfLocalSectionPresent) {
        long pos_section2Length;
        const long nbits_section2Length = 3*8;
        pos_section2Length = offset_section2*8;

        section2Length = grib_decode_unsigned_long(message, &pos_section2Length, nbits_section2Length);

        if (hdr->bufrHeaderCentre == 98) {
            err = bufr_decode_rdb_keys(message, offset_section2, hdr);
        }
    }

    offset_section3 = BUFR_SECTION0_LEN + section1Length + section2Length;  /*bytes*/
    nbits_numberOfSubsets = 2*8;
    pos_numberOfSubsets   = (offset_section3+4)*8;
    hdr->numberOfSubsets  = grib_decode_unsigned_long(message, &pos_numberOfSubsets, nbits_numberOfSubsets);

    pos_section3Flags   = (offset_section3+6)*8;
    section3Flags       = (long)grib_decode_unsigned_long(message, &pos_section3Flags, nbits_section3Flags);
    hdr->observedData   = (section3Flags & 1<<7) ? 1 : 0;
    hdr->compressedData = (section3Flags & 1<<6) ? 1 : 0;

    if (hdr->ecmwfLocalSectionPresent && hdr->bufrHeaderCentre == 98 && section2Length == 52) {
        err = bufr_decode_extra_rdb_keys(message, offset_section2, hdr);
    }

    return err;
}

static int bufr_decode_header(grib_context* c, const void* message, off_t offset, size_t size, codes_bufr_header* hdr)
{
    int err = GRIB_SUCCESS;

    hdr->message_offset = (unsigned long)offset;
    hdr->message_size   = (unsigned long)size;

    err = bufr_extract_edition(message, &hdr->edition);

    if (hdr->edition == 3) {
        err = bufr_decode_edition3(message, hdr);
    } else if (hdr->edition == 4) {
        err = bufr_decode_edition4(message, hdr);
    } else {
        grib_context_log(c, GRIB_LOG_ERROR, "Unsupported BUFR edition: %ld", hdr->edition);
        err = GRIB_DECODING_ERROR;
    }

    return err;
}

static int count_bufr_messages(grib_context* c, FILE* f, int* n)
{
    int err=0;
    void* mesg=NULL;
    size_t size=0;
    off_t offset=0;
    int done = 0;

    *n = 0;
    if (!c) c=grib_context_get_default();

    while(!done) {
        mesg = wmo_read_bufr_from_file_malloc(f, 0, &size, &offset, &err);
        /*printf("Count so far=%ld, mesg=%x, err=%d (%s)\n", *count, mesg, err, grib_get_error_message(err));*/
        if (!mesg) {
            if (err == GRIB_END_OF_FILE || err == GRIB_PREMATURE_END_OF_FILE) {
                done = 1; /* reached the end */
            }
        }
        if (mesg && !err) {
            grib_context_free(c,mesg);
            (*n)++;
        }
    }
    rewind(f);
    if (err==GRIB_END_OF_FILE) err=GRIB_SUCCESS;
    return err;
}

int codes_bufr_extract_headers_malloc(grib_context* c, const char* filename, codes_bufr_header** result, int* num_messages)
{
    int err = 0, i = 0;
    FILE* fp = NULL;
    void* mesg = NULL;
    size_t size = 0;
    off_t offset = 0;

    if (!c) c=grib_context_get_default();
    fp = fopen(filename, "rb");
    if (!fp) {
        grib_context_log(c, GRIB_LOG_ERROR, "codes_bufr_extract_headers_malloc: Unable to read file \"%s\"", filename);
        perror(filename);
        return GRIB_IO_PROBLEM;
    }
    err = count_bufr_messages(c, fp, num_messages);
    if (err) {
        grib_context_log(c, GRIB_LOG_ERROR, "codes_bufr_extract_headers_malloc: Unable to count BUFR messages in file \"%s\"", filename);
        fclose(fp);
        return err;
    }

    size = *num_messages;
    if (size == 0) {
        grib_context_log(c, GRIB_LOG_ERROR, "codes_bufr_extract_headers_malloc: No BUFR messages in file \"%s\"", filename);
        return GRIB_INVALID_MESSAGE;
    }
    *result = (codes_bufr_header*)calloc(size, sizeof(codes_bufr_header));
    if (!*result) {
        fclose(fp);
        return GRIB_OUT_OF_MEMORY;
    }
    i = 0;
    while (err != GRIB_END_OF_FILE) {
        mesg = wmo_read_bufr_from_file_malloc(fp, 0, &size, &offset, &err);
        if (mesg != NULL && err == 0) {
            int err2 = bufr_decode_header(c, mesg, offset, size, &(*result)[i++]);
            if (err2) {
                fclose(fp);
                return err2;
            }
            grib_context_free(c, mesg);
        }
        if (!mesg) {
            if (err != GRIB_END_OF_FILE && err != GRIB_PREMATURE_END_OF_FILE) {
                /* An error occurred */
                grib_context_log(c, GRIB_LOG_ERROR, "codes_bufr_extract_headers_malloc: Unable to read BUFR message");
            }
        }
    }

    fclose(fp);
    return GRIB_SUCCESS;
}
