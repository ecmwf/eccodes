#!/bin/bash

for f in grib_accessor*; do
    echo "Processing $f"
    sed -i '/^<<<<<<</d' $f
    sed -i '/^=======/,/^>>>>>>>/d' $f
done
