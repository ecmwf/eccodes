#include <iostream>
#include <vector>
#include <assert.h>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <byteswap.h>
#include <array>
#include <memory>

#include "SpAec.h"


class GenericSection {
public:
    GenericSection() = default;
    GenericSection(uint8_t number, uint32_t length, size_t offset, std::vector<uint8_t> data = {}) : number_(number), length_(length), offset_(offset), data_(std::move(data)) {}

    uint8_t number() const { return number_; }
    uint32_t length() const { return length_; }
    size_t offset() const { return offset_; }
    const std::vector<uint8_t>& data() const { return data_; }

private:
    uint8_t number_;
    uint32_t length_;
    size_t offset_;
    std::vector<uint8_t> data_;
};


class Message {
public:
    Message(std::ifstream& in_file, size_t offset, uint64_t length)
        : in_file_(in_file), offset_(offset), length_(length) {
        parseSections();
    }

    uint64_t length() const { return length_; }
    size_t offset() const { return offset_; }
    auto getSection(uint8_t section_number) const { return sections_[section_number].get(); }

    auto begin() const { return sections_.begin(); }
    auto end() const { return sections_.end(); }

private:
    void parseSections() {
        // Section 0 (Indicator) is 16 bytes: "GRIB"(4) + reserved(2) + discipline(1) + edition(1) + length(8)
        size_t pos = offset_ + 16;
        size_t msg_end = offset_ + length_;

        sections_[0] = std::make_unique<GenericSection>(0, 16, pos);
        sections_[2] = std::make_unique<GenericSection>(2, 0, 0);

        while (pos + 4 < msg_end) {
            in_file_.seekg(pos);

            // Check for end marker "7777"
            char marker[4];
            in_file_.read(marker, 4);
            if (std::memcmp(marker, "7777", 4) == 0)
                break;

            // First 4 bytes are section length (big-endian)
            uint32_t section_length;
            std::memcpy(&section_length, marker, 4);
            section_length = bswap_32(section_length);

            uint8_t section_number;
            in_file_.read(reinterpret_cast<char*>(&section_number), sizeof(section_number));

            std::vector<uint8_t> data(section_length);
            in_file_.read(reinterpret_cast<char*>(data.data()), section_length);

            sections_[section_number] = std::make_unique<GenericSection>(section_number, section_length, pos, std::move(data));

            pos += section_length;
        }
    }

    std::ifstream& in_file_;
    size_t offset_;
    uint64_t length_;
    std::array<std::unique_ptr<GenericSection>, 8> sections_;
};


class Grib {
public:
    explicit Grib(const std::string& filename) : in_file_(filename, std::ios::binary) {
        if (!in_file_) {
            throw std::runtime_error("Failed to open file: " + filename);
        }
        scanMessages();
    }

    ~Grib() {
        if (in_file_.is_open()) {
            in_file_.close();
        }
    }

    auto begin() const { return messages_.begin(); }
    auto end() const { return messages_.end(); }
    size_t size() const { return messages_.size(); }

private:
    void scanMessages() {
        in_file_.seekg(0, std::ios::end);
        size_t file_size = in_file_.tellg();
        in_file_.seekg(0, std::ios::beg);

        while (in_file_.good() && static_cast<size_t>(in_file_.tellg()) + 16 <= file_size) {
            size_t pos = in_file_.tellg();

            char magic[4];
            in_file_.read(magic, 4);
            if (std::memcmp(magic, "GRIB", 4) != 0) {
                in_file_.seekg(pos + 1);
                continue;
            }

            // Skip reserved(2) + discipline(1) + edition(1)
            in_file_.seekg(4, std::ios::cur);

            // Read 8-byte big-endian message length
            uint64_t msg_length;
            in_file_.read(reinterpret_cast<char*>(&msg_length), sizeof(msg_length));
            msg_length = bswap_64(msg_length);

            messages_.emplace_back(in_file_, pos, msg_length);

            in_file_.seekg(pos + msg_length);
        }
    }

    std::ifstream in_file_;
    std::vector<Message> messages_;
};


uint32_t readU32BE(const uint8_t* p) {
    uint32_t v;
    std::memcpy(&v, p, 4);
    return bswap_32(v);
}

uint16_t readU16BE(const uint8_t* p) {
    uint16_t v;
    std::memcpy(&v, p, 2);
    return bswap_16(v);
}

int16_t readS16BE(const uint8_t* p) {
    // return static_cast<int16_t>(readU16BE(p));
    // return reinterpret_cast<const int16_t*>(p)[0];
    int16_t v;
    std::memcpy(&v, p, 2);
    v = bswap_16(v);
    int16_t sign = v & 0x8000;
    int16_t value = v & 0x7FFF;
    return sign ? -value : value;
}

float readFloatBE(const uint8_t* p) {
    uint32_t bits = readU32BE(p);
    float f;
    std::memcpy(&f, &bits, sizeof(f));
    return f;
}


int main(int argc, char* argv[])
{
    // std::string in_name = "/home/joobog/git/ccsds/data/ccsds2/ccsds.grib2";
    // std::string in_name = "/home/joobog/git/playground/hans_hersbach_ccsds/CCSDS/_inv_out_ccsds";
    // std::string in_name = "/home/joobog/git/playground/quick_check/6_oper_ccsds.grib";
    // std::string in_name = "/home/joobog/build/eccodes/data/ccsds.grib2";
    std::string in_name = "/home/joobog/git/ccsds/data/ust_140215_ccsds.grib";
    Grib grib(in_name);

    for (const auto& message : grib) {
        auto s5 = message.getSection(5);
        if (!s5) {
            std::cerr << "No Section 5 found" << std::endl;
            continue;
        }

        // Section 5 data starts after 5-byte header (length + section number)
        // Template 5.42 (CCSDS):
        //   data[0..3]   Number of data points (uint32 BE)
        //   data[4..5]   Template number (uint16 BE)
        //   data[6..9]   Reference value (IEEE 754 float BE)
        //   data[10..11] Binary scale factor (int16 BE)
        //   data[12..13] Decimal scale factor (int16 BE)
        //   data[14]     Bits per value (uint8)
        //   data[15]     Type of original field values (uint8)
        //   data[16]     CCSDS flags (uint8)
        //   data[17]     CCSDS block size (uint8)
        //   data[18..19] CCSDS RSI (uint16 BE)
        const auto& d5 = s5->data();

        uint32_t numDataPoints     = readU32BE(&d5[0]);
        float referenceValue       = readFloatBE(&d5[6]);
        int16_t binaryScaleFactor  = readS16BE(&d5[10]);
        int16_t decimalScaleFactor = readS16BE(&d5[12]);
        uint8_t bitsPerValue       = d5[14];
        uint8_t ccsdsFlags         = d5[16];
        uint8_t ccsdsBlockSize     = d5[17];
        uint16_t ccsdsRsi          = readU16BE(&d5[18]);

        // std::cout << "numDataPoints:      " << numDataPoints << std::endl;
        // std::cout << "referenceValue:     " << referenceValue << std::endl;
        // std::cout << "binaryScaleFactor:  " << binaryScaleFactor << std::endl;
        // std::cout << "decimalScaleFactor: " << decimalScaleFactor << std::endl;
        // std::cout << "bitsPerValue:       " << (uint32_t)bitsPerValue << std::endl;
        // std::cout << "ccsdsFlags:         " << (uint32_t)ccsdsFlags << std::endl;
        // std::cout << "ccsdsBlockSize:     " << (uint32_t)ccsdsBlockSize << std::endl;
        // std::cout << "ccsdsRsi:           " << ccsdsRsi << std::endl;


        auto s7 = message.getSection(7);
        if (!s7) {
            std::cerr << "No Section 7 found" << std::endl;
            continue;
        }
        const auto& d7 = s7->data();
        size_t encodedSize = s7->length() - 5;
        Span<uint8_t> encodedData(const_cast<uint8_t*>(d7.data()), encodedSize);
        std::vector<double> decodedData(numDataPoints);
        Span<double> decodedSpan(decodedData);
        if (bitsPerValue != 0) {
            encoder::SpAec<double> spaec(bitsPerValue, ccsdsBlockSize, ccsdsRsi, CodeOptionSet::Basic, static_cast<double>(referenceValue), binaryScaleFactor, decimalScaleFactor);
            spaec.decode(encodedData, decodedSpan);

            auto stats = spaec.getStats();
            stats.print();
            stats.printMetrics();

            // print first 10 decoded values
            // std::cout << "First 10 decoded values:" << std::endl;
            // for (size_t i = 0; i < std::min(size_t(3), decodedData.size()); ++i) {
            //     std::cout << " " << decodedData[i] << std::endl;
            // }

            // std::cout << "First 3 non-zero values:" << std::endl;
            // size_t count = 0;
            // for (size_t i = 0; i < decodedData.size() && count < 4; ++i) {
            //     if (decodedData[i] != 0.0) {
            //         std::cout << " " << decodedData[i] << std::endl;
            //         count++;
            //     }
            // }
        }
        else {
            std::cerr << "Bits per value is 0, skipping" << std::endl;
            continue;
        }


    }

    return 0;
}
