# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
#
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#
BEGIN {
	OFMT = "%.4f"
	print "<?xml version=\"1.0\" ?>"

	if (JMETER=="true") {
		print "<testResults version=\"1.2\">"
	} else {
		print "<testsuite errors=\"0\" failures=\"0\" hostname=\"localhost\" name=\"test\" tests=\"0\" time=\"0\" timestamp=\"0\">"
	}
	test_name = "none"
	dir = ""
	times["real"] = ""
	times["user"] = ""
	times["sys"] = ""
}

/TEST\:/	{test_name = $2}
/PASS\:/	{ if (JMETER=="true")
			for (key in times) jmeter_sample(dir,test_name,key,times[key],TIMESTAMP,"true") 
		  else
			junit_sample(dir,test_name,times["real"], "true")
		}

/FAIL\:/	{ if (JMETER=="true")
			for (key in times) jmeter_sample(dir,test_name,key,times[key],TIMESTAMP,"false") 
		  else
			junit_sample(dir,test_name,times["real"], "false")
		}

/^real|^user|^sys/	{times[$1]=get_time_in_msecs($2)}

/DIRECTORY:/ {dir = get_dir($2)}

END {
	if (JMETER=="true") {
		print "</testResults>"
	} else {
		print "</testsuite>"
	}
}

function jmeter_sample(dir, test_name,test_type,time,timestamp,success) {
	print "<sample lb=\"" dir test_name "_" test_type "\" t=\"" time "\" ts=\"" timestamp "\" s=\"" success "\" />"
}

function junit_sample(dir,test_name,time,success) {
	if (success=="true") {
		print "<testcase classname=\"" dir "\" name=\"" test_name "\" time=\"" time / 1000.0 "\" />"
	} else {
		print "<testcase classname=\""  dir "\" name=\"" test_name "\" time=\"" time / 1000.0 "\">"
       		print "<failure type=\"error\">Non zero-return</failure>"
		print "</testcase>"
	}
}

function get_time_in_msecs(timestring) {
	if ( timestring ~ /s$/ ) {
	# It must be the 0m0.003s format
		len = split(timestring,bits,"[hms]")
	} else {
	# It must be the 2:00.0 format
		len = split(timestring,bits,":")
	}
	val = 0
	count = 0
	for (i = len-1;i>=0;i--) {
		if (! bits[i] ~ /\s*/) {
			val = val + (bits[i] * 1000  * (60 ^ count))
			count ++
		}
	}
	return val;
}

function get_dir(dir) {
	len = split(dir,bits,"/")
	return bits[len]
}

