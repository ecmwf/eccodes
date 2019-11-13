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
 * Common tigge functions
 */

#include "tigge_tools.h"
#include "eccodes_windef.h"

#include <sys/types.h>

#ifndef ECCODES_ON_WINDOWS
  #include <dirent.h>
#else
  #include <direct.h>
  #include <io.h>
#endif

#include <stdio.h>

extern void validate(const char* path);

#ifndef ECCODES_ON_WINDOWS
void scan(const char* name)
{
    DIR *dir;
    if((dir = opendir(name)) != NULL)
    {
        struct dirent* e;
        char tmp[1024];
        while( (e = readdir(dir)) != NULL)
        {
            if(e->d_name[0] == '.') continue;
            sprintf(tmp,"%s/%s",name,e->d_name);
            scan(tmp);
        }

        closedir(dir);
    }
    else {
        validate(name);
    }
}
#else
void scan(const char* name)
{
    struct _finddata_t fileinfo;
    intptr_t handle;
    char tmp[1024];
    sprintf(tmp, "%s/*", name);
    if((handle = _findfirst(tmp, &fileinfo)) != -1)
    {
        do {
            if(fileinfo.name[0] != '.') {
                sprintf(tmp, "%s/%s", name, fileinfo.name);
                scan(tmp);
            }
        } while(!_findnext(handle, &fileinfo));

        _findclose(handle);
    }
    else
        validate(name);
}
#endif
