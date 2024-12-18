/*
 * (C) Copyright 2005- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "grib_api_internal.h"
#include "grib_dumper.h"
#include "grib_dumper_factory.h"

struct table_entry {
  const char *type;
  eccodes::Dumper** dumper;
};

static struct table_entry table[] = {
  {"bufr_decode_C",&grib_dumper_bufr_decode_c,},
  {"bufr_decode_filter",&grib_dumper_bufr_decode_filter,},
  {"bufr_decode_fortran",&grib_dumper_bufr_decode_fortran,},
  {"bufr_decode_python",&grib_dumper_bufr_decode_python,},
  {"bufr_encode_C",&grib_dumper_bufr_encode_c,},
  {"bufr_encode_filter",&grib_dumper_bufr_encode_filter,},
  {"bufr_encode_fortran",&grib_dumper_bufr_encode_fortran,},
  {"bufr_encode_python",&grib_dumper_bufr_encode_python,},
  {"bufr_simple",&grib_dumper_bufr_simple,},
  {"debug",&grib_dumper_debug,},
  {"default",&grib_dumper_default,},
  {"grib_encode_c",&grib_dumper_grib_encode_c,},
  {"json",&grib_dumper_json,},
  {"serialize",&grib_dumper_serialize,},
  {"wmo",&grib_dumper_wmo,},
};

eccodes::Dumper *grib_dumper_factory(const char *op, const grib_handle *h, FILE *out, unsigned long option_flags, void *arg)
{
  size_t i = 0;
  const size_t num_table_entries = sizeof(table) / sizeof(table[0]);
  for (i = 0; i < num_table_entries; i++)
    if (strcmp(op, table[i].type) == 0) {
      eccodes::Dumper *d = *(table[i].dumper);
      d->depth_ = 0;
      d->context_ = h->context;
      d->option_flags_ = option_flags;
      d->arg_ = arg;
      d->out_ = out;
      d->init();
      grib_context_log(
          h->context, GRIB_LOG_DEBUG, "Creating dumper of type : %s ", op);
      return d;
    }
  grib_context_log(
      h->context, GRIB_LOG_ERROR, "Unknown type : '%s' for dumper", op);
  return NULL;
}

void grib_dump_content(const grib_handle *h, FILE *f, const char *mode, unsigned long flags, void *data) {
  eccodes::Dumper *dumper;
  dumper = grib_dumper_factory(mode ? mode : "serialize", h, f, flags, data);
  if (!dumper) {
    fprintf(stderr, "Here are some possible values for the dumper mode:\n");
    const size_t num_table_entries = sizeof(table) / sizeof(table[0]);
    for (size_t i = 0; i < num_table_entries; i++) {
      const char *t = table[i].type;
      if (strstr(t, "bufr") == NULL && strstr(t, "grib") == NULL) {
        fprintf(stderr, "\t%s\n", t);
      }
    }
    return;
  }
  dumper->header(h);
  grib_dump_accessors_block(dumper, h->root->block);
  dumper->footer(h);
  dumper->destroy();
}


void grib_dump_accessors_block(eccodes::Dumper *dumper, grib_block_of_accessors *block) {
  grib_accessor *a = block->first;
  while (a) {
    a->dump(dumper);
    a = a->next_;
  }
}

void grib_dump_accessors_list(eccodes::Dumper *dumper, grib_accessors_list *al) {
  grib_accessors_list *cur = al;
  while (cur) {
    cur->accessor->dump(dumper);
    cur = cur->next_;
  }
}

int grib_print(grib_handle *h, const char *name, eccodes::Dumper *d) {
  grib_accessor *act = grib_find_accessor(h, name);
  if (act) {
    act->dump(d);
    return GRIB_SUCCESS;
  }
  return GRIB_NOT_FOUND;
}


void grib_dump_keys(grib_handle *h, FILE *f, const char *mode, unsigned long flags, void *data, const char **keys, size_t num_keys)
{
  size_t i;
  grib_accessor *acc = NULL;
  eccodes::Dumper *dumper =
      grib_dumper_factory(mode ? mode : "serialize", h, f, flags, data);
  if (!dumper)
    return;
  for (i = 0; i < num_keys; ++i) {
    acc = grib_find_accessor(h, keys[i]);
    if (acc)
      acc->dump(dumper);
  }
  dumper->destroy();
}

/* Note: if the dumper passed in is non-NULL, it will be freed up */
eccodes::Dumper *grib_dump_content_with_dumper(grib_handle *h, eccodes::Dumper *dumper, FILE *f, const char *mode, unsigned long flags, void *data)
{
  long count = 1;
  if (dumper != NULL) {
    count = dumper->count();
    count++;
    dumper->destroy();
  }
  dumper = grib_dumper_factory(mode ? mode : "serialize", h, f, flags, data);
  if (!dumper)
    return NULL;
  dumper->count(count);

  dumper->header(h);
  grib_dump_accessors_block(dumper, h->root->block);
  dumper->footer(h);
  return dumper;
}

void codes_dump_bufr_flat(grib_accessors_list *al, grib_handle *h, FILE *f, const char *mode, unsigned long flags, void *data)
{
  eccodes::Dumper *dumper = NULL;
  Assert(h->product_kind == PRODUCT_BUFR);
  dumper = grib_dumper_factory(mode ? mode : "serialize", h, f, flags, data);
  if (!dumper)
    return;
  dumper->header(h);
  grib_dump_accessors_list(dumper, al);
  dumper->footer(h);
  dumper->destroy();
}
