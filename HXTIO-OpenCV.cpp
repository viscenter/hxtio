#include <iostream>
#include <limits>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "HXTIO.hpp"

static constexpr auto MAX_16BPC = std::numeric_limits<uint16_t>::max();

int main(int argc, char* argv[])
{
    // Options
    std::string path = argv[1];
    uint32_t binID = std::stoul(argv[2]);

    // Load the HXT
    auto hxt = libhxt::HXT::Read(path);
    hxt.printMetadata();

    // Get the bin
    auto label = hxt.binLabel(binID);
    auto bin = hxt.bin(binID);

    // Print bin metadata
    std::cout << "Energy: " << label << std::endl;
    std::cout << "Size: " << bin.cols() << "x" << bin.rows() << std::endl;

    // Convert to a cv::Mat
    auto mat = cv::Mat(bin.data());
    mat = mat.reshape(0, hxt.rows());

    // Convert to 16bpc range
    double min{0}, max{0};
    cv::minMaxLoc(mat, &min, &max);
    mat.convertTo(mat, CV_16UC1, MAX_16BPC / max);

    // Export
    auto outpath = std::to_string(label) + ".png";
    cv::imwrite(outpath, mat);
}
