# This program was automatically generated with bufr_dump -Epython
# and then edited to add the bitmap.
# See ECC-869

from __future__ import print_function
import traceback
import sys
from eccodes import *

OUTPUT_FILENAME = 'outfile_ecc_869_test.bufr'

def bufr_encode():
    ibufr = codes_bufr_new_from_samples('BUFR3_local')
    ivalues = (0, 0, 0, 0 ,)
    codes_set_array(ibufr, 'inputShortDelayedDescriptorReplicationFactor', ivalues)

    SIZE_OF_BMP = 41
    bitMask = [1]*SIZE_OF_BMP
    bitMask[0] = 0  # marineObservingPlatformIdentifier
    bitMask[37] = 0  # pressureReducedToMeanSeaLevel
    codes_set_array(ibufr, 'inputDataPresentIndicator', bitMask)

    codes_set(ibufr, 'edition', 3)
    codes_set(ibufr, 'masterTableNumber', 0)
    codes_set(ibufr, 'bufrHeaderSubCentre', 0)
    codes_set(ibufr, 'bufrHeaderCentre', 98)
    codes_set(ibufr, 'updateSequenceNumber', 0)
    codes_set(ibufr, 'dataCategory', 1)
    codes_set(ibufr, 'dataSubCategory', 182)
    codes_set(ibufr, 'masterTablesVersionNumber', 22)
    codes_set(ibufr, 'localTablesVersionNumber', 0)
    codes_set(ibufr, 'typicalYearOfCentury', 18)
    codes_set(ibufr, 'typicalMonth', 11)
    codes_set(ibufr, 'typicalDay', 21)
    codes_set(ibufr, 'typicalHour', 21)
    codes_set(ibufr, 'typicalMinute', 4)
    codes_set(ibufr, 'rdbType', 1)
    codes_set(ibufr, 'oldSubtype', 182)
    codes_set(ibufr, 'localYear', 2018)
    codes_set(ibufr, 'localMonth', 11)
    codes_set(ibufr, 'localDay', 21)
    codes_set(ibufr, 'localHour', 21)
    codes_set(ibufr, 'localMinute', 4)
    codes_set(ibufr, 'localSecond', 0)
    codes_set(ibufr, 'ident',' 3101544')
    codes_set(ibufr, 'rdbtimeDay', 21)
    codes_set(ibufr, 'rdbtimeHour', 21)
    codes_set(ibufr, 'rdbtimeMinute', 26)
    codes_set(ibufr, 'rdbtimeSecond', 59)
    codes_set(ibufr, 'rectimeDay', 10)
    codes_set(ibufr, 'rectimeHour', 10)
    codes_set(ibufr, 'rectimeMinute', 0)
    codes_set(ibufr, 'rectimeSecond', 0)
    codes_set(ibufr, 'correction1', 0)
    codes_set(ibufr, 'correction1Part', 0)
    codes_set(ibufr, 'correction2', 0)
    codes_set(ibufr, 'correction2Part', 0)
    codes_set(ibufr, 'correction3', 0)
    codes_set(ibufr, 'correction3Part', 0)
    codes_set(ibufr, 'correction4', 0)
    codes_set(ibufr, 'correction4Part', 0)
    codes_set(ibufr, 'qualityControl', 70)
    codes_set(ibufr, 'newSubtype', 0)
    codes_set(ibufr, 'numberOfSubsets', 1)
    codes_set(ibufr, 'localLatitude', -3.598198000000000008e+01)
    codes_set(ibufr, 'localLongitude', -4.484317000000000064e+01)
    codes_set(ibufr, 'observedData', 1)
    codes_set(ibufr, 'compressedData', 0)

    # Create the structure of the data section
    codes_set_array(ibufr, 'unexpandedDescriptors',
        (315009,
         222000, 236000,
         101000 + SIZE_OF_BMP, 31031,
         33007, 33007)
    )

    codes_set(ibufr, 'marineObservingPlatformIdentifier', 3101544)
    codes_set(ibufr, 'marineObservingPlatformIdentifier->percentConfidence', 44)
    codes_set(ibufr, 'longStationName','SIO Lagrangian Drifter Lab')
    codes_set(ibufr, 'dataBuoyType', 1)
    codes_set(ibufr, '#1#timeSignificance', 26)
    codes_set(ibufr, '#1#year', 2018)
    codes_set(ibufr, '#1#month', 11)
    codes_set(ibufr, '#1#day', 21)
    codes_set(ibufr, '#1#hour', 21)
    codes_set(ibufr, '#1#minute', 4)
    codes_set(ibufr, 'latitude', -3.598198000000000008e+01)
    codes_set(ibufr, 'longitude', -4.484317000000000064e+01)
    codes_set(ibufr, 'platformTransmitterIdNumber',' 300234065315740')
    codes_set(ibufr, 'dataCollectionLocationSystem', 8)
    codes_set(ibufr, 'directionOfMotionOfMovingObservingPlatform', CODES_MISSING_LONG)
    codes_set(ibufr, 'platformDriftSpeed', CODES_MISSING_DOUBLE)
    codes_set(ibufr, 'qualityOfBuoySatelliteTransmission', 0)
    codes_set(ibufr, 'qualityOfBuoyLocation', 0)
    codes_set(ibufr, 'locationQualityClassRangeOfRadiusOf66Confidence', 3)
    codes_set(ibufr, 'batteryVoltageLargeRange', 1.020000000000000107e+01)
    codes_set(ibufr, 'drogueType', 1)
    codes_set(ibufr, 'lagrangianDrifterDrogueStatus', 2)
    codes_set(ibufr, 'drogueDepth', 15)
    codes_set(ibufr, 'lagrangianDrifterSubmergenceTimeSubmerged', 30)
    codes_set(ibufr, '#2#timeSignificance', 25)
    codes_set(ibufr, '#2#year', 2018)
    codes_set(ibufr, '#2#month', 11)
    codes_set(ibufr, '#2#day', 21)
    codes_set(ibufr, '#2#hour', 21)
    codes_set(ibufr, '#2#minute', 4)
    codes_set_missing(ibufr, 'temperatureObservationPrecision')
    codes_set(ibufr, 'oceanographicWaterTemperature', 2.914499999999999886e+02)
    codes_set_missing(ibufr, 'methodOfSalinityOrDepthMeasurement')
    codes_set(ibufr, 'seaSurfaceSalinity', CODES_MISSING_DOUBLE)
    codes_set(ibufr, 'surfaceType', CODES_MISSING_LONG)
    codes_set(ibufr, 'iceThickness', CODES_MISSING_DOUBLE)
    codes_set(ibufr, 'nonCoordinatePressure', CODES_MISSING_DOUBLE)
    codes_set(ibufr, 'pressureReducedToMeanSeaLevel', 1.021700000000000000e+05)
    codes_set(ibufr, 'pressureReducedToMeanSeaLevel->percentConfidence', 45)

    # Encode the keys back in the data section
    codes_set(ibufr, 'pack', 1)

    outfile = open(OUTPUT_FILENAME, 'wb')
    codes_write(ibufr, outfile)
    print ("Created output BUFR file ",OUTPUT_FILENAME)
    codes_release(ibufr)


def main():
    try:
        bufr_encode()
    except CodesInternalError as err:
        traceback.print_exc(file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
