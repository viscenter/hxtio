#pragma once

#include <array>
#include <exception>
#include <numeric>
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

    /** @brief Default constructor */
    Array2D() = default;

    /** @brief Constructor with dimensions */
    Array2D(Index_t rows, Index_t cols) : rows_{rows}, cols_{cols}
    {
        data_.resize(rows * cols);
    }

    /** Constructor with range initialization */
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

    /** @brief Number of rows */
    Index_t rows() { return rows_; }

    /** @brief Number of columns */
    Index_t cols() { return cols_; }

    /** @brief Element access */
    T& operator()(Index_t y, Index_t x) { return data_.at((y * cols_) + x); }

    /** @overload operator()(Index_t y, Index_t x) */
    const T& operator()(Index_t y, Index_t x) const
    {
        return data_.at((y * cols_) + x);
    }

    /** @brief Return copy of raw data */
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

    /** @brief Number of pages */
    Index_t pages() { return pages_; }

    /** @brief Number of rows */
    Index_t rows() { return rows_; }

    /** @brief Number of cols */
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

    /** @brief Return copy of raw data */
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

template <typename T, int N>
class NDimensionalArray
{
    static_assert(N > 0, "invalid number of dimensions");

public:
    /** Storage container alias */
    using Container = std::vector<T>;
    /** Container index type */
    using DataIndex = typename Container::size_type;
    /** Extents type */
    using Extent = std::array<DataIndex, N>;
    /** N-Dim Array Index type */
    using ItemIndex = std::array<DataIndex, N>;
    /** Slice type */
    using Slice = NDimensionalArray<T, N - 1>;

    /** @brief Default constructor */
    NDimensionalArray() = default;

    /** @brief Constructor with dimensions */
    explicit NDimensionalArray(Extent e) : extents_(e)
    {
        auto size = std::accumulate(
            e.begin(), e.end(), DataIndex(1), std::multiplies<DataIndex>());

        if (size == 0) {
            throw std::range_error("Array extent is zero");
        }

        data_.resize(size);
    }

    /** @brief Constructor with range initialization */
    NDimensionalArray(
        Extent e,
        typename Container::iterator first,
        typename Container::iterator last)
        : extents_(e), data_{first, last}
    {
    }

    /** @brief Set the extent of the array's dimensions
     *
     * @warning Does not guarantee validity of stored values after resize
     */
    void setExtents(Extent e)
    {
        extents_ = e;

        auto size = std::accumulate(
            e.begin(), e.end(), DataIndex(1), std::multiplies<DataIndex>());

        if (size == 0) {
            throw std::range_error("Array extent is zero");
        }

        data_.resize(size);
    }

    /** @brief Get the extent of the array's dimensions */
    Extent extents() const { return extents_; }

    /** @brief Element access */
    T& operator()(ItemIndex index)
    {
        DataIndex dIdx = 0;
        for (auto it = 0; it < extents_.size(); it++) {
            auto begin = std::next(extents_.begin(), it + 1);
            auto offset = std::accumulate(
                begin, extents_.end(), DataIndex(1),
                std::multiplies<DataIndex>());
            dIdx += index[it] * offset;
        }

        return data_.at(dIdx);
    }

    /** @brief Get slice of array by dropping highest dimension */
    Slice slice(DataIndex index)
    {
        auto offset = std::accumulate(
            std::next(extents_.begin(), 1), extents_.end(), DataIndex(1),
            std::multiplies<DataIndex>());

        auto b = std::next(data_.begin(), index * offset);
        auto e = std::next(data_.begin(), (index + 1) * offset);

        typename Slice::Extent newExtent;
        std::copy(
            std::next(extents_.begin(), 1), extents_.end(), newExtent.begin());

        return Slice(newExtent, b, e);
    }

    /** @brief Return copy of raw data */
    Container data() const { return data_; }

private:
    /** Dimension extents */
    Extent extents_;
    /** Data storage */
    Container data_;
};
}