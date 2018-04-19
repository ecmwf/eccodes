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
KNOWN_GRIB_KEYS = ['7777', 'GRIBEditionNumber', 'N', 'NV',
                   'Ni', 'Nj', 'PLPresent', 'PVPresent', 'addEmptySection2',
                   'addExtraLocalSection', 'alternativeRowScanning',
                   'angleDivisor', 'angleMultiplier', 'angularPrecision',
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
KNOWN_BUFR_KEYS = ['edition', 'masterTableNumber', 'bufrHeaderSubCentre', 'bufrHeaderCentre',
                   'updateSequenceNumber', 'dataCategory', 'dataSubCategory', 'masterTablesVersionNumber',
                   'localTablesVersionNumber', 'typicalYearOfCentury', 'typicalMonth', 'typicalDay',
                   'typicalHour', 'typicalMinute', 'typicalDate', 'typicalTime', 'rdbType', 'newSubtype', 'rdbtimeDay',
                   'rdbtimeHour', 'rdbtimeMinute', 'rdbtimeSecond', 'rectimeDay', 'rectimeHour', 'rectimeMinute', 'rectimeSecond',
                   'correction1', 'correction1Part', 'correction2', 'correction2Part', 'correction3', 'correction3Part',
                   'correction4', 'correction4Part', 'qualityControl', 'numberOfSubsets', 'localLatitude',
                   'localLongitude', 'observedData', 'compressedData', 'unexpandedDescriptors', 'subsetNumber', '#1#blockNumber',
                   '#1#blockNumber->percentConfidence', '#1#stationNumber', '#1#stationNumber->percentConfidence',
                   '#1#stationType', '#1#stationType->percentConfidence', '#1#year', '#1#year->percentConfidence',
                   '#1#month', '#1#month->percentConfidence', '#1#day', '#1#day->percentConfidence', '#1#hour',
                   '#1#hour->percentConfidence', '#1#minute', '#1#minute->percentConfidence', '#1#latitude',
                   '#1#latitude->percentConfidence', '#1#longitude', '#1#longitude->percentConfidence',
                   '#1#heightOfStation', '#1#heightOfStation->percentConfidence', '#1#nonCoordinatePressure',
                   '#1#nonCoordinatePressure->percentConfidence', '#1#pressureReducedToMeanSeaLevel',
                   '#1#pressureReducedToMeanSeaLevel->percentConfidence', '#1#3HourPressureChange',
                   '#1#3HourPressureChange->percentConfidence', '#1#characteristicOfPressureTendency',
                   '#1#characteristicOfPressureTendency->percentConfidence', '#1#windDirectionAt10M',
                   '#1#windDirectionAt10M->percentConfidence', '#1#windSpeedAt10M',
                   '#1#windSpeedAt10M->percentConfidence',
                   '#1#airTemperatureAt2M', '#1#airTemperatureAt2M->percentConfidence', '#1#dewpointTemperatureAt2M',
                   '#1#dewpointTemperatureAt2M->percentConfidence', '#1#relativeHumidity',
                   '#1#relativeHumidity->percentConfidence', '#1#horizontalVisibility',
                   '#1#horizontalVisibility->percentConfidence', '#1#presentWeather',
                   '#1#presentWeather->percentConfidence', '#1#pastWeather1', '#1#pastWeather1->percentConfidence',
                   '#1#pastWeather2', '#1#pastWeather2->percentConfidence', '#1#cloudCoverTotal',
                   '#1#cloudCoverTotal->percentConfidence', '#1#verticalSignificanceSurfaceObservations',
                   '#1#verticalSignificanceSurfaceObservations->percentConfidence', '#1#cloudAmount',
                   '#1#cloudAmount->percentConfidence', '#1#heightOfBaseOfCloud',
                   '#1#heightOfBaseOfCloud->percentConfidence', '#1#cloudType', '#1#cloudType->percentConfidence',
                   '#2#cloudType', '#2#cloudType->percentConfidence', '#3#cloudType', '#3#cloudType->percentConfidence',
                   '#2#verticalSignificanceSurfaceObservations',
                   '#2#verticalSignificanceSurfaceObservations->percentConfidence',
                   '#2#cloudAmount', '#2#cloudAmount->percentConfidence', '#4#cloudType',
                   '#4#cloudType->percentConfidence', '#2#heightOfBaseOfCloud',
                   '#2#heightOfBaseOfCloud->percentConfidence',
                   '#3#verticalSignificanceSurfaceObservations',
                   '#3#verticalSignificanceSurfaceObservations->percentConfidence',
                   '#3#cloudAmount', '#3#cloudAmount->percentConfidence', '#5#cloudType',
                   '#5#cloudType->percentConfidence',
                   '#3#heightOfBaseOfCloud', '#3#heightOfBaseOfCloud->percentConfidence',
                   '#4#verticalSignificanceSurfaceObservations',
                   '#4#verticalSignificanceSurfaceObservations->percentConfidence', '#4#cloudAmount',
                   '#4#cloudAmount->percentConfidence',
                   '#6#cloudType', '#6#cloudType->percentConfidence', '#4#heightOfBaseOfCloud',
                   '#4#heightOfBaseOfCloud->percentConfidence',
                   '#5#verticalSignificanceSurfaceObservations',
                   '#5#verticalSignificanceSurfaceObservations->percentConfidence', '#5#cloudAmount',
                   '#5#cloudAmount->percentConfidence', '#7#cloudType', '#7#cloudType->percentConfidence',
                   '#5#heightOfBaseOfCloud',
                   '#5#heightOfBaseOfCloud->percentConfidence', '#1#totalPrecipitationPast6Hours',
                   '#1#totalPrecipitationPast6Hours->percentConfidence', '#1#totalSnowDepth',
                   '#1#totalSnowDepth->percentConfidence',
                   '#1#centre', '#1#generatingApplication']


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
            with open(TEST_OUTPUT, "w") as test:
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


class TestBufrFile(unittest.TestCase):
    """Test BufrFile functionality."""

    def test_memory_management(self):
        """Messages in BufrFile can be opened and closed properly."""
        with BufrFile(TESTBUFR) as bufr_file:
            self.assertEqual(len(bufr_file), 3)
            for i in range(len(bufr_file)):
                msg = BufrMessage(bufr_file)
                self.assertEqual(msg["bufrHeaderCentre"], 98)
                self.assertEqual(msg['count'], i + 1)
            self.assertEqual(len(bufr_file.open_messages), 3)
        self.assertEquals(len(bufr_file.open_messages), 0)

    def test_message_counting_works(self):
        """The BufrFile is aware of its messages."""
        with BufrFile(TESTBUFR) as bufr_file:
            msg_count = len(bufr_file)
        self.assertEqual(msg_count, 3)

    def test_iterator_protocol(self):
        """The BufrFile allows pythonic iteration over all messages."""
        latitudes = []
        with BufrFile(TESTBUFR) as bufr_file:
            for msg in bufr_file:
                latitudes.append(msg["localLatitude"])
        self.assertSequenceEqual(latitudes, [70.93, 77, 78.92])

    def test_read_past_last_message(self):
        """Trying to open message on exhausted BUFR file raises IOError."""
        with BufrFile(TESTBUFR) as bufr_file:
            for _ in range(len(bufr_file)):
                BufrMessage(bufr_file)
            self.assertRaises(IOError, lambda: BufrMessage(bufr_file))

    def test_read_invalid_file(self):
        """Trying to open message on nonexistent file raises IOError."""
        with NamedTemporaryFile(mode='r') as f:
            with BufrFile(f.name) as bufr_file:
                self.assertRaises(IOError, lambda: BufrMessage(bufr_file))


class TestBufrMessage(unittest.TestCase):
    """Test BufrMessage functionality"""

    def test_metadata(self):
        """Metadata is read correctly from BufrMessage."""
        with BufrFile(TESTBUFR) as bufr_file:
            msg = BufrMessage(bufr_file)
            msg.unpack()
            msg_keys = list(msg.keys())
            self.assertEqual(len(msg_keys), 200)
            for key in KNOWN_BUFR_KEYS:
                assert key in msg_keys
            # Size of message in bytes
            self.assertEqual(msg.size(), 220)
            self.assertEqual(len(list(msg.keys())), len(msg))

    def test_content(self):
        """Data values are read correctly from BufrMessage."""
        with BufrFile(TESTBUFR) as bufr_file:
            msg = BufrMessage(bufr_file)
            msg.unpack()
            self.assertEqual(msg["airTemperatureAt2M"], 274.5)

    # TODO: Test behaviour with missing messages (SUP-1874)

    def test_value_setting(self):
        """Keys can be set properly."""
        with BufrFile(TESTBUFR) as bufr_file:
            msg = BufrMessage(bufr_file)
            key, val = "localLongitude", 5
            msg[key] = val
            self.assertEqual(msg[key], val)

    def test_serialize(self):
        """Message can be serialized to file."""
        with BufrFile(TESTBUFR) as bufr_file:
            msg = BufrMessage(bufr_file)
            with open(TEST_OUTPUT, "w") as test:
                msg.write(test)
        os.unlink(TEST_OUTPUT)

    def test_clone(self):
        """Messages can be used to produce clone Messages."""
        with BufrFile(TESTBUFR) as bufr_file:
            msg = BufrMessage(bufr_file)
            msg2 = BufrMessage(clone=msg)
            self.assertSequenceEqual(list(msg.keys()), list(msg2.keys()))

    def test_copy_data(self):
        """Can copy data section from one message to another"""
        bufr = BufrMessage(sample='BUFR3')
        with BufrFile('../../data/bufr/metar_with_2_bias.bufr') as bufr_file:
            bufrin = BufrMessage(bufr_file)
            ivalues = (
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 0, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                1, 1, 1, 1, 1, 1, 1, 1, 0)
            bufr['inputDataPresentIndicator'] = ivalues
            bufr['edition'] = 3
            bufr['masterTableNumber'] = 0
            bufr['bufrHeaderSubCentre'] = 0
            bufr['bufrHeaderCentre'] = 98
            bufr['updateSequenceNumber'] = 1
            bufr['dataCategory'] = 0
            bufr['dataSubCategory'] = 140
            bufr['masterTablesVersionNumber'] = 13
            bufr['localTablesVersionNumber'] = 1
            bufr['typicalYearOfCentury'] = 15
            bufr['typicalMonth'] = 5
            bufr['typicalDay'] = 4
            bufr['typicalHour'] = 9
            bufr['typicalMinute'] = 30
            bufr['numberOfSubsets'] = 1
            bufr['observedData'] = 1
            bufr['compressedData'] = 0
            ivalues = (
                307011, 7006, 10004, 222000, 101023, 31031, 1031, 1032, 101023, 33007,
                225000, 236000, 101023, 31031, 1031, 1032, 8024, 101001, 225255, 225000,
                236000, 101023, 31031, 1031, 1032, 8024, 101001, 225255,
                1063, 2001, 4001, 4002, 4003, 4004, 4005, 5002,
                6002, 7001, 7006, 11001, 11016, 11017, 11002)
            bufr['unexpandedDescriptors'] = ivalues
            bufrin.unpack()
            bufrin.copy_data(bufr)
            with open(TEST_OUTPUT, 'w') as test:
                bufr.write(test)
            os.unlink(TEST_OUTPUT)


if __name__ == "__main__":
    unittest.main()
