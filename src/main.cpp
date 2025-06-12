#include "./wad.hpp"
#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

// enum with the possible formats
enum class Format : std::uint8_t {
  JSON,
  JSON_VERBOSE,
  DSL,
  DSL_VERBOSE
};

int main(int argc, char *argv[]) {
  try {

    if (argc < 4 || argc > 5) {
      std::cout << "Usage: wadconvert -<format> <wad file> <output json file> "
                   "[--verbose]\n";
      std::cout
          << "  -<format>: The format to convert to (-json, -jsonverbose, "
             "-dsl, -dslverbose)\n";
      std::cout << "  wad file: Path to the WAD file to convert\n";
      std::cout << "  output json file: Path to the output JSON file\n";
      std::cout << "  --verbose: Optional flag for detailed output\n";
      return 1;
    }

    Format      format;
    std::string formatStr       = argv[1];
    std::string wadFilePath     = argv[2];
    std::string destinationPath = argv[3];
    bool        verbose = (argc == 5 && std::string(argv[4]) == "--verbose");

    // remove the leading '-' from the format string only if it exists
    if (formatStr[0] == '-') {
      formatStr = formatStr.substr(1);
    }

    if (formatStr == "json") {
      format = Format::JSON;
    } else if (formatStr == "jsonverbose") {
      format = Format::JSON_VERBOSE;
    } else if (formatStr == "dsl") {
      format = Format::DSL;
    } else if (formatStr == "dslverbose") {
      format = Format::DSL_VERBOSE;
    } else {
      std::cerr
          << "Invalid format specified. Use -json, -jsonverbose, -dsl, or "
             "-dslverbose.\n";
      return 1;
    }

    if (verbose) {
      std::cout << "Converting WAD file to " << formatStr << " format...\n";
    }

    WAD wad(wadFilePath, verbose);  // Pass verbose flag to WAD constructor
    wad.processWAD();

    // Convert WAD data to the proper format depending on the argument passed
    switch (format) {
      // convert to JSON
      case Format::JSON: {

        std::ofstream jsonFile;
        jsonFile.open(destinationPath);

        if (!jsonFile) {
          std::cerr << "Unable to open output JSON file: " << destinationPath
                    << "\n";
          return 1;
        }

        jsonFile << wad.toJSON();
        jsonFile.close();

        break;
      }

      // convert to JSON verbose
      case Format::JSON_VERBOSE: {

        std::ofstream jsonFile;
        jsonFile.open(destinationPath);

        if (!jsonFile) {
          std::cerr << "Unable to open output JSON file: " << destinationPath
                    << "\n";
          return 1;
        }

        jsonFile << wad.toJSONVerbose();
        jsonFile.close();

        break;
      }

      // convert to custom DSL format
      case Format::DSL: {

        std::ofstream dslFile;
        dslFile.open(destinationPath);

        if (!dslFile) {
          std::cerr << "Unable to open output DSL file: " << destinationPath
                    << "\n";
          return 1;
        }

        dslFile << wad.toDSL();
        dslFile.close();

        break;
      }

      default:
        break;
    }

    if (verbose) {
      std::cout << "WAD file converted to " << formatStr
                << " format successfully.\n";
    } else {
      std::cout << std::filesystem::path(wadFilePath).filename().string()
                << " converted to " << formatStr << ".\n";
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  } catch (...) {
    std::cerr << "Unknown error occurred.\n";
    return 1;
  }

  return 0;
}
