// HEXITEC IO
#include <fstream>
#include <vector>

namespace libhxt
{

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
