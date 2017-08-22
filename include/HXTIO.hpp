#pragma once

#include <fstream>

#include "yanda.hpp"

namespace libhxt
{

/** HXT File Access Modes */
enum class AccessMode { Cached, Streaming };

/**
 * @class HXT
 * @author Seth Parker
 * @brief A class for loading interacting  with HXT files produced by the
 * HEXITEC sensor and software
 */
class HXT
{
public:
    /** @brief Bin type. Holds a single energy band. */
    using Bin = yanda::NDimensionalArray<double, 2>;

    /** @brief Cube type. Holds all energy bands. */
    using Cube = yanda::NDimensionalArray<double, 3>;

    /**
     * @brief Load an HXT file from disk
     *
     * If `mode` is set to `AccessMode::Cached`, all bins will be immediately
     * loaded into memory. If `mode` is set to `AccessMode::Streaming`, bins
     * will be loaded from disk on demand.
     */
    static HXT Read(std::string path, AccessMode mode = AccessMode::Cached);

    /** @brief Prints loaded metadata values to stdout */
    void printMetadata();

    /** @brief Number of rows in each bin */
    uint32_t rows() { return numRows_; }
    /** @brief Number of columns in each bin */
    uint32_t cols() { return numCols_; }
    /** @brief Number of bins */
    uint32_t numberOfBins() { return numBins_; }

    /**
     * @brief Get a single bin by index
     *
     * If `mode` is set to `AccessMode::Streaming`, the bin will load from disk
     * on demand. Subsequent calls to `bin()` will reread the bin from disk.
     */
    Bin bin(uint32_t b);

    /**
     * @brief Get the bin label by index
     *
     * This is typically the bin energy
     */
    double binLabel(size_t b) { return binLabels_.at(b); }

    /**
     * @brief Get all of the bins
     *
     * If `mode` is set to `AccessMode::Streaming`, the internal access mode
     * will be reset to `AccessMode::Cached` and all of the bins will be loaded
     * from disk immediately. Subsequent calls to `bin()` or `bins()` will not
     * result in more reads from disk.
     */
    Cube bins();

    /** @brief Get all of the bin labels */
    std::vector<double> binLabels() { return binLabels_; }

protected:
    /**
     * Default constructor. Protected since an HXT cannot be constructed from
     * scratch.
     */
    HXT() = default;

private:
    /** Path to HXT file */
    std::string path_;

    /** HEXITEC file identifier label */
    std::string label_;
    /** HEXITEC file version */
    uint64_t version_{0};

    // Currently unused metadata fields
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

    /** Number of rows in each bin */
    uint32_t numRows_{0};
    /** Number of colums in each bin */
    uint32_t numCols_{0};
    /** Number of bins */
    uint32_t numBins_{0};

    /** List of bin labels */
    std::vector<double> binLabels_;

    /** Current access mode */
    AccessMode accessMode_{AccessMode::Cached};
    /** Bin storage */
    Cube bins_;

    /** Position of first bin data byte in HXT file */
    std::fpos_t dataStart_{0};

    /** Cache all bins from disk */
    void cache_bins_();

    /** Read a single bin from disk */
    Bin read_bin_(uint32_t b);

    /** Calculate the byte position of a specific pixel in the HXT file */
    inline std::fpos_t pixelBytePosition_(uint32_t b, uint32_t y, uint32_t x);
};
}
