#!/bin/env python

"""
Unit tests for high level Python interface for GRIB.

Author: Daniel Lee, DWD, 2016
"""

import os
from tempfile import NamedTemporaryFile
import unittest

from eccodes import GribFile
from eccodes import GribIndex
from eccodes import GribMessage
from eccodes.high_level.gribmessage import IndexNotSelectedError

TESTGRIB = "../../data/high_level_api.grib2"
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
KNOWN_GRIB_KEYS = ['7777', 'GRIBEditionNumber', 'N', 'NV',
                   'Ni', 'Nj', 'PLPresent', 'PVPresent', 'addEmptySection2',
                   'addExtraLocalSection', 'alternativeRowScanning',
                   'angleDivisor', 'angleSubdivisions',
                   'average', 'backgroundProcess', 'basicAngleOfTheInitialProductionDomain',
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
                   'latitudeOfFirstGridPoint', 'latitudeOfFirstGridPointInDegrees',
                   'latitudeOfLastGridPoint', 'latitudeOfLastGridPointInDegrees', 'latitudes',
                   'legBaseDate', 'legBaseTime', 'legNumber',
                   'lengthOfHeaders', 'level', 'localDefinitionNumber',
                   'localDir', 'localTablesVersion',
                   'longitudeOfFirstGridPoint', 'longitudeOfFirstGridPointInDegrees',
                   'longitudeOfLastGridPoint', 'longitudeOfLastGridPointInDegrees', 'longitudes',
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
                   'resolutionAndComponentFlags', 'resolutionAndComponentFlags1',
                   'resolutionAndComponentFlags2', 'resolutionAndComponentFlags6',
                   'resolutionAndComponentFlags7', 'resolutionAndComponentFlags8',
                   'scaleFactorOfEarthMajorAxis', 'scaleFactorOfEarthMinorAxis',
                   'scaleFactorOfFirstFixedSurface', 'scaleFactorOfRadiusOfSphericalEarth',
                   'scaleFactorOfSecondFixedSurface', 'scaleValuesBy',
                   'scaledValueOfEarthMajorAxis', 'scaledValueOfEarthMinorAxis',
                   'scaledValueOfFirstFixedSurface', 'scaledValueOfRadiusOfSphericalEarth',
                   'scaledValueOfSecondFixedSurface', 'scanningMode',
                   'scanningMode5', 'scanningMode6', 'scanningMode7',
                   'scanningMode8', 'second', 'section0Length',
                   'section1Length', 'section2Length', 'section2Padding',
                   'section3Length', 'section3Padding', 'section4Length',
                   'section5Length', 'section6Length', 'section7Length',
                   'section8Length', 'sectionNumber', 'selectStepTemplateInstant', 'selectStepTemplateInterval',
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
                   'values', 'year']


class TestGribFile(unittest.TestCase):
    """Test GribFile functionality."""

    def test_memory_management(self):
        """Messages in GribFile can be opened and closed properly."""
        with GribFile(TESTGRIB) as grib_file:
            self.assertEqual(len(grib_file), 5)
            for i in range(len(grib_file)):
                msg = GribMessage(grib_file)
                self.assertEqual(msg["shortName"], "msl")
                self.assertEqual(msg['count'], i + 1)
            self.assertEqual(len(grib_file.open_messages), 5)
        self.assertEqual(len(grib_file.open_messages), 0)

    def test_message_counting_works(self):
        """The GribFile is aware of its messages."""
        with GribFile(TESTGRIB) as grib_file:
            msg_count = len(grib_file)
        self.assertEqual(msg_count, 5)

    def test_iterator_protocol(self):
        """The GribFile allows pythonic iteration over all messages."""
        step_ranges = []
        with GribFile(TESTGRIB) as grib_file:
            for msg in grib_file:
                step_ranges.append(msg["stepRange"])
        self.assertSequenceEqual(step_ranges, ["0", "6", "12", "18", "24"])

    def test_read_past_last_message(self):
        """Trying to open message on exhausted GRIB file raises IOError."""
        with GribFile(TESTGRIB) as grib_file:
            for _ in range(len(grib_file)):
                GribMessage(grib_file)
            self.assertRaises(IOError, lambda: GribMessage(grib_file))

    def test_read_invalid_file(self):
        """Trying to open message on nonexistent GRIB file raises IOError."""
        with NamedTemporaryFile(mode='r') as f:
            with GribFile(f.name) as grib_file:
                self.assertRaises(IOError, lambda: GribMessage(grib_file))


class TestGribMessage(unittest.TestCase):
    """Test GribMessage functionality."""

    def test_metadata(self):
        """Metadata is read correctly from GribMessage."""
        with GribFile(TESTGRIB) as grib_file:
            msg = GribMessage(grib_file)
            msg_keys = list(msg.keys())
            for key in KNOWN_GRIB_KEYS:
                assert key in msg_keys, "key '%s' not found" % key
            # Size of message in bytes
            self.assertEqual(msg.size(), 160219)
            self.assertEqual(len(list(msg.keys())), len(msg))

    def test_missing_message_behaviour(self):
        """Key with MISSING value."""
        with GribFile(TESTGRIB) as grib_file:
            msg = GribMessage(grib_file)
            self.assertTrue(msg.missing("scaleFactorOfSecondFixedSurface"))
            msg["scaleFactorOfSecondFixedSurface"] = 5
            msg.set_missing("scaleFactorOfSecondFixedSurface")
            # with self.assertRaises(KeyError):
            #    msg["scaleFactorOfSecondFixedSurface"]

    def test_value_setting(self):
        """Keys can be set properly."""
        with GribFile(TESTGRIB) as grib_file:
            msg = GribMessage(grib_file)
            msg["scaleFactorOfSecondFixedSurface"] = 5
            msg["values"] = [1, 2, 3]
            self.assertEqual(msg['scaleFactorOfSecondFixedSurface'], 5)

    def test_multi_value_setting(self):
        """Multiple keys/values can be set properly."""
        msg = GribMessage(sample='GRIB1')
        msg['paramId', 'stepType', 'edition'] = 49, 'avg', 2
        self.assertEqual(msg['shortName'], '10fg')
        # Another test
        with GribFile(TESTGRIB) as grib_file:
            msg = GribMessage(grib_file)
            msg['setLocalDefinition', 'localDefinitionNumber'] = 1, 25
            msg['typeOfFirstFixedSurface', 'typeOfSecondFixedSurface'] = 1, 8
            msg[('typeOfFirstFixedSurface', 'typeOfSecondFixedSurface')] = (1, 8)  # Also works
            self.assertEqual(msg['localDefinitionNumber'], 25)
            self.assertEqual(msg['typeOfLevel'], 'entireAtmosphere')

    def test_serialize(self):
        """Message can be serialized to file."""
        with GribFile(TESTGRIB) as grib_file:
            msg = GribMessage(grib_file)
            with open(TEST_OUTPUT, "wb") as test:
                msg.write(test)
        os.unlink(TEST_OUTPUT)

    def test_clone(self):
        """Messages can be used to produce clone Messages."""
        with GribFile(TESTGRIB) as grib_file:
            msg = GribMessage(grib_file)
            msg2 = GribMessage(clone=msg)
            self.assertSequenceEqual(list(msg.keys()), list(msg2.keys()))


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

    def test_index_comprehension(self):
        """GribIndex understands underlying GRIB index properly."""
        with GribIndex(TESTGRIB, TEST_KEYS) as idx:
            self.assertEqual(idx.size(TEST_KEYS[1]), 5)
            self.assertSequenceEqual(idx.values(TEST_KEYS[1]), TEST_STEPRANGE)
            with self.assertRaises(IndexNotSelectedError):
                # Note: The following will issue a message to stderr:
                #   ECCODES ERROR   :  please select a value for index key "dataDate"
                # This is expected behaviour
                idx.select({TEST_KEYS[1]: TEST_VALUES[0]})
            # Now it will be OK as we have selected all necessary keys
            idx.select(SELECTION_DICTIONARY)
            self.assertEqual(len(idx.open_messages), 1)




if __name__ == "__main__":
    unittest.main()
