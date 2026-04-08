#pragma once

#include <iostream>
#include <sstream>

struct Parameters {
    public:
        Parameters() : bitsPerValue_(0), decimalScaleFactor_(0), binaryScaleFactor_(0), referenceValue_(0.0) {}
        Parameters(long bitsPerValue, long decimalScaleFactor, long binaryScaleFactor, double referenceValue, bool isConstantField)
            : bitsPerValue_(bitsPerValue),
              decimalScaleFactor_(decimalScaleFactor),
              binaryScaleFactor_(binaryScaleFactor),
              referenceValue_(referenceValue) {}
        void print() {
            std::cout << "reference_value: " << referenceValue_ << std::endl;
            std::cout << "binary_scale_factor: " << binaryScaleFactor_ << std::endl;
            std::cout << "decimal_scale_factor: " << decimalScaleFactor_ << std::endl;
            std::cout << "bits_per_value: " << bitsPerValue_ << std::endl;
            std::cout << "is_constant_field: " << (isConstantField_ ? "true" : "false") << std::endl;
            // std::cout << "n_vals: " << nVals_ << std::endl;
        }

        long bitsPerValue() const {
            /* check we don't decode bpv > max(ulong) as it is not currently supported by the algorithm */
            // if (bitsPerValue_ > (sizeof(long) * 8)) {
            //     throw std::runtime_error("Invalid bits_per_value");
            // }
            return bitsPerValue_;
        }

        void bitsPerValue(long bitsPerValue) {
            bitsPerValue_ = bitsPerValue;

            /* check we don't encode bpv > max(ulong)-1 as it is not currently supported by the algorithm */
            // if (bitsPerValue > (sizeof(long) * 8 - 1)){
            //     std::stringstream ss;
            //     ss << "Invalid bits_per_value: " << bitsPerValue;
            //     throw std::runtime_error(ss.str());
            // }
        }

        long decimalScaleFactor() const { return decimalScaleFactor_; }
        void decimalScaleFactor(long decimalScaleFactor) { decimalScaleFactor_ = decimalScaleFactor; }
        long binaryScaleFactor() const { return binaryScaleFactor_; }
        void binaryScaleFactor(long binaryScaleFactor) { binaryScaleFactor_ = binaryScaleFactor; }
        double referenceValue() const { return referenceValue_; }
        void referenceValue(double referenceValue) { referenceValue_ = referenceValue; }
        bool isConstantField() const { return isConstantField_; }
        void isConstantField(bool isConstantField) { isConstantField_ = isConstantField; }
        // size_t nVals() const { return nVals_; }
        // void nVals(size_t nVals) { nVals_ = nVals; }

    private:
        long bitsPerValue_;
        long decimalScaleFactor_;
        long binaryScaleFactor_;
        double referenceValue_;
        bool isConstantField_;
        // size_t nVals_;
};

