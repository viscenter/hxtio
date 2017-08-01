#pragma once

#include <fstream>

#include "HXTArrays.hpp"

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

    /** @brief Default constructor */
    Array2D() = default;

    /** @brief Constructor with dimensions */
    Array2D(Index_t rows, Index_t cols) : rows_{rows}, cols_{cols}
    {
        data_.resize(rows * cols);
    }

    /** Constructor for initialization from existing STL container */
    Array2D(
        Index_t r,
        Index_t c,
        typename Container::iterator first,
        typename Container::iterator last)
        : rows_{r}, cols_{c}, data_{first, last}
    {
    }

    /** @brief Set size of array
     *
     * @warning Does not guarantee validity of stored values after resize
     */
    void setDimensions(Index_t rows, Index_t cols)
    {
        rows_ = rows;
        cols_ = cols;
        data_.resize(rows * cols);
    }

    Index_t rows() { return rows_; }
    Index_t cols() { return cols_; }

    /** @brief Element access */
    T& operator()(Index_t y, Index_t x) { return data_.at((y * cols_) + x); }

    /** @overload operator()(Index_t y, Index_t x) */
    const T& operator()(Index_t y, Index_t x) const
    {
        return data_.at((y * cols_) + x);
    }

    Container data() const { return data_; }

protected:
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
class Array3D
{
public:
    /** Storage container alias */
    using Container = std::vector<T>;
    /** Container index type */
    using Index_t = typename Container::size_type;

    /** @brief Default constructor */
    Array3D() = default;

    /** @brief Constructor with dimensions */
    Array3D(Index_t pages, Index_t rows, Index_t cols)
        : pages_{pages}, rows_{rows}, cols_{cols}
    {
        data_.resize(pages * rows * cols);
    }

    /** @brief Set size of array
     *
     * @warning Does not guarantee validity of stored values after resize
     */
    void setDimensions(Index_t pages, Index_t rows, Index_t cols)
    {
        pages_ = pages;
        rows_ = rows;
        cols_ = cols;
        data_.resize(pages * rows * cols);
    }

    Index_t pages() { return pages_; }
    Index_t rows() { return rows_; }
    Index_t cols() { return cols_; }

    /** @brief Element access */
    T& operator()(Index_t z, Index_t y, Index_t x)
    {
        return data_.at((z * cols_ * rows_) + (y * cols_) + x);
    }

    /** @overload operator()(Index_t z, Index_t y, Index_t x) */
    const T& operator()(Index_t z, Index_t y, Index_t x) const
    {
        return data_.at((z * cols_ * rows_) + (y * cols_) + x);
    }

    /** @brief Get page of array */
    Array2D<T> page(Index_t z)
    {
        auto b = std::next(data_.begin(), z * cols_ * rows_);
        auto e = std::next(data_.begin(), (z + 1) * cols_ * rows_);
        return Array2D<T>(rows_, cols_, b, e);
    }

    Container data() const { return data_; }

private:
    /** Number of pages */
    Index_t pages_{0};
    /** Number of rows */
    Index_t rows_{0};
    /** Number of columns */
    Index_t cols_{0};
    /** Data storage */
    Container data_;
};

class HXT
{
public:
    using Bin = Array2D<double>;
    using Cube = Array3D<double>;

    static HXT Read(std::string path);

    void printMetadata();

    uint32_t rows() { return numRows_; }
    uint32_t cols() { return numCols_; }
    uint32_t numberOfBins() { return numBins_; }

    // Single bin access
    Bin bin(uint32_t b) { return bins_.page(b); }
    double binLabel(size_t b) { return binLabels_.at(b); }

    // All bin access
    Cube bins() { return bins_; }
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

    Cube bins_;

    std::string path_;
    std::fpos_t dataStart_{0};

    inline std::fpos_t pixelBytePosition_(uint32_t b, uint32_t y, uint32_t x);
};
}
