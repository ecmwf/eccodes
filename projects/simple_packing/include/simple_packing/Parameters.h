#pragma once

namespace sp
{

// The packing parameters of GRIB simple packing: bits per value, binary and
// decimal scale factors, reference value, and whether the field is constant.

struct Parameters
{
public:
    Parameters() : bitsPerValue_(0), decimalScaleFactor_(0), binaryScaleFactor_(0), referenceValue_(0.0), isConstantField_(false) {}
    Parameters(long bitsPerValue, long decimalScaleFactor, long binaryScaleFactor, double referenceValue, bool isConstantField) :
        bitsPerValue_(bitsPerValue), decimalScaleFactor_(decimalScaleFactor), binaryScaleFactor_(binaryScaleFactor), referenceValue_(referenceValue), isConstantField_(isConstantField) {}

    long bitsPerValue() const { return bitsPerValue_; }
    void bitsPerValue(long bitsPerValue) { bitsPerValue_ = bitsPerValue; }
    long decimalScaleFactor() const { return decimalScaleFactor_; }
    void decimalScaleFactor(long decimalScaleFactor) { decimalScaleFactor_ = decimalScaleFactor; }
    long binaryScaleFactor() const { return binaryScaleFactor_; }
    void binaryScaleFactor(long binaryScaleFactor) { binaryScaleFactor_ = binaryScaleFactor; }
    double referenceValue() const { return referenceValue_; }
    void referenceValue(double referenceValue) { referenceValue_ = referenceValue; }
    bool isConstantField() const { return isConstantField_; }
    void isConstantField(bool isConstantField) { isConstantField_ = isConstantField; }

private:
    long bitsPerValue_;
    long decimalScaleFactor_;
    long binaryScaleFactor_;
    double referenceValue_;
    bool isConstantField_;
};

}  // namespace sp
