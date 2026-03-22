#pragma once

#include <iostream>
#include <vector>
#include <assert.h>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <byteswap.h>
#include <array>
#include <memory>


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
    void parseSections();

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
    void scanMessages();

    std::ifstream in_file_;
    std::vector<Message> messages_;
};


uint32_t readU32BE(const uint8_t* p);
uint16_t readU16BE(const uint8_t* p);
int16_t readS16BE(const uint8_t* p);
float readFloatBE(const uint8_t* p);
