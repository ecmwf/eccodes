#include "Message.h"



void Message::parseSections() {
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


void Grib::scanMessages() {
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
