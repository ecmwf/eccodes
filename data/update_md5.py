import tempfile
import hashlib
import tempfile
import os
import requests
import sys

# With this script, you can update md5 checksums in data-listing files.
# Synopsis:
#  (A) cd ~/src/eccodes/data; python3 update_md5.py grib_data_files.txt
#  (B) cd ~/src/eccodes/data; python3 update_md5.py bufr/bufr_ref_files.txt
# md5sums get calcurated from locally stored files if avilable. Otherwise
# they get downloaded download.ecmwf.org/test-data/eccodes/data/.


def compute_md5(dataspooldir, datafilename):
    #print (dataspooldir, datafilename)
    try:
        localfile_path = os.path.join(dataspooldir, datafilename)
        with open(localfile_path, 'rb') as fp:
            md5 = hashlib.md5(fp.read())
        return md5.hexdigest()
    except FileNotFoundError:
        pass
    # go visit the source
    print ('Fetching %s from the source!' % datafilename)
    with requests.get('http://download.ecmwf.org/test-data/eccodes/data/'
                      + datafilename) as r:
        with open(localfile_path, 'wb') as w:
            w.write(r.content)
        return hashlib.md5(r.content).hexdigest()

def update_md5(wfd, update_target_file, data_spool_dir):
    datasubdir, _ = os.path.split(update_target_file)
    with open(update_target_file) as rfp:
        for line in rfp.readlines():
            fname = line.strip().split(':')[0]
            output = fname.encode('utf-8') + b':' + \
                compute_md5(data_spool_dir,
                            os.path.join(datasubdir,fname)).encode()
            os.write(wfd, output)
            os.write(wfd, b'\n')

if __name__ == '__main__':
    update_target_file = sys.argv[1]

    data_spool_dir = '../build/data'
    dir_, basename = os.path.split(update_target_file)
    wfd, tmp_fname = tempfile.mkstemp('.tmp', basename + '', dir_)
    try:
        update_md5(wfd, update_target_file, data_spool_dir)
    except:
        try:
            os.close(wfd)
        except (KeyboardInterrupt, SystemExit):
            raise
        except:
            pass
        os.unlink(tmp_fname)
        raise

    os.close(wfd)
    backup_file = update_target_file + '.backup'
    if os.path.exists(backup_file):
        os.unlink(backup_file)
    os.rename(update_target_file, backup_file)
    os.rename(tmp_fname, update_target_file)
