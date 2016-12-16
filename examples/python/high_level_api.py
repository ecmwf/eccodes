#!/bin/env python

"""
Unit tests for high level Python interface.

Author: Daniel Lee, DWD, 2016
"""

import os
from tempfile import NamedTemporaryFile
import unittest

from eccodes import GribFile
from eccodes import GribIndex
from eccodes import GribMessage
from eccodes.high_level.gribmessage import IndexNotSelectedError
from eccodes import BufrFile, BufrMessage

TESTGRIB = "../../data/high_level_api.grib2"
TESTBUFR = "../../data/bufr/syno_multi.bufr"
TEST_OUTPUT = "test-output.codes"
TEST_INDEX = "test.index"
TEST_KEYS = ("dataDate", "stepRange")
TEST_VALUES = 20110225, 0
SELECTION_DICTIONARY = {}
for i1 in range(len(TEST_KEYS)):
    SELECTION_DICTIONARY[TEST_KEYS[i1]] = TEST_VALUES[i1]
TEST_INDEX_OUTPUT = TESTGRIB
TEST_STEPRANGE = ('0', '12', '18', '24', '6')
# These keys should be available even if new keys are defined
KNOWN_GRIB_KEYS = ['7777', 'EPS information', 'GRIBEditionNumber', 'N', 'NV',
                   'Ni', 'Nj', 'PLPresent', 'PVPresent',
                   'Parameter information', 'addEmptySection2',
                   'addExtraLocalSection', 'alternativeRowScanning',
                   'angleDivisor', 'angleMultiplier', 'angularPrecision',
                   'average', 'backgroundProcess',
                   'basicAngleOfTheInitialProductionDomain',
                   'binaryScaleFactor', 'bitMapIndicator', 'bitmapPresent',
                   'bitsPerValue', 'bottomLevel', 'centre',
                   'centreDescription', 'cfName', 'cfNameECMF', 'cfVarName',
                   'cfVarNameECMF', 'changeDecimalPrecision', 'class',
                   'climateDateFrom', 'climateDateTo', 'codedValues',
                   'dataDate', 'dataRepresentationTemplateNumber', 'dataTime',
                   'day', 'decimalPrecision', 'decimalScaleFactor',
                   'deleteCalendarId', 'deleteExtraLocalSection', 'deletePV',
                   'discipline', 'distinctLatitudes', 'distinctLongitudes',
                   'editionNumber', 'endStep', 'eps',
                   'experimentVersionNumber', 'extraLocalSectionPresent',
                   'forecastTime', 'g2grid', 'gaussianGridName',
                   'genVertHeightCoords', 'generatingProcessIdentifier',
                   'getNumberOfValues', 'global', 'globalDomain',
                   'grib 2 Section 5 DATA REPRESENTATION SECTION',
                   'grib 2 Section 6 BIT-MAP SECTION', 'grib 2 Section 7 data',
                   'grib2LocalSectionNumber', 'grib2LocalSectionPresent',
                   'grib2divider', 'gridDefinitionDescription',
                   'gridDefinitionTemplateNumber',
                   'gridDescriptionSectionPresent', 'gridType', 'hour',
                   'hoursAfterDataCutoff', 'iDirectionIncrement',
                   'iDirectionIncrementGiven', 'iDirectionIncrementInDegrees',
                   'iScansNegatively', 'iScansPositively', 'identifier',
                   'ieeeFloats', 'ifsParam', 'ijDirectionIncrementGiven',
                   'indicatorOfUnitOfTimeRange',
                   'interpretationOfNumberOfPoints', 'isConstant',
                   'isHindcast', 'isOctahedral', 'is_uerra',
                   'jDirectionIncrementGiven', 'jPointsAreConsecutive',
                   'jScansPositively', 'julianDay', 'kurtosis', 'latLonValues',
                   'latitudeOfFirstGridPoint',
                   'latitudeOfFirstGridPointInDegrees',
                   'latitudeOfLastGridPoint',
                   'latitudeOfLastGridPointInDegrees', 'latitudes',
                   'legBaseDate', 'legBaseTime', 'legNumber',
                   'lengthOfHeaders', 'level', 'localDefinitionNumber',
                   'localDir', 'localTablesVersion',
                   'longitudeOfFirstGridPoint',
                   'longitudeOfFirstGridPointInDegrees',
                   'longitudeOfLastGridPoint',
                   'longitudeOfLastGridPointInDegrees', 'longitudes',
                   'mAngleMultiplier', 'mBasicAngle', 'marsClass',
                   'marsStream', 'marsType', 'masterDir', 'maximum',
                   'md5Headers', 'md5Section1', 'md5Section3', 'md5Section4',
                   'md5Section5', 'md5Section6', 'md5Section7', 'minimum',
                   'minute', 'minutesAfterDataCutoff', 'missingValue',
                   'modelName', 'month', 'name', 'nameECMF',
                   'nameOfFirstFixedSurface', 'nameOfSecondFixedSurface',
                   'neitherPresent', 'numberOfDataPoints',
                   'numberOfForecastsInEnsemble', 'numberOfMissing',
                   'numberOfOctectsForNumberOfPoints', 'numberOfSection',
                   'numberOfValues', 'oceanAtmosphereCoupling',
                   'offsetValuesBy', 'optimizeScaleFactor', 'packingError',
                   'packingType', 'paramId', 'paramIdECMF',
                   'parameterCategory', 'parameterName', 'parameterNumber',
                   'parameterUnits', 'perturbationNumber', 'pressureUnits',
                   'productDefinitionTemplateNumber',
                   'productDefinitionTemplateNumberInternal', 'productType',
                   'productionStatusOfProcessedData', 'radius',
                   'referenceDate', 'referenceValue', 'referenceValueError',
                   'resolutionAndComponentFlags',
                   'resolutionAndComponentFlags1',
                   'resolutionAndComponentFlags2',
                   'resolutionAndComponentFlags6',
                   'resolutionAndComponentFlags7',
                   'resolutionAndComponentFlags8',
                   'scaleFactorOfEarthMajorAxis',
                   'scaleFactorOfEarthMinorAxis',
                   'scaleFactorOfFirstFixedSurface',
                   'scaleFactorOfRadiusOfSphericalEarth',
                   'scaleFactorOfSecondFixedSurface', 'scaleValuesBy',
                   'scaledValueOfEarthMajorAxis',
                   'scaledValueOfEarthMinorAxis',
                   'scaledValueOfFirstFixedSurface',
                   'scaledValueOfRadiusOfSphericalEarth',
                   'scaledValueOfSecondFixedSurface', 'scanningMode',
                   'scanningMode5', 'scanningMode6', 'scanningMode7',
                   'scanningMode8', 'second', 'section0Length',
                   'section1Length', 'section2Length', 'section2Padding',
                   'section3Length', 'section3Padding', 'section4Length',
                   'section5Length', 'section6Length', 'section7Length',
                   'section8Length', 'sectionNumber',
                   'selectStepTemplateInstant', 'selectStepTemplateInterval',
                   'setBitsPerValue', 'setCalendarId', 'shapeOfTheEarth',
                   'shortName', 'shortNameECMF', 'significanceOfReferenceTime',
                   'skewness', 'sourceOfGridDefinition', 'standardDeviation',
                   'startStep', 'stepRange', 'stepType', 'stepTypeInternal',
                   'stepUnits', 'stream', 'subCentre',
                   'subdivisionsOfBasicAngle', 'tablesVersion',
                   'tablesVersionLatest', 'tempPressureUnits', 'topLevel',
                   'totalLength', 'type', 'typeOfEnsembleForecast',
                   'typeOfFirstFixedSurface', 'typeOfGeneratingProcess',
                   'typeOfLevel', 'typeOfOriginalFieldValues',
                   'typeOfProcessedData', 'typeOfSecondFixedSurface', 'units',
                   'unitsECMF', 'unitsOfFirstFixedSurface',
                   'unitsOfSecondFixedSurface', 'unpackedError',
                   'uvRelativeToGrid', 'validityDate', 'validityTime',
                   'values', 'x', 'year']
KNOWN_BUFR_KEYS = ['3HourPressureChange', '7777', 'BUFRstr',
                   'airTemperatureAt2M', 'blockNumber', 'bufrHeaderCentre',
                   'bufrHeaderSubCentre', 'bufrTemplate',
                   'bufrdcExpandedDescriptors', 'centre',
                   'characteristicOfPressureTendency', 'cloudAmount',
                   'cloudCoverTotal', 'cloudType', 'compressedData',
                   'corr1Data', 'corr2Data', 'corr3Data', 'corr4Data',
                   'correction1', 'correction1Part', 'correction2',
                   'correction2Part', 'correction3', 'correction3Part',
                   'correction4', 'correction4Part', 'createNewData',
                   'dataCategory', 'dataPresentIndicator', 'dataSubCategory',
                   'day', 'defaultSequence', 'dewpointTemperatureAt2M', 'ed',
                   'edition', 'expandedAbbreviations', 'expandedCodes',
                   'expandedCrex_scales', 'expandedCrex_units',
                   'expandedCrex_widths', 'expandedNames',
                   'expandedOriginalCodes', 'expandedOriginalReferences',
                   'expandedOriginalScales', 'expandedOriginalWidths',
                   'expandedTypes', 'expandedUnits', 'generatingApplication',
                   'globalDomain', 'heightOfBaseOfCloud', 'heightOfStation',
                   'horizontalVisibility', 'hour', 'isSatellite',
                   'isSatelliteType', 'latitude', 'lengthDescriptors',
                   'localDay', 'localHour', 'localLatitude', 'localLongitude',
                   'localMinute', 'localMonth', 'localSecond',
                   'localSectionPresent', 'localTablesVersionNumber',
                   'localYear', 'longitude', 'masterTableNumber',
                   'masterTablesVersionNumber', 'md5Data', 'md5Structure',
                   'messageLength', 'minute', 'month', 'nonCoordinatePressure',
                   'numberOfSubsets', 'numberOfUnexpandedDescriptors',
                   'observedData', 'operator', 'pastWeather1', 'pastWeather2',
                   'presentWeather', 'pressureReducedToMeanSeaLevel',
                   'qualityControl', 'rdbSubtype', 'rdbType', 'rdbtime',
                   'rdbtimeDay', 'rdbtimeHour', 'rdbtimeMinute',
                   'rdbtimeSecond', 'rectime', 'rectimeDay', 'rectimeHour',
                   'rectimeMinute', 'rectimeSecond', 'relativeHumidity',
                   'reservedSection2', 'reservedSection3', 'section1Length',
                   'section1Padding', 'section2Length', 'section2Padding',
                   'section3Flags', 'section3Length', 'section3Padding',
                   'section4Length', 'section4Padding', 'section5Length',
                   'sequences', 'spare', 'spare1', 'stationNumber',
                   'stationType', 'subsetNumber', 'tableNumber',
                   'templatesLocalDir', 'templatesMasterDir', 'totalLength',
                   'totalPrecipitationPast6Hours', 'totalSnowDepth',
                   'typicalCentury', 'typicalDate', 'typicalDay',
                   'typicalHour', 'typicalMinute', 'typicalMonth',
                   'typicalSecond', 'typicalTime', 'typicalYear',
                   'typicalYearOfCentury', 'unexpandedDescriptors',
                   'updateSequenceNumber',
                   'verticalSignificanceSurfaceObservations',
                   'windDirectionAt10M', 'windSpeedAt10M', 'year']


class TestGribFile(unittest.TestCase):

    """Test GribFile functionality."""

    def test_memory_management(self):
        """Messages in GribFile can be opened and closed properly."""
        with GribFile(TESTGRIB) as grib:
            self.assertEqual(len(grib), 5)
            for i in range(len(grib)):
                msg = GribMessage(grib)
                self.assertEqual(msg["shortName"], "msl")
            self.assertEqual(len(grib.open_messages), 5)
        self.assertEqual(len(grib.open_messages), 0)

    def test_message_counting_works(self):
        """The GribFile is aware of its messages."""
        with GribFile(TESTGRIB) as grib:
            msg_count = len(grib)
        self.assertEqual(msg_count, 5)

    def test_iterator_protocol(self):
        """The GribFile allows pythonic iteration over all messages."""
        step_ranges = []
        with GribFile(TESTGRIB) as grib:
            for msg in grib:
                step_ranges.append(msg["stepRange"])
        self.assertSequenceEqual(step_ranges, ["0", "6", "12", "18", "24"])

    def test_read_past_last_message(self):
        """Trying to open message on exhausted GRIB file raises IOError."""
        with GribFile(TESTGRIB) as grib:
            for _ in range(len(grib)):
                GribMessage(grib)
            self.assertRaises(IOError, lambda: GribMessage(grib))

    def test_read_invalid_file(self):
        """Trying to open message on nonexistent GRIB file raises IOError."""
        with NamedTemporaryFile(mode='r') as f:
            with GribFile(f.name) as grib:
                self.assertRaises(IOError, lambda: GribMessage(grib))


class TestGribMessage(unittest.TestCase):

    """Test GribMessage functionality."""

    def test_metadata(self):
        """Metadata is read correctly from GribMessage."""
        with GribFile(TESTGRIB) as grib:
            msg = GribMessage(grib)
            for key in KNOWN_GRIB_KEYS:
                assert key in msg.keys()
            self.assertEqual(msg.size(), 160219)
            self.assertEqual(len(msg.keys()), len(msg))

    def test_missing_message_behavior(self):
        """Missing messages are detected properly."""
        with GribFile(TESTGRIB) as grib:
            msg = GribMessage(grib)
            self.assertTrue(msg.missing("scaleFactorOfSecondFixedSurface"))
            msg["scaleFactorOfSecondFixedSurface"] = 5
            msg.set_missing("scaleFactorOfSecondFixedSurface")
            with self.assertRaises(KeyError):
                msg["scaleFactorOfSecondFixedSurface"]

    def test_value_setting(self):
        """Keys can be set properly."""
        with GribFile(TESTGRIB) as grib:
            msg = GribMessage(grib)
            msg["scaleFactorOfSecondFixedSurface"] = 5
            msg["values"] = [1, 2, 3]

    def test_serialize(self):
        """Message can be serialized to file."""
        with GribFile(TESTGRIB) as grib:
            msg = GribMessage(grib)
            with open(TEST_OUTPUT, "w") as test:
                msg.write(test)
        os.unlink(TEST_OUTPUT)

    def test_clone(self):
        """Messages can be used to produce clone Messages."""
        with GribFile(TESTGRIB) as grib:
            msg = GribMessage(grib)
            msg2 = GribMessage(clone=msg)
            self.assertSequenceEqual(msg.keys(), msg2.keys())


class TestGribIndex(unittest.TestCase):

    """Test GribIndex functionality."""

    def test_memory_management(self):
        """GribIndex closes GribMessages properly."""
        with GribIndex(TESTGRIB, TEST_KEYS) as idx:
            idx.select(SELECTION_DICTIONARY)
            self.assertEqual(len(idx.open_messages), 1)
        self.assertEqual(len(idx.open_messages), 0)

    def test_create_and_serialize_index(self):
        """GribIndex can be saved to file, file can be added to index."""
        with GribIndex(TESTGRIB, TEST_KEYS) as idx:
            idx.write(TEST_INDEX)
        with GribIndex(file_index=TEST_INDEX) as idx:
            idx.add(TESTGRIB)
        os.unlink(TEST_INDEX)

    # TODO: Following test disabled due to error message:
    #   GRIB_API ERROR: please select a value for index key "dataDate"
    # Must investigate
    #
    def _test_index_comprehension(self):
        """GribIndex understands underlying GRIB index properly."""
        with GribIndex(TESTGRIB, TEST_KEYS) as idx:
            self.assertEqual(idx.size(TEST_KEYS[1]), 5)
            self.assertSequenceEqual(idx.values(TEST_KEYS[1]), TEST_STEPRANGE)
            with self.assertRaises(IndexNotSelectedError):
                idx.select({TEST_KEYS[1]: TEST_VALUES[0]})
            idx.select(SELECTION_DICTIONARY)
            self.assertEqual(len(idx.open_messages), 1)


class TestBufrFile(unittest.TestCase):

    """Test BufrFile functionality."""

    def test_memory_management(self):
        """Messages in BufrFile can be opened and closed properly."""
        with BufrFile(TESTBUFR) as bufr:
            self.assertEqual(len(bufr), 3)
            for i in range(len(bufr)):
                msg = BufrMessage(bufr)
                self.assertEqual(msg["bufrHeaderCentre"], 98)
            self.assertEqual(len(bufr.open_messages), 3)
        self.assertEquals(len(bufr.open_messages), 0)

    def test_message_counting_works(self):
        """The BufrFile is aware of its messages."""
        with BufrFile(TESTBUFR) as bufr:
            msg_count = len(bufr)
        self.assertEqual(msg_count, 3)

    def test_iterator_protocol(self):
        """The BufrFile allows pythonic iteration over all messages."""
        latitudes = []
        with BufrFile(TESTBUFR) as bufr:
            for msg in bufr:
                latitudes.append(msg["localLatitude"])
        self.assertSequenceEqual(latitudes, [70.93, 77, 78.92])

    def test_read_past_last_message(self):
        """Trying to open message on exhausted BUFR file raises IOError."""
        with BufrFile(TESTBUFR) as bufr:
            for _ in range(len(bufr)):
                BufrMessage(bufr)
            self.assertRaises(IOError, lambda: BufrMessage(bufr))

    def test_read_invalid_file(self):
        """Trying to open message on nonexistent GRIB file raises IOError."""
        with NamedTemporaryFile(mode='r') as f:
            with BufrFile(f.name) as bufr:
                self.assertRaises(IOError, lambda: BufrMessage(bufr))


class TestBufrMessage(unittest.TestCase):

    """Test BufrMessage functionality"""

    def test_metadata(self):
        """Metadata is read correctly from BufrMessage."""
        with BufrFile(TESTBUFR) as bufr:
            msg = BufrMessage(bufr)
            for key in KNOWN_BUFR_KEYS:
                assert key in msg.keys()
            self.assertEqual(msg.size(), 220)
            self.assertEqual(len(msg.keys()), len(msg))

    def test_content(self):
        """Data values are read correctly from BufrMessage."""
        with BufrFile(TESTBUFR) as bufr:
            msg = BufrMessage(bufr)
            self.assertEqual(msg["airTemperatureAt2M"], 274.5)

    # TODO: Test behavior with missing messages (SUP-1874)

    # This fails due to SUP-1875
    def test_value_setting(self):
        """Keys can be set properly."""
        with BufrFile(TESTBUFR) as bufr:
            msg = BufrMessage(bufr)
            key, val = "localLongitude", 5
            msg[key] = val
            self.assertEqual(msg[key], val)

    def test_serialize(self):
        """Message can be serialized to file."""
        with BufrFile(TESTBUFR) as bufr:
            msg = BufrMessage(bufr)
            with open(TEST_OUTPUT, "w") as test:
                msg.write(test)
        os.unlink(TEST_OUTPUT)

    def test_clone(self):
        """Messages can be used to produce clone Messages."""
        with BufrFile(TESTBUFR) as bufr:
            msg = BufrMessage(bufr)
            msg2 = BufrMessage(clone=msg)
            self.assertSequenceEqual(msg.keys(), msg2.keys())



if __name__ == "__main__":
    unittest.main()
