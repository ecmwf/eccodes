/*
 * Copyright 2005-2018 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

/***************************************************************************
 *   Jean Baptiste Filippi - 01.11.2005                                                           *
 *                                                                         *
 ***************************************************************************/
#include "grib_api_internal.h"

void grib_get_buffer_ownership(const grib_context *c, grib_buffer *b)
{
    unsigned char* newdata;
    if(b->property == GRIB_MY_BUFFER)
        return;

    newdata = (unsigned char*)grib_context_malloc(c, b->length);
    memcpy(newdata, b->data, b->length);
    b->data = newdata;
    b->property = GRIB_MY_BUFFER;
}

grib_buffer* grib_create_growable_buffer(const grib_context* c)
{
    grib_buffer  *b =  (grib_buffer*)grib_context_malloc_clear(c,sizeof(grib_buffer));

    if(b == NULL)
    {
        grib_context_log(c,GRIB_LOG_ERROR,"grib_new_buffer: cannot allocate buffer");
        return NULL;
    }

    b->property = GRIB_MY_BUFFER;
    b->length   = 10240;
    b->ulength  = 0;
    b->data     = (unsigned char*)grib_context_malloc_clear(c,b->length);
    b->growable = 1;

    if(!b->data)
    {
        grib_context_log(c,GRIB_LOG_ERROR,"grib_new_buffer: cannot allocate buffer");
        grib_context_free(c,b);
        return NULL;
    }

    return b;
}

grib_buffer* grib_new_buffer(const grib_context* c, const unsigned char* data, size_t buflen)
{
    grib_buffer  *b =  (grib_buffer*)grib_context_malloc_clear(c,sizeof(grib_buffer));

    if(b == NULL)
    {
        grib_context_log(c,GRIB_LOG_ERROR,"grib_new_buffer: cannot allocate buffer");
        return NULL;
    }

    b->property = GRIB_USER_BUFFER;
    b->length   = buflen;
    b->ulength  = buflen;
    b->ulength_bits  = buflen*8;
    b->data     = (unsigned char*)data;

    return b;
}

void grib_buffer_delete(const grib_context *c, grib_buffer *b)
{
    if(b->property == GRIB_MY_BUFFER)
        grib_context_free(c,b->data);
    b->length = 0;
    b->ulength = 0;
    grib_context_free(c,b);
}

static void grib_grow_buffer_to(const grib_context *c, grib_buffer *b, size_t ns)
{
    unsigned char* newdata;

    if(ns>b->length)
    {
        grib_get_buffer_ownership(c, b);
        newdata = (unsigned char*)grib_context_malloc_clear(c, ns);
        memcpy(newdata, b->data, b->length);
        grib_context_free(c,b->data);
        b->data = newdata;
        b->length = ns;
    }
}

void grib_grow_buffer(const grib_context *c, grib_buffer *b, size_t new_size)
{
    if (new_size > b->length) {
        size_t len = 0;
        size_t inc= b->length > 2048 ? b->length : 2048;
        len = ((new_size + 2*inc)/1024) * 1024;
        grib_grow_buffer_to(c,b,len);
    }
}

void grib_buffer_set_ulength_bits(const grib_context *c, grib_buffer *b, size_t length_bits)
{
    size_t length=length_bits/8;
    if (length_bits%8) length++;
    grib_grow_buffer(c,b,length);
    b->ulength_bits = length_bits;
    b->ulength = length;
}

void grib_buffer_set_ulength(const grib_context *c, grib_buffer *b, size_t length)
{
    grib_grow_buffer(c,b,length);
    b->ulength = length;
    b->ulength_bits = length*8;
}

static void update_offsets(grib_accessor* a,long len)
{
    while(a)
    {
        grib_section* s = a->sub_section;
        a->offset += len;
        grib_context_log(a->context,GRIB_LOG_DEBUG, "::::: grib_buffer : accessor %s is moving by %d bytes to %ld",a->name ,len, a->offset);
        if(s) update_offsets(s->block->first,len);
        a = a->next;
    }
}

static void update_offsets_after(grib_accessor* a,long len)
{
    while(a)
    {
        update_offsets(a->next,len);
        a = a->parent->owner;
    }
}

#if 0
/* new GCC compiler v4.5.0 complains function is defined but not used*/
void grib_recompute_sections_lengths(grib_section* s)
{
    if(s)
    {
        long   plen = 0;
        size_t  len = 1;

        grib_accessor* a = s->block->first;

        while(a)
        {
            /* grib_recompute_sections_lengths(grib_get_sub_section(a)); */
            grib_recompute_sections_lengths(a->sub_section);
            a = a->next;
        }

        if(s->aclength)
        {
            int ret;
            if(s->owner)
                plen = grib_get_next_position_offset(s->block->last) - s->owner->offset;
            else
                plen = grib_get_next_position_offset(s->block->last);

            if((ret = grib_pack_long(s->aclength, &plen, &len)) != GRIB_SUCCESS)
                ;

#if 0
            if(s->h->context->debug)
                printf("SECTION updating length %ld .. %s\n",plen,s->owner->name);
#endif

        }
    }
}
#endif


#if 0
/* new GCC compiler v4.5.0 complains function is defined but not used*/
static void update_sections_lengths(grib_section* s)
{
    long   plen = 0;
    size_t  len = 1;

    if(!s) return;


    if(s->aclength)
    {
        int ret;
        if(s->owner)
            plen = grib_get_next_position_offset(s->block->last) - s->owner->offset;
        else
            plen = grib_get_next_position_offset(s->block->last);

        /* if(s->owner) */
        /* s->owner->length = plen; */

        /* if(s->aclength)  */
        if((ret = grib_pack_long(s->aclength, &plen, &len)) != GRIB_SUCCESS)
            ;

        if(s->h->context->debug)
        {
            printf("SECTION updating length %ld .. %s\n",plen,s->owner->name);
            printf("NEXT_POS = %ld, owner offset= %ld %s %s\n",
                    grib_get_next_position_offset(s->block->last),
                    s->owner ? s->owner->offset : 0L, s->owner->name,
                            s->block->last->name);
        }
    }

    if(s->owner)
        update_sections_lengths(s->owner->parent);

}
#endif


void grib_buffer_replace( grib_accessor *a, const unsigned char* data,
        size_t newsize,int update_lengths,int update_paddings)
{
    size_t offset   = a->offset;
    long   oldsize  = grib_get_next_position_offset(a)-offset;
    long   increase = (long)newsize - (long)oldsize;

    grib_buffer *buffer     = grib_handle_of_accessor(a)->buffer;
    size_t message_length   = buffer->ulength;

    grib_context_log(a->context,GRIB_LOG_DEBUG,
            "grib_buffer_replace %s offset=%ld oldsize=%ld newsize=%ld message_length=%ld update_paddings=%d",
            a->name,(long)offset,oldsize,(long)newsize,(long)message_length,update_paddings);

    grib_buffer_set_ulength(a->context,
            buffer,
            buffer->ulength+increase);

    /* move the end */
    if(increase)
        memmove(
                buffer->data + offset + newsize,
                buffer->data + offset + oldsize,
                message_length - offset - oldsize);

    /* copy new data */
    DebugAssert( buffer->data + offset );
    DebugAssert( data || (newsize==0) );/* if data==NULL then newsize must be 0 */
    if (data) {
        /* Note: memcpy behaviour is undefined if either dest or src is NULL */
        memcpy(buffer->data + offset, data, newsize);
    }

    if(increase)
    {
        update_offsets_after(a,increase);
        if(update_lengths)
        {
            grib_update_size(a,newsize);
            grib_section_adjust_sizes(grib_handle_of_accessor(a)->root,1,0);
            if(update_paddings)
                grib_update_paddings(grib_handle_of_accessor(a)->root);
        }
    }
}

void grib_update_sections_lengths(grib_handle* h)
{
    grib_section_adjust_sizes(h->root,2,0);
    grib_update_paddings(h->root);
}
