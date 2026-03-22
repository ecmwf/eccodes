#pragma once

#include <cstddef>
#include <cassert>
#include <vector>

template <typename ValueType>
class Span {
    public:
        Span() : data_{nullptr}, size_{0} {}
        Span(const Span<ValueType>& other) : data_{other.data()}, size_{other.size()} {}
        Span(ValueType* data, size_t size) : data_{data}, size_{size} {}
        Span(std::vector<ValueType>& vec) : data_{vec.data()}, size_{vec.size()}{}

        ValueType* data() const { return data_; }
        size_t size() const { return size_; }

        ValueType* begin() const { return data_; }
        ValueType* end() const { return data_ + size_; }

        const ValueType* cbegin() const { return data_; }
        const ValueType* cend() const { return data_ + size_; }

        ValueType& operator[](size_t index) const { 
            return data_[index];
            assert(index < size_ && "Index out of bounds in Span::operator[]");
        }

        Span<ValueType> drop(size_t n) const {
            assert(n <= size_ && "Drop count exceeds span size.");
            return Span<ValueType>(data_ + n, size_ - n);
        }

    private:
        ValueType* data_;
        size_t size_;
};


template <typename ValueType>
class SplitBlock {
    public:
        SplitBlock(Span<ValueType> data, size_t splitSize) : data_(data), splitSizeV_(splitSize)
        {
            assert(splitSizeV_ > 0 && "Block size must be greater than zero.");
        }

        Span<ValueType> full(size_t idx) const {
            assert(splitSizeV_ > 0 && "Block size must be greater than zero.");
            assert(idx * splitSizeV_ + splitSizeV_ <= data_.size() && "Full block out of range.");
            return Span<ValueType>(data_.data() + idx * splitSizeV_, splitSizeV_);
        }

        Span<ValueType> get(size_t idx) const {
            assert(splitSizeV_ > 0 && "Block size must be greater than zero.");
            size_t start = idx * splitSizeV_;
            assert(start < data_.size() && "Block index out of range.");
            size_t remaining = data_.size() - start;
            size_t len = remaining < splitSizeV_ ? remaining : splitSizeV_;
            return Span<ValueType>(data_.data() + start, len);
        }

        Span<ValueType> part() const {
            assert(splitSizeV_ > 0 && "Block size must be greater than zero.");
            size_t start = nFullB_ * splitSizeV_;
            assert(start < data_.size() && "Part block out of range.");
            size_t remainder = data_.size() - start;
            return Span<ValueType>(data_.data() + start, remainder);
        }

        size_t nFull() const { return nFullB_; }
        size_t nPart() const { return nPartB_; }
        size_t n() const {return nFullB_ + nPartB_;}
        bool hasPart() const {return nPartB_ != 0;}

    private:
        const Span<ValueType> data_;
        const size_t splitSizeV_;
        const size_t nFullB_ = data_.size() / splitSizeV_;
        const size_t nPartB_ = (data_.size() % splitSizeV_ == 0) ? 0 : 1;
};
