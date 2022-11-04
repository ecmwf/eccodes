/*
* (C) Copyright 2005- ECMWF.
*
* This software is licensed under the terms of the Apache Licence Version 2.0
* which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
*
* In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
* virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
*/

/* Sequences and replication (not delayed) are resolved in this class.
   Number of elements to which a delayed replication applies are recomputed to
   take account of the expansion.
   Expanded descriptors cannot contain sequences and only delayed replication
   can appear
*/

#include "grib_api_internal.h"
/*
This is used by make_class.pl

START_CLASS_DEF
CLASS      = accessor
SUPER      = grib_accessor_class_long
IMPLEMENTS = unpack_long;pack_long
IMPLEMENTS = unpack_double
IMPLEMENTS = init;dump;destroy
IMPLEMENTS = value_count; get_native_type
MEMBERS    = const char* unexpandedDescriptors
MEMBERS    = const char* sequence
MEMBERS    = const char* expandedName
MEMBERS    = const char* tablesAccessorName
MEMBERS    = bufr_descriptors_array* expanded
MEMBERS    = int rank
MEMBERS    = grib_accessor* expandedAccessor
MEMBERS    = int do_expand
MEMBERS    = grib_accessor* tablesAccessor

END_CLASS_DEF

*/

/* START_CLASS_IMP */

/*

Don't edit anything between START_CLASS_IMP and END_CLASS_IMP
Instead edit values between START_CLASS_DEF and END_CLASS_DEF
or edit "accessor.class" and rerun ./make_class.pl

*/

static int get_native_type(grib_accessor*);
static int pack_long(grib_accessor*, const long* val, size_t* len);
static int unpack_double(grib_accessor*, double* val, size_t* len);
static int unpack_long(grib_accessor*, long* val, size_t* len);
static int value_count(grib_accessor*, long*);
static void destroy(grib_context*, grib_accessor*);
static void dump(grib_accessor*, grib_dumper*);
static void init(grib_accessor*, const long, grib_arguments*);
static void init_class(grib_accessor_class*);

typedef struct grib_accessor_expanded_descriptors
{
    grib_accessor att;
    /* Members defined in gen */
    /* Members defined in long */
    /* Members defined in expanded_descriptors */
    const char* unexpandedDescriptors;
    const char* sequence;
    const char* expandedName;
    const char* tablesAccessorName;
    bufr_descriptors_array* expanded;
    int rank;
    grib_accessor* expandedAccessor;
    int do_expand;
    grib_accessor* tablesAccessor;
} grib_accessor_expanded_descriptors;

extern grib_accessor_class* grib_accessor_class_long;

static grib_accessor_class _grib_accessor_class_expanded_descriptors = {
    &grib_accessor_class_long,                      /* super */
    "expanded_descriptors",                      /* name */
    sizeof(grib_accessor_expanded_descriptors),  /* size */
    0,                           /* inited */
    &init_class,                 /* init_class */
    &init,                       /* init */
    0,                  /* post_init */
    &destroy,                    /* free mem */
    &dump,                       /* describes himself */
    0,                /* get length of section */
    0,              /* get length of string */
    &value_count,                /* get number of values */
    0,                 /* get number of bytes */
    0,                /* get offset to bytes */
    &get_native_type,            /* get native type */
    0,                /* get sub_section */
    0,               /* grib_pack procedures long */
    0,                 /* grib_pack procedures long */
    &pack_long,                  /* grib_pack procedures long */
    &unpack_long,                /* grib_unpack procedures long */
    0,                /* grib_pack procedures double */
    &unpack_double,              /* grib_unpack procedures double */
    0,                /* grib_pack procedures string */
    0,              /* grib_unpack procedures string */
    0,          /* grib_pack array procedures string */
    0,        /* grib_unpack array procedures string */
    0,                 /* grib_pack procedures bytes */
    0,               /* grib_unpack procedures bytes */
    0,            /* pack_expression */
    0,              /* notify_change */
    0,                /* update_size */
    0,             /* preferred_size */
    0,                     /* resize */
    0,      /* nearest_smaller_value */
    0,                       /* next accessor */
    0,                    /* compare vs. another accessor */
    0,      /* unpack only ith value */
    0,  /* unpack a given set of elements */
    0,     /* unpack a subarray */
    0,                      /* clear */
    0,                 /* clone accessor */
};


grib_accessor_class* grib_accessor_class_expanded_descriptors = &_grib_accessor_class_expanded_descriptors;


static void init_class(grib_accessor_class* c)
{
    c->next_offset    =    (*(c->super))->next_offset;
    c->string_length    =    (*(c->super))->string_length;
    c->byte_count    =    (*(c->super))->byte_count;
    c->byte_offset    =    (*(c->super))->byte_offset;
    c->sub_section    =    (*(c->super))->sub_section;
    c->pack_missing    =    (*(c->super))->pack_missing;
    c->is_missing    =    (*(c->super))->is_missing;
    c->pack_double    =    (*(c->super))->pack_double;
    c->pack_string    =    (*(c->super))->pack_string;
    c->unpack_string    =    (*(c->super))->unpack_string;
    c->pack_string_array    =    (*(c->super))->pack_string_array;
    c->unpack_string_array    =    (*(c->super))->unpack_string_array;
    c->pack_bytes    =    (*(c->super))->pack_bytes;
    c->unpack_bytes    =    (*(c->super))->unpack_bytes;
    c->pack_expression    =    (*(c->super))->pack_expression;
    c->notify_change    =    (*(c->super))->notify_change;
    c->update_size    =    (*(c->super))->update_size;
    c->preferred_size    =    (*(c->super))->preferred_size;
    c->resize    =    (*(c->super))->resize;
    c->nearest_smaller_value    =    (*(c->super))->nearest_smaller_value;
    c->next    =    (*(c->super))->next;
    c->compare    =    (*(c->super))->compare;
    c->unpack_double_element    =    (*(c->super))->unpack_double_element;
    c->unpack_double_element_set    =    (*(c->super))->unpack_double_element_set;
    c->unpack_double_subarray    =    (*(c->super))->unpack_double_subarray;
    c->clear    =    (*(c->super))->clear;
    c->make_clone    =    (*(c->super))->make_clone;
}

/* END_CLASS_IMP */

#define MYDEBUG 0
#define DESC_SIZE_INIT 400 /* Initial size for grib_bufr_descriptors_array_new */
#define DESC_SIZE_INCR 400 /* Increment size for grib_bufr_descriptors_array_new */

typedef struct change_coding_params
{
    int associatedFieldWidth;
    int localDescriptorWidth;
    int extraWidth;
    int extraScale;
    int newStringWidth;
    double referenceFactor;
} change_coding_params;

/* Handy macro to catch errors.
 * Arguments: array is a pointer to 'bufr_descriptors_array', result is pointer to 'bufr_descriptor' */
#define DESCRIPTORS_POP_FRONT_OR_RETURN(array, result)         \
    {                                                          \
        if (array->n == 0) {                                   \
            *err = GRIB_INTERNAL_ERROR;                        \
            return;                                            \
        }                                                      \
        result = grib_bufr_descriptors_array_pop_front(array); \
    }

static void init(grib_accessor* a, const long len, grib_arguments* args)
{
    grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
    int n                                    = 0;
    grib_handle* hand                        = grib_handle_of_accessor(a);
    self->tablesAccessorName                 = grib_arguments_get_name(hand, args, n++);
    self->expandedName                       = grib_arguments_get_name(hand, args, n++);
    self->rank                               = grib_arguments_get_long(hand, args, n++);
    if (self->rank != 0) {
        self->expandedAccessor = grib_find_accessor(hand, self->expandedName);
    }
    else {
        self->expandedAccessor = 0;
    }
    self->unexpandedDescriptors = grib_arguments_get_name(hand, args, n++);
    self->sequence              = grib_arguments_get_name(hand, args, n++);
    self->do_expand             = 1;
    self->expanded              = 0;
    a->length                   = 0;
}

static void dump(grib_accessor* a, grib_dumper* dumper)
{
    grib_dump_long(dumper, a, NULL);
}

static bufr_descriptors_array* do_expand(grib_accessor* a, bufr_descriptors_array* unexpanded, change_coding_params* ccp, int* err);

#define BUFR_DESCRIPTORS_ARRAY_USED_SIZE(v) ((v)->n)
#define SILENT 1

#if MYDEBUG
static int global_depth = -1;

static char* descriptor_type_name(int dtype)
{
    switch (dtype) {
        case BUFR_DESCRIPTOR_TYPE_STRING:      return "string";
        case BUFR_DESCRIPTOR_TYPE_LONG:        return "long";
        case BUFR_DESCRIPTOR_TYPE_DOUBLE:      return "double";
        case BUFR_DESCRIPTOR_TYPE_TABLE:       return "table";
        case BUFR_DESCRIPTOR_TYPE_FLAG:        return "flag";
        case BUFR_DESCRIPTOR_TYPE_UNKNOWN:     return "unknown";
        case BUFR_DESCRIPTOR_TYPE_REPLICATION: return "replication";
        case BUFR_DESCRIPTOR_TYPE_OPERATOR:    return "operator";
        case BUFR_DESCRIPTOR_TYPE_SEQUENCE:    return "sequence";
    }
    Assert(!"bufr_descriptor_type_name failed");
    return "unknown";
}
#endif

static void __expand(grib_accessor* a, bufr_descriptors_array* unexpanded, bufr_descriptors_array* expanded,
                     change_coding_params* ccp, int* err)
{
    int k, j, i;
    grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
    size_t size                              = 0;
    long* v_array                            = NULL;
    bufr_descriptor* u                       = NULL;
    bufr_descriptor* vv                      = NULL;
    /* ECC-1422: 'ur' is the array of bufr_descriptor pointers for replications.
     * Its max size is X (from FXY) which is 6 bits so no need for malloc */
    bufr_descriptor* ur[65]                  = {0,};
    bufr_descriptor* urc                     = NULL;
    int idx;
    bufr_descriptor* u0                      = NULL;
    grib_context* c                          = a->context;
    bufr_descriptor* us                      = NULL;
    bufr_descriptors_array* inner_expanded   = NULL;
    bufr_descriptors_array* inner_unexpanded = NULL;
    grib_handle* hand                        = grib_handle_of_accessor(a);
#if MYDEBUG
    int idepth;
#endif

    if (BUFR_DESCRIPTORS_ARRAY_USED_SIZE(unexpanded) == 0)
        return;

    us          = grib_bufr_descriptor_clone(grib_bufr_descriptors_array_get(unexpanded, 0));
    us->context = c;

    *err = 0;
#if MYDEBUG
    for (idepth = 0; idepth < global_depth; idepth++)
        printf("\t");
    printf("expanding ==> %d-%02d-%03d\n", us->F, us->X, us->Y);
#endif
    switch (us->F) {
        case 3:
            /* sequence */
            DESCRIPTORS_POP_FRONT_OR_RETURN(unexpanded, u);
#if MYDEBUG
            for (idepth = 0; idepth < global_depth; idepth++)
                printf("\t");
            printf("+++ pop  %06ld [%s]\n", u->code, descriptor_type_name(u->type));
#endif
            /*this is to get the sequence elements of the sequence unexpanded[i] */
            *err = grib_set_long(hand, self->sequence, u->code);
            *err = grib_get_size(hand, self->sequence, &size);
            grib_bufr_descriptor_delete(u);
            if (*err)
                goto cleanup;
            v_array = (long*)grib_context_malloc_clear(c, sizeof(long) * size);
            *err = grib_get_long_array(hand, self->sequence, v_array, &size);
            if (*err)
                goto cleanup;

            inner_unexpanded = grib_bufr_descriptors_array_new(c, DESC_SIZE_INIT, DESC_SIZE_INCR);
            for (i = 0; i < size; i++) {
                vv               = grib_bufr_descriptor_new(self->tablesAccessor, v_array[i], !SILENT, err);
                inner_unexpanded = grib_bufr_descriptors_array_push(inner_unexpanded, vv);
            }
            grib_context_free(c, v_array);
            inner_expanded = do_expand(a, inner_unexpanded, ccp, err);
            if (*err)
                return;
            grib_bufr_descriptors_array_delete(inner_unexpanded);
#if MYDEBUG
            for (i = 0; i < inner_expanded->n; i++) {
                for (idepth = 0; idepth < global_depth; idepth++)
                    printf("\t");
                printf("+++ push %06ld\n", inner_expanded->v[i]->code);
            }
#endif
            size     = BUFR_DESCRIPTORS_ARRAY_USED_SIZE(inner_expanded);
            grib_bufr_descriptors_array_append(expanded, inner_expanded);
            break;

        case 1:
            if (us->Y == 0) {
                /* delayed replication */
                bufr_descriptor* uidx = 0;
                DESCRIPTORS_POP_FRONT_OR_RETURN(unexpanded, u);
#if MYDEBUG
                for (idepth = 0; idepth < global_depth; idepth++)
                    printf("\t");
                printf("+++ pop  %06ld [%s]\n", u->code, descriptor_type_name(u->type));
                for (idepth = 0; idepth < global_depth; idepth++)
                    printf("\t");
                printf("+++ push %06ld\n", u->code);
#endif
                grib_bufr_descriptors_array_push(expanded, u);
                idx              = expanded->n - 1;
                size             = 0;
                inner_unexpanded = grib_bufr_descriptors_array_new(c, DESC_SIZE_INIT, DESC_SIZE_INCR);

                /* Number of descriptors to replicate cannot be more than what's left */
                if (us->X + 1 > unexpanded->n) {
                    grib_context_log(c, GRIB_LOG_ERROR,
                                     "Delayed replication: %06ld: expected %d but only found %lu elements",
                                     u->code, us->X, unexpanded->n - 1);
                    *err = GRIB_DECODING_ERROR;
                    return;
                }
                for (j = 0; j < us->X + 1; j++) {
                    DESCRIPTORS_POP_FRONT_OR_RETURN(unexpanded, u0);
                    grib_bufr_descriptors_array_push(inner_unexpanded, u0);
#if MYDEBUG
                    for (idepth = 0; idepth < global_depth; idepth++)
                        printf("\t");
                    printf("+++ pop  %06ld [%s]\n", u0->code, descriptor_type_name(u0->type));
#endif
                }
                inner_expanded = do_expand(a, inner_unexpanded, ccp, err);
                if (*err)
                    return;
                grib_bufr_descriptors_array_delete(inner_unexpanded);
                size = BUFR_DESCRIPTORS_ARRAY_USED_SIZE(inner_expanded);
#if MYDEBUG
                for (i = 0; i < inner_expanded->n; i++) {
                    for (idepth = 0; idepth < global_depth; idepth++)
                        printf("\t");
                    printf("+++ push %06ld\n", inner_expanded->v[i]->code);
                }
#endif
                expanded = grib_bufr_descriptors_array_append(expanded, inner_expanded);
                uidx     = grib_bufr_descriptors_array_get(expanded, idx);
                if (size > 100) {
                    grib_context_log(c, GRIB_LOG_ERROR,
                                     "Delayed replication %06ld: Too many elements (%lu). "
                            "Hint: This may be due to associated field descriptors", uidx->code, size);
                    *err = GRIB_DECODING_ERROR;
                    return;
                }
                grib_bufr_descriptor_set_code(0, (size - 1) * 1000 + 100000, uidx);
                size++;
            }
            else {
                /* replication with fixed number of descriptors (non-delayed) */
                DESCRIPTORS_POP_FRONT_OR_RETURN(unexpanded, u);
#if MYDEBUG
                for (idepth = 0; idepth < global_depth; idepth++)
                    printf("\t");
                printf("+++ pop  %06ld [%s]\n", u->code, descriptor_type_name(u->type));
#endif
                grib_bufr_descriptor_delete(u);
                size = us->X * us->Y;
                memset(ur, 0, us->X); /* ECC-1422 */
                for (j = 0; j < us->X; j++) {
                    DESCRIPTORS_POP_FRONT_OR_RETURN(unexpanded, ur[j]);
#if MYDEBUG
                    for (idepth = 0; idepth < global_depth; idepth++)
                        printf("\t");
                    printf("+++ pop  %06ld [%s]\n", ur[j]->code, descriptor_type_name(ur[j]->type));
#endif
                }
                inner_unexpanded = grib_bufr_descriptors_array_new(c, DESC_SIZE_INIT, DESC_SIZE_INCR);
                for (j = 0; j < us->X; j++) {
                    urc = grib_bufr_descriptor_clone(ur[j]);
                    grib_bufr_descriptors_array_push(inner_unexpanded, urc);
                }
                for (k = 1; k < us->Y; k++) {
                    for (j = 0; j < us->X; j++) {
                        urc = grib_bufr_descriptor_clone(ur[j]);
                        grib_bufr_descriptors_array_push(inner_unexpanded, urc);
                    }
                }
                for (i = 0; i < us->X; i++)
                    grib_bufr_descriptor_delete(ur[i]);

                inner_expanded = do_expand(a, inner_unexpanded, ccp, err);
                if (*err)
                    return;
                grib_bufr_descriptors_array_delete(inner_unexpanded);
#if MYDEBUG
                for (i = 0; i < inner_expanded->n; i++) {
                    for (idepth = 0; idepth < global_depth; idepth++)
                        printf("\t");
                    printf("+++ push %06ld\n", inner_expanded->v[i]->code);
                }
#endif
                size     = BUFR_DESCRIPTORS_ARRAY_USED_SIZE(inner_expanded);
                grib_bufr_descriptors_array_append(expanded, inner_expanded);
            }
            break;

        case 0:
            DESCRIPTORS_POP_FRONT_OR_RETURN(unexpanded, u);
            size = 1;
            if (ccp->associatedFieldWidth && u->X != 31) {
                bufr_descriptor* au = grib_bufr_descriptor_new(self->tablesAccessor, 999999, !SILENT, err);
                au->width           = ccp->associatedFieldWidth;
                grib_bufr_descriptor_set_scale(au, 0);
                strcpy(au->shortName, "associatedField");
                /* au->name=grib_context_strdup(c,"associated field");  See ECC-489 */
                strcpy(au->units, "associated units");
#if MYDEBUG
                for (idepth = 0; idepth < global_depth; idepth++)
                    printf("\t");
                printf("+++ push %06ld (s=%ld r=%ld w=%ld)", au->code, au->scale, au->reference, au->width);
#endif
                grib_bufr_descriptors_array_push(expanded, au);
                size++;
            }
#if MYDEBUG
            for (idepth = 0; idepth < global_depth; idepth++)
                printf("\t");
            printf("+++ pop  %06ld [%s]\n", u->code, descriptor_type_name(u->type));
            for (idepth = 0; idepth < global_depth; idepth++)
                printf("\t");
            printf("+++ push %06ld [%s] (s=%ld r=%ld w=%ld)",
                   u->code, descriptor_type_name(u->type), u->scale, u->reference, u->width);
#endif
            if (u->type != BUFR_DESCRIPTOR_TYPE_FLAG &&
                u->type != BUFR_DESCRIPTOR_TYPE_TABLE &&
                u->type != BUFR_DESCRIPTOR_TYPE_STRING) {
                if (ccp->localDescriptorWidth > 0) {
                    u->width     = ccp->localDescriptorWidth;
                    u->reference = 0;
                    grib_bufr_descriptor_set_scale(u, 0);
                    ccp->localDescriptorWidth = 0;
                }
                else {
                    u->width += ccp->extraWidth;
                    u->reference *= ccp->referenceFactor;
                    grib_bufr_descriptor_set_scale(u, u->scale + ccp->extraScale);
                }
            }
            else if (u->type == BUFR_DESCRIPTOR_TYPE_STRING && ccp->newStringWidth != 0) {
                u->width = ccp->newStringWidth;
            }
#if MYDEBUG
            printf("->(s=%ld r=%ld w=%ld)\n", u->scale, u->reference, u->width);
#endif
            grib_bufr_descriptors_array_push(expanded, u);
            break;

        case 2:
            DESCRIPTORS_POP_FRONT_OR_RETURN(unexpanded, u);
#if MYDEBUG
            for (idepth = 0; idepth < global_depth; idepth++)
                printf("\t");
            printf("+++ pop  %06ld [%s]\n", u->code, descriptor_type_name(u->type));
#endif
            switch (us->X) {
                case 1:
                    ccp->extraWidth = us->Y ? us->Y - 128 : 0;
                    size            = 0;
                    grib_bufr_descriptor_delete(u);
                    break;
                case 2:
                    ccp->extraScale = us->Y ? us->Y - 128 : 0;
                    size            = 0;
                    grib_bufr_descriptor_delete(u);
                    break;
                case 4:
                    /* associated field*/
                    ccp->associatedFieldWidth = us->Y;
                    grib_bufr_descriptor_delete(u);
                    break;
                case 6:
                    /*signify data width*/
                    ccp->localDescriptorWidth = us->Y;
                    size                      = 0;
                    grib_bufr_descriptor_delete(u);
                    break;
                case 7:
                    if (us->Y) {
                        ccp->extraScale      = us->Y;
                        ccp->referenceFactor = grib_power(us->Y, 10);
                        ccp->extraWidth      = ((10 * us->Y) + 2) / 3;
                    }
                    else {
                        ccp->extraWidth      = 0;
                        ccp->extraScale      = 0;
                        ccp->referenceFactor = 1;
                    }
                    size = 0;
                    grib_bufr_descriptor_delete(u);
                    break;
                case 8:
                    ccp->newStringWidth = us->Y * 8;
                    break;
                default:
#if MYDEBUG
                    for (idepth = 0; idepth < global_depth; idepth++)
                        printf("\t");
                    printf("+++ push %06ld\n", u->code);
#endif
                    grib_bufr_descriptors_array_push(expanded, u);
                    size = 1;
            }
            break;

        default:
            DESCRIPTORS_POP_FRONT_OR_RETURN(unexpanded, u);
#if MYDEBUG
            for (idepth = 0; idepth < global_depth; idepth++)
                printf("\t");
            printf("+++ pop  %06ld [%s]\n", u->code, descriptor_type_name(u->type));
            for (idepth = 0; idepth < global_depth; idepth++)
                printf("\t");
            printf("+++ push %06ld\n", u->code);
#endif
            grib_bufr_descriptors_array_push(expanded, u);
            size = 1;
    }
#if MYDEBUG
    for (idepth = 0; idepth < global_depth; idepth++)
        printf("\t");
    printf("expanding <== %d-%.2d-%.3d (size=%ld)\n\n", us->F, us->X, us->Y, size);
#endif
cleanup:
    if (us) grib_bufr_descriptor_delete(us);
}

static bufr_descriptors_array* do_expand(grib_accessor* a, bufr_descriptors_array* unexpanded, change_coding_params* ccp, int* err)
{
    bufr_descriptors_array* expanded = NULL;
    grib_context* c                  = a->context;
#if MYDEBUG
    int idepth;
    global_depth++;
#endif

    expanded = grib_bufr_descriptors_array_new(c, DESC_SIZE_INIT, DESC_SIZE_INCR);

#if MYDEBUG
    {
        int i;
        for (idepth = 0; idepth < global_depth; idepth++)
            printf("\t");
        printf("to be expanded ==> \n");
        for (i = 0; i < unexpanded->n; i++) {
            bufr_descriptor* xx = grib_bufr_descriptors_array_get(unexpanded, i);
            for (idepth = 0; idepth < global_depth; idepth++)
                printf("\t");
            printf("%06ld\n", xx->code);
        }
        for (idepth = 0; idepth < global_depth; idepth++)
            printf("\t");
        printf("to be expanded <== \n\n");
    }
#endif
    while (unexpanded->n) {
        __expand(a, unexpanded, expanded, ccp, err);
        if (*err) {
            grib_bufr_descriptors_array_delete(expanded);
            return NULL;
        }
    }
#if MYDEBUG
    {
        int i;
        for (idepth = 0; idepth < global_depth; idepth++)
            printf("\t");
        printf("expanded ==> \n");
        for (i = 0; i < expanded->n; i++) {
            bufr_descriptor* xx = grib_bufr_descriptors_array_get(expanded, i);
            for (idepth = 0; idepth < global_depth; idepth++)
                printf("\t");
            printf("==  %-6d== %06ld ", i, xx->code);
            printf("s=%ld r=%ld w=%ld", xx->scale, xx->reference, xx->width);
            printf("\n");
        }
        for (idepth = 0; idepth < global_depth; idepth++)
            printf("\t");
        printf("expanded <== \n\n");
    }
#endif

#if MYDEBUG
    global_depth--;
#endif

    return expanded;
}

static int expand(grib_accessor* a)
{
    grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
    int err                                  = 0;
    size_t unexpandedSize                    = 0;
    /* grib_iarray* unexp=0; */
    int i;
    long* u      = 0;
    char key[50] = {0,};
    long centre, masterTablesVersionNumber, localTablesVersionNumber, masterTablesNumber;
    change_coding_params ccp;
    bufr_descriptors_array* unexpanded      = NULL;
    bufr_descriptors_array* unexpanded_copy = NULL;
    bufr_descriptors_array* expanded        = NULL;
    grib_context* c                         = a->context;
    grib_handle* h                          = grib_handle_of_accessor(a);
    int operator206yyy_width                = 0; /* width specified by operator 206YYY */

    if (!self->do_expand) {
        return err;
    }
    self->do_expand = 0;
    if (self->rank != 0) {
        err            = expand(self->expandedAccessor);
        self->expanded = ((grib_accessor_expanded_descriptors*)self->expandedAccessor)->expanded;
        return err;
    }

    err = grib_get_size(h, self->unexpandedDescriptors, &unexpandedSize);
    if (err)
        return err;
    if (unexpandedSize == 0) {
        grib_context_log(c, GRIB_LOG_ERROR, "%s: Unexpanded size is zero!", a->name);
        return GRIB_DECODING_ERROR;
    }
    
    u = (long*)grib_context_malloc_clear(c, sizeof(long) * unexpandedSize);
    if (!u) {
        err = GRIB_OUT_OF_MEMORY;
        return err;
    }
    err = grib_get_long_array(h, self->unexpandedDescriptors, u, &unexpandedSize);
    if (err)
        return err;

    err = grib_get_long(h, "bufrHeaderCentre", &centre);
    if (err)
        return err;
    err = grib_get_long(h, "masterTablesVersionNumber", &masterTablesVersionNumber);
    if (err)
        return err;
    err = grib_get_long(h, "localTablesVersionNumber", &localTablesVersionNumber);
    if (err)
        return err;
    err = grib_get_long(h, "masterTableNumber", &masterTablesNumber);
    if (err)
        return err;

    sprintf(key, "%ld_%ld_%ld_%ld_%ld", centre, masterTablesVersionNumber, localTablesVersionNumber, masterTablesNumber, u[0]);
    expanded = grib_context_expanded_descriptors_list_get(c, key, u, unexpandedSize);
    if (expanded) {
        self->expanded = expanded;
        grib_context_free(c, u);
        return GRIB_SUCCESS;
    }

    if (!self->tablesAccessor) {
        self->tablesAccessor = grib_find_accessor(h, self->tablesAccessorName);
        Assert(self->tablesAccessor);
    }

    unexpanded           = grib_bufr_descriptors_array_new(c, unexpandedSize, DESC_SIZE_INCR);
    unexpanded_copy      = grib_bufr_descriptors_array_new(c, unexpandedSize, DESC_SIZE_INCR);
    operator206yyy_width = 0;
    for (i = 0; i < unexpandedSize; i++) {
        bufr_descriptor *aDescriptor1, *aDescriptor2;
        /* ECC-1274: clear error and only issue msg once */
        err = 0;
        aDescriptor1 = grib_bufr_descriptor_new(self->tablesAccessor, u[i], SILENT, &err);
        err = 0;
        aDescriptor2 = grib_bufr_descriptor_new(self->tablesAccessor, u[i], !SILENT, &err);

        /* ECC-433: Operator 206YYY */
        if (aDescriptor1->F == 2 && aDescriptor1->X == 6) {
            Assert(aDescriptor1->type == BUFR_DESCRIPTOR_TYPE_OPERATOR);
            operator206yyy_width = aDescriptor1->Y; /* Store the width for the following descriptor */
            DebugAssert(operator206yyy_width > 0);
        }
        else if (operator206yyy_width > 0) {
            if (err == GRIB_NOT_FOUND) {
                DebugAssert(aDescriptor1->type == BUFR_DESCRIPTOR_TYPE_UNKNOWN);
                err                 = 0;                       /* Clear any error generated due to local descriptor */
                aDescriptor1->nokey = aDescriptor2->nokey = 1; /* Do not show this descriptor in dump */
            }
            /* The width specified by operator takes precedence over element's own width */
            aDescriptor1->width = aDescriptor2->width = operator206yyy_width;
            operator206yyy_width                      = 0; /* Restore. Operator no longer in scope */
        }

        grib_bufr_descriptors_array_push(unexpanded, aDescriptor1);
        grib_bufr_descriptors_array_push(unexpanded_copy, aDescriptor2);
    }

    grib_context_free(c, u);

    ccp.extraWidth           = 0;
    ccp.localDescriptorWidth = -1;
    ccp.extraScale           = 0;
    ccp.referenceFactor      = 1;
    ccp.associatedFieldWidth = 0;
    ccp.newStringWidth       = 0;
    self->expanded           = do_expand(a, unexpanded, &ccp, &err);
    if (err) {
        grib_bufr_descriptors_array_delete(unexpanded);
        grib_bufr_descriptors_array_delete(unexpanded_copy);
        return err;
    }
    grib_context_expanded_descriptors_list_push(c, key, self->expanded, unexpanded_copy);
    grib_bufr_descriptors_array_delete(unexpanded);

    return err;
}

int grib_accessor_class_expanded_descriptors_set_do_expand(grib_accessor* a, long do_expand)
{
    grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
    self->do_expand                          = do_expand;
    return 0;
}

bufr_descriptors_array* grib_accessor_class_expanded_descriptors_get_expanded(grib_accessor* a, int* err)
{
    grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
    *err                                     = expand(a);
    return self->expanded;
}

static int unpack_double(grib_accessor* a, double* val, size_t* len)
{
    grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
    int ret                                  = 0;
    int i;
    size_t expandedSize;

    if (self->rank != 2) {
        long* lval = (long*)grib_context_malloc_clear(a->context, *len * sizeof(long));
        ret        = unpack_long(a, lval, len);
        if (ret)
            return ret;
        for (i = 0; i < *len; i++)
            val[i] = (double)lval[i];
        grib_context_free(a->context, lval);
    }
    else {
        ret = expand(a);
        if (ret)
            return ret;

        expandedSize = BUFR_DESCRIPTORS_ARRAY_USED_SIZE(self->expanded);
        if (*len < expandedSize) {
            grib_context_log(a->context, GRIB_LOG_ERROR,
                             "Wrong size (%ld) for %s, it contains %lu values", *len, a->name, expandedSize);
            *len = 0;
            return GRIB_ARRAY_TOO_SMALL;
        }
        *len = expandedSize;
        for (i = 0; i < *len; i++)
            val[i] = self->expanded->v[i]->reference;
    }
    return ret;
}

static int unpack_long(grib_accessor* a, long* val, size_t* len)
{
    grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
    int ret                                  = 0;
    size_t rlen                              = 0;
    size_t i;

    ret = expand(a);
    if (ret)
        return ret;
    if (!self->expanded)
        return GRIB_DECODING_ERROR;
    rlen = BUFR_DESCRIPTORS_ARRAY_USED_SIZE(self->expanded);

    if (*len < rlen) {
        grib_context_log(a->context, GRIB_LOG_ERROR,
                         "Wrong size (%ld) for %s, it contains %lu values", *len, a->name, rlen);
        *len = 0;
        return GRIB_ARRAY_TOO_SMALL;
    }

    *len = rlen;
    switch (self->rank) {
        case 0:
            for (i = 0; i < *len; i++)
                val[i] = self->expanded->v[i]->code;
            break;
        case 1:
            for (i = 0; i < *len; i++)
                val[i] = self->expanded->v[i]->scale;
            break;
        case 2:
            return GRIB_INVALID_TYPE;
            break;
        case 3:
            for (i = 0; i < *len; i++)
                val[i] = self->expanded->v[i]->width;
            break;
        case 4:
            for (i = 0; i < *len; i++)
                val[i] = self->expanded->v[i]->type;
            break;
    }

    return GRIB_SUCCESS;
}

static int pack_long(grib_accessor* a, const long* val, size_t* len)
{
    grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
    self->do_expand                          = 1;
    return GRIB_NOT_IMPLEMENTED;
}

static int value_count(grib_accessor* a, long* rlen)
{
    grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
    int err                                  = 0;
    *rlen                                    = 0;

    err = expand(a);
    if (err) {
        grib_context_log(a->context, GRIB_LOG_ERROR, "%s unable to compute size", a->name);
        grib_bufr_descriptors_array_delete(self->expanded);
        return err;
    }
    *rlen = BUFR_DESCRIPTORS_ARRAY_USED_SIZE(self->expanded);
    return err;
}

static void destroy(grib_context* c, grib_accessor* a)
{
    /* grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a; */
    /* if (self->rank==0 && self->expanded) { */
    /* grib_bufr_descriptors_array_delete(self->expanded); */
    /* } */
}

static int get_native_type(grib_accessor* a)
{
    grib_accessor_expanded_descriptors* self = (grib_accessor_expanded_descriptors*)a;
    if (self->rank == 2)
        return GRIB_TYPE_DOUBLE;
    else
        return GRIB_TYPE_LONG;
}
