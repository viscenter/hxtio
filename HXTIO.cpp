//
// Created by Seth on 7/14/17.
//
#include <iostream>

#include "HXTIO.hpp"

using namespace libhxt;

using Buffer = std::vector<char>;

void HXT::printMetadata()
{
    printf("MSS: %d, %d, %d, %d\n", mssX_, mssY_, mssZ_, mssRot_);
    printf("Gal: %d, %d, %d, %d, %d\n", galX_, galY_, galZ_, galRot_, galRot2_);
    printf("Prefix: %s\n", filePreFix_.c_str());
    printf("Timestamp: %s\n", timeStamp_.c_str());
    printf("R, C, B: %d, %d, %d\n", numRows_, numCols_, numBins_);
}

HXT HXT::Read(std::string path)
{
    // Open file
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs.good()) {
        throw std::runtime_error("Cannot open file");
    }

    // Setup the output HXT
    HXT hxt;
    hxt.path_ = path;

    // Read the file id
    Buffer label(8, '\0');
    ifs.read(&label[0], 8);
    hxt.label_ = std::string(label.data());
    if (hxt.label_ != "HEXITECH") {
        throw std::runtime_error("Unknown file type");
    }

    // Read the version
    ifs.read(reinterpret_cast<char*>(&hxt.version_), sizeof(version_));

    if (hxt.version_ != 2 && hxt.version_ != 3) {
        throw std::runtime_error("Unsupported HXT version");
    }

    // Read metadata
    ifs.read(reinterpret_cast<char*>(&hxt.mssX_), sizeof(mssX_));
    ifs.read(reinterpret_cast<char*>(&hxt.mssY_), sizeof(mssY_));
    ifs.read(reinterpret_cast<char*>(&hxt.mssZ_), sizeof(mssZ_));
    ifs.read(reinterpret_cast<char*>(&hxt.mssRot_), sizeof(mssRot_));
    ifs.read(reinterpret_cast<char*>(&hxt.galX_), sizeof(galX_));
    ifs.read(reinterpret_cast<char*>(&hxt.galY_), sizeof(galY_));
    ifs.read(reinterpret_cast<char*>(&hxt.galZ_), sizeof(galZ_));
    ifs.read(reinterpret_cast<char*>(&hxt.galRot_), sizeof(galRot_));
    ifs.read(reinterpret_cast<char*>(&hxt.galRot2_), sizeof(galRot2_));

    // File prefix
    int32_t prefixSize;
    ifs.read(reinterpret_cast<char*>(&prefixSize), sizeof(int32_t));
    Buffer prefix(static_cast<uint32_t>(prefixSize), '\0');
    ifs.read(&prefix[0], prefixSize);
    hxt.filePreFix_ = std::string(prefix.data());

    // Timestamp
    // Default size: 13 bytes
    int timestampSize{13};

    // Do Version 3 specific things
    if (hxt.version_ == 3) {
        ifs.seekg(100 - prefixSize, std::ios_base::seekdir::cur);
        timestampSize = 16;
    }
    Buffer time(static_cast<uint32_t>(timestampSize), '\0');
    ifs.read(&time[0], timestampSize);
    hxt.timeStamp_ = std::string(time.data());

    // More metadata
    ifs.read(reinterpret_cast<char*>(&hxt.numRows_), sizeof(numRows_));
    ifs.read(reinterpret_cast<char*>(&hxt.numCols_), sizeof(numCols_));
    ifs.read(reinterpret_cast<char*>(&hxt.numBins_), sizeof(numBins_));
    for (auto i = 0; i < hxt.numBins_; i++) {
        double v;
        ifs.read(reinterpret_cast<char*>(&v), sizeof(v));
        hxt.binLabels_.push_back(v);
    }

    // Capture the data start position
    hxt.dataStart_ = ifs.tellg();

    // Load the bins
    hxt.readBins_(ifs);

    // Close the file
    ifs.close();

    return hxt;
}

HXT::Bin HXT::readBins_(std::ifstream& ifs)
{
    // Make sure the file's still open
    if (!ifs.good()) {
        throw std::runtime_error("File not open");
    }

    // Fill the output matrix
    Bin bin;
    Row row;
    for (uint32_t b = 0; b < numBins_; b++) {
        bin.clear();
        double value;
        for (uint32_t y = 0; y < numRows_; y++) {
            row.clear();
            for (uint32_t x = 0; x < numCols_; x++) {
                auto pos = pixelBytePosition_(b, y, x);
                ifs.seekg(pos);
                ifs.read(reinterpret_cast<char*>(&value), sizeof(double));
                if (ifs.fail()) {
                    auto msg = "Only read " + std::to_string(ifs.gcount()) +
                               " bytes. Expected: 8";
                    throw std::runtime_error(msg);
                }

                row.push_back(value);
            }
            bin.push_back(row);
        }
        bins_.push_back(bin);
    }

    ifs.close();

    return bin;
}

std::fpos_t HXT::pixelBytePosition_(uint32_t b, uint32_t y, uint32_t x)
{
    return dataStart_ +
           sizeof(double) * ((numBins_ * numCols_ * y) + (numBins_ * x) + b);
}