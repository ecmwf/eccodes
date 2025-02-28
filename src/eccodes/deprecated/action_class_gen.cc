// #define F(x)  if (flg & x) { fprintf(f, "%s=>1,", #x); flg &= !x; }
// static int count = 0;
// static void xref(grib_action* act, FILE* f, const char* path)
// {
//     grib_action_gen* a = (grib_action_gen*)act;
//     unsigned long flg  = act->flags;
//     int position       = a->len > 0 ? count++ : -1;

//     fprintf(f, "bless({path=>'%s',size => %ld, name=> '%s', position=> %d, ", path, (long)a->len, act->name, position);

//     fprintf(f, " params=> [");
//     grib_arguments_print(act->context, a->params, NULL);
//     fprintf(f, "], flags=> {");

//     F(GRIB_ACCESSOR_FLAG_READ_ONLY);
//     F(GRIB_ACCESSOR_FLAG_DUMP);
//     F(GRIB_ACCESSOR_FLAG_EDITION_SPECIFIC);
//     F(GRIB_ACCESSOR_FLAG_CAN_BE_MISSING);
//     F(GRIB_ACCESSOR_FLAG_HIDDEN);
//     F(GRIB_ACCESSOR_FLAG_CONSTRAINT);
//     F(GRIB_ACCESSOR_FLAG_NO_COPY);
//     F(GRIB_ACCESSOR_FLAG_COPY_OK);
//     F(GRIB_ACCESSOR_FLAG_FUNCTION);
//     F(GRIB_ACCESSOR_FLAG_DATA);
//     F(GRIB_ACCESSOR_FLAG_NO_FAIL);
//     F(GRIB_ACCESSOR_FLAG_TRANSIENT);
//     F(GRIB_ACCESSOR_FLAG_STRING_TYPE);
//     F(GRIB_ACCESSOR_FLAG_LONG_TYPE);
//     F(GRIB_ACCESSOR_FLAG_DOUBLE_TYPE);

//     /* make sure all flags are processed */
//     if (flg) {
//         printf("FLG = %ld\n", (long)flg);
//     }
//     Assert(flg == 0);

//     fprintf(f, "}, defaults=> [");
//     grib_arguments_print(act->context, act->default_value, NULL);

//     fprintf(f, "]}, 'xref::%s'),\n", act->op);
// }

static void xref(grib_action* act, FILE* f, const char* path)
{
    Assert(!"xref is disabled");
}
