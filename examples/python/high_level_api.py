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

    def test_iteration_works(self):
        """The GribFile allows proper iteration over all messages."""
        step_ranges = []
        with GribFile(TESTGRIB) as grib:
            for _ in range(len(grib)):
                msg = GribMessage(grib)
                step_ranges.append(msg["stepRange"])
        self.assertSequenceEqual(step_ranges, ["0", "6", "12", "18", "24"])

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
            key_count = 253
            self.assertEqual(len(msg), key_count)
            self.assertEqual(msg.size(), 160219)
            self.assertEqual(len(msg.keys()), key_count)

    def test_missing_message_behaviour(self):
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
