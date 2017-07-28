
#include <string>
#include <HXTIO.hpp>

int main(int argc, char* argv[])
{
    // Options
    std::string path = argv[1];
    uint32_t binID = std::stoul(argv[2]);

    // Load the HXT
    auto hxt = libhxt::HXT::Read(argv[1]);
    hxt.printMetadata();

    // Get the bin
    auto bin = hxt.bin(binID);
    auto label = hxt.binLabel(binID);
}