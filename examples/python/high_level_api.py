#!/bin/env python

"""
Unit tests for ``PythonicGrib``.

Author: Daniel Lee, DWD, 2014
"""

import os
from tempfile import NamedTemporaryFile
import unittest

from eccodes import GribFile
from eccodes import GribIndex
from eccodes import GribMessage
from eccodes.high_level.gribmessage import IndexNotSelectedError


TESTGRIB = "../../data/high_level_api.grib2"
TEST_OUTPUT = "test-output.grib"
TEST_INDEX = "test.index"
TEST_KEYS = ("dataDate", "stepRange")
TEST_VALUES = 20110225, 0
SELECTION_DICTIONARY = {}
for i1 in range(len(TEST_KEYS)):
    SELECTION_DICTIONARY[TEST_KEYS[i1]] = TEST_VALUES[i1]
TEST_INDEX_OUTPUT = TESTGRIB
TEST_STEPRANGE = ('0', '12', '18', '24', '6')
# These keys should be available even if new keys become available
KNOWN_GRIB_KEYS = ['globalDomain', 'GRIBEditionNumber',
                   'tablesVersionLatest', 'grib2divider', 'angularPrecision',
                   'missingValue', 'ieeeFloats', 'isHindcast',
                   'section0Length', 'identifier', 'discipline',
                   'editionNumber', 'totalLength', 'sectionNumber',
                   'section1Length', 'numberOfSection', 'centre',
                   'centreDescription', 'subCentre', 'tablesVersion',
                   'masterDir', 'localTablesVersion', 'localDir',
                   'significanceOfReferenceTime', 'year', 'month', 'day',
                   'hour', 'minute', 'second', 'dataDate', 'julianDay',
                   'dataTime', 'productionStatusOfProcessedData',
                   'typeOfProcessedData', 'md5Section1',
                   'selectStepTemplateInterval', 'selectStepTemplateInstant',
                   'stepType', 'setCalendarId', 'deleteCalendarId',
                   'is_uerra', 'sectionNumber', 'grib2LocalSectionPresent',
                   'section2Length', 'numberOfSection', 'addEmptySection2',
                   'grib2LocalSectionNumber', 'marsClass', 'marsType',
                   'marsStream', 'experimentVersionNumber', 'class', 'type',
                   'stream', 'productDefinitionTemplateNumberInternal',
                   'localDefinitionNumber', 'eps', 'oceanAtmosphereCoupling',
                   'legBaseDate', 'legBaseTime', 'legNumber',
                   'referenceDate', 'climateDateFrom', 'climateDateTo',
                   'addExtraLocalSection', 'deleteExtraLocalSection',
                   'extraLocalSectionPresent', 'section2Padding',
                   'sectionNumber', 'gridDescriptionSectionPresent',
                   'section3Length', 'numberOfSection',
                   'sourceOfGridDefinition', 'numberOfDataPoints',
                   'numberOfOctectsForNumberOfPoints',
                   'interpretationOfNumberOfPoints', 'PLPresent',
                   'gridDefinitionTemplateNumber',
                   'gridDefinitionDescription', 'shapeOfTheEarth',
                   'scaleFactorOfRadiusOfSphericalEarth',
                   'scaledValueOfRadiusOfSphericalEarth',
                   'scaleFactorOfEarthMajorAxis',
                   'scaledValueOfEarthMajorAxis',
                   'scaleFactorOfEarthMinorAxis',
                   'scaledValueOfEarthMinorAxis', 'radius', 'Ni', 'Nj',
                   'basicAngleOfTheInitialProductionDomain', 'mBasicAngle',
                   'angleMultiplier', 'mAngleMultiplier',
                   'subdivisionsOfBasicAngle', 'angleDivisor',
                   'latitudeOfFirstGridPoint', 'longitudeOfFirstGridPoint',
                   'resolutionAndComponentFlags',
                   'resolutionAndComponentFlags1',
                   'resolutionAndComponentFlags2',
                   'iDirectionIncrementGiven', 'jDirectionIncrementGiven',
                   'uvRelativeToGrid', 'resolutionAndComponentFlags6',
                   'resolutionAndComponentFlags7',
                   'resolutionAndComponentFlags8',
                   'ijDirectionIncrementGiven', 'latitudeOfLastGridPoint',
                   'longitudeOfLastGridPoint', 'iDirectionIncrement', 'N',
                   'scanningMode', 'iScansNegatively', 'jScansPositively',
                   'jPointsAreConsecutive', 'alternativeRowScanning',
                   'iScansPositively', 'scanningMode5', 'scanningMode6',
                   'scanningMode7', 'scanningMode8', 'g2grid',
                   'latitudeOfFirstGridPointInDegrees',
                   'longitudeOfFirstGridPointInDegrees',
                   'latitudeOfLastGridPointInDegrees',
                   'longitudeOfLastGridPointInDegrees',
                   'iDirectionIncrementInDegrees', 'global', 'latLonValues',
                   'latitudes', 'longitudes', 'distinctLatitudes',
                   'distinctLongitudes', 'isOctahedral', 'gaussianGridName',
                   'section3Padding', 'gridType', 'md5Section3',
                   'sectionNumber', 'section4Length', 'numberOfSection',
                   'NV', 'neitherPresent', 'productDefinitionTemplateNumber',
                   'genVertHeightCoords', 'Parameter information',
                   'parameterCategory', 'parameterNumber', 'parameterUnits',
                   'parameterName', 'typeOfGeneratingProcess',
                   'backgroundProcess', 'generatingProcessIdentifier',
                   'hoursAfterDataCutoff', 'minutesAfterDataCutoff',
                   'indicatorOfUnitOfTimeRange', 'x', 'stepUnits',
                   'forecastTime', 'startStep', 'endStep', 'stepRange',
                   'stepTypeInternal', 'validityDate', 'validityTime',
                   'typeOfFirstFixedSurface', 'unitsOfFirstFixedSurface',
                   'nameOfFirstFixedSurface',
                   'scaleFactorOfFirstFixedSurface',
                   'scaledValueOfFirstFixedSurface',
                   'typeOfSecondFixedSurface', 'unitsOfSecondFixedSurface',
                   'nameOfSecondFixedSurface',
                   'scaleFactorOfSecondFixedSurface',
                   'scaledValueOfSecondFixedSurface', 'pressureUnits',
                   'typeOfLevel', 'level', 'bottomLevel', 'topLevel',
                   'tempPressureUnits', 'EPS information',
                   'typeOfEnsembleForecast', 'perturbationNumber',
                   'numberOfForecastsInEnsemble', 'x', 'paramIdECMF',
                   'paramId', 'shortNameECMF', 'shortName', 'unitsECMF',
                   'units', 'nameECMF', 'name', 'cfNameECMF', 'cfName',
                   'cfVarNameECMF', 'cfVarName', 'modelName', 'ifsParam',
                   'PVPresent', 'deletePV', 'md5Section4', 'lengthOfHeaders',
                   'md5Headers', 'sectionNumber',
                   'grib 2 Section 5 DATA REPRESENTATION SECTION',
                   'section5Length', 'numberOfSection', 'numberOfValues',
                   'dataRepresentationTemplateNumber', 'packingType',
                   'referenceValue', 'referenceValueError',
                   'binaryScaleFactor', 'decimalScaleFactor',
                   'optimizeScaleFactor', 'bitsPerValue',
                   'typeOfOriginalFieldValues', 'md5Section5',
                   'sectionNumber', 'grib 2 Section 6 BIT-MAP SECTION',
                   'section6Length', 'numberOfSection', 'bitMapIndicator',
                   'bitmapPresent', 'md5Section6', 'sectionNumber',
                   'grib 2 Section 7 data', 'section7Length',
                   'numberOfSection', 'codedValues', 'values',
                   'packingError', 'unpackedError', 'maximum', 'minimum',
                   'average', 'numberOfMissing', 'standardDeviation',
                   'skewness', 'kurtosis', 'isConstant',
                   'changeDecimalPrecision', 'decimalPrecision',
                   'setBitsPerValue', 'getNumberOfValues', 'scaleValuesBy',
                   'offsetValuesBy', 'productType', 'md5Section7',
                   'section8Length', '7777']


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

if __name__ == "__main__":
    unittest.main()
