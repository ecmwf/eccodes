:

# Script which creates directories representing the accessors and their hierarchy.
# Directories are created under $TMPDIR.
# Use a tree viewer to view the results

set -eu

ROOT=$TMPDIR/ecCodes_Accessor_Class_Hierarchy
rm -fr $ROOT; mkdir $ROOT

# Assume we're in the src directory
for f in grib_accessor_class_*.c; do
    fbase=`echo $f | sed -e 's/\.c$//'`
    super=`grep 'SUPER *= *' $f | awk '{print $3}'`
    if [ "$super" != "" ] ; then
        super_path=`find $ROOT -type d -name $super`
        if [ "$super_path" = "" ]; then
            mkdir $ROOT/$super
            mkdir $ROOT/$super/$fbase
        else
            # Parent is in tree
            fbase_path=`find $ROOT -type d -name $fbase` # do I exist?
            if [ "$fbase_path" != "" ]; then # yes, I exist
                mv $fbase_path $super_path
            else
                mkdir "$super_path/$fbase"   # no, I don't exist
            fi
        fi
    else
        if [ ! -d "$ROOT/$fbase" ]; then
            mkdir "$ROOT/$fbase"
        fi
    fi
done

echo "Done. See $ROOT"
