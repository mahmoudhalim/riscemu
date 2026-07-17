#pragma once
#include <elf.h>
#include <expected>
#include <filesystem>
#include <string>

class Memory;

struct LoadInfo {
  uint32_t entry_point;
  uint32_t max_addr; // address of last loaded byte + 1
};

using LoadResult = std::expected<LoadInfo, std::string>;

class ELFLoader {
public:
  static LoadResult load(const std::filesystem::path& filename,
                         Memory& Memory);
};
