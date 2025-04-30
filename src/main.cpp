#include "./wad.hpp"
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  if (argc < 3 || argc > 4) {
    std::cout
        << "Usage: wadconvert <wad file> <output json file> [--verbose]\n";
    std::cout << "  wad file: Path to the WAD file to convert\n";
    std::cout << "  output json file: Path to the output JSON file\n";
    std::cout << "  --verbose: Optional flag for detailed output\n";
    return 1;
  }

  std::string wadFilePath  = argv[1];
  std::string jsonFilePath = argv[2];
  bool        verbose      = (argc == 4 && std::string(argv[3]) == "--verbose");

  if (verbose) {
    std::cout << "Converting WAD file to JSON...\n";
  }
  try {
    WAD wad(wadFilePath, verbose);  // Pass verbose flag to WAD constructor
    wad.processWAD();
    nlohmann::json jsonData = wad.toJSON();

    std::ofstream jsonFile(jsonFilePath);
    if (!jsonFile) {
      std::cerr << "Unable to open output JSON file: " << jsonFilePath << "\n";
      return 1;
    }

    jsonFile << jsonData.dump(1);
    jsonFile.close();

    if (verbose) {
      std::cout << "WAD file converted to JSON successfully.\n";
    } else {
      std::cout << std::filesystem::path(wadFilePath).filename().string()
                << " converted to JSON.\n";
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
