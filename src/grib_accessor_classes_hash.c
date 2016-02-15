/* C code produced by gperf version 3.0.2 */
/* Command-line: gperf -W classes -t -G -H grib_accessor_classes_get_id -N grib_accessor_classes_hash -m 1 -j 1 accessor_class_list.gperf  */
/* Computed positions: -k'2-3,7,11' */

/*
 * Copyright 2005-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 1 "accessor_class_list.gperf"

#include "grib_api_internal.h"
#include "grib_accessor_class.h"

#line 6 "accessor_class_list.gperf"
struct accessor_class_hash { char *name; grib_accessor_class **cclass;};

#define TOTAL_KEYWORDS 149
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 40
#define MIN_HASH_VALUE 7
#define MAX_HASH_VALUE 349
/* maximum key range = 343, duplicates = 0 */

#ifdef __GNUC__

#else
#ifdef __cplusplus

#endif
#endif
static unsigned int
grib_accessor_classes_get_id (const char *str, unsigned int len)
{
  static unsigned short asso_values[] =
    {
      350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
      350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
      350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
      350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
      350, 350, 350, 350, 350, 350, 350, 350,   0,  35,
       10, 350,   3,   7, 350, 350, 350, 350, 350, 350,
      350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
      350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
      350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
      350, 350, 350, 350, 350,  46, 350,   0,  91,  74,
       37,   0,  64, 147, 110,  29, 350,   2,  79,  74,
       21,   1,  34, 350,  10,  42,   2,  94, 102, 350,
        9,   0,   0, 350, 350, 350, 350, 350, 350, 350,
      350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
      350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
      350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
      350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
      350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
      350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
      350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
      350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
      350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
      350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
      350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
      350, 350, 350, 350, 350, 350, 350, 350, 350, 350,
      350, 350, 350, 350, 350, 350
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[10]];
      /*FALLTHROUGH*/
      case 10:
      case 9:
      case 8:
      case 7:
        hval += asso_values[(unsigned char)str[6]];
      /*FALLTHROUGH*/
      case 6:
      case 5:
      case 4:
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      /*FALLTHROUGH*/
      case 2:
        hval += asso_values[(unsigned char)str[1]];
        break;
    }
  return hval;
}

static struct accessor_class_hash classes[] =
  {
    {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 20 "accessor_class_list.gperf"
    {"bytes", &grib_accessor_class_bytes},
#line 110 "accessor_class_list.gperf"
    {"lookup", &grib_accessor_class_lookup},
#line 115 "accessor_class_list.gperf"
    {"nearest", &grib_accessor_class_nearest},
#line 97 "accessor_class_list.gperf"
    {"ieeefloat", &grib_accessor_class_ieeefloat},
#line 99 "accessor_class_list.gperf"
    {"iterator", &grib_accessor_class_iterator},
    {""},
#line 18 "accessor_class_list.gperf"
    {"box", &grib_accessor_class_box},
#line 144 "accessor_class_list.gperf"
    {"statistics", &grib_accessor_class_statistics},
    {""}, {""}, {""},
#line 50 "accessor_class_list.gperf"
    {"data_raw_packing", &grib_accessor_class_data_raw_packing},
#line 150 "accessor_class_list.gperf"
    {"transient", &grib_accessor_class_transient},
#line 111 "accessor_class_list.gperf"
    {"mars_param", &grib_accessor_class_mars_param},
#line 112 "accessor_class_list.gperf"
    {"mars_step", &grib_accessor_class_mars_step},
#line 36 "accessor_class_list.gperf"
    {"data_constant_field", &grib_accessor_class_data_constant_field},
#line 35 "accessor_class_list.gperf"
    {"data_complex_packing", &grib_accessor_class_data_complex_packing},
#line 93 "accessor_class_list.gperf"
    {"gen", &grib_accessor_class_gen},
#line 11 "accessor_class_list.gperf"
    {"array", &grib_accessor_class_array},
#line 107 "accessor_class_list.gperf"
    {"long", &grib_accessor_class_long},
    {""}, {""}, {""}, {""},
#line 26 "accessor_class_list.gperf"
    {"concept", &grib_accessor_class_concept},
#line 31 "accessor_class_list.gperf"
    {"data_2order_packing", &grib_accessor_class_data_2order_packing},
#line 141 "accessor_class_list.gperf"
    {"size", &grib_accessor_class_size},
#line 14 "accessor_class_list.gperf"
    {"bit", &grib_accessor_class_bit},
#line 16 "accessor_class_list.gperf"
    {"bits", &grib_accessor_class_bits},
#line 44 "accessor_class_list.gperf"
    {"data_g2secondary_bitmap", &grib_accessor_class_data_g2secondary_bitmap},
#line 15 "accessor_class_list.gperf"
    {"bitmap", &grib_accessor_class_bitmap},
#line 32 "accessor_class_list.gperf"
    {"data_2order_packing_count", &grib_accessor_class_data_2order_packing_count},
#line 49 "accessor_class_list.gperf"
    {"data_png_packing", &grib_accessor_class_data_png_packing},
#line 124 "accessor_class_list.gperf"
    {"pad", &grib_accessor_class_pad},
#line 68 "accessor_class_list.gperf"
    {"g1area", &grib_accessor_class_g1area},
#line 126 "accessor_class_list.gperf"
    {"padto", &grib_accessor_class_padto},
#line 108 "accessor_class_list.gperf"
    {"long_vector", &grib_accessor_class_long_vector},
#line 57 "accessor_class_list.gperf"
    {"dirty", &grib_accessor_class_dirty},
#line 17 "accessor_class_list.gperf"
    {"bits_per_value", &grib_accessor_class_bits_per_value},
#line 22 "accessor_class_list.gperf"
    {"codeflag", &grib_accessor_class_codeflag},
#line 113 "accessor_class_list.gperf"
    {"md5", &grib_accessor_class_md5},
#line 103 "accessor_class_list.gperf"
    {"latitudes", &grib_accessor_class_latitudes},
    {""},
#line 55 "accessor_class_list.gperf"
    {"data_simple_packing", &grib_accessor_class_data_simple_packing},
#line 27 "accessor_class_list.gperf"
    {"constant", &grib_accessor_class_constant},
#line 129 "accessor_class_list.gperf"
    {"position", &grib_accessor_class_position},
#line 84 "accessor_class_list.gperf"
    {"g2date", &grib_accessor_class_g2date},
#line 95 "accessor_class_list.gperf"
    {"gts_header", &grib_accessor_class_gts_header},
#line 62 "accessor_class_list.gperf"
    {"forward", &grib_accessor_class_forward},
    {""},
#line 48 "accessor_class_list.gperf"
    {"data_jpeg2000_packing", &grib_accessor_class_data_jpeg2000_packing},
#line 89 "accessor_class_list.gperf"
    {"g2step", &grib_accessor_class_g2step},
    {""},
#line 51 "accessor_class_list.gperf"
    {"data_secondary_bitmap", &grib_accessor_class_data_secondary_bitmap},
#line 39 "accessor_class_list.gperf"
    {"data_g1secondary_bitmap", &grib_accessor_class_data_g1secondary_bitmap},
#line 85 "accessor_class_list.gperf"
    {"g2end_step", &grib_accessor_class_g2end_step},
#line 142 "accessor_class_list.gperf"
    {"spectral_truncation", &grib_accessor_class_spectral_truncation},
#line 45 "accessor_class_list.gperf"
    {"data_g2shsimple_packing", &grib_accessor_class_data_g2shsimple_packing},
#line 146 "accessor_class_list.gperf"
    {"step_in_units", &grib_accessor_class_step_in_units},
    {""},
#line 46 "accessor_class_list.gperf"
    {"data_g2simple_packing", &grib_accessor_class_data_g2simple_packing},
#line 43 "accessor_class_list.gperf"
    {"data_g2complex_packing", &grib_accessor_class_data_g2complex_packing},
#line 145 "accessor_class_list.gperf"
    {"statistics_spectral", &grib_accessor_class_statistics_spectral},
#line 42 "accessor_class_list.gperf"
    {"data_g22order_packing", &grib_accessor_class_data_g22order_packing},
#line 151 "accessor_class_list.gperf"
    {"unsigned", &grib_accessor_class_unsigned},
#line 101 "accessor_class_list.gperf"
    {"ksec1expver", &grib_accessor_class_ksec1expver},
    {""},
#line 104 "accessor_class_list.gperf"
    {"latlon_increment", &grib_accessor_class_latlon_increment},
#line 78 "accessor_class_list.gperf"
    {"g1p1p2", &grib_accessor_class_g1p1p2},
    {""}, {""},
#line 70 "accessor_class_list.gperf"
    {"g1date", &grib_accessor_class_g1date},
#line 132 "accessor_class_list.gperf"
    {"scale", &grib_accessor_class_scale},
#line 156 "accessor_class_list.gperf"
    {"vector", &grib_accessor_class_vector},
    {""},
#line 72 "accessor_class_list.gperf"
    {"g1end_of_interval_monthly", &grib_accessor_class_g1end_of_interval_monthly},
    {""}, {""},
#line 154 "accessor_class_list.gperf"
    {"values", &grib_accessor_class_values},
#line 47 "accessor_class_list.gperf"
    {"data_g2simple_packing_with_preprocessing", &grib_accessor_class_data_g2simple_packing_with_preprocessing},
    {""},
#line 60 "accessor_class_list.gperf"
    {"element", &grib_accessor_class_element},
#line 40 "accessor_class_list.gperf"
    {"data_g1shsimple_packing", &grib_accessor_class_data_g1shsimple_packing},
#line 13 "accessor_class_list.gperf"
    {"assert", &grib_accessor_class_assert},
    {""},
#line 41 "accessor_class_list.gperf"
    {"data_g1simple_packing", &grib_accessor_class_data_g1simple_packing},
#line 38 "accessor_class_list.gperf"
    {"data_g1complex_packing", &grib_accessor_class_data_g1complex_packing},
#line 152 "accessor_class_list.gperf"
    {"validity_date", &grib_accessor_class_validity_date},
    {""},
#line 102 "accessor_class_list.gperf"
    {"label", &grib_accessor_class_label},
#line 155 "accessor_class_list.gperf"
    {"variable", &grib_accessor_class_variable},
#line 87 "accessor_class_list.gperf"
    {"g2latlon", &grib_accessor_class_g2latlon},
#line 33 "accessor_class_list.gperf"
    {"data_apply_bitmap", &grib_accessor_class_data_apply_bitmap},
#line 131 "accessor_class_list.gperf"
    {"round", &grib_accessor_class_round},
#line 59 "accessor_class_list.gperf"
    {"double", &grib_accessor_class_double},
#line 134 "accessor_class_list.gperf"
    {"section", &grib_accessor_class_section},
#line 92 "accessor_class_list.gperf"
    {"gds_not_present_bitmap", &grib_accessor_class_gds_not_present_bitmap},
#line 67 "accessor_class_list.gperf"
    {"g1_section4_length", &grib_accessor_class_g1_section4_length},
    {""},
#line 34 "accessor_class_list.gperf"
    {"data_apply_gdsnotpresent", &grib_accessor_class_data_apply_gdsnotpresent},
#line 148 "accessor_class_list.gperf"
    {"time", &grib_accessor_class_time},
#line 149 "accessor_class_list.gperf"
    {"times", &grib_accessor_class_times},
#line 82 "accessor_class_list.gperf"
    {"g2bitmap", &grib_accessor_class_g2bitmap},
    {""}, {""},
#line 61 "accessor_class_list.gperf"
    {"evaluate", &grib_accessor_class_evaluate},
    {""},
#line 157 "accessor_class_list.gperf"
    {"when", &grib_accessor_class_when},
#line 143 "accessor_class_list.gperf"
    {"sprintf", &grib_accessor_class_sprintf},
#line 105 "accessor_class_list.gperf"
    {"latlonvalues", &grib_accessor_class_latlonvalues},
    {""}, {""},
#line 73 "accessor_class_list.gperf"
    {"g1fcperiod", &grib_accessor_class_g1fcperiod},
    {""},
#line 12 "accessor_class_list.gperf"
    {"ascii", &grib_accessor_class_ascii},
    {""},
#line 153 "accessor_class_list.gperf"
    {"validity_time", &grib_accessor_class_validity_time},
#line 65 "accessor_class_list.gperf"
    {"g1_increment", &grib_accessor_class_g1_increment},
#line 98 "accessor_class_list.gperf"
    {"ifs_param", &grib_accessor_class_ifs_param},
#line 109 "accessor_class_list.gperf"
    {"longitudes", &grib_accessor_class_longitudes},
#line 83 "accessor_class_list.gperf"
    {"g2bitmap_present", &grib_accessor_class_g2bitmap_present},
    {""}, {""},
#line 135 "accessor_class_list.gperf"
    {"section_length", &grib_accessor_class_section_length},
    {""}, {""}, {""},
#line 69 "accessor_class_list.gperf"
    {"g1bitmap", &grib_accessor_class_g1bitmap},
    {""}, {""}, {""},
#line 23 "accessor_class_list.gperf"
    {"codetable", &grib_accessor_class_codetable},
#line 137 "accessor_class_list.gperf"
    {"section_pointer", &grib_accessor_class_section_pointer},
    {""},
#line 19 "accessor_class_list.gperf"
    {"budgdate", &grib_accessor_class_budgdate},
    {""}, {""}, {""},
#line 106 "accessor_class_list.gperf"
    {"library_version", &grib_accessor_class_library_version},
#line 24 "accessor_class_list.gperf"
    {"codetable_title", &grib_accessor_class_codetable_title},
#line 136 "accessor_class_list.gperf"
    {"section_padding", &grib_accessor_class_section_padding},
#line 127 "accessor_class_list.gperf"
    {"padtoeven", &grib_accessor_class_padtoeven},
    {""},
#line 79 "accessor_class_list.gperf"
    {"g1param", &grib_accessor_class_g1param},
    {""}, {""}, {""},
#line 77 "accessor_class_list.gperf"
    {"g1number_of_coded_values_sh_simple", &grib_accessor_class_g1number_of_coded_values_sh_simple},
#line 76 "accessor_class_list.gperf"
    {"g1number_of_coded_values_sh_complex", &grib_accessor_class_g1number_of_coded_values_sh_complex},
    {""}, {""},
#line 37 "accessor_class_list.gperf"
    {"data_dummy_field", &grib_accessor_class_data_dummy_field},
    {""}, {""},
#line 63 "accessor_class_list.gperf"
    {"from_scale_factor_scaled_value", &grib_accessor_class_from_scale_factor_scaled_value},
#line 53 "accessor_class_list.gperf"
    {"data_sh_unpacked", &grib_accessor_class_data_sh_unpacked},
#line 86 "accessor_class_list.gperf"
    {"g2grid", &grib_accessor_class_g2grid},
    {""}, {""}, {""},
#line 54 "accessor_class_list.gperf"
    {"data_shsimple_packing", &grib_accessor_class_data_shsimple_packing},
#line 64 "accessor_class_list.gperf"
    {"g1_half_byte_codeflag", &grib_accessor_class_g1_half_byte_codeflag},
#line 28 "accessor_class_list.gperf"
    {"count_file", &grib_accessor_class_count_file},
#line 56 "accessor_class_list.gperf"
    {"decimal_precision", &grib_accessor_class_decimal_precision},
#line 140 "accessor_class_list.gperf"
    {"simple_packing_error", &grib_accessor_class_simple_packing_error},
    {""},
#line 96 "accessor_class_list.gperf"
    {"ibmfloat", &grib_accessor_class_ibmfloat},
    {""},
#line 88 "accessor_class_list.gperf"
    {"g2level", &grib_accessor_class_g2level},
    {""}, {""},
#line 128 "accessor_class_list.gperf"
    {"padtomultiple", &grib_accessor_class_padtomultiple},
#line 138 "accessor_class_list.gperf"
    {"select_step_template", &grib_accessor_class_select_step_template},
#line 147 "accessor_class_list.gperf"
    {"suppressed", &grib_accessor_class_suppressed},
#line 91 "accessor_class_list.gperf"
    {"gds_is_present", &grib_accessor_class_gds_is_present},
#line 139 "accessor_class_list.gperf"
    {"signed", &grib_accessor_class_signed},
#line 94 "accessor_class_list.gperf"
    {"global_gaussian", &grib_accessor_class_global_gaussian},
    {""},
#line 122 "accessor_class_list.gperf"
    {"offset_file", &grib_accessor_class_offset_file},
#line 66 "accessor_class_list.gperf"
    {"g1_message_length", &grib_accessor_class_g1_message_length},
#line 30 "accessor_class_list.gperf"
    {"count_total", &grib_accessor_class_count_total},
#line 133 "accessor_class_list.gperf"
    {"scale_values", &grib_accessor_class_scale_values},
    {""}, {""},
#line 125 "accessor_class_list.gperf"
    {"padding", &grib_accessor_class_padding},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 52 "accessor_class_list.gperf"
    {"data_sh_packed", &grib_accessor_class_data_sh_packed},
    {""},
#line 21 "accessor_class_list.gperf"
    {"change_scanning_direction", &grib_accessor_class_change_scanning_direction},
    {""}, {""},
#line 71 "accessor_class_list.gperf"
    {"g1day_of_the_year_date", &grib_accessor_class_g1day_of_the_year_date},
    {""}, {""},
#line 130 "accessor_class_list.gperf"
    {"reference_value_error", &grib_accessor_class_reference_value_error},
    {""}, {""},
#line 29 "accessor_class_list.gperf"
    {"count_missing", &grib_accessor_class_count_missing},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 81 "accessor_class_list.gperf"
    {"g1verificationdate", &grib_accessor_class_g1verificationdate},
#line 10 "accessor_class_list.gperf"
    {"abstract_vector", &grib_accessor_class_abstract_vector},
    {""}, {""},
#line 114 "accessor_class_list.gperf"
    {"missing", &grib_accessor_class_missing},
#line 121 "accessor_class_list.gperf"
    {"octect_number", &grib_accessor_class_octect_number},
    {""},
#line 9 "accessor_class_list.gperf"
    {"abstract_long_vector", &grib_accessor_class_abstract_long_vector},
#line 100 "accessor_class_list.gperf"
    {"julian_day", &grib_accessor_class_julian_day},
    {""},
#line 58 "accessor_class_list.gperf"
    {"divdouble", &grib_accessor_class_divdouble},
#line 75 "accessor_class_list.gperf"
    {"g1monthlydate", &grib_accessor_class_g1monthlydate},
    {""}, {""}, {""}, {""}, {""},
#line 25 "accessor_class_list.gperf"
    {"codetable_units", &grib_accessor_class_codetable_units},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 90 "accessor_class_list.gperf"
    {"g2step_range", &grib_accessor_class_g2step_range},
    {""}, {""}, {""}, {""},
#line 74 "accessor_class_list.gperf"
    {"g1forecastmonth", &grib_accessor_class_g1forecastmonth},
    {""},
#line 117 "accessor_class_list.gperf"
    {"number_of_points", &grib_accessor_class_number_of_points},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 118 "accessor_class_list.gperf"
    {"number_of_points_gaussian", &grib_accessor_class_number_of_points_gaussian},
    {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 123 "accessor_class_list.gperf"
    {"offset_values", &grib_accessor_class_offset_values},
#line 80 "accessor_class_list.gperf"
    {"g1step_range", &grib_accessor_class_g1step_range},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 116 "accessor_class_list.gperf"
    {"number_of_coded_values", &grib_accessor_class_number_of_coded_values},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""}, {""}, {""},
#line 119 "accessor_class_list.gperf"
    {"number_of_values", &grib_accessor_class_number_of_values},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 120 "accessor_class_list.gperf"
    {"number_of_values_data_raw_packing", &grib_accessor_class_number_of_values_data_raw_packing}
  };

#ifdef __GNUC__

#endif
struct accessor_class_hash *
grib_accessor_classes_hash (const char * str, unsigned int len)
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = grib_accessor_classes_get_id (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = classes[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &classes[key];
        }
    }
  return 0;
}
