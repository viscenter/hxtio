#include <iostream>

#include "HXTIO.hpp"

int main(int argc, char* argv[])
{
    // Options
    std::string path = argv[1];
    uint32_t binID = std::stoul(argv[2]);

    // Load the HXT
    auto hxt = libhxt::HXT::Read(path, libhxt::AccessMode::Streaming);
    hxt.printMetadata();

    // Get the bin
    auto label = hxt.binLabel(binID);
    auto bin = hxt.bin(binID);

    // Print bin metadata
    std::cout << "Energy: " << label << std::endl;
    std::cout << "Size: " << bin.extents()[1] << "x" << bin.extents()[0]
              << std::endl;
}