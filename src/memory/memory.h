#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <vector>

class Memory {
public:
  explicit Memory(size_t size_bytes = 4 * 1024 * 1024);

  uint8_t read_byte(uint32_t address) const;
  uint16_t read_halfword(uint32_t address) const;
  uint32_t read_word(uint32_t address) const;

  void write_byte(uint32_t address, uint8_t value);
  void write_halfword(uint32_t address, uint16_t value);
  void write_word(uint32_t address, uint32_t value);

  size_t load_file(const std::filesystem::path& path);

  size_t size_bytes() const { return bytes_.size(); }

private:
  std::vector<uint8_t> bytes_;
};
