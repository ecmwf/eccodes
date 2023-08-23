import traceback
import sys
import eccodes as ec
import numpy as np
 
OUTPUT = 'p_out.grib_samples.grib'
VERBOSE = 1  # verbose error reporting
 
def example():
    level_values = [0,10,25,35,45,55,65,75,85,95,105,115,125,135,145,155,165,175,185,195,213,238,263,288,313,338,363,388,413,438,463,488,525,575,625,675,725,775,825,875,925,975,1050,1150,1250,1350,1450,1550,1650,1750,1850,1950,2050,2150,2250,2350,2450,2550,2650,2750,2850,2950,3050,3150,3250,3350,3450,3550,3650,3750,3850,3950,4050,4150,4250,4350,4450,4550,4650,4750,4850,4950,5050,5150,5250,5350,5450,5550,5650,5750,5850,5950,6050,6150,6250,6350,6450,6550,6650,6750,6850,6950,7050,7150,7250,7350,7450,7550,7650,7750,7850,7950,8050,8150,8250,8350,8450,8550,8650,8750,8850,8950,9050,9150,9250,9350,9450,9550,9650,9750,9850,9950,10050,10150,10250,10350,10450,10550,10650,10750,10850,10950,11050,11150,11250,11350,11450,11550,11650,11750,11850,11950,12050,12150,12250,12350,12450,12550,12650,12750,12850,12950,13050,13150,13250,13350,13450,13550,13650,13750,13850,13950,14050,14150,14250,14350,14450,14550,14650,14750,14850,14950,15050,15150,15250,15350,15450,15550,15650,15750,15850,15950,16050,16150,16250,16350,16450,16550,16650,16750,16850,16950,17050,17150,17250,17350,17450,17550,17650,17750,17850,17950,18100,18300,18500,18700,18900,19100,19300,19500,19700,19900,20100,20300,20500,20700,20900,21100,21300,21500,21700,21900,22100,22300,22500,22700,22900,23100,23300,23500,23700,23900,24100,24300,24500,24700,24900,25100,25300,25500,26000]
    decimal_scale_factor = 2
    bits_per_value = 8
    missing_value = 9999.
    fin = open('Z__C_RJTD_20170807000000_RDR_JMAGPV_Ggis1km_Prr10lv_ANAL_grib2.bin', 'rb')
    gid = ec.codes_grib_new_from_file(fin)
    if gid is None:
        return
    curr_vals = ec.codes_get_values(gid)
    max_level_value = 0
    curr_levels = np.where(curr_vals == missing_value, np.nan, curr_vals)
    max_level = round(pow(10., decimal_scale_factor) * np.nanmax(curr_levels))
    for i, level_value in enumerate(level_values):
        if level_value == max_level:
            max_level_value = i + 1
            break
    keys = {
        'discipline': 0,
        'editionNumber': 2,
        'centre': 34,
        'tablesVersion': '2',
        'subCentre': 0,
        'significanceOfReferenceTime': 0,
        'dataDate': 20220801,
        'dataTime':  000,
        'productionStatusOfProcessedData': 0,
        'typeOfProcessedData': 0,
        'numberOfDataPoints': 8601600,
        'interpretationOfNumberOfPoints': 0,
        'gridDefinitionTemplateNumber': 0,
        'shapeOfTheEarth': 4,
        'Ni': 2560,
        'Nj': 3360,
        'iScansNegatively': 0,
        'jScansPositively': 0,
        'jPointsAreConsecutive': 0,
        'alternativeRowScanning': 0,
        'latitudeOfFirstGridPointInDegrees': 47.995833,
        'longitudeOfFirstGridPointInDegrees': 118.006250,
        'latitudeOfLastGridPointInDegrees': 20.004167,
        'longitudeOfLastGridPointInDegrees': 149.993750,
        'iDirectionIncrementInDegrees': 0.012500,
        'jDirectionIncrementInDegrees': 0.008333,
        'gridType': 'regular_ll',
        'productDefinitionTemplateNumber': 50008,
        'parameterCategory': 1,
        'parameterNumber': 201,
        'typeOfGeneratingProcess': 0,
        'generatingProcessIdentifier': 255,
        'indicatorOfUnitOfTimeRange': 0,
        'forecastTime': -10,
        'typeOfFirstFixedSurface': 1,
        'scaleFactorOfFirstFixedSurface': 'MISSING',
        'scaledValueOfFirstFixedSurface': 'MISSING',
        'typeOfSecondFixedSurface': 255,
        'scaleFactorOfSecondFixedSurface': 'MISSING',
        'scaledValueOfSecondFixedSurface': 'MISSING',
        'numberOfValues': len(curr_vals),
        'packingType': 'grid_run_length',
        'bitsPerValue': bits_per_value,
        'maxLevelValue': max_level_value,
        'numberOfLevelValues': len(level_values),
        'decimalScaleFactor': decimal_scale_factor,
        'levelValues': level_values,
        'bitMapIndicator': 255,
        'bitmapPresent': 0
    }
    clone_id = ec.codes_clone(gid)
    fout = open(OUTPUT, 'wb')
    for key in keys:
        if keys[key] == 'MISSING':
            ec.codes_set_missing(clone_id, key)
        elif type(keys[key]) == list:
            ec.codes_set_array(clone_id, key, keys[key])
        else:
            ec.codes_set(clone_id, key, keys[key])
    ec.codes_set_values(clone_id, curr_vals)
    ec.codes_write(clone_id, fout)

    fin.close()
    fout.close()
 
def main():
    try:
        example()
    except ec.CodesInternalError as err:
        if VERBOSE:
            traceback.print_exc(file=sys.stderr)
        else:
            sys.stderr.write(err.msg + '\n')
 
        return 1
 
if __name__ == "__main__":
    sys.exit(main())
