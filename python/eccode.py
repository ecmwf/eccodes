"""
@package eccode
@brief This package is a low level Python interface to GRIB API. It offers almost one to one bindings to the C API functions.

The Python interface to GRIB API uses by default the <a href="http://numpy.scipy.org/"><b>NumPy</b></a> package
as the container of choice for the possible arrays of values that can be encoded/decoded in and from a grib message.
Numpy is a package used for scientific computing in Python and an efficient container for generic data.

Alternatively, the Python interface can be built without Numpy support, in which case, the data container used
will be Python's native 'array' object.

The Python interface and its support for NumPy can be enabled/disabled from the configure by using the following configure flags:\n

@code
--enable-python
--disable-numpy

Ex. ./configure --enable-python --disable-numpy
@endcode

When the '--enable-python' flag is used, then the system Python will be used to build the interface.

NumPy support can be disabled by using the '--disable-numpy' flag.

@em Requirements:

    - Python 2.5 or higher
    - NumPy (optional)

"""
import eccode_swig as _internal
import types
import sys
import os
from array import array
from functools import wraps
#import inspect

KEYTYPES = {
    1:int,
    2:float,
    3:str,
}

# GRIB-51 Skip function arguments type checking if the
# environment variable is defined
no_type_checks = os.environ.get('GRIB_API_PYTHON_NO_TYPE_CHECKS') is not None

# function arguments type checking decorator
# got inspired from http://code.activestate.com/recipes/454322-type-checking-decorator/
# modified to support multiple allowed types and all types in the same decorator call
def require(**_params_):
    def check_types(_func_, _params_ = _params_):
        if no_type_checks:
            return _func_
        @wraps(_func_)
        def modified(*args, **kw):
            arg_names = _func_.func_code.co_varnames
            #argnames, varargs, kwargs, defaults = inspect.getargspec(_func_)
            kw.update(zip(arg_names, args))
            for name, allowed_types in _params_.iteritems():
                param = kw[name]
                if type(allowed_types) == types.TypeType:
                    allowed_types = (allowed_types,)
                assert type(param) in allowed_types,  \
                    "Parameter '%s' should be type %s" % (name, " or ".join([t.__name__ for t in allowed_types]))
            return _func_(**kw)
        return modified
    return check_types

class InternalError(Exception):
    """
    @brief Wrap errors coming from the C API in a Python exception object.
    """
    def __init__(self, value):
        # Call the base class constructor with the parameters it needs
        Exception.__init__(self, value)
        if type(value) is int:
            err,self.msg = _internal.grib_c_get_error_string(value,1024)
            assert(err == 0)
        else:
            self.msg = value
    def __str__(self):
        return self.msg

class Bunch(dict):
    """
    The collector of a bunch of named stuff :).
    """
    def __init__(self, **kw):
        dict.__init__(self, kw)
        self.__dict__.update(kw)

    def __setitem__(self,key,value):
        dict.__setitem__(self,key,value)
        self.__dict__[key] = value

    def __setattr__(self,key,value):
        dict.__setitem__(self,key,value)
        self.__dict__[key] = value

    def __delitem__(self,key):
        dict.__delitem__(self,key)
        del self.__dict__[key]

    def __delattr__(self,key):
        dict.__delitem__(self,key)
        del self.__dict__[key]

    def __str__(self):
        state = ["%s=%r" % (attribute, value)
                 for (attribute, value)
                 in self.__dict__.items()]
        return '\n'.join(state)

def with_numpy():
    """
    @brief Is numpy enabled?
    @return 0->disabled, 1->enabled
    """
    numpy = 0
    try:
        _internal.with_numpy()
        numpy = 1
    except AttributeError:
        pass

    return numpy

@require(errid=int)
def CHECK(errid):
    """
    Utility function checking the GRIB API error code and raising 
    an error if that was set.

    @param errid  the C interface error id to check
    @exception InternalError 
    """
    if errid:
        raise InternalError(errid)

@require(fileobj=file)
def gts_new_from_file(fileobj,headers_only = False):
    """
    @brief Load in memory a GTS message from a file.
    
    The message can be accessed through its id and it will be available\n
    until @ref release is called.\n

    The message can be loaded headers only by using the headers_only argument.
    Default is to have the headers only option set to off (False). If set to on (True),
    data values will be skipped. This will result in a significant performance gain
    if one is only interested in browsing through messages to retrieve metadata.
    Any attempt to retrieve data values keys when in the headers only mode will
    result in a key not found error.
    
    \b Examples: \ref get.py "get.py"
    
    @param fileobj        python file object
    @param headers_only   whether or not to load the message with the headers only                            
    @return id of the GTS loaded in memory
    @exception InternalError 
    """
    err,gribid = _internal.grib_c_new_gts_from_file(fileobj, headers_only, 0)
    if err:
        if err == _internal.GRIB_END_OF_FILE:
            return None
        else:
            CHECK(err)
    else:
        return gribid

@require(fileobj=file)
def bufr_new_from_file(fileobj,headers_only = False):
    """
    @brief Load in memory a BUFR message from a file.
    
    The message can be accessed through its id and it will be available\n
    until @ref release is called.\n

    The message can be loaded headers only by using the headers_only argument.
    Default is to have the headers only option set to off (False). If set to on (True),
    data values will be skipped. This will result in a significant performance gain
    if one is only interested in browsing through messages to retrieve metadata.
    Any attempt to retrieve data values keys when in the headers only mode will
    result in a key not found error.
    
    \b Examples: \ref get.py "get.py"
    
    @param fileobj        python file object
    @param headers_only   whether or not to load the message with the headers only                            
    @return id of the BUFR loaded in memory
    @exception InternalError 
    """
    err,gribid = _internal.grib_c_new_bufr_from_file(fileobj, headers_only, 0)
    if err:
        if err == _internal.GRIB_END_OF_FILE:
            return None
        else:
            CHECK(err)
    else:
        return gribid

@require(fileobj=file)
def grib_new_from_file(fileobj,headers_only = False):
    """
    @brief Load in memory a grib message from a file.
    
    The message can be accessed through its gribid and it will be available\n
    until @ref release is called.\n

    The message can be loaded headers only by using the headers_only argument.
    Default is to have the headers only option set to off (False). If set to on (True),
    data values will be skipped. This will result in a significant performance gain
    if one is only interested in browsing through messages to retrieve metadata.
    Any attempt to retrieve data values keys when in the headers only mode will
    result in a key not found error.
    
    \b Examples: \ref get.py "get.py"
    
    @param fileobj        python file object
    @param headers_only   whether or not to load the message with the headers only                            
    @return id of the grib loaded in memory
    @exception InternalError 
    """
    err, gribid = _internal.grib_c_new_from_file(fileobj, 0, headers_only)
    if err:
        if err == _internal.GRIB_END_OF_FILE:
            return None
        else:
            CHECK(err)
    else:
        return gribid

@require(fileobj=file)
def grib_count_in_file(fileobj):
    """
    @brief Count the messages in a file.
    
    \b Examples: \ref count_messages.py "count_messages.py"
    
    @param fileobj  python file object
    @return number of messages in the file
    @exception InternalError 
    """
    err, num = _internal.grib_c_count_in_file(fileobj)
    CHECK(err)
    return num

def grib_multi_support_on():
    """
    @brief Turn on the support for multiple fields in a single message.
    
    @exception InternalError 
    """
    _internal.grib_c_multi_support_on()

def grib_multi_support_off():
    """
    @brief Turn off the support for multiple fields in a single message.
    
    @exception InternalError 
    """
    _internal.grib_c_multi_support_off()

@require(gribid=int)
def release(gribid):
    """
    @brief Free the memory for the message referred as gribid.
    
    \b Examples: \ref get.py "get.py"
    
    @param gribid      id of the grib loaded in memory
    @exception InternalError 
    """
    CHECK(_internal.grib_c_release(gribid))

@require(gribid=int,key=str)
def get_string(gribid,key):
    """
    @brief Get the string value of a key from a grib message.
    
    @param gribid      id of the grib loaded in memory
    @param key         key name
    @return string value of key
    @exception InternalError 
    """
    length = get_string_length(gribid,key)
    err,value = _internal.grib_c_get_string(gribid,key,length)
    CHECK(err)

    stpos = value.find('\0')
    if stpos != -1:
        value = value[0:stpos]
        
    return value


@require(gribid=int,key=str,value=str)
def set_string(gribid,key,value):
    """
    @brief Set the value for a string key in a grib message.
    
    @param gribid      id of the grib loaded in memory
    @param key         key name
    @param value       string value
    @exception InternalError 
    """
    CHECK(_internal.grib_c_set_string(gribid,key,value,len(value)))

def grib_gribex_mode_on():
    """
    @brief Turn on the compatibility mode with gribex.
    
    @exception InternalError 
    """
    _internal.grib_c_gribex_mode_on()

def grib_gribex_mode_off():
    """
    @brief Turn off the compatibility mode with gribex.
    
    @exception InternalError 
    """
    _internal.grib_c_gribex_mode_off()

@require(gribid=int, fileobj=file)
def write(gribid, fileobj):
    """
    @brief Write a message to a file.

    \b Examples: \ref set.py "set.py"
    
    @param gribid      id of the grib loaded in memory
    @param fileobj     python file object
    @exception InternalError 
    """
    CHECK(_internal.grib_c_write(gribid, fileobj))

@require(multigribid=int,fileobj=file)
def grib_multi_write(multigribid, fileobj):
    """
    @brief Write a multi field message to a file.

    \b Examples: \ref multi_write.py "multi_write.py"
    
    @param multigribid      id of the multi field grib loaded in memory
    @param fileobj          python file object
    @exception InternalError 
    """
    CHECK(_internal.grib_c_multi_write(multigribid, fileobj))

@require(ingribid=int,startsection=int,multigribid=int)
def grib_multi_append(ingribid, startsection, multigribid):
    """
    @brief Append a single field grib message to a multi field grib message.

    Only the sections with section number greather or equal "startsection"
    are copied from the input single message to the multi field output grib.

    \b Examples: \ref multi_write.py "multi_write.py"

    @param ingribid      id of the input single grib
    @param startsection  starting from startsection (included) all the sections are copied 
                         from the input single grib to the output multi grib
    @param multigribid   id of the output multi filed grib
    @exception InternalError 
    """
    CHECK(_internal.grib_c_multi_append(ingribid, startsection, multigribid))

@require(gribid=int,key=str)
def get_size(gribid,key):
    """
    @brief Get the size of an array key.

    \b Examples: \ref get.py "get.py",\ref count_messages.py "count_messages.py"
    
    @param gribid      id of the grib loaded in memory
    @param key         name of the key
    @exception InternalError 
    """
    err,result = _internal.grib_c_get_size_long(gribid,key)
    CHECK(err)
    return result

@require(gribid=int,key=str)
def get_string_length(gribid,key):
    """
    @brief Get the length of the string version of a key.

    @param gribid      id of the grib loaded in memory
    @param key         name of the key
    @exception InternalError 
    """
    err,result = _internal.grib_c_get_string_length(gribid,key)
    CHECK(err)
    return result

@require(iterid=int)
def grib_skip_computed(iterid):
    """
    @brief Skip the computed keys in a keys iterator.
    
    The computed keys are not coded in the message, they are computed
    from other keys.
    
    @see grib_keys_iterator_new,grib_keys_iterator_next,grib_keys_iterator_delete
    
    @param iterid      keys iterator id
    @exception InternalError 
    """
    CHECK(_internal.grib_c_skip_computed(iterid))

@require(iterid=int)
def grib_skip_coded(iterid):
    """
    @brief Skip the coded keys in a keys iterator.
    
    The coded keys are actually coded in the message.
    
    @see keys_iterator_new,keys_iterator_next,keys_iterator_delete
    
    @param iterid      keys iterator id
    @exception InternalError 
    """
    CHECK(_internal.grib_c_skip_coded(iterid))

@require(iterid=int)
def grib_skip_edition_specific(iterid):
    """
    @brief Skip the edition specific keys in a keys iterator.
    
    @see keys_iterator_new,keys_iterator_next,keys_iterator_delete
    
    @param iterid      keys iterator id
    @exception InternalError 
    """
    CHECK(_internal.grib_c_skip_edition_specific(iterid))

@require(iterid=int)
def grib_skip_duplicates(iterid):
    """
    @brief Skip the duplicate keys in a keys iterator.
    
    @see keys_iterator_new,keys_iterator_next,keys_iterator_delete
    
    @param iterid      keys iterator id
    @exception InternalError 
    """
    CHECK(_internal.grib_c_skip_duplicates(iterid))

@require(iterid=int)
def grib_skip_read_only(iterid):
    """
    @brief Skip the read_only keys in a keys iterator.
    
    Read only keys cannot be set.
    
    @see keys_iterator_new,keys_iterator_next,keys_iterator_delete
    
    @param iterid      keys iterator id
    @exception InternalError 
    """
    CHECK(_internal.grib_c_skip_read_only(iterid))

@require(iterid=int)
def grib_skip_function(iterid):
    """
    @brief Skip the function keys in a keys iterator.

    @see grib_keys_iterator_new,keys_iterator_next,keys_iterator_delete

    @param iterid      keys iterator id
    @exception InternalError 
    """
    CHECK(_internal.grib_c_skip_function(iterid))

@require(gribid=int,mode=int)
def iterator_new(gribid,mode):
    """
    @brief Create a new geoiterator for the given message, using its geometry and values.

    The geoiterator can be used to go through all the geopoints in a message and
    retrieve the values corresponding to those geopoints.

    \b Examples: \ref iterator.py "iterator.py"

    @param gribid  id of the grib loaded in memory
    @param mode    flags for future use
    @return geoiterator id
    """
    err, iterid = _internal.grib_c_iterator_new(gribid,mode)
    CHECK(err)
    return iterid

@require(iterid=int)
def iterator_delete(iterid):
    """
    @brief Delete a geoiterator and free memory.

    \b Examples: \ref iterator.py "iterator.py"

    @param iterid  geoiterator id
    @exception InternalError 
    """
    CHECK(_internal.grib_c_iterator_delete(iterid))

@require(iterid=int)
def iterator_next(iterid):
    """
    @brief Retrieve the next value from a geoiterator.

    \b Examples: \ref iterator.py "iterator.py"

    @param iterid geoiterator id
    @return tuple with the latitude, longitude and value
    @exception InternalError 
    """
    err,lat,lon,value = _internal.grib_c_iterator_next(iterid)
    if (err == 0):
        return []
    elif (err < 0):
        CHECK(err)
    else:
        return (lat,lon,value)

@require(gribid=int)
def keys_iterator_new(gribid,namespace=None):
    """
    @brief Create a new iterator on the keys.
    
    The keys iterator can be navigated to give all the key names which
    can then be used to get or set the key values with \ref get or
    \ref set.
    The set of keys returned can be controlled with the input variable
    namespace or using the functions
    \ref grib_skip_read_only, \ref grib_skip_duplicates,
    \ref grib_skip_coded,\ref grib_skip_computed.
    If namespace is a non empty string only the keys belonging to
    that namespace are returned. Available namespaces are "ls" (to get the same
    default keys as the grib_ls and "mars" to get the keys used by mars.

    \b Examples: \ref iterator.py "iterator.py"
    
    @param gribid      id of the grib loaded in memory
    @param namespace   the namespace of the keys to search for (all the keys if empty)
    @return keys iterator id to be used in the keys iterator functions
    @exception InternalError 
    """
    err, iterid = _internal.grib_c_keys_iterator_new(gribid,namespace)
    CHECK(err)
    return iterid

@require(iterid=int)
def keys_iterator_next(iterid):
    """
    @brief Advance to the next keys iterator value.

    \b Examples: \ref keys_iterator.py "keys_iterator.py"
    
    @param iterid      keys iterator id created with @ref keys_iterator_new
    @exception InternalError 
    """
    res = _internal.grib_c_keys_iterator_next(iterid)
    if res < 0:
        CHECK(res)
    return res

@require(iterid=int)
def keys_iterator_delete(iterid):
    """
    @brief Delete a keys iterator and free memory.

    \b Examples: \ref keys_iterator.py "keys_iterator.py"
    
    @param iterid      keys iterator id created with @ref keys_iterator_new
    @exception InternalError 
    """
    CHECK(_internal.grib_c_keys_iterator_delete(iterid))

@require(iterid=int)
def keys_iterator_get_name(iterid):
    """
    @brief Get the name of a key from a keys iterator.

    \b Examples: \ref keys_iterator.py "keys_iterator.py"
    
    @param iterid      keys iterator id created with @ref keys_iterator_new
    @return key name to be retrieved
    @exception InternalError 
    """
    err, name = _internal.grib_c_keys_iterator_get_name(iterid,1024)
    CHECK(err)
    return name

@require(iterid=int)
def keys_iterator_rewind(iterid):
    """
    @brief Rewind a keys iterator.
    
    @param iterid      keys iterator id created with @ref keys_iterator_new
    @exception InternalError 
    """
    CHECK(_internal.grib_c_keys_iterator_rewind(iterid))

@require(gribid=int,key=str)
def get_long(gribid,key):
    """
    @brief Get the value of a key in a grib message as an int.
    
    @param gribid      id of the grib loaded in memory
    @param key         key name
    @return value of key as int
    @exception InternalError 
    """
    err, value = _internal.grib_c_get_long(gribid,key)
    CHECK(err)
    return value

@require(gribid=int,key=str)
def get_double(gribid,key):
    """
    @brief Get the value of a key in a grib message as a float.
    
    @param gribid      id of the grib loaded in memory
    @param key         key name
    @return value of key as float
    @exception InternalError 
    """
    err, value = _internal.grib_c_get_double(gribid,key)
    CHECK(err)
    return value

@require(gribid=int,key=str,value=(int,long,float,str))
def set_long(gribid,key,value):
    """
    @brief Set the integer value for a key in a grib message.

    A TypeError exception will be thrown if value cannot be represented
    as an integer.
    
    @param gribid      id of the grib loaded in memory
    @param key         key name
    @param value       value to set
    @exception InternalError,TypeError
    """
    try:
        value = int(value)
    except (ValueError,TypeError):
        raise TypeError("Invalid type")
    
    if (value > sys.maxint):
        raise TypeError("Invalid type")

    CHECK(_internal.grib_c_set_long(gribid,key,value))

@require(gribid=int,key=str,value=(int,long,float,str))
def set_double(gribid,key,value):
    """
    @brief Set the double value for a key in a grib message.

    A TypeError exception will be thrown if value cannot be represented
    as a float.
    
    @param gribid      id of the grib loaded in memory
    @param key         key name
    @param value       float value to set
    @exception InternalError,TypeError
    """
    try:
        value = float(value)
    except (ValueError,TypeError):
        raise TypeError("Invalid type")

    CHECK(_internal.grib_c_set_double(gribid,key,value))

@require(samplename=str)
def grib_new_from_samples(samplename):
    """
    @brief Create a new valid gribid from a sample. 
    
    The available samples are picked up from the directory pointed to
    by the environment variable GRIB_SAMPLES_PATH.
    To know where the samples directory is run the grib_info tool.\n
    
    \b Examples: \ref samples.py "samples.py"
    
    @param samplename   name of the sample to be used
    @return id of the grib loaded in memory
    @exception InternalError 
    """
    err,gribid = _internal.grib_c_new_from_samples(0,samplename)
    CHECK(err)
    return gribid

@require(gribid_src=int)
def clone(gribid_src):
    """
    @brief Create a copy of a message.
    
    Create a copy of a given message (\em gribid_src) resulting in a new
    message in memory (\em gribid_dest) identical to the original one.
    
    \b Examples: \ref clone.py "clone.py"
    
    @param gribid_src     id of grib to be cloned
    @return id of clone
    @exception InternalError 
    """
    err,newgribid_src = _internal.grib_c_clone(gribid_src,0)
    CHECK(err)
    return newgribid_src

@require(gribid=int,key=str)
def set_double_array(gribid, key, inarray):
    """
    @brief Set the value of the key to a double array.

    The input array can be a numpy.ndarray or a python sequence like tuple, list, array, ...

    If NumPy is enabled, the wrapper will internally try to convert the input to a NumPy array
    before extracting its data and length. This is possible as NumPy
    allows the construction of arrays from arbitrary python sequences.

    The elements of the input sequence need to be convertible to a double.

    @param gribid   id of the grib loaded in memory
    @param key      key name
    @param inarray  tuple,list,array,numpy.ndarray 
    @exception InternalError 
    """
    if with_numpy():
        CHECK(_internal.grib_set_double_ndarray(gribid,key,inarray))
    else:
        nval = len(inarray)
        a = _internal.new_doubleArray(nval)
        s = _internal.intp()
        s.assign(nval)

        for i in range(nval):
            _internal.doubleArray_setitem(a,i,inarray[i])

        CHECK(_internal.grib_c_set_real8_array(gribid,key,a,s))

        _internal.delete_doubleArray(a)

@require(gribid=int,key=str)
def get_double_array(gribid,key):
    """
    @brief Get the value of the key as a double array. 
    
    If NumPy is enabled, the double array will be stored in a NumPy ndarray.
    Otherwise, Python's native array type will be used.

    @param gribid   id of the grib loaded in memory
    @param key      key name
    @return numpy.ndarray or array
    @exception InternalError 
    """
    if with_numpy():
        nval = get_size(gribid,key)
        err,result = _internal.grib_get_double_ndarray(gribid,key,nval)
        CHECK(err)
        return result
    else:
        nval = get_size(gribid,key)
        a = _internal.new_doubleArray(nval)
        s = _internal.intp()
        s.assign(nval)

        CHECK(_internal.grib_c_get_real8_array(gribid,key,a,s))

        result = array("d")
        for i in range(nval):
            result.append(_internal.doubleArray_getitem(a,i))

        _internal.delete_doubleArray(a)

        return result

@require(gribid=int,key=str)
def set_long_array(gribid, key, inarray):
    """
    @brief Set the value of the key to an integer array.

    If NumPy is enabled, the wrapper will internally try to convert the input to a NumPy array
    before extracting its data and length. This is possible as NumPy
    allows the construction of arrays from arbitrary python sequences.

    The elements of the input sequence need to be convertible to an int.
    
    @param gribid      id of the grib loaded in memory
    @param key         key name
    @param inarray     tuple,list,python array,numpy array
    @exception InternalError 
    """
    if with_numpy():
        CHECK(_internal.grib_set_long_ndarray(gribid, key, inarray))
    else:
        nval = len(inarray)
        a = _internal.new_longArray(nval)
        s = _internal.intp()
        s.assign(nval)

        for i in range(nval):
            _internal.longArray_setitem(a,i,inarray[i])

        CHECK(_internal.grib_c_set_long_array(gribid,key,a,s))

        _internal.delete_longArray(a)

@require(gribid=int,key=str)
def get_long_array(gribid,key):
    """
    @brief Get the integer array of values for a key from a grib message.
    
    If NumPy is enabled, the integer array will be stored in a NumPy ndarray.
    Otherwise, Python's native array type will be used.
    
    @param gribid      id of the grib loaded in memory
    @param key         key name
    @return numpy.ndarray or array
    @exception InternalError 
    """
    if with_numpy():
        nval = get_size(gribid,key)
        err,result = _internal.grib_get_long_ndarray(gribid,key,nval)
        CHECK(err)
        return result
    else:
        nval = get_size(gribid,key)
        a = _internal.new_longArray(nval)
        s = _internal.intp()
        s.assign(nval)

        CHECK(_internal.grib_c_get_long_array(gribid,key,a,s))

        result = array("l")
        for i in range(nval):
            result.append(_internal.longArray_getitem(a,i))

        _internal.delete_longArray(a)

        return result

def grib_multi_new():
    """
    @brief Create a new multi field and return its id.

    \b Examples: \ref multi_write.py "multi_write.py"

    @return id of the multi field
    @exception InternalError 
    """
    err,mgid = _internal.grib_c_multi_new()
    CHECK(err)
    return mgid

@require(gribid=int)
def grib_multi_release(gribid):
    """
    @brief Release a multi field from memory.

    \b Examples: \ref multi_write.py "multi_write.py"

    @param gribid    id of the multi field we want to release the memory for
    @exception InternalError 
    """
    CHECK(_internal.grib_c_multi_release(gribid))

@require(gribid_src=int,namespace=str,gribid_dest=int)
def grib_copy_namespace(gribid_src,namespace,gribid_dest):
    """
    @brief Copy the value of all the keys belonging to a namespace from the source message
    to the destination message.
    
    @param gribid_src     id of source message
    @param gribid_dest    id of destination message
    @param namespace      namespace to be copied
    @exception InternalError 
    """
    CHECK(_internal.grib_c_copy_namespace(gribid_src, namespace, gribid_dest))

@require(filename=str,keys=(tuple,list))
def index_new_from_file(filename, keys):
    """
    @brief Create a new index from a file.

    \b Examples: \ref index.py "index.py"

    @param filename   path of the file to index on
    @param keys       sequence of keys to index on
    @return index id
    @exception InternalError 
    """
    ckeys = ",".join(keys)
    err,iid = _internal.grib_c_index_new_from_file(filename, ckeys)
    CHECK(err)
    return iid

@require(gribid=int)
def set_key_vals(gribid, key_vals):
    """
    Set the values for several keys at once in a grib message.
    
    @param gribid      id of the grib loaded in memory
    @param key_vals    can be a string, list/tuple or dictionary.
                       If a string, format must be "key1=val1,key2=val2"
                       If a list, it must contain strings of the form "key1=val1"
    @exception         InternalError
    """
    if (len(key_vals) == 0):
        raise InternalError("Empty key/values argument")
    key_vals_str = ""
    if isinstance(key_vals, str):
        # Plain string. We need to do a DEEP copy so as not to change the original
        key_vals_str = ''.join(key_vals)
    elif isinstance(key_vals, list) or isinstance(key_vals, tuple):
        # A list of key=val strings
        for kv in key_vals:
            if not isinstance(kv, str):
                raise TypeError("Invalid list/tuple element type '%s'"%kv)
            if not '=' in str(kv):
                raise InternalError("Invalid list/tuple element format '%s'"%kv)
            if len(key_vals_str) > 0:
                key_vals_str += ','
            key_vals_str += kv
    elif isinstance(key_vals, dict):
        # A dictionary mapping keys to values
        for key in key_vals.iterkeys():
            if len(key_vals_str) > 0:
                key_vals_str += ','
            key_vals_str += key+'='+str(key_vals[key])
    else:
        raise TypeError("Invalid argument type")

    CHECK(_internal.grib_c_set_key_vals(gribid, key_vals_str))

@require(indexid=int, filename=str)
def index_add_file(indexid, filename):
    """
    @brief Add a file to an index.

    \b Examples: \ref index.py "index.py"

    @param indexid    id of the index to add the file to
    @param filename   path of the file to be added to index
    @exception InternalError 
    """
    err = _internal.grib_c_index_add_file(indexid, filename)
    CHECK(err)

@require(indexid=int)
def index_release(indexid):
    """
    @brief Delete an index.

    \b Examples: \ref index.py "index.py"
    
    @param indexid   id of an index created from a file.
    @exception InternalError 
    """
    CHECK(_internal.grib_c_index_release(indexid))

@require(indexid=int,key=str)
def index_get_size(indexid,key):
    """
    @brief Get the number of distinct values for the index key. The key must belong to the index.
    
    \b Examples: \ref index.py "index.py"
    
    @param indexid    id of an index created from a file. The index must have been created on the given key.
    @param key        key for which the number of values is computed
    @return number of distinct values for key in index
    @exception InternalError 
    """
    err,value = _internal.grib_c_index_get_size_long(indexid,key)
    CHECK(err)
    return value

@require(indexid=int,key=str)
def index_get_long(indexid,key):
    """
    @brief Get the distinct values of the key in argument contained in the index. The key must belong to the index. 
    
    This function is used when the type of the key was explicitly defined as long or when the native type of the key is long.
    
    \b Examples: \ref index.py "index.py"
    
    @param indexid   id of an index created from a file. The index must have been created with the key in argument.
    @param key       key for wich the values are returned
    @return tuple with values of key in index
    @exception InternalError 
    """
    nval = index_get_size(indexid,key)

    a = _internal.new_longArray(nval)
    s = _internal.intp()
    s.assign(nval)

    CHECK(_internal.grib_c_index_get_long(indexid,key,a,s))

    result = []
    for i in range(nval):
        result.append(_internal.longArray_getitem(a,i))

    _internal.delete_longArray(a)

    return tuple(result)

def index_get_string(indexid,key):
    """
    @brief Get the distinct values of the key in argument contained in the index. The key must belong to the index. 

    This function is used when the type of the key was explicitly defined as string or when the native type of the key is string.
    
    \b Examples: \ref index.py "index.py"
    
    @param indexid   id of an index created from a file. The index must have been created with the key in argument.
    @param key       key for wich the values are returned
    @param return tuple with values of key in index
    @exception InternalError 
    """
    nval = index_get_size(indexid,key)
    max_val_size = 1024

    err,raw_result,outnval = _internal.grib_c_index_get_string(indexid,key,max_val_size,nval)
    CHECK(err)

    assert nval == outnval

    result = []
    for i in range(nval):
        low = i*max_val_size
        high = (i+1)*max_val_size
        value = raw_result[low:high].rstrip()
        result.append(value)

    return tuple(result)

@require(iid=int,key=str)
def index_get_double(iid,key):
    """
    @brief Get the distinct values of the key in argument contained in the index. The key must belong to the index. 
    
    This function is used when the type of the key was explicitly defined as double or when the native type of the key is double.
    
    \b Examples: \ref index.py "index.py"
    
    @param indexid   id of an index created from a file. The index must have been created with the key in argument.
    @param key       key for wich the values are returned
    @return tuple with values of key in index
    @exception InternalError 
    """
    nval = index_get_size(iid,key)

    a = _internal.new_doubleArray(nval)
    s = _internal.intp()
    s.assign(nval)

    CHECK(_internal.grib_c_index_get_real8(iid,key,a,s))

    result = []
    for i in range(nval):
        result.append(_internal.doubleArray_getitem(a,i))

    _internal.delete_doubleArray(a)

    return tuple(result)

@require(indexid=int,key=str,val=int)
def index_select_long(indexid,key,val):
    """
    @brief Select the message subset with key==value. The value is an integer.
    
    The key must have been created with integer type or have integer as native type if the type was not explicitly defined in the index creation.
    
    \b Examples: \ref index.py "index.py"
    
    @param indexid   id of an index created from a file. The index must have been created with the key in argument.
    @param key       key to be selected
    @param value     value of the key to select
    @exception InternalError 
    """
    CHECK(_internal.grib_c_index_select_long(indexid,key,val))

@require(iid=int,key=str,val=float)
def index_select_double(iid,key,val):
    """
    @brief Select the message subset with key==value. The value is a double.
    
    The key must have been created with integer type or have integer as native type if the type was not explicitly defined in the index creation.
    
    \b Examples: \ref index.py "index.py"
    
    @param indexid   id of an index created from a file. The index must have been created with the key in argument.
    @param key       key to be selected
    @param value     value of the key to select
    @exception InternalError 
    """
    CHECK(_internal.grib_c_index_select_real8(iid,key,val))

@require(indexid=int,key=str,val=str)
def index_select_string(indexid,key,val):
    """
    @brief Select the message subset with key==value. The value is a integer. 

    The key must have been created with string type or have string as native type if the type was not explicitly defined in the index creation.
    
    In case of error, if the status parameter (optional) is not given, the program will
    exit with an error message.\n Otherwise the error message can be
    gathered with @ref grib_get_error_string.
    
    \b Examples: \ref index.py "index.py"
    
    @param indexid   id of an index created from a file. The index must have been created with the key in argument.
    @param key       key to be selected
    @param value     value of the key to select
    @exception InternalError 
    """
    CHECK(_internal.grib_c_index_select_string(indexid,key,val))

def new_from_index(indexid):
    """
    @brief Create a new handle from an index after having selected the key values.

    All the keys belonging to the index must be selected before calling this function. 
    Successive calls to this function will return all the handles compatible with the constraints defined selecting the values of the index keys.
    
    The message can be accessed through its gribid and it will be available until @ref release is called.
    
    \b Examples: \ref index.py "index.py"
    
    @param indexid   id of an index created from a file.
    @return id of the grib loaded in memory or None if end of index
    @exception InternalError 
    """
    err,gribid = _internal.grib_c_new_from_index(indexid,0)

    if err:
        if err == _internal.GRIB_END_OF_INDEX:
            return None
        else:
            CHECK(err)
    else:
        return gribid

@require(gribid=int)
def get_message_size(gribid):
    """
    @brief Get the size of a coded message.
    
    @param gribid      id of the grib loaded in memory
    @return size in bytes of the message
    @exception InternalError 
    """
    err,value = _internal.grib_c_get_message_size(gribid)
    CHECK(err)
    return value

@require(gribid=int)
def get_message_offset(gribid):
    """
    @brief Get the offset of a coded message.
    
    @param gribid      id of the grib loaded in memory
    @return offset in bytes of the message
    @exception InternalError 
    """
    err,value = _internal.grib_c_get_message_offset(gribid)
    CHECK(err)
    return value

@require(gribid=int,key=str,index=int)
def get_double_element(gribid,key,index):
    """
    @brief Get as double the i-th element of the "key" array.

    @param gribid      id of the grib loaded in memory
    @param key         the key to be searched
    @param index       zero based index of value to retrieve
    @return value
    @exception InternalError 

    """
    err,value = _internal.grib_c_get_real8_element(gribid,key,index)
    CHECK(err)
    return value

@require(gribid=int,key=str,indexes=(list,tuple))
def get_double_elements(gribid,key,indexes):
    """
    @brief Get as double array the elements of the "key" array whose indexes are listed in the input array.

    @param gribid      id of the grib loaded in memory
    @param key         the key to be searched
    @param indexes     list or tuple of indexes
    @return numpy.ndarray or array
    @exception InternalError 

    """
    if with_numpy():
        nidx = len(indexes)
        err,result = _internal.grib_get_double_ndelements(gribid,key,indexes,nidx)
        CHECK(err)
        return result
    else:
        nidx = len(indexes)

        pidx = _internal.new_intArray(nidx)
        pval = _internal.new_doubleArray(nidx)
        psize = _internal.intp()
        psize.assign(nidx)

        for i in range(len(indexes)):
            _internal.intArray_setitem(pidx,i,indexes[i])

        err = _internal.grib_c_get_real8_elements(gribid,key,pidx,pval,psize)
        CHECK(err)

        result = array("d")
        for i in range(psize.value()):
            result.append(_internal.doubleArray_getitem(pval,i))

        _internal.delete_intArray(pidx)
        _internal.delete_doubleArray(pval)

        return result

def get_elements(gribid,key,indexes):
    """
    @brief Retrieve the elements of the key array for the indexes specified in the input.

    @param gribid      id of the grib loaded in memory
    @param key         the key to be searched
    @param indexes     single index or a list of indexes
    @return numpy.ndarray or array containing the values of key for the given indexes
    @exception InternalError
    """
    try:
        iter(indexes)
    except TypeError:
        indexes = (indexes,)

    nidx = len(indexes)
    err,result = _internal.grib_get_double_ndelements(gribid,key,indexes,nidx)
    CHECK(err)
    return result

@require(gribid=int,key=str)
def set_missing(gribid,key):
    """
    @brief Set as missing the value for a key in a grib message.
    
    It can be used to set a missing value in the grib header but not in 
    the data values.
    
    \b Examples: \ref set_missing.py "set_missing.py"
    
    @param  gribid     id of the grib loaded in memory
    @param  key        key name
    @exception InternalError 
    """
    CHECK(_internal.grib_c_set_missing(gribid,key))

@require(gribid=int,key=str)
def is_missing(gribid,key):
    """
    @brief Check if the value of a key is MISSING.

    The value of a key is considered as MISSING when all the bits assigned to it
    are set to 1. This is different from the actual key missing from the grib message.
    The value of a key MISSING has a special significance and that can be read about
    in the WMO documentation.
    
    @param gribid      id of the grib loaded in memory
    @param key         key name
    @return 0->not missing, 1->missing
    @exception InternalError 
    """
    err,value = _internal.grib_c_is_missing(gribid,key)
    CHECK(err)
    return value


@require(gribid=int,key=str)
def is_defined(gribid,key):
    """
    @brief Check if a key is defined (exists)
    @param gribid      id of the grib loaded in memory
    @param key         key name
    @return 0->not defined, 1->defined
    @exception InternalError 
    """
    err,value = _internal.grib_c_is_defined(gribid,key)
    CHECK(err)
    return value


@require(gribid=int,inlat=(int,float),inlon=(int,float))
def grib_find_nearest(gribid,inlat,inlon,is_lsm = False,npoints = 1):
    """
    @brief Find the nearest grid point or the nearest four grid points to a given latitude/longitude.

    The number of nearest points returned can be controled through the npoints function argument.

    \b Examples: \ref nearest.py "nearest.py"
    
    @param gribid     id of the grib loaded in memory
    @param inlat      latitude of the point
    @param inlon      longitude of the point
    @param is_lsm     True if the nearest land point is required otherwise False.
    @param npoints    1 or 4 nearest grid points
    @return (npoints*(outlat,outlon,value,dist,index))
    @exception InternalError 
    """
    if npoints == 1:
        err,outlat,outlon,value,dist,idx = _internal.grib_c_find_nearest_single(gribid,is_lsm,inlat,inlon)
        CHECK(err)
        return (Bunch(lat = outlat,lon = outlon,value = value,distance = dist,index = idx),)
    elif npoints == 4:
        poutlat = _internal.new_doubleArray(4)
        poutlon = _internal.new_doubleArray(4)
        pvalues = _internal.new_doubleArray(4)
        pdist = _internal.new_doubleArray(4)
        pidx = _internal.new_intArray(4)

        CHECK(_internal.grib_c_find_nearest_four_single(gribid,is_lsm,inlat,inlon,poutlat,poutlon,pvalues,pdist,pidx))

        result = []
        for i in range(4):
            result.append(Bunch( \
                lat = _internal.doubleArray_getitem(poutlat,i), \
                lon = _internal.doubleArray_getitem(poutlon,i), \
                value = _internal.doubleArray_getitem(pvalues,i), \
                distance = _internal.doubleArray_getitem(pdist,i), \
                index = _internal.intArray_getitem(pidx,i), \
            ))

        _internal.delete_doubleArray(poutlat)
        _internal.delete_doubleArray(poutlon)
        _internal.delete_doubleArray(pvalues)
        _internal.delete_doubleArray(pdist)
        _internal.delete_intArray(pidx)

        return tuple(result)
    else:
        raise ValueError("Invalid value for npoints. Expecting 1 or 4.")

@require(gribid=int,key=str)
def get_native_type(gribid, key):
    """
    @brief Retrieve the native type of a key. 

    Possible values can be int, float or string.

    @param gribid   id of the grib loaded in memory
    @param key      key we want to find out the type for
    @return type of key given as input or None if not determined
    @exception InternalError 
    """
    err,itype = _internal.grib_c_get_native_type(gribid,key)
    CHECK(err)
    if itype in KEYTYPES:
        return KEYTYPES[itype]
    else:
        return None

@require(gribid=int,key=str)
def get(gribid,key, ktype=None):
    """
    @brief Get the value of a key in a grib message.

    The type of value returned depends on the native type of the requested key.
    The type of value returned can be forced by using the type argument of the
    function. The type argument can be int, float or str.
    
    The \em gribid references a grib message loaded in memory.
    
    \b Examples: \ref get.py "get.py", \ref print_data.py "print_data.py"
    
    @see grib_new_from_file, release, set
    
    @param gribid      id of the grib loaded in memory
    @param key         key name
    @param ktype       the type we want the output in (int, float or str), native type if not specified
    @return scalar value of key as int, float or str
    @exception InternalError 
    """
    if ktype is None:
        ktype = get_native_type(gribid, key)

    result = None
    if ktype is int:
        result = get_long(gribid, key)
    elif ktype is float:
        result = get_double(gribid, key)
    elif ktype is str:
        result = get_string(gribid, key)

    return result

@require(gribid=int,key=str)
def grib_get_array(gribid,key, ktype=None):
    """
    @brief Get the contents of an array key. 
    
    The output array will be stored in a NumPy ndarray or array. 
    The type of the array returned depends on the native type of the requested key. 
    The type of value returned can be forced by using the type argument of the function.
    The type argument can be int or float.

    @param gribid      id of the grib loaded in memory
    @param key         the key to get the value for
    @param ktype       the type we want the output in (can be int or float), native type if not specified
    @return numpy.ndarray or array
    @exception InternalError 
    """
    if ktype is None:
        ktype = get_native_type(gribid,key)

    result = None
    if ktype is int:
        result = get_long_array(gribid, key)
    elif ktype is float:
        result = get_double_array(gribid, key)

    return result

@require(gribid=int)
def get_values(gribid):
    """
    @brief Retrieve the contents of the 'values' key.
    
    A NumPy ndarray or Python array containing the values in the message is returned.

    \b Examples: \ref print_data.py "print_data.py", \ref samples.py "samples.py"

    @param gribid   id of the grib loaded in memory
    @return numpy.ndarray or array
    @exception InternalError 
    """
    return get_double_array(gribid,"values")

@require(gribid=int)
def set_values(gribid,values):
    """
    @brief Set the contents of the 'values' key.

    The input array can be a numpy.ndarray or a python sequence like tuple, list, array, ...

    If NumPy is enabled, the wrapper will internally try to convert the input to a NumPy array
    before extracting its data and length. This is possible as NumPy
    allows the construction of arrays from arbitrary python sequences.

    The elements of the input sequence need to be convertible to a double.

    \b Examples: \ref clone.py "clone.py", \ref samples.py "samples.py"

    @param gribid   id of the grib loaded in memory
    @param values   array of values to set as tuple, list, array or numpy.ndarray
    """
    set_double_array(gribid,"values",values)

@require(gribid=int,key=str)
def set(gribid,key,value):
    """
    @brief Set the value for a scalar key in a grib message.

    The input value can be a python int, float or str.
    
    \b Examples: \ref set.py "set.py"
    
    @see grib_new_from_file, release, get
    
    @param gribid      id of the grib loaded in memory
    @param key         key name
    @param value       scalar value to set for key
    @exception InternalError 
    """
    if isinstance(value,int):
        set_long(gribid,key,value)
    elif isinstance(value,float):
        set_double(gribid,key,value)
    elif isinstance(value,str):
        set_string(gribid,key,value)
    else:
        raise InternalError("Invalid type of value when setting key '%s'." % key)

@require(gribid=int,key=str)
def set_array(gribid,key,value):
    """
    @brief Set the value for an array key in a grib message.

    Some array keys can be "values","pl", "pv" respectively the data values,
    the list of number of points for each latitude in a reduced grid and the list of
    vertical levels.

    The input array can be a numpy.ndarray or a python sequence like tuple, list, array, ...

    If NumPy is enabled, the wrapper will internally try to convert the input to a NumPy array
    before extracting its data and length. This is possible as NumPy
    allows the construction of arrays from arbitrary python sequences.

    @param gribid      id of the grib loaded in memory
    @param key         key name
    @param value       array value to set for key
    @exception InternalError 

    """
    val0 = None
    try:
        val0 = value[0]
    except TypeError:
        pass

    if isinstance(val0, int):
        set_long_array(gribid, key, value)
    elif isinstance(val0, float):
        set_double_array(gribid, key, value)
    else:
        raise InternalError("Invalid type of value when setting key '%s'." % key)

@require(indexid=int,key=str)
def index_get(indexid,key, ktype=str):
    """
    @brief Get the distinct values of an index key. The key must belong to the index.
    
    \b Examples: \ref index.py "index.py"
    
    @param indexid   id of an index created from a file. The index must have been created on the given key.
    @param key       key for which the values are returned
    @return array of values
    @exception InternalError 
    """
    # Cannot get the native type of a key from an index
    # so right now the default is str. The user can overwrite
    # the type but there is no way right now to do it automatically.

    #if ktype is None:
    #    ktype = get_native_type(indexid,key)

    result = None
    if ktype is int:
        result = index_get_long(indexid,key)
    elif ktype is float:
        result = index_get_double(indexid,key)
    elif ktype is str:
        result = index_get_string(indexid,key)

    return result

@require(indexid=int,key=str)
def index_select(indexid,key,value):
    """
    @brief Select the message subset with key==value. 
    
    \b Examples: \ref index.py "index.py"
    
    @param indexid   id of an index created from a file. The index must have been created with the key in argument.
    @param key       key to be selected
    @param value     value of the key to select
    @exception InternalError 
    """
    if isinstance(value,int):
        index_select_long(indexid,key,value)
    elif isinstance(value,float):
        index_select_double(indexid,key,value)
    elif isinstance(value,str):
        index_select_string(indexid,key,value)
    else:
        raise InternalError("Invalid type of value when setting key '%s'." % key)

@require(indexid=int, filename=str)
def index_write(indexid, filename):
    """
    @brief Write an index to a file for later reuse. 
    
    An index can be loaded back from an index file with \ref index_read.

    \b Examples: \ref index.py "index.py"

    @param indexid    id of the index
    @param filename   file to save the index to
    @exception InternalError 
    """
    CHECK(_internal.grib_c_index_write(indexid, filename))

@require(filename=str)
def index_read(filename):
    """
    @brief Loads an index previously saved with \ref index_write to a file.

    \b Examples: \ref index.py "index.py"

    @param filename    file to load the index from
    @return id of the loaded index
    @exception InternalError 
    """
    err, indexid = _internal.grib_c_index_read(filename)
    CHECK(err)
    return indexid

@require(flag=bool)
def no_fail_on_wrong_length(flag):
    """
    @brief Do not fail if the message has the wrong length.

    @param flag True/False
    """
    if flag:
        _internal.no_fail_on_wrong_length(1)
    else:
        _internal.no_fail_on_wrong_length(0)

@require(flag=bool)
def gts_header(flag):
    """
    @brief Set the GTS header on/off.

    @param flag True/False
    """
    if flag:
        _internal.grib_c_gts_header_on()
    else:
        _internal.grib_c_gts_header_off()

def get_api_version():
    """
    @brief Get the api version.

    Returns the version of the api as a string in the format "major.minor.revision".
    """
    div = lambda v,d: (v/d,v%d)
    v = _internal.grib_c_get_api_version()
    v,revision = div(v,100)
    v,minor = div(v,100)
    major = v

    return "%d.%d.%d" % (major,minor,revision)

__version__ = get_api_version()

@require(gribid=int)
def get_message(gribid):
    """
    @brief Get the binary message.

    Returns the binary string message associated with the grib identified by gribid.

    @see new_from_message
    
    @param gribid      id of the grib loaded in memory
    @return binary string message associated with gribid
    @exception InternalError 
    """
    error,message = _internal.grib_c_get_message(gribid)
    CHECK(error)
    return message
    
@require(message=str)
def new_from_message(message):
    """
    @brief Create a grib handle from a message in memory.

    Create a new grib message from the input binary string and return its grib id.

    @see get_message

    @param message binary string message
    @return gribid of the newly created grib message
    @exception InternalError 
    """
    error,gribid = _internal.grib_c_new_from_message(0,message,len(message))
    CHECK(error)
    return gribid
