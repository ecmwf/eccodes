#!/bin/env python

"""
Unit tests for high level Python interface for BUFR.

Author: Daniel Lee, DWD, 2016
"""

import os
from tempfile import NamedTemporaryFile
import unittest

from eccodes import BufrFile, BufrMessage

TESTBUFR = "../../data/bufr/syno_multi.bufr"
TEST_OUTPUT = "test-output.codes"

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
        self.assertEqual(len(bufr_file.open_messages), 0)

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
            self.assertEqual(len(msg_keys), 202)
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
            with open(TEST_OUTPUT, "wb") as test:
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
            with open(TEST_OUTPUT, 'wb') as test:
                bufr.write(test)
            os.unlink(TEST_OUTPUT)


if __name__ == "__main__":
    unittest.main()
