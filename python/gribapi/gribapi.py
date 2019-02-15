"""
@package gribapi
@brief This package is the \b Python interface to ecCodes. It offers almost one to one bindings to the C API functions.

The Python interface to ecCodes uses the <a href="http://numpy.scipy.org/"><b>NumPy</b></a> package
as the container of choice for the possible arrays of values that can be encoded/decoded in and from a message.
Numpy is a package used for scientific computing in Python and an efficient container for generic data.

The Python interface can be enabled/disabled from CMake by using the following flag:\n

@code{.unparsed}
    -DENABLE_PYTHON=ON
or
    -DENABLE_PYTHON=OFF
@endcode

When this is enabed, then the system Python will be used to build the interface.

@em Requirements:

    - Python 2.6 or higher
    - NumPy

"""
import gribapi_swig as _internal
# from gribapi import gribapi_swig as _internal
import types
import sys
import os
from functools import wraps
# import inspect
from . import errors
from errors import *  # noqa

KEYTYPES = {
    1: int,
    2: float,
    3: str,
}

CODES_PRODUCT_ANY = 0
""" Generic product kind """
CODES_PRODUCT_GRIB = 1
""" GRIB product kind """
CODES_PRODUCT_BUFR = 2
""" BUFR product kind """
CODES_PRODUCT_METAR = 3
""" METAR product kind """
CODES_PRODUCT_GTS = 4
""" GTS product kind """
CODES_PRODUCT_TAF = 5
""" TAF product kind """

# Constants for 'missing'
GRIB_MISSING_DOUBLE = -1e+100
GRIB_MISSING_LONG = 2147483647


# GRIB-51 Skip function arguments type checking if the
# environment variable is defined
no_type_checks = os.environ.get('ECCODES_PYTHON_NO_TYPE_CHECKS') is not None


# Function-arguments type-checking decorator
# inspired from http://code.activestate.com/recipes/454322-type-checking-decorator/
# modified to support multiple allowed types and all types in the same decorator call
# This returns a decorator. _params_ is the dict with the type specs
def require(**_params_):
    """
    The actual decorator. Receives the target function in _func_
    """
    def check_types(_func_, _params_=_params_):
        if no_type_checks:
            return _func_

        @wraps(_func_)
        # The wrapper function. Replaces the target function and receives its args
        def modified(*args, **kw):
            arg_names = _func_.func_code.co_varnames
            # argnames, varargs, kwargs, defaults = inspect.getargspec(_func_)
            kw.update(zip(arg_names, args))
            for name, allowed_types in _params_.iteritems():
                param = kw[name]
                if isinstance(allowed_types, types.TypeType):
                    allowed_types = (allowed_types,)
                assert any([isinstance(param, type1) for type1 in allowed_types]), \
                    "Parameter '%s' should be of type %s" % (name, " or ".join([t.__name__ for t in allowed_types]))
            return _func_(**kw)
        return modified
    return check_types

# @cond
class Bunch(dict):
    """
    The collector of a bunch of named stuff :).
    """
    def __init__(self, **kw):
        dict.__init__(self, kw)
        self.__dict__.update(kw)

    def __setitem__(self, key, value):
        dict.__setitem__(self, key, value)
        self.__dict__[key] = value

    def __setattr__(self, key, value):
        dict.__setitem__(self, key, value)
        self.__dict__[key] = value

    def __delitem__(self, key):
        dict.__delitem__(self, key)
        del self.__dict__[key]

    def __delattr__(self, key):
        dict.__delitem__(self, key)
        del self.__dict__[key]

    def __str__(self):
        state = ["%s=%r" % (attribute, value)
                 for (attribute, value)
                 in self.__dict__.items()]
        return '\n'.join(state)
# @endcond


# @cond
@require(errid=int)
def GRIB_CHECK(errid):
    """
    Utility function checking the ecCodes error code and raising
    an error if that was set.

    @param errid  the C interface error id to check
    @exception GribInternalError
    """
    if errid:
        errors.raise_grib_error(errid)
# @endcond


@require(fileobj=file)
def gts_new_from_file(fileobj, headers_only=False):
    """
    @brief Load in memory a GTS message from a file.

    The message can be accessed through its id and will be available\n
    until @ref grib_release is called.\n

    @param fileobj        python file object
    @param headers_only   whether or not to load the message with the headers only
    @return               id of the GTS loaded in memory
    @exception GribInternalError
    """
    err, gtsid = _internal.grib_c_new_gts_from_file(fileobj, headers_only, 0)
    if err:
        if err == _internal.GRIB_END_OF_FILE:
            return None
        else:
            GRIB_CHECK(err)
    else:
        return gtsid


@require(fileobj=file)
def metar_new_from_file(fileobj, headers_only=False):
    """
    @brief Load in memory a METAR message from a file.

    The message can be accessed through its id and will be available\n
    until @ref grib_release is called.\n

    @param fileobj        python file object
    @param headers_only   whether or not to load the message with the headers only
    @return               id of the METAR loaded in memory
    @exception GribInternalError
    """
    err, metarid = _internal.grib_c_new_metar_from_file(fileobj, headers_only, 0)
    if err:
        if err == _internal.GRIB_END_OF_FILE:
            return None
        else:
            GRIB_CHECK(err)
    else:
        return metarid


@require(fileobj=file, product_kind=int)
def codes_new_from_file(fileobj, product_kind, headers_only=False):
    """
    @brief Load in memory a message from a file for a given product.

    The message can be accessed through its id and will be available\n
    until @ref grib_release is called.\n

    \b Examples: \ref get_product_kind.py "get_product_kind.py"

    @param fileobj        python file object
    @param product_kind   one of CODES_PRODUCT_GRIB, CODES_PRODUCT_BUFR, CODES_PRODUCT_METAR or CODES_PRODUCT_GTS
    @param headers_only   whether or not to load the message with the headers only
    @return               id of the message loaded in memory
    @exception GribInternalError
    """
    if product_kind == CODES_PRODUCT_GRIB:
        return grib_new_from_file(fileobj, headers_only)
    if product_kind == CODES_PRODUCT_BUFR:
        return bufr_new_from_file(fileobj, headers_only)
    if product_kind == CODES_PRODUCT_METAR:
        return metar_new_from_file(fileobj, headers_only)
    if product_kind == CODES_PRODUCT_GTS:
        return gts_new_from_file(fileobj, headers_only)
    if product_kind == CODES_PRODUCT_ANY:
        return any_new_from_file(fileobj, headers_only)
    raise Exception("Invalid product kind: " + product_kind)


@require(fileobj=file)
def any_new_from_file(fileobj, headers_only=False):
    """
    @brief Load in memory a message from a file.

    The message can be accessed through its id and will be available\n
    until @ref grib_release is called.\n

    \b Examples: \ref grib_get_keys.py "grib_get_keys.py"

    @param fileobj        python file object
    @param headers_only   whether or not to load the message with the headers only
    @return               id of the message loaded in memory
    @exception GribInternalError
    """
    err, msgid = _internal.grib_c_new_any_from_file(fileobj, headers_only, 0)
    if err:
        if err == _internal.GRIB_END_OF_FILE:
            return None
        else:
            GRIB_CHECK(err)
    else:
        return msgid


@require(fileobj=file)
def bufr_new_from_file(fileobj, headers_only=False):
    """
    @brief Load in memory a BUFR message from a file.

    The message can be accessed through its id and will be available\n
    until @ref grib_release is called.\n

    \b Examples: \ref bufr_get_keys.py "bufr_get_keys.py"

    @param fileobj        python file object
    @param headers_only   whether or not to load the message with the headers only
    @return               id of the BUFR loaded in memory
    @exception GribInternalError
    """
    err, bufrid = _internal.grib_c_new_bufr_from_file(fileobj, headers_only, 0)
    if err:
        if err == _internal.GRIB_END_OF_FILE:
            return None
        else:
            GRIB_CHECK(err)
    else:
        return bufrid


@require(fileobj=file)
def grib_new_from_file(fileobj, headers_only=False):
    """
    @brief Load in memory a GRIB message from a file.

    The message can be accessed through its gribid and will be available\n
    until @ref grib_release is called.\n

    The message can be loaded headers only by using the headers_only argument.
    Default is to have the headers only option set to off (False). If set to on (True),
    data values will be skipped. This will result in a significant performance gain
    if one is only interested in browsing through messages to retrieve metadata.
    Any attempt to retrieve data values keys when in the headers only mode will
    result in a key not found error.

    \b Examples: \ref grib_get_keys.py "grib_get_keys.py"

    @param fileobj        python file object
    @param headers_only   whether or not to load the message with the headers only
    @return               id of the grib loaded in memory
    @exception GribInternalError
    """
    err, gribid = _internal.grib_c_new_from_file(fileobj, 0, headers_only)
    if err:
        if err == _internal.GRIB_END_OF_FILE:
            return None
        else:
            GRIB_CHECK(err)
    else:
        return gribid


@require(fileobj=file)
def grib_count_in_file(fileobj):
    """
    @brief Count the messages in a file.

    \b Examples: \ref count_messages.py "count_messages.py"

    @param fileobj  python file object
    @return         number of messages in the file
    @exception GribInternalError
    """
    err, num = _internal.grib_c_count_in_file(fileobj)
    GRIB_CHECK(err)
    return num


def grib_multi_support_on():
    """
    @brief Turn on the support for multiple fields in a single GRIB message.

    @exception GribInternalError
    """
    _internal.grib_c_multi_support_on()


def grib_multi_support_off():
    """
    @brief Turn off the support for multiple fields in a single GRIB message.

    @exception GribInternalError
    """
    _internal.grib_c_multi_support_off()


@require(msgid=int)
def grib_release(msgid):
    """
    @brief Free the memory for the message referred to by msgid.

    \b Examples: \ref grib_get_keys.py "grib_get_keys.py"

    @param msgid      id of the message loaded in memory
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.grib_c_release(msgid))


@require(msgid=int, key=str)
def grib_get_string(msgid, key):
    """
    @brief Get the string value of a key from a message.

    @param msgid       id of the message loaded in memory
    @param key         key name
    @return            string value of key
    @exception GribInternalError
    """
    length = grib_get_string_length(msgid, key)
    err, value = _internal.grib_c_get_string(msgid, key, length)
    GRIB_CHECK(err)

    stpos = value.find('\0')
    if stpos != -1:
        value = value[0:stpos]

    return value


@require(msgid=int, key=str, value=str)
def grib_set_string(msgid, key, value):
    """
    @brief Set the value for a string key in a message.

    @param msgid      id of the message loaded in memory
    @param key        key name
    @param value      string value
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.grib_c_set_string(msgid, key, value, len(value)))


def grib_gribex_mode_on():
    """
    @brief Turn on the compatibility mode with GRIBEX.

    @exception GribInternalError
    """
    _internal.grib_c_gribex_mode_on()


def grib_gribex_mode_off():
    """
    @brief Turn off the compatibility mode with GRIBEX.

    @exception GribInternalError
    """
    _internal.grib_c_gribex_mode_off()


@require(msgid=int, fileobj=file)
def grib_write(msgid, fileobj):
    """
    @brief Write a message to a file.

    \b Examples: \ref grib_set_keys.py "grib_set_keys.py"

    @param msgid      id of the message loaded in memory
    @param fileobj    python file object
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.grib_c_write(msgid, fileobj))


@require(multigribid=int, fileobj=file)
def grib_multi_write(multigribid, fileobj):
    """
    @brief Write a multi-field GRIB message to a file.

    \b Examples: \ref grib_multi_write.py "grib_multi_write.py"

    @param multigribid      id of the multi-field grib loaded in memory
    @param fileobj          python file object
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.grib_c_multi_write(multigribid, fileobj))


@require(ingribid=int, startsection=int, multigribid=int)
def grib_multi_append(ingribid, startsection, multigribid):
    """
    @brief Append a single-field GRIB message to a multi-field GRIB message.

    Only the sections with section number greather or equal "startsection"
    are copied from the input single message to the multi-field output grib.

    \b Examples: \ref grib_multi_write.py "grib_multi_write.py"

    @param ingribid      id of the input single-field GRIB
    @param startsection  starting from startsection (included) all the sections are copied
                         from the input single grib to the output multi-field grib
    @param multigribid   id of the output multi-field GRIB
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.grib_c_multi_append(ingribid, startsection, multigribid))


@require(msgid=int, key=str)
def grib_get_size(msgid, key):
    """
    @brief Get the size of an array key.

    \b Examples: \ref grib_get_keys.py "grib_get_keys.py",\ref count_messages.py "count_messages.py"

    @param msgid      id of the message loaded in memory
    @param key        name of the key
    @exception GribInternalError
    """
    err, result = _internal.grib_c_get_size_long(msgid, key)
    GRIB_CHECK(err)
    return result


@require(msgid=int, key=str)
def grib_get_string_length(msgid, key):
    """
    @brief Get the length of the string version of a key.

    @param msgid      id of the message loaded in memory
    @param key        name of the key
    @exception GribInternalError
    """
    err, result = _internal.grib_c_get_string_length(msgid, key)
    GRIB_CHECK(err)
    return result


@require(iterid=int)
def grib_skip_computed(iterid):
    """
    @brief Skip the computed keys in a keys iterator.

    The computed keys are not coded in the message, they are computed
    from other keys.

    @see grib_keys_iterator_new,grib_keys_iterator_next,grib_keys_iterator_delete

    @param iterid      keys iterator id
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.grib_c_skip_computed(iterid))


@require(iterid=int)
def grib_skip_coded(iterid):
    """
    @brief Skip the coded keys in a keys iterator.

    The coded keys are actually coded in the message.

    @see grib_keys_iterator_new,grib_keys_iterator_next,grib_keys_iterator_delete

    @param iterid      keys iterator id
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.grib_c_skip_coded(iterid))


@require(iterid=int)
def grib_skip_edition_specific(iterid):
    """
    @brief Skip the edition specific keys in a keys iterator.

    @see grib_keys_iterator_new,grib_keys_iterator_next,grib_keys_iterator_delete

    @param iterid      keys iterator id
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.grib_c_skip_edition_specific(iterid))


@require(iterid=int)
def grib_skip_duplicates(iterid):
    """
    @brief Skip the duplicate keys in a keys iterator.

    @see grib_keys_iterator_new,grib_keys_iterator_next,grib_keys_iterator_delete

    @param iterid      keys iterator id
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.grib_c_skip_duplicates(iterid))


@require(iterid=int)
def grib_skip_read_only(iterid):
    """
    @brief Skip the read_only keys in a keys iterator.

    Read only keys cannot be set.

    @see grib_keys_iterator_new,grib_keys_iterator_next,grib_keys_iterator_delete

    @param iterid      keys iterator id
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.grib_c_skip_read_only(iterid))


@require(iterid=int)
def grib_skip_function(iterid):
    """
    @brief Skip the function keys in a keys iterator.

    @see grib_keys_iterator_new,grib_keys_iterator_next,grib_keys_iterator_delete

    @param iterid      keys iterator id
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.grib_c_skip_function(iterid))


@require(gribid=int, mode=int)
def grib_iterator_new(gribid, mode):
    """
    @brief Create a new geoiterator for the given GRIB message, using its geometry and values.

    The geoiterator can be used to go through all the geopoints in a GRIB message and
    retrieve the values corresponding to those geopoints.

    \b Examples: \ref grib_iterator.py "grib_iterator.py"

    @param gribid  id of the GRIB loaded in memory
    @param mode    flags for future use
    @return        geoiterator id
    """
    err, iterid = _internal.grib_c_iterator_new(gribid, mode)
    GRIB_CHECK(err)
    return iterid


@require(iterid=int)
def grib_iterator_delete(iterid):
    """
    @brief Delete a geoiterator and free memory.

    \b Examples: \ref grib_iterator.py "grib_iterator.py"

    @param iterid  geoiterator id
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.grib_c_iterator_delete(iterid))


@require(iterid=int)
def grib_iterator_next(iterid):
    """
    @brief Retrieve the next value from a geoiterator.

    \b Examples: \ref grib_iterator.py "grib_iterator.py"

    @param    iterid geoiterator id
    @return   tuple with the latitude, longitude and value
    @exception GribInternalError
    """
    err, lat, lon, value = _internal.grib_c_iterator_next(iterid)
    if err == 0:
        return []
    elif err < 0:
        GRIB_CHECK(err)
    else:
        return (lat, lon, value)


@require(msgid=int)
def grib_keys_iterator_new(msgid, namespace=None):
    """
    @brief Create a new iterator on the keys.

    The keys iterator can be navigated to give all the key names which
    can then be used to get or set the key values with \ref grib_get or
    \ref grib_set.
    The set of keys returned can be controlled with the input variable
    namespace or using the functions
    \ref grib_skip_read_only, \ref grib_skip_duplicates,
    \ref grib_skip_coded,\ref grib_skip_computed.
    If namespace is a non empty string only the keys belonging to
    that namespace are returned. Example namespaces are "ls" (to get the same
    default keys as the grib_ls) and "mars" to get the keys used by mars.

    \b Examples: \ref grib_iterator.py "grib_iterator.py"

    @param msgid      id of the message loaded in memory
    @param namespace   the namespace of the keys to search for (all the keys if None)
    @return keys iterator id to be used in the keys iterator functions
    @exception GribInternalError
    """
    err, iterid = _internal.grib_c_keys_iterator_new(msgid, namespace)
    GRIB_CHECK(err)
    return iterid


@require(iterid=int)
def grib_keys_iterator_next(iterid):
    """
    @brief Advance to the next keys iterator value.

    \b Examples: \ref grib_keys_iterator.py "grib_keys_iterator.py"

    @param iterid      keys iterator id created with @ref grib_keys_iterator_new
    @exception GribInternalError
    """
    res = _internal.grib_c_keys_iterator_next(iterid)
    if res < 0:
        GRIB_CHECK(res)
    return res


@require(iterid=int)
def grib_keys_iterator_delete(iterid):
    """
    @brief Delete a keys iterator and free memory.

    \b Examples: \ref grib_keys_iterator.py "grib_keys_iterator.py"

    @param iterid      keys iterator id created with @ref grib_keys_iterator_new
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.grib_c_keys_iterator_delete(iterid))


@require(iterid=int)
def grib_keys_iterator_get_name(iterid):
    """
    @brief Get the name of a key from a keys iterator.

    \b Examples: \ref grib_keys_iterator.py "grib_keys_iterator.py"

    @param iterid      keys iterator id created with @ref grib_keys_iterator_new
    @return key name to be retrieved
    @exception GribInternalError
    """
    err, name = _internal.grib_c_keys_iterator_get_name(iterid, 1024)
    GRIB_CHECK(err)
    return name


@require(iterid=int)
def grib_keys_iterator_rewind(iterid):
    """
    @brief Rewind a keys iterator.

    @param iterid      keys iterator id created with @ref grib_keys_iterator_new
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.grib_c_keys_iterator_rewind(iterid))

# BUFR keys iterator
@require(msgid=int)
def codes_bufr_keys_iterator_new(msgid):
    """
    @brief Create a new iterator on the BUFR keys.

    The keys iterator can be navigated to give all the key names which
    can then be used to get or set the key values with \ref codes_get or
    \ref codes_set.

    \b Examples: \ref bufr_keys_iterator.py "bufr_keys_iterator.py"

    @param msgid      id of the BUFR message loaded in memory
    @return keys iterator id to be used in the keys iterator functions
    @exception GribInternalError
    """
    err, iterid = _internal.codes_c_bufr_keys_iterator_new(msgid)
    GRIB_CHECK(err)
    return iterid


@require(iterid=int)
def codes_bufr_keys_iterator_next(iterid):
    """
    @brief Advance to the next BUFR keys iterator value.

    \b Examples: \ref bufr_keys_iterator.py "bufr_keys_iterator.py"

    @param iterid      keys iterator id created with @ref codes_bufr_keys_iterator_new
    @exception GribInternalError
    """
    res = _internal.codes_c_bufr_keys_iterator_next(iterid)
    if res < 0:
        GRIB_CHECK(res)
    return res


@require(iterid=int)
def codes_bufr_keys_iterator_delete(iterid):
    """
    @brief Delete a BUFR keys iterator and free memory.

    \b Examples: \ref bufr_keys_iterator.py "bufr_keys_iterator.py"

    @param iterid      keys iterator id created with @ref codes_bufr_keys_iterator_new
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.codes_c_bufr_keys_iterator_delete(iterid))


@require(iterid=int)
def codes_bufr_keys_iterator_get_name(iterid):
    """
    @brief Get the name of a key from a BUFR keys iterator.

    \b Examples: \ref bufr_keys_iterator.py "bufr_keys_iterator.py"

    @param iterid      keys iterator id created with @ref codes_bufr_keys_iterator_new
    @return key name to be retrieved
    @exception GribInternalError
    """
    err, name = _internal.codes_c_bufr_keys_iterator_get_name(iterid, 1024)
    GRIB_CHECK(err)
    return name


@require(iterid=int)
def codes_bufr_keys_iterator_rewind(iterid):
    """
    @brief Rewind a BUFR keys iterator.

    @param iterid      keys iterator id created with @ref codes_bufr_keys_iterator_new
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.codes_c_bufr_keys_iterator_rewind(iterid))



@require(msgid=int, key=str)
def grib_get_long(msgid, key):
    """
    @brief Get the value of a key in a message as an integer.

    @param msgid       id of the message loaded in memory
    @param key         key name
    @return            value of key as int
    @exception GribInternalError
    """
    err, value = _internal.grib_c_get_long(msgid, key)
    GRIB_CHECK(err)
    return value


@require(msgid=int, key=str)
def grib_get_double(msgid, key):
    """
    @brief Get the value of a key in a message as a float.

    @param msgid      id of the message loaded in memory
    @param key        key name
    @return           value of key as float
    @exception GribInternalError
    """
    err, value = _internal.grib_c_get_double(msgid, key)
    GRIB_CHECK(err)
    return value


@require(msgid=int, key=str, value=(int, long, float, str))
def grib_set_long(msgid, key, value):
    """
    @brief Set the integer value for a key in a message.

    A TypeError exception will be thrown if value cannot be represented
    as an integer.

    @param msgid      id of the message loaded in memory
    @param key        key name
    @param value      value to set
    @exception GribInternalError,TypeError
    """
    try:
        value = int(value)
    except (ValueError, TypeError):
        raise TypeError("Invalid type")

    if value > sys.maxint:
        raise TypeError("Invalid type")

    GRIB_CHECK(_internal.grib_c_set_long(msgid, key, value))


@require(msgid=int, key=str, value=(int, long, float, str))
def grib_set_double(msgid, key, value):
    """
    @brief Set the double value for a key in a message.

    A TypeError exception will be thrown if value cannot be represented
    as a float.

    @param msgid       id of the message loaded in memory
    @param key         key name
    @param value       float value to set
    @exception GribInternalError,TypeError
    """
    try:
        value = float(value)
    except (ValueError, TypeError):
        raise TypeError("Invalid type")

    GRIB_CHECK(_internal.grib_c_set_double(msgid, key, value))


@require(samplename=str, product_kind=int)
def codes_new_from_samples(samplename, product_kind):
    """
    @brief Create a new valid message from a sample for a given product.

    The available samples are picked up from the directory pointed to
    by the environment variable ECCODES_SAMPLES_PATH.
    To know where the samples directory is run the codes_info tool.\n

    \b Examples: \ref grib_samples.py "grib_samples.py"

    @param samplename     name of the sample to be used
    @param product_kind   CODES_PRODUCT_GRIB or CODES_PRODUCT_BUFR
    @return               id of the message loaded in memory
    @exception GribInternalError
    """
    if product_kind == CODES_PRODUCT_GRIB:
        return grib_new_from_samples(samplename)
    if product_kind == CODES_PRODUCT_BUFR:
        return codes_bufr_new_from_samples(samplename)
    raise Exception("Invalid product kind: " + product_kind)


@require(samplename=str)
def grib_new_from_samples(samplename):
    """
    @brief Create a new valid GRIB message from a sample.

    The available samples are picked up from the directory pointed to
    by the environment variable ECCODES_SAMPLES_PATH.
    To know where the samples directory is run the codes_info tool.\n

    \b Examples: \ref grib_samples.py "grib_samples.py"

    @param samplename   name of the sample to be used
    @return             id of the message loaded in memory
    @exception GribInternalError
    """
    err, msgid = _internal.grib_c_grib_new_from_samples(0, samplename)
    GRIB_CHECK(err)
    return msgid


@require(samplename=str)
def codes_bufr_new_from_samples(samplename):
    """
    @brief Create a new valid BUFR message from a sample.

    The available samples are picked up from the directory pointed to
    by the environment variable ECCODES_SAMPLES_PATH.
    To know where the samples directory is run the codes_info tool.\n

    \b Examples: \ref bufr_copy_data.py "bufr_copy_data.py"

    @param samplename   name of the BUFR sample to be used
    @return             id of the message loaded in memory
    @exception GribInternalError
    """
    err, msgid = _internal.grib_c_bufr_new_from_samples(0, samplename)
    GRIB_CHECK(err)
    return msgid

@require(msgid_src=int, msgid_dst=int)
def codes_bufr_copy_data(msgid_src, msgid_dst):
    """
    @brief Copy data values from a BUFR message msgid_src to another message msgid_dst

    Copies all the values in the data section that are present in the same position
    in the data tree and with the same number of values to the output handle.

    @param msgid_src     id of the message from which the data are copied
    @param msgid_dst     id of the message to which the data are copied
    @return id of new message
    @exception GribInternalError
    """
    err, msgid_dst = _internal.grib_c_bufr_copy_data(msgid_src, msgid_dst)
    GRIB_CHECK(err)
    return msgid_dst


@require(msgid_src=int)
def grib_clone(msgid_src):
    """
    @brief Create a copy of a message.

    Create a copy of a given message (\em msgid_src) resulting in a new
    message in memory (\em msgid_dest) identical to the original one.

    \b Examples: \ref grib_clone.py "grib_clone.py"

    @param msgid_src   id of message to be cloned
    @return            id of clone
    @exception GribInternalError
    """
    err, newmsgid_src = _internal.grib_c_clone(msgid_src, 0)
    GRIB_CHECK(err)
    return newmsgid_src


@require(msgid=int, key=str)
def grib_set_double_array(msgid, key, inarray):
    """
    @brief Set the value of the key to a double array.

    The input array can be a numpy.ndarray or a python sequence like tuple, list, array, ...

    The wrapper will internally try to convert the input to a NumPy array
    before extracting its data and length. This is possible as NumPy
    allows the construction of arrays from arbitrary python sequences.

    The elements of the input sequence need to be convertible to a double.

    @param msgid    id of the message loaded in memory
    @param key      key name
    @param inarray  tuple,list,array,numpy.ndarray
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.grib_set_double_ndarray(msgid, key, inarray))


@require(msgid=int, key=str)
def grib_get_double_array(msgid, key):
    """
    @brief Get the value of the key as a NumPy array of doubles.

    @param msgid   id of the message loaded in memory
    @param key     key name
    @return        numpy.ndarray
    @exception GribInternalError
    """
    nval = grib_get_size(msgid, key)
    err, result = _internal.grib_get_double_ndarray(msgid, key, nval)
    GRIB_CHECK(err)
    return result


@require(msgid=int, key=str)
def grib_get_string_array(msgid, key):
    """
    @brief Get the value of the key as a list of strings.

    @param msgid   id of the message loaded in memory
    @param key     key name
    @return        list
    @exception GribInternalError
    """
    nval = grib_get_size(msgid, key)
    a = _internal.new_stringArray(nval)
    s = _internal.sizetp()
    s.assign(nval)

    GRIB_CHECK(_internal.grib_c_get_string_array(msgid, key, a, s))

    newsize = s.value()
    result = list()
    for i in range(newsize):
        result.append(_internal.stringArray_getitem(a, i))

    _internal.delete_stringArray(a)

    return result


@require(msgid=int, key=str)
def grib_set_string_array(msgid, key, inarray):
    """
    @brief Set the value of the key to a string array.

    The input array can be a python sequence like tuple, list, array, ...

    The wrapper will internally try to convert the input to a NumPy array
    before extracting its data and length. This is possible as NumPy
    allows the construction of arrays from arbitrary python sequences.

    The elements of the input sequence need to be convertible to a double.

    @param msgid   id of the message loaded in memory
    @param key     key name
    @param inarray tuple,list,array
    @exception GribInternalError
    """
    GRIB_CHECK( _internal.grib_c_set_string_array(msgid, key, list(inarray)) )


@require(msgid=int, key=str)
def grib_set_long_array(msgid, key, inarray):
    """
    @brief Set the value of the key to an integer array.

    The input array can be a numpy.ndarray or a python sequence like tuple, list, array, ...

    The wrapper will internally try to convert the input to a NumPy array
    before extracting its data and length. This is possible as NumPy
    allows the construction of arrays from arbitrary python sequences.

    The elements of the input sequence need to be convertible to an int.

    @param msgid       id of the message loaded in memory
    @param key         key name
    @param inarray     tuple,list,python array,numpy.ndarray
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.grib_set_long_ndarray(msgid, key, inarray))


@require(msgid=int, key=str)
def grib_get_long_array(msgid, key):
    """
    @brief Get the integer array of values for a key from a message.

    @param msgid      id of the message loaded in memory
    @param key        key name
    @return           numpy.ndarray
    @exception GribInternalError
    """
    nval = grib_get_size(msgid, key)
    err, result = _internal.grib_get_long_ndarray(msgid, key, nval)
    GRIB_CHECK(err)
    return result


def grib_multi_new():
    """
    @brief Create a new multi-field GRIB message and return its id.

    \b Examples: \ref grib_multi_write.py "grib_multi_write.py"

    @return id of the multi-field message
    @exception GribInternalError
    """
    err, mgid = _internal.grib_c_multi_new()
    GRIB_CHECK(err)
    return mgid


@require(gribid=int)
def grib_multi_release(gribid):
    """
    @brief Release a multi-field message from memory.

    \b Examples: \ref grib_multi_write.py "grib_multi_write.py"

    @param gribid    id of the multi-field we want to release the memory for
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.grib_c_multi_release(gribid))


@require(gribid_src=int, namespace=str, gribid_dest=int)
def grib_copy_namespace(gribid_src, namespace, gribid_dest):
    """
    @brief Copy the value of all the keys belonging to a namespace from the source message
    to the destination message.

    @param gribid_src     id of source message
    @param gribid_dest    id of destination message
    @param namespace      namespace to be copied
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.grib_c_copy_namespace(gribid_src, namespace, gribid_dest))


@require(filename=str, keys=(tuple, list))
def grib_index_new_from_file(filename, keys):
    """
    @brief Create a new index from a file.

    \b Examples: \ref grib_index.py "grib_index.py"

    @param filename   path of the file to index on
    @param keys       sequence of keys to index on.
                      The type of the key can be explicitly declared appending :l for long (or alternatively :i), :d for double, :s for string to the key name.
    @return index id
    @exception GribInternalError
    """
    ckeys = ",".join(keys)
    err, iid = _internal.grib_c_index_new_from_file(filename, ckeys)
    GRIB_CHECK(err)
    return iid


@require(indexid=int, filename=str)
def grib_index_add_file(indexid, filename):
    """
    @brief Add a file to an index.

    \b Examples: \ref grib_index.py "grib_index.py"

    @param indexid    id of the index to add the file to
    @param filename   path of the file to be added to index
    @exception GribInternalError
    """
    err = _internal.grib_c_index_add_file(indexid, filename)
    GRIB_CHECK(err)


@require(indexid=int)
def grib_index_release(indexid):
    """
    @brief Delete an index.

    \b Examples: \ref grib_index.py "grib_index.py"

    @param indexid   id of an index created from a file.
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.grib_c_index_release(indexid))


@require(indexid=int, key=str)
def grib_index_get_size(indexid, key):
    """
    @brief Get the number of distinct values for the index key.
    The key must belong to the index.

    \b Examples: \ref grib_index.py "grib_index.py"

    @param indexid    id of an index created from a file. The index must have been created on the given key.
    @param key        key for which the number of values is computed
    @return           number of distinct values for key in index
    @exception GribInternalError
    """
    err, value = _internal.grib_c_index_get_size_long(indexid, key)
    GRIB_CHECK(err)
    return value


@require(indexid=int, key=str)
def grib_index_get_long(indexid, key):
    """
    @brief Get the distinct values of the key in argument contained in the index.
    The key must belong to the index.

    This function is used when the type of the key was explicitly defined as long or when the native type of the key is long.

    \b Examples: \ref grib_index.py "grib_index.py"

    @param indexid   id of an index created from a file. The index must have been created with the key in argument.
    @param key       key for wich the values are returned
    @return          tuple with values of key in index
    @exception GribInternalError
    """
    nval = grib_index_get_size(indexid, key)

    a = _internal.new_longArray(nval)
    s = _internal.intp()
    s.assign(nval)

    GRIB_CHECK(_internal.grib_c_index_get_long(indexid, key, a, s))

    result = []
    for i in range(nval):
        result.append(_internal.longArray_getitem(a, i))

    _internal.delete_longArray(a)

    return tuple(result)


@require(indexid=int, key=str)
def grib_index_get_string(indexid, key):
    """
    @brief Get the distinct values of the key in argument contained in the index.
    The key must belong to the index.

    This function is used when the type of the key was explicitly defined as string or when the native type of the key is string.

    \b Examples: \ref grib_index.py "grib_index.py"

    @param indexid   id of an index created from a file. The index must have been created with the key in argument.
    @param key       key for wich the values are returned
    @return          tuple with values of key in index
    @exception GribInternalError
    """
    nval = grib_index_get_size(indexid, key)
    max_val_size = 1024

    err, raw_result, outnval = _internal.grib_c_index_get_string(indexid, key, max_val_size, nval)
    GRIB_CHECK(err)

    assert nval == outnval

    result = []
    for i in range(nval):
        low = i * max_val_size
        high = (i + 1) * max_val_size
        value = raw_result[low:high].rstrip()
        result.append(value)

    return tuple(result)


@require(indexid=int, key=str)
def grib_index_get_double(indexid, key):
    """
    @brief Get the distinct values of the key in argument contained in the index.
    The key must belong to the index.

    This function is used when the type of the key was explicitly defined as double or when the native type of the key is double.

    \b Examples: \ref grib_index.py "grib_index.py"

    @param indexid  id of an index created from a file. The index must have been created with the key in argument.
    @param key      key for wich the values are returned
    @return         tuple with values of key in index
    @exception GribInternalError
    """
    nval = grib_index_get_size(indexid, key)

    a = _internal.new_doubleArray(nval)
    s = _internal.intp()
    s.assign(nval)

    GRIB_CHECK(_internal.grib_c_index_get_real8(indexid, key, a, s))

    result = []
    for i in range(nval):
        result.append(_internal.doubleArray_getitem(a, i))

    _internal.delete_doubleArray(a)

    return tuple(result)


@require(indexid=int, key=str, value=int)
def grib_index_select_long(indexid, key, value):
    """
    @brief Select the message subset with key==value.
    The value is an integer.

    The key must have been created with integer type or have integer as native type if the type was not explicitly defined in the index creation.

    \b Examples: \ref grib_index.py "grib_index.py"

    @param indexid   id of an index created from a file. The index must have been created with the key in argument.
    @param key       key to be selected
    @param value     value of the key to select
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.grib_c_index_select_long(indexid, key, value))


@require(indexid=int, key=str, value=float)
def grib_index_select_double(indexid, key, value):
    """
    @brief Select the message subset with key==value.
    The value is a double.

    The key must have been created with integer type or have integer as native type if the type was not explicitly defined in the index creation.

    \b Examples: \ref grib_index.py "grib_index.py"

    @param indexid   id of an index created from a file. The index must have been created with the key in argument.
    @param key       key to be selected
    @param value     value of the key to select
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.grib_c_index_select_real8(indexid, key, value))


@require(indexid=int, key=str, value=str)
def grib_index_select_string(indexid, key, value):
    """
    @brief Select the message subset with key==value.
    The value is an integer.

    The key must have been created with string type or have string as native type if the type was not explicitly defined in the index creation.

    \b Examples: \ref grib_index.py "grib_index.py"

    @param indexid   id of an index created from a file. The index must have been created with the key in argument.
    @param key       key to be selected
    @param value     value of the key to select
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.grib_c_index_select_string(indexid, key, value))


@require(indexid=int)
def grib_new_from_index(indexid):
    """
    @brief Create a new handle from an index after having selected the key values.

    All the keys belonging to the index must be selected before calling this function.
    Successive calls to this function will return all the handles compatible with the constraints defined selecting the values of the index keys.

    The message can be accessed through its gribid and will be available until @ref grib_release is called.

    \b Examples: \ref grib_index.py "grib_index.py"

    @param indexid   id of an index created from a file.
    @return          id of the message loaded in memory or None if end of index
    @exception GribInternalError
    """
    err, gribid = _internal.grib_c_new_from_index(indexid, 0)

    if err:
        if err == _internal.GRIB_END_OF_INDEX:
            return None
        else:
            GRIB_CHECK(err)
    else:
        return gribid


@require(msgid=int)
def grib_get_message_size(msgid):
    """
    @brief Get the size of a coded message.

    @param msgid     id of the message loaded in memory
    @return          size in bytes of the message
    @exception GribInternalError
    """
    err, value = _internal.grib_c_get_message_size(msgid)
    GRIB_CHECK(err)
    return value


@require(msgid=int)
def grib_get_message_offset(msgid):
    """
    @brief Get the offset of a coded message.

    @param msgid    id of the message loaded in memory
    @return         offset in bytes of the message
    @exception GribInternalError
    """
    err, value = _internal.grib_c_get_message_offset(msgid)
    GRIB_CHECK(err)
    return value


@require(msgid=int, key=str, index=int)
def grib_get_double_element(msgid, key, index):
    """
    @brief Get as double the i-th element of the "key" array.

    @param msgid       id of the message loaded in memory
    @param key         the key to be searched
    @param index       zero based index of value to retrieve
    @return            value
    @exception GribInternalError

    """
    err, value = _internal.grib_c_get_real8_element(msgid, key, index)
    GRIB_CHECK(err)
    return value


@require(msgid=int, key=str, indexes=(list, tuple))
def grib_get_double_elements(msgid, key, indexes):
    """
    @brief Get as double array the elements of the "key" array whose indexes are listed in the input array.

    @param msgid       id of the message loaded in memory
    @param key         the key to be searched
    @param indexes     list or tuple of indexes
    @return            numpy.ndarray
    @exception GribInternalError

    """
    nidx = len(indexes)
    err, result = _internal.grib_get_double_ndelements(msgid, key, indexes, nidx)
    GRIB_CHECK(err)
    return result


@require(msgid=int, key=str)
def grib_get_elements(msgid, key, indexes):
    """
    @brief Retrieve the elements of the key array for the indexes specified in the input.

    @param msgid      id of the message loaded in memory
    @param key        the key to be searched
    @param indexes    single index or a list of indexes
    @return           numpy.ndarray containing the values of key for the given indexes
    @exception GribInternalError
    """
    try:
        iter(indexes)
    except TypeError:
        indexes = (indexes,)

    nidx = len(indexes)
    err, result = _internal.grib_get_double_ndelements(msgid, key, indexes, nidx)
    GRIB_CHECK(err)
    return result


@require(msgid=int, key=str)
def grib_set_missing(msgid, key):
    """
    @brief Set as missing the value for a key in a GRIB message.

    It can be used to set a missing value in the GRIB header but not in
    the data values.

    \b Examples: \ref grib_set_missing.py "grib_set_missing.py"

    @param  msgid     id of the message loaded in memory
    @param  key       key name
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.grib_c_set_missing(msgid, key))


@require(gribid=int)
def grib_set_key_vals(gribid, key_vals):
    """
    Set the values for several keys at once in a grib message.

    @param gribid      id of the grib loaded in memory
    @param key_vals    can be a string, list/tuple or dictionary.
                       If a string, format must be "key1=val1,key2=val2"
                       If a list, it must contain strings of the form "key1=val1"
    @exception         GribInternalError
    """
    if len(key_vals) == 0:
        raise errors.InvalidKeyValueError("Empty key/values argument")
    key_vals_str = ""
    if isinstance(key_vals, str):
        # Plain string. We need to do a DEEP copy so as not to change the original
        key_vals_str = ''.join(key_vals)
    elif isinstance(key_vals, list) or isinstance(key_vals, tuple):
        # A list of key=val strings
        for kv in key_vals:
            if not isinstance(kv, str):
                raise TypeError("Invalid list/tuple element type '%s'" % kv)
            if '=' not in str(kv):
                raise errors.GribInternalError("Invalid list/tuple element format '%s'" % kv)
            if len(key_vals_str) > 0:
                key_vals_str += ','
            key_vals_str += kv
    elif isinstance(key_vals, dict):
        # A dictionary mapping keys to values
        for key in key_vals.iterkeys():
            if len(key_vals_str) > 0:
                key_vals_str += ','
            key_vals_str += key + '=' + str(key_vals[key])
    else:
        raise TypeError("Invalid argument type")

    GRIB_CHECK(_internal.grib_c_set_key_vals(gribid, key_vals_str))


@require(msgid=int, key=str)
def grib_is_missing(msgid, key):
    """
    @brief Check if the value of a key is MISSING.

    The value of a key is considered as MISSING when all the bits assigned to it
    are set to 1. This is different from the actual key missing from the grib message.
    The value of a key MISSING has a special significance and that can be read about
    in the WMO documentation.

    @param msgid      id of the message loaded in memory
    @param key        key name
    @return           0->not missing, 1->missing
    @exception GribInternalError
    """
    err, value = _internal.grib_c_is_missing(msgid, key)
    GRIB_CHECK(err)
    return value


@require(msgid=int, key=str)
def grib_is_defined(msgid, key):
    """
    @brief Check if a key is defined (exists)
    @param msgid      id of the message loaded in memory
    @param key        key name
    @return           0->not defined, 1->defined
    @exception        GribInternalError
    """
    err, value = _internal.grib_c_is_defined(msgid, key)
    GRIB_CHECK(err)
    return value


@require(gribid=int, inlat=(int, float), inlon=(int, float))
def grib_find_nearest(gribid, inlat, inlon, is_lsm=False, npoints=1):
    """
    @brief Find the nearest grid point or the nearest four grid points to a given latitude/longitude.

    The number of nearest points returned can be controled through the npoints function argument.

    \b Examples: \ref grib_nearest.py "grib_nearest.py"

    @param gribid     id of the grib loaded in memory
    @param inlat      latitude of the point
    @param inlon      longitude of the point
    @param is_lsm     True if the nearest land point is required otherwise False.
    @param npoints    1 or 4 nearest grid points
    @return (npoints*(outlat,outlon,value,dist,index))
    @exception GribInternalError
    """
    if npoints == 1:
        err, outlat, outlon, value, dist, idx = _internal.grib_c_find_nearest_single(gribid, is_lsm, inlat, inlon)
        GRIB_CHECK(err)
        return (Bunch(lat=outlat, lon=outlon, value=value, distance=dist, index=idx),)
    elif npoints == 4:
        poutlat = _internal.new_doubleArray(4)
        poutlon = _internal.new_doubleArray(4)
        pvalues = _internal.new_doubleArray(4)
        pdist = _internal.new_doubleArray(4)
        pidx = _internal.new_intArray(4)

        GRIB_CHECK(_internal.grib_c_find_nearest_four_single(gribid, is_lsm, inlat, inlon, poutlat, poutlon, pvalues, pdist, pidx))

        result = []
        for i in range(4):
            result.append(Bunch(lat=_internal.doubleArray_getitem(poutlat, i),
                                lon=_internal.doubleArray_getitem(poutlon, i),
                                value=_internal.doubleArray_getitem(pvalues, i),
                                distance=_internal.doubleArray_getitem(pdist, i),
                                index=_internal.intArray_getitem(pidx, i)))

        _internal.delete_doubleArray(poutlat)
        _internal.delete_doubleArray(poutlon)
        _internal.delete_doubleArray(pvalues)
        _internal.delete_doubleArray(pdist)
        _internal.delete_intArray(pidx)

        return tuple(result)
    else:
        raise ValueError("Invalid value for npoints. Expecting 1 or 4.")


@require(msgid=int, key=str)
def grib_get_native_type(msgid, key):
    """
    @brief Retrieve the native type of a key.

    Possible values can be int, float or string.

    @param msgid   id of the message loaded in memory
    @param key     key we want to find out the type for
    @return        type of key given as input or None if not determined
    @exception GribInternalError
    """
    err, itype = _internal.grib_c_get_native_type(msgid, key)
    GRIB_CHECK(err)
    if itype in KEYTYPES:
        return KEYTYPES[itype]
    else:
        return None


@require(msgid=int, key=str)
def grib_get(msgid, key, ktype=None):
    """
    @brief Get the value of a key in a message.

    The type of value returned depends on the native type of the requested key.
    The type of value returned can be forced by using the type argument of the
    function. The type argument can be int, float or str.

    The \em msgid references a message loaded in memory.

    \b Examples: \ref grib_get_keys.py "grib_get_keys.py", \ref grib_print_data.py "grib_print_data.py"

    @see grib_new_from_file, grib_release, grib_set

    @param msgid      id of the message loaded in memory
    @param key        key name
    @param ktype      the type we want the output in (int, float or str), native type if not specified
    @return scalar value of key as int, float or str
    @exception GribInternalError
    """
    if not key:
        raise ValueError("Invalid key name")

    if ktype is None:
        ktype = grib_get_native_type(msgid, key)

    result = None
    if ktype is int:
        result = grib_get_long(msgid, key)
    elif ktype is float:
        result = grib_get_double(msgid, key)
    elif ktype is str:
        result = grib_get_string(msgid, key)

    return result


@require(msgid=int, key=str)
def grib_get_array(msgid, key, ktype=None):
    """
    @brief Get the contents of an array key.

    The type of the array returned depends on the native type of the requested key.
    For numeric data, the output array will be stored in a NumPy ndarray.
    The type of value returned can be forced by using the type argument of the function.
    The type argument can be int, float or string.

    @param msgid      id of the message loaded in memory
    @param key        the key to get the value for
    @param ktype      the type we want the output in (can be int, float or string), native type if not specified
    @return           numpy.ndarray
    @exception GribInternalError
    """
    if ktype is None:
        ktype = grib_get_native_type(msgid, key)

    result = None
    if ktype is int:
        result = grib_get_long_array(msgid, key)
    elif ktype is float:
        result = grib_get_double_array(msgid, key)
    elif ktype is str:
        result = grib_get_string_array(msgid, key)

    return result


@require(gribid=int)
def grib_get_values(gribid):
    """
    @brief Retrieve the contents of the 'values' key for a GRIB message.

    A NumPy ndarray containing the values in the GRIB message is returned.

    \b Examples: \ref grib_print_data.py "grib_print_data.py", \ref grib_samples.py "grib_samples.py"

    @param gribid   id of the GRIB loaded in memory
    @return         numpy.ndarray
    @exception GribInternalError
    """
    return grib_get_double_array(gribid, "values")


@require(gribid=int)
def grib_set_values(gribid, values):
    """
    @brief Set the contents of the 'values' key for a GRIB message.

    The input array can be a numpy.ndarray or a python sequence like tuple, list, array, ...

    The wrapper will internally try to convert the input to a NumPy array
    before extracting its data and length. This is possible as NumPy
    allows the construction of arrays from arbitrary python sequences.

    The elements of the input sequence need to be convertible to a double.

    \b Examples: \ref grib_clone.py "grib_clone.py", \ref grib_samples.py "grib_samples.py"

    @param gribid   id of the GRIB loaded in memory
    @param values   array of values to set as tuple, list, array or numpy.ndarray
    """
    grib_set_double_array(gribid, "values", values)


@require(msgid=int, key=str)
def grib_set(msgid, key, value):
    """
    @brief Set the value for a scalar key in a message.

    The input value can be a python int, float or str.

    \b Examples: \ref grib_set_keys.py "grib_set_keys.py"

    @see grib_new_from_file, grib_release, grib_get

    @param msgid      id of the message loaded in memory
    @param key        key name
    @param value      scalar value to set for key
    @exception GribInternalError
    """
    if isinstance(value, int):
        grib_set_long(msgid, key, value)
    elif isinstance(value, float):
        grib_set_double(msgid, key, value)
    elif isinstance(value, str):
        grib_set_string(msgid, key, value)
    #elif hasattr(value, "__iter__"):
    #    # The value passed in is iterable; i.e. a list or array etc
    #    grib_set_array(msgid, key, value)
    else:
        raise errors.GribInternalError("Invalid type of value when setting key '%s'." % key)


@require(msgid=int, key=str)
def grib_set_array(msgid, key, value):
    """
    @brief Set the value for an array key in a message.

    Examples of array keys:
    "values" - data values
    "pl" - list of number of points for each latitude in a reduced grid
    "pv" - list of vertical levels

    The input array can be a numpy.ndarray or a python sequence like tuple, list, array, ...

    The wrapper will internally try to convert the input to a NumPy array
    before extracting its data and length. This is possible as NumPy
    allows the construction of arrays from arbitrary python sequences.

    @param msgid       id of the message loaded in memory
    @param key         key name
    @param value       array to set for key
    @exception GribInternalError
    """
    val0 = None
    try:
        val0 = value[0]
    except TypeError:
        pass

    if isinstance(val0, float):
        grib_set_double_array(msgid, key, value)
    elif isinstance(val0, int):
        grib_set_long_array(msgid, key, value)
    elif isinstance(val0, str):
        grib_set_string_array(msgid, key, value)
    else:
        raise errors.GribInternalError("Invalid type of value when setting key '%s'." % key)


@require(indexid=int, key=str)
def grib_index_get(indexid, key, ktype=str):
    """
    @brief Get the distinct values of an index key.
    The key must belong to the index.

    \b Examples: \ref grib_index.py "grib_index.py"

    @param indexid   id of an index created from a file. The index must have been created on the given key.
    @param key       key for which the values are returned
    @param ktype     the type we want the output in (int, float or str), str if not specified
    @return array of values
    @exception GribInternalError
    """
    # Cannot get the native type of a key from an index
    # so right now the default is str. The user can overwrite
    # the type but there is no way right now to do it automatically.

    # if ktype is None:
    #     ktype = grib_get_native_type(indexid,key)

    result = None
    if ktype is int:
        result = grib_index_get_long(indexid, key)
    elif ktype is float:
        result = grib_index_get_double(indexid, key)
    elif ktype is str:
        result = grib_index_get_string(indexid, key)

    return result


@require(indexid=int, key=str)
def grib_index_select(indexid, key, value):
    """
    @brief Select the message subset with key==value.

    \b Examples: \ref grib_index.py "grib_index.py"

    @param indexid   id of an index created from a file. The index must have been created with the key in argument.
    @param key       key to be selected
    @param value     value of the key to select
    @exception GribInternalError
    """
    if isinstance(value, int):
        grib_index_select_long(indexid, key, value)
    elif isinstance(value, float):
        grib_index_select_double(indexid, key, value)
    elif isinstance(value, str):
        grib_index_select_string(indexid, key, value)
    else:
        raise errors.GribInternalError("Invalid type of value when setting key '%s'." % key)


@require(indexid=int, filename=str)
def grib_index_write(indexid, filename):
    """
    @brief Write an index to a file for later reuse.

    An index can be loaded back from an index file with \ref grib_index_read.

    \b Examples: \ref grib_index.py "grib_index.py"

    @param indexid    id of the index
    @param filename   path of file to save the index to
    @exception GribInternalError
    """
    GRIB_CHECK(_internal.grib_c_index_write(indexid, filename))


@require(filename=str)
def grib_index_read(filename):
    """
    @brief Loads an index previously saved with \ref grib_index_write to a file.

    \b Examples: \ref grib_index.py "grib_index.py"

    @param filename    path of file to load the index from
    @return id of the loaded index
    @exception GribInternalError
    """
    err, indexid = _internal.grib_c_index_read(filename)
    GRIB_CHECK(err)
    return indexid


@require(flag=bool)
def grib_no_fail_on_wrong_length(flag):
    """
    @brief Do not fail if the message has the wrong length.

    @param flag True/False
    """
    if flag:
        _internal.no_fail_on_wrong_length(1)
    else:
        _internal.no_fail_on_wrong_length(0)


@require(flag=bool)
def grib_gts_header(flag):
    """
    @brief Set the GTS header on/off.

    @param flag True/False
    """
    if flag:
        _internal.grib_c_gts_header_on()
    else:
        _internal.grib_c_gts_header_off()


def grib_get_api_version():
    """
    @brief Get the API version.

    Returns the version of the API as a string in the format "major.minor.revision".
    """
    div = lambda v, d: (v / d, v % d)
    v = _internal.grib_c_get_api_version()
    v, revision = div(v, 100)
    v, minor = div(v, 100)
    major = v

    return "%d.%d.%d" % (major, minor, revision)

__version__ = grib_get_api_version()


@require(msgid=int)
def grib_get_message(msgid):
    """
    @brief Get the binary message.

    Returns the binary string message associated with the message identified by msgid.

    @see grib_new_from_message

    @param msgid      id of the message loaded in memory
    @return           binary string message associated with msgid
    @exception GribInternalError
    """
    error, message = _internal.grib_c_get_message(msgid)
    GRIB_CHECK(error)
    return message


@require(message=str)
def grib_new_from_message(message):
    """
    @brief Create a handle from a message in memory.

    Create a new message from the input binary string and return its id.

    @see grib_get_message

    @param         message binary string message
    @return        msgid of the newly created message
    @exception GribInternalError
    """
    error, msgid = _internal.grib_c_new_from_message(0, message, len(message))
    GRIB_CHECK(error)
    return msgid


@require(defs_path=str)
def grib_set_definitions_path(defs_path):
    """
    @brief Set the definitions path

    @param defs_path   definitions path
    """
    _internal.grib_c_set_definitions_path(defs_path)


@require(samples_path=str)
def grib_set_samples_path(samples_path):
    """
    @brief Set the samples path

    @param samples_path   samples path
    """
    _internal.grib_c_set_samples_path(samples_path)
