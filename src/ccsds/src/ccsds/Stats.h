#pragma once

#include <cstdint>
#include <cstddef>
#include <array>
#include "CodeOption.h"
#include <iostream>
#include <vector>
#include <algorithm>

struct ComprStats {
    size_t overhead = 0;
    size_t compressed_before = 0;
    size_t compressed_after = 0;
    size_t uncompressed = 0;
    size_t nBlocks = 0;

    virtual void print() const {
        std::cout << "  ov: " << overhead << " un: " << uncompressed << " nB: " << nBlocks << std::endl;
    }
};

struct ZbComprStats : public ComprStats {
};

struct SsComprStats : public ComprStats {
    size_t min_k = 0;
    size_t max_k = 0;
    double avg_k = 0.0;
    size_t median_k = 0;

    void print() const override {
        std::cout << "  ov: " << overhead << " un: " << uncompressed << " nB: " << nBlocks
                  << " min_k: " << min_k << " max_k: " << max_k << " avg_k: " << avg_k << " median_k: " << median_k
                  << std::endl;
    }
};

struct SeComprStats : public ComprStats {
};

struct NcComprStats : public ComprStats {
};




struct SpAecStats
{
    uint8_t bpv = 0;    
    uint8_t blockSize = 0;
    uint16_t rsiSize = 0;
    CodeOptionSet codeOptionSet = CodeOptionSet::Basic;
    int binaryScaleFactor = 0;
    int decimalScaleFactor = 0;
    float referenceValue = 0.0f;
    size_t nBlocks = 0;
    size_t nValues = 0;
    size_t compressedSize = 0;
    static constexpr size_t kMax = 30;
    static constexpr size_t segmentSize = 64;

    struct {
        std::array<size_t, kMax> ssNoRef = {0};
        std::array<size_t, kMax> ssRef = {0};
        uint64_t seNoRef = 0;
        uint64_t seRef = 0;
        uint64_t ncNoRef = 0;
        uint64_t ncRef = 0;
        std::array<size_t, segmentSize + 1> zbNoRef = {0};
        std::array<size_t, segmentSize + 1> zbRef = {0};
        std::array<size_t, segmentSize + 1> zbRosNoRef = {0};
        std::array<size_t, segmentSize + 1> zbRosRef = {0};
    } counter;

    size_t totalNumberOfBits() const {
        return nValues * bpv;
    }

    size_t overhead() {
        const ZeroBlockCodeOption zbId_{bpv, codeOptionSet};
        const NoCompressionCodeOption ncId_{bpv, codeOptionSet};
        const SplitSampleIdLength ssId_{bpv, codeOptionSet};
        const SecondExtensionCodeOption seId_{bpv, codeOptionSet};


            size_t overheadBits = 0;
            // SS
            for (size_t k = 0; k < kMax; ++k) {
                overheadBits += counter.ssRef[k] * (ssId_.length + bpv);
                overheadBits += counter.ssNoRef[k] * ssId_.length;
            }
            // SE
            overheadBits += counter.seRef * (seId_.length + bpv);
            overheadBits += counter.seNoRef * seId_.length;
            // NC
            overheadBits += counter.ncRef * (ncId_.length + blockSize);
            overheadBits += counter.ncNoRef * ncId_.length;
            // ZB
            for (size_t i = 0; i < segmentSize; ++i) {
                overheadBits += counter.zbRef[i] * (zbId_.length + bpv);
                overheadBits += counter.zbNoRef[i] * zbId_.length;
                overheadBits += counter.zbRosRef[i] * (zbId_.length + bpv);
                overheadBits += counter.zbRosNoRef[i] * (zbId_.length);
            }

            return overheadBits;
    }



    ZbComprStats zeroStats() {
        ZbComprStats stats;
        const ZeroBlockCodeOption zbId_{bpv, codeOptionSet};

        // REF
        for (size_t i = 0; i < counter.zbRef.size(); i++) {
            size_t n = counter.zbRef[i] * i;
            stats.overhead += n * zbId_.length;
            stats.overhead += n * counter.zbRef[i] * bpv;
            stats.compressed_before += n * bpv * blockSize;
            stats.compressed_after += n * (zbId_.length + bpv + i < 5 ? i : (i + 1));
            // stats.uncompressed += 0;
            stats.nBlocks += n;
        }
        // NO REF
        for (size_t i = 0; i < counter.zbNoRef.size(); i++) {
            size_t n = counter.zbNoRef[i] * i;
            stats.overhead += n * zbId_.length;
            stats.compressed_before += n * bpv * blockSize;
            stats.compressed_after += n * (zbId_.length + i < 5 ? i : (i + 1));
            // stats.uncompressed += 0;
            stats.nBlocks += n;
        }
        // ROS REF
        for (size_t i = 0; i < counter.zbRosRef.size(); i++) {
            size_t n = counter.zbRosRef[i];
            stats.overhead += n * zbId_.length;
            stats.overhead += n * counter.zbRosRef[i] * bpv;
            stats.compressed_before += n * bpv * blockSize;
            stats.compressed_after += n * (zbId_.length + bpv + 5);
            // stats.uncompressed += 0;
            stats.nBlocks += n;
        }
        // ROS NO REF
        for (size_t i = 0; i < counter.zbRosNoRef.size(); i++) {
            size_t n = counter.zbRosNoRef[i];
            stats.overhead += i * n * zbId_.length;
            stats.compressed_before += n * bpv * blockSize;
            stats.compressed_after += n * (zbId_.length + 5);
            // stats.uncompressed += 0;
            stats.nBlocks += n;
        }
        return stats;
    }

    SsComprStats ssStats() {
        SsComprStats stats;
        const SplitSampleIdLength ssId_{bpv, codeOptionSet};
        // REF
        for (size_t k = 0; k < kMax; ++k) {
            size_t n = counter.ssRef[k];
            stats.uncompressed += n * k * (blockSize - 1);
            stats.overhead += n * (ssId_.length + bpv);
            stats.nBlocks += n;
        }
        // NO REF
        for (size_t k = 0; k < kMax; ++k) {
            size_t n = counter.ssNoRef[k];
            stats.uncompressed += n * k * blockSize;
            stats.overhead += n * (ssId_.length + bpv);
            stats.nBlocks += n;
        }

        std::vector<size_t> ks(stats.nBlocks);

        size_t pos = 0;
        size_t n = 0;
        for (size_t k = 0; k < kMax; k++) {
            n = counter.ssRef[k];
            for (size_t j = 0; j < n; ++j) {
                ks[pos++] = k;
            }
            n = counter.ssNoRef[k];
            for (size_t j = 0; j < n; ++j) {
                ks[pos++] = k;
            }
        }

        // Compute min, max, avg, median of ks
        stats.min_k = kMax;
        stats.max_k = 0;

        for (size_t i = 0; i < stats.nBlocks; ++i) {
            stats.avg_k += ks[i];
            if (ks[i] < stats.min_k) {
                stats.min_k = ks[i];
            }
            if (ks[i] > stats.max_k) {
                stats.max_k = ks[i];
            }
        }
        stats.avg_k /= stats.nBlocks;
        std::sort(ks.begin(), ks.end());

        stats.median_k = ks[stats.nBlocks / 2];    

        return stats;
    }

    SeComprStats seStats() {
        SeComprStats stats;
        const SecondExtensionCodeOption seId_{bpv, codeOptionSet};
        size_t n;
        // REF
        n = counter.seRef;
        stats.overhead += n * (seId_.length + bpv);
        stats.nBlocks += n;
        // NO REF
        n = counter.seNoRef;
        stats.overhead += n * (seId_.length + bpv);
        stats.nBlocks += n;

        stats.uncompressed = 0;
        return stats;
    }


    NcComprStats ncStats() {
        NcComprStats stats;
        const NoCompressionCodeOption ncId_{bpv, codeOptionSet};
        size_t n = 0;
        // REF
        n = counter.ncRef;
        stats.uncompressed += n * bpv * blockSize;
        stats.overhead += n * (ncId_.length + bpv);
        stats.nBlocks += n;
        n = counter.ncNoRef;
        stats.uncompressed += n * bpv * blockSize;
        stats.overhead += n * (ncId_.length + bpv);
        stats.nBlocks += n;
        return stats;
    }

    void printMetrics() {
        std::cout << "Overhead percentage" << ": " << static_cast<double>(overhead()) / totalNumberOfBits() * 100.0 << "%" << std::endl;
        auto zb_stats = zeroStats();
        auto ss_stats = ssStats();
        auto se_stats = seStats();
        auto nc_stats = ncStats();

        ComprStats total_stats;
        total_stats.overhead = zb_stats.overhead + ss_stats.overhead + se_stats.overhead + nc_stats.overhead;
        total_stats.uncompressed = zb_stats.uncompressed + ss_stats.uncompressed + se_stats.uncompressed + nc_stats.uncompressed;
        total_stats.nBlocks = zb_stats.nBlocks + ss_stats.nBlocks + se_stats.nBlocks + nc_stats.nBlocks;

        auto total_size = nValues * bpv;
        auto compressed_size = compressedSize * 8; // convert bytes to bits

        zb_stats.print();
        ss_stats.print();
        se_stats.print();
        nc_stats.print();


        auto compressed_bits_before = total_size - total_stats.uncompressed;
std::cout << "Compressed size (bits): " << compressed_size << " uncompressed (bits): " << total_stats.uncompressed << " overhead (bits): " << total_stats.overhead << std::endl;
        auto compressed_bits_after = compressed_size - total_stats.uncompressed;

        std::cout << "Compressibility: " << (double)(compressed_bits_before - compressed_bits_after) / compressed_bits_before * 100.0 << "%" << " before: " << compressed_bits_before << " bits, after: " << compressed_bits_after << " bits" << std::endl;
        std::cout << "Block distribution: " << std::endl;
        size_t total_blocks = (nValues + blockSize - 1) / blockSize;
        std::cout << "  Zero blocks: " 
            << zb_stats.nBlocks << " (" << (double)zb_stats.nBlocks / total_blocks * 100.0 << "%"
            << " of data reduced to " << (double) zb_stats.compressed_after / zb_stats.compressed_before * 100.0 << "%)"
            << std::endl;
        std::cout << "  Split sample blocks: " << ss_stats.nBlocks << " (" << (double)ss_stats.nBlocks / total_blocks * 100.0 << "%)" << std::endl;
        std::cout << "  Second extension blocks: " << se_stats.nBlocks << " (" << (double)se_stats.nBlocks / total_blocks * 100.0 << "%)" << std::endl;
        std::cout << "  No compression blocks: " << nc_stats.nBlocks << " (" << (double)nc_stats.nBlocks / total_blocks * 100.0 << "%)" << std::endl;
                                                   //
        auto compression_ratio = ((double)(total_size - compressedSize * 8) / total_size) * 100.0;
        std::cout << "Compression ratio: " << compression_ratio << std::endl;
        auto uncompression_ratio = ((double)(total_stats.uncompressed) / total_size) * 100.0;
        std::cout << "Uncompression ratio: " << uncompression_ratio << std::endl;
        compression_ratio = (double) compressed_size / total_size * 100.0;
        std::cout << "Actual compression ratio: " << compression_ratio << std::endl;

        std::cout << "Check uncompressed: " << (double) total_stats.uncompressed  / total_size * 100.0 << "%" << std::endl;

        total_stats.print();
    }


    void print() {
        std::cout << "SpAecStats:" << std::endl;
        std::cout << "  bpv: " << (uint32_t)bpv << std::endl;
        std::cout << "  blockSize: " << (uint32_t)blockSize << std::endl;
        std::cout << "  rsiSize: " << rsiSize << std::endl;
        std::cout << "  codeOptionSet: " << static_cast<uint32_t>(codeOptionSet) << std::endl;
        std::cout << "  binaryScaleFactor: " << binaryScaleFactor << std::endl;
        std::cout << "  decimalScaleFactor: " << decimalScaleFactor << std::endl;
        std::cout << "  referenceValue: " << referenceValue << std::endl;
        std::cout << "  nBlocks: " << nBlocks << std::endl;
        std::cout << "  nValues: " << nValues << std::endl;

        size_t totalSSNoRef = 0;
        size_t totalSSRef = 0;
        for (size_t k = 0; k < kMax; ++k) {
            totalSSNoRef += counter.ssNoRef[k];
            totalSSRef += counter.ssRef[k];
            if (counter.ssNoRef[k] > 0 || counter.ssRef[k] > 0) {
                std::cout << "    ss k=" << k << ": " << counter.ssNoRef[k] + counter.ssRef[k] << " blocks" << std::endl;
            }
        }

        if (counter.seNoRef > 0 || counter.seRef > 0) {
            std::cout << "    se: " << counter.seNoRef + counter.seRef << std::endl;
        }

        if (counter.ncNoRef > 0 || counter.ncRef > 0) {
            std::cout << "    nc: " << counter.ncNoRef + counter.ncRef << std::endl;
        }

        for (size_t i = 0; i < segmentSize; ++i) {
            if (counter.zbNoRef[i] > 0 || counter.zbRef[i] > 0) {
                std::cout << "    zb: n=" << i << ": " << counter.zbNoRef[i] + counter.zbRef[i] << " blocks" << std::endl;
            }
        }
        for (size_t i = 0; i < segmentSize; ++i) {
            if (counter.zbRosNoRef[i] > 0 || counter.zbRosRef[i] > 0) {
                std::cout << "    zb ROS: n=" << i << ": " << counter.zbRosNoRef[i] + counter.zbRosRef[i] << " blocks" << std::endl;
            }
        }
    }
};
