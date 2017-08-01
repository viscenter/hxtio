#pragma once
// HEXITEC IO
#include <fstream>
#include <vector>

namespace libhxt
{

/**
 * @class Array2D
 * @brief Generic 2D array template
 */
template <typename T>
class Array2D
{
public:
    /** Storage container alias */
    using Container = std::vector<T>;
    /** Container index type */
    using Index_t = typename Container::size_type;

    /** @brief Constructor with dimensions */
    Array2D(Index_t rows, Index_t cols) : rows_{rows}, cols_{cols}
    {
        data_.resize(rows * cols);
    }

    /** @brief Element access */
    virtual T& operator()(Index_t y, Index_t x)
    {
        return data_.at((y * cols_) + x);
    }

    /** @overload operator()(Index_t y, Index_t x) */
    virtual const T& operator()(Index_t y, Index_t x) const
    {
        return data_.at((y * cols_) + x);
    }

protected:
    /** Default constructor */
    Array2D() = default;

    /** Constructor for initialization from existing STL container */
    Array2D(
        Index_t r,
        Index_t c,
        typename Container::iterator first,
        typename Container::iterator last)
        : rows_{r}, cols_{c}, data_{first, last}
    {
    }

    /** Number of rows */
    Index_t rows_{0};
    /** Number of columns */
    Index_t cols_{0};
    /** Data storage */
    Container data_;
};

/**
 * @class Array3D
 * @brief Generic 3D array template
 */
template <typename T>
class Array3D : public Array2D<T>
{
public:
    /** Storage container alias */
    using Container = std::vector<T>;
    /** Container index type */
    using Index_t = typename Container::size_type;

    /** @brief Constructor with dimensions */
    Array3D(Index_t pages, Index_t rows, Index_t cols) : pages_{pages}
    {
        this->rows_ = rows;
        this->cols_ = cols;
        this->data_.resize(pages * rows * cols);
    }

    /** @brief Element access */
    T& operator()(Index_t z, Index_t y, Index_t x)
    {
        return this->data_.at(
            (z * this->cols_ * this->rows_) + (y * this->cols_) + x);
    }

    /** @overload operator()(Index_t z, Index_t y, Index_t x) */
    const T& operator()(Index_t z, Index_t y, Index_t x) const
    {
        return this->data_.at(
            (z * this->cols_ * this->rows_) + (y * this->cols_) + x);
    }

    /** @brief Get page of array */
    Array2D<T> page(Index_t z)
    {
        auto b = std::next(this->data_.begin(), z * this->cols_ * this->rows_);
        auto e =
            std::next(this->data_.begin(), (z + 1) * this->cols_ * this->rows_);
        return Array2D<T>(this->rows_, this->cols_, b, e);
    }

    /** Deleted operator that doesn't make sense for 3D array */
    T& operator()(Index_t y, Index_t x) = delete;

    /** @overload T& operator()(Index_t y, Index_t x) */
    const T& operator()(Index_t y, Index_t x) const = delete;

private:
    Index_t pages_{0};
};

class HXT
{
public:
    using Row = std::vector<double>;
    using Bin = std::vector<Row>;

    static HXT Read(std::string path);

    void printMetadata();

    uint32_t rows() { return numRows_; }
    uint32_t cols() { return numCols_; }
    uint32_t numberOfBins() { return numBins_; }

    // Single bin access
    Bin bin(uint32_t b) { return bins_[b]; }
    double binLabel(size_t b) { return binLabels_[b]; }

    // All bin access
    std::vector<Bin> bins() { return bins_; }
    std::vector<double> binLabels() { return binLabels_; }

protected:
    // Currently protected since we can't make one from scratch
    HXT() = default;

private:
    std::string label_;
    uint64_t version_{0};
    uint32_t mssX_{0};
    uint32_t mssY_{0};
    uint32_t mssZ_{0};
    uint32_t mssRot_{0};
    uint32_t galX_{0};
    uint32_t galY_{0};
    uint32_t galZ_{0};
    uint32_t galRot_{0};
    uint32_t galRot2_{0};

    std::string filePreFix_;
    std::string timeStamp_;

    uint32_t numRows_{0};
    uint32_t numCols_{0};
    uint32_t numBins_{0};
    std::vector<double> binLabels_;

    std::vector<Bin> bins_;

    std::string path_;
    std::fpos_t dataStart_{0};

    Bin readBins_(std::ifstream& ifs);
    inline std::fpos_t pixelBytePosition_(uint32_t b, uint32_t y, uint32_t x);
};
}
