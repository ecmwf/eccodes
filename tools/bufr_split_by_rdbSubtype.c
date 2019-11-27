/*
 * Copyright 2005-2019 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/*
 * Description:
 * Split an input BUFR file into output files according to 'rdbSubtype'.
 * The output files are named split_rdbSubtype.<rdbSubtype>.bufr.
 * This is much faster than bufr_copy/bufr_filter
 *
 */

#include "grib_api_internal.h"

static int verbose = 0;
static const char* OUTPUT_FILENAME_DEFAULT = "split_rdbSubtype.undef.bufr";
static const char* OUTPUT_FILENAME_SUBTYPE = "split_rdbSubtype.%ld.bufr";


static void usage(const char* prog)
{
    printf("Usage: %s [-v] infile\n",prog);
    exit(1);
}
static int file_exists(const char* path)
{
    struct stat s;
    int stat_val = stat(path,&s);
    if (stat_val != 0) return 0; /*error doing stat*/
    return S_ISREG( s.st_mode );
}

/* If rdbSubtype can be extracted, return GRIB_SUCCESS otherwise error code. */
/* If BUFR message does not have an ECMWF local section, set rdbSubtype to -1 */
static int decode_rdbSubtype(const void* message, long* rdbSubtype)
{
    int err = GRIB_SUCCESS;
    long edition = 0;
    long pos_edition = 7*8;
    const long nbits_edition = 8;
    long section1Flags=0;
    long nbits_section1Flags=1*8;
    long pos_section1Flags=17*8;
    long section1Length = 0;
    const long nbits_section1Length = 24;

    long bufrHeaderCentre = 0;
    long nbits_bufrHeaderCentre=2*8;
    long pos_bufrHeaderCentre=12*8;

    long pos_section1Length = 8*8;
    int ecmwfLocalSectionPresent = 0;

    Assert(message);
    *rdbSubtype = -1; /* default */

    edition = (long)grib_decode_unsigned_long(message, &pos_edition, nbits_edition);
    if (edition!=2 && edition!=3 && edition!=4) {
        fprintf(stderr, "ERROR: Unsupported BUFR edition: %ld", edition);
        return GRIB_DECODING_ERROR;
    }
    section1Length=(long)grib_decode_unsigned_long(message, &pos_section1Length, nbits_section1Length);
    if (edition==3) {
        pos_section1Flags=15*8;
        nbits_bufrHeaderCentre=1*8;
        pos_bufrHeaderCentre=13*8;
    }
    if (edition==2) {
        pos_section1Flags=15*8;
    }

    bufrHeaderCentre = (long)grib_decode_unsigned_long(message, &pos_bufrHeaderCentre, nbits_bufrHeaderCentre);

    section1Flags=(long)grib_decode_unsigned_long(message, &pos_section1Flags, nbits_section1Flags);
    if (section1Flags != 0 && section1Flags != 128) {
        fprintf(stderr, "ERROR: Invalid BUFR section1 flags: %ld", section1Flags);
        return GRIB_DECODING_ERROR;
    }
    ecmwfLocalSectionPresent = (bufrHeaderCentre==98 && section1Flags!=0);

    /*printf("SPLT: edition=%ld section1Length=%ld bufrHeaderCentre=%ld section1Flags=%ld", edition,section1Length,bufrHeaderCentre,section1Flags);*/
    if ( ecmwfLocalSectionPresent ) {
        long oldSubtype=0;
        long newSubtype=0;
        long nbits_oldSubtype = 1*8;
        long nbits_newSubtype = 2*8;
        const long section0Length = 8;
        long pos_oldSubtype = (section0Length + section1Length + 5)*8;
        long pos_newSubtype = (section0Length + section1Length + 49)*8;

        oldSubtype=(long)grib_decode_unsigned_long(message, &pos_oldSubtype, nbits_oldSubtype);
        newSubtype=(long)grib_decode_unsigned_long(message, &pos_newSubtype, nbits_newSubtype);
        /*printf(" oldSubtype=%ld newSubtype=%ld\n", oldSubtype, newSubtype);*/

        if (oldSubtype<255) *rdbSubtype=oldSubtype;
        else                *rdbSubtype=newSubtype;
    }
    /*else printf(" oldSubtype=undef newSubtype=undef\n");*/
    return err;
}
static int split_file_by_subtype(FILE* in, const char* filename, unsigned long *count)
{
    void* mesg=NULL;
    FILE* out;
    size_t size=0;
    off_t offset=0;
    int err=GRIB_SUCCESS;
    char ofilename[2048]={0,};
    grib_context* c=grib_context_get_default();

    if (!in) return 1;
    sprintf(ofilename, "%s", OUTPUT_FILENAME_DEFAULT); /*default name*/

    while ( err!=GRIB_END_OF_FILE ) {
        mesg=wmo_read_bufr_from_file_malloc(in, 0, &size, &offset, &err);
        if (mesg!=NULL && err==0) {
            /* Decode subtype from mesg */
            long rdbSubtype=0;
            int status = decode_rdbSubtype(mesg, &rdbSubtype);
            if (status != GRIB_SUCCESS) {
                fprintf(stderr,"ERROR: Failed to decode rdbSubtype from message %lu\n", *count);
                return status;
            }

            sprintf(ofilename, "%s", OUTPUT_FILENAME_DEFAULT);
            if (rdbSubtype != -1) sprintf(ofilename, OUTPUT_FILENAME_SUBTYPE, rdbSubtype);

            if (verbose) {
                if (!file_exists(ofilename)) printf("Writing output to %s\n", ofilename);
            }
            out=fopen(ofilename,"ab");
            if (!out) {
                fprintf(stderr,"ERROR: Failed to open output file '%s'\n", ofilename);
                perror(ofilename);
                return GRIB_IO_PROBLEM;
            }
            if (fwrite(mesg,1,size,out)!=size) {
                fprintf(stderr,"ERROR: Failed to append to file '%s'\n", ofilename);
                perror(ofilename);
                fclose(out);
                return GRIB_IO_PROBLEM;
            }
            grib_context_free(c,mesg);
            fclose(out);
            (*count)++;
        }
    }

    if (err==GRIB_END_OF_FILE) err=GRIB_SUCCESS;

    return err;
}

int main(int argc,char* argv[])
{
    FILE* infh = NULL;
    char* filename;
    int i, status=0;
    struct stat s;
    int err=0;
    unsigned long count=0;

    if (argc!=2 && argc!=3) usage(argv[0]);

    i=1;
    if (strcmp(argv[i], "-v")==0) {
        i++;
        verbose = 1;
        if (argc !=3) usage(argv[0]);
    }

    filename=argv[i];
    if (stat(filename, &s)==0) {
        if (S_ISDIR(s.st_mode)) {
            fprintf(stderr, "ERROR: %s: Is a directory\n", filename);
            return 1;
        }
    }
    infh=fopen(filename,"rb");
    if (!infh) {
        perror(filename);
        return 1;
    }

    count=0;
    err=split_file_by_subtype(infh, filename, &count);
    if (err) {
        fprintf(stderr,"ERROR: Failed to split BUFR file %s", filename);
        fprintf(stderr,"\n");
        status = 1;
    } else {
        if (verbose) printf ("%7lu %s\n", count, filename);
    }

    fclose(infh);

    return status;
}
