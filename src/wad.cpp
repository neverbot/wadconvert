#include "wad.hpp"
#include <fstream>
#include <iostream>

/**
 * @brief WAD constructor
 * @param filepath Path to the WAD file
 * @throws std::runtime_error if the file cannot be opened or is not a valid WAD
 * file
 */
WAD::WAD(const std::string &filepath, bool verbose) {
  filepath_ = filepath;
  verbose_  = verbose;

  std::ifstream file(filepath_, std::ios::binary);

  if (!file) {
    throw std::runtime_error("Unable to open WAD file: " + filepath_);
  }

  // Read header
  file.read(reinterpret_cast<char *>(&header_), sizeof(Header));
  if (!file) {
    throw std::runtime_error("Unable to read WAD header");
  }

  // Verify WAD type
  std::string id(header_.identification, 4);
  if (id != "IWAD" && id != "PWAD") {
    throw std::runtime_error("Not a valid WAD file");
  }

  if (verbose_) {
    std::cout << "WAD type: " << id << std::endl;
    std::cout << "Num lumps: " << header_.numlumps << std::endl;
  }

  // Read directory
  readDirectory();
}

/**
 * @brief Read the WAD directory
 * @throws std::runtime_error if the directory cannot be read
 */
void WAD::readDirectory() {
  // Open the WAD file in binary mode.
  std::ifstream file(filepath_, std::ios::binary);

  // Move the file read position to the start of the directory, using the
  // offset from the header (header_.infotableofs).
  file.seekg(header_.infotableofs);

  // Prepare the directory_ vector to hold all directory entries. The number of
  // entries is specified in the header (header_.numlumps).
  directory_.resize(header_.numlumps);

  // Read the entire directory into memory: each lump has a fixed-size record
  // (16 bytes), and we read all of them at once.
  file.read(reinterpret_cast<char *>(directory_.data()),
            header_.numlumps * sizeof(Directory));
}

/**
 * @brief Find a lump by name
 * @param name Lump name
 * @param offset Offset of the lump in the file
 * @param size Size of the lump
 * @return true if the lump is found, false otherwise
 */
bool WAD::findLump(const std::string &name, uint32_t &offset,
                   uint32_t &size) const {
  for (const auto &entry : directory_) {
    std::string lumpName(entry.name, strnlen(entry.name, 8));

    if (lumpName == name) {
      offset = entry.filepos;
      size   = entry.size;
      return true;
    }
  }
  return false;
}

/**
 * @brief Read a lump from the WAD file
 * @param offset Offset of the lump in the file
 * @param size Size of the lump
 * @return Vector containing the lump data
 * @throws std::runtime_error if the lump cannot be read
 */
std::vector<uint8_t> WAD::readLump(std::streamoff offset, std::size_t size) {
  std::vector<uint8_t> data(size);
  std::ifstream        file(filepath_, std::ios::binary);
  if (!file) {
    throw std::runtime_error("Unable to open file: " + filepath_);
  }
  file.seekg(offset);
  file.read(reinterpret_cast<char *>(data.data()), size);
  return data;
}

/**
 * @brief Read vertices from the WAD file
 * @param offset Offset of the vertices in the file
 * @param size Size of the vertices
 * @return Vector containing the vertices
 */
std::vector<WAD::Vertex> WAD::readVertices(std::streamoff offset,
                                           std::size_t    size) {
  auto                data = readLump(offset, size);
  std::vector<Vertex> vertices(size / sizeof(Vertex));
  std::memcpy(vertices.data(), data.data(), size);
  return vertices;
}

/**
 * @brief Read linedefs from the WAD file
 * @param offset Offset of the linedefs in the file
 * @param size Size of the linedefs
 * @return Vector containing the linedefs
 */
std::vector<WAD::Linedef> WAD::readLinedefs(std::streamoff offset,
                                            std::size_t    size) {
  auto                 data = readLump(offset, size);
  std::vector<Linedef> linedefs(size / sizeof(Linedef));
  std::memcpy(linedefs.data(), data.data(), size);
  return linedefs;
}

/**
 * @brief Read sidedefs from the WAD file
 * @param offset Offset of the sidedefs in the file
 * @param size Size of the sidedefs
 * @return Vector containing the sidedefs
 */
std::vector<WAD::Sidedef> WAD::readSidedefs(std::streamoff offset,
                                            std::size_t    size) {
  auto                 data = readLump(offset, size);
  std::vector<Sidedef> sidedefs(size / sizeof(Sidedef));
  std::memcpy(sidedefs.data(), data.data(), size);
  return sidedefs;
}

/**
 * @brief Read sectors from the WAD file
 * @param offset Offset of the sectors in the file
 * @param size Size of the sectors
 * @return Vector containing the sectors
 */
std::vector<WAD::Sector> WAD::readSectors(std::streamoff offset,
                                          std::size_t    size) {
  auto                data = readLump(offset, size);
  std::vector<Sector> sectors(size / sizeof(Sector));
  std::memcpy(sectors.data(), data.data(), size);
  return sectors;
}

/**
 * @brief Read things from the WAD file
 * @param offset Offset of the things in the file
 * @param size Size of the things
 * @return Vector containing the things
 */
std::vector<WAD::Thing> WAD::readThings(std::streamoff offset,
                                        std::size_t    size) {
  auto               data = readLump(offset, size);
  std::vector<Thing> things(size / sizeof(Thing));
  std::memcpy(things.data(), data.data(), size);
  return things;
}

/**
 * @brief Process the WAD file and load all data
 * @throws std::runtime_error if any of the lumps cannot be read
 * @note This function reads all the lumps in the WAD file and stores them in
 *       the corresponding vectors. It also prints the number of loaded lumps
 *       to the console.
 */
void WAD::processWAD() {
  uint32_t offset, size;

  // Read vertices
  if (findLump("VERTEXES", offset, size)) {
    vertices_ = readVertices(offset, size);
    if (verbose_) {
      std::cout << "Loaded " << vertices_.size() << " vertices\n";
    }
  }

  // Read linedefs
  if (findLump("LINEDEFS", offset, size)) {
    linedefs_ = readLinedefs(offset, size);
    if (verbose_) {
      std::cout << "Loaded " << linedefs_.size() << " linedefs\n";
    }
  }

  // Read sidedefs
  if (findLump("SIDEDEFS", offset, size)) {
    sidedefs_ = readSidedefs(offset, size);
    if (verbose_) {
      std::cout << "Loaded " << sidedefs_.size() << " sidedefs\n";
    }
  }

  // Read sectors
  if (findLump("SECTORS", offset, size)) {
    sectors_ = readSectors(offset, size);
    if (verbose_) {
      std::cout << "Loaded " << sectors_.size() << " sectors\n";
    }
  }

  // Read things
  if (findLump("THINGS", offset, size)) {
    things_ = readThings(offset, size);
    if (verbose_) {
      std::cout << "Loaded " << things_.size() << " things\n";
    }
  }
}

nlohmann::json WAD::toJSON() const {
  nlohmann::json j;

  j["vertices"] = nlohmann::json::array();
  for (const auto &v : vertices_) {
    j["vertices"].push_back({{"x", v.x}, {"y", v.y}});
  }

  j["linedefs"] = nlohmann::json::array();
  for (const auto &l : linedefs_) {
    j["linedefs"].push_back({{"start", l.start_vertex},
                             {"end", l.end_vertex},
                             {"flags", l.flags},
                             {"type", l.line_type},
                             {"tag", l.sector_tag},
                             {"right_sidedef", l.right_sidedef},
                             {"left_sidedef", l.left_sidedef}});
  }

  j["sidedefs"] = nlohmann::json::array();
  for (const auto &s : sidedefs_) {
    j["sidedefs"].push_back(
        {{"x_offset", s.x_offset},
         {"y_offset", s.y_offset},
         {"upper_texture",
          std::string(s.upper_texture, strnlen(s.upper_texture, 8))},
         {"lower_texture",
          std::string(s.lower_texture, strnlen(s.lower_texture, 8))},
         {"middle_texture",
          std::string(s.middle_texture, strnlen(s.middle_texture, 8))},
         {"sector", s.sector}});
  }

  j["sectors"] = nlohmann::json::array();
  for (const auto &s : sectors_) {
    j["sectors"].push_back(
        {{"floor_height", s.floor_height},
         {"ceiling_height", s.ceiling_height},
         {"floor_texture",
          std::string(s.floor_texture, strnlen(s.floor_texture, 8))},
         {"ceiling_texture",
          std::string(s.ceiling_texture, strnlen(s.ceiling_texture, 8))},
         {"light_level", s.light_level},
         {"type", s.type},
         {"tag", s.tag}});
  }

  j["things"] = nlohmann::json::array();
  for (const auto &t : things_) {
    j["things"].push_back({{"x", t.x},
                           {"y", t.y},
                           {"angle", t.angle},
                           {"type", t.type},
                           {"flags", t.flags}});
  }

  return j;
}
