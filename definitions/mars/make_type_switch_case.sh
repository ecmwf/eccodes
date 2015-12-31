# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#
echo "			switch (val) {"
cat type.table | awk '{print "\t\t\t\tcase "$1":\n\t\t\t\t\t/* "$3" "$4" "$5" "$6" "$7" "$8" "$9" ("$2") */\n\t\t\t\t\tbreak;"}'

echo "				default :"
echo "					grib_context_log(a->parent->h->context,GRIB_LOG_ERROR,"unknown type %d",(int)val);"
echo "					return GRIB_ENCODING_ERROR;"
echo "			}"
