#include "memory.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

Memory::Memory(size_t size_bytes) { bytes_.resize(size_bytes); }

uint8_t Memory::read_byte(uint32_t address) const {
  assert(address < size_bytes() && "Memory out of bounds");
  return bytes_[address];
}

uint16_t Memory::read_halfword(uint32_t address) const {
  assert((address & 1) == 0 && "Unaligned read_halfword");
  assert(address + 1 < size_bytes() && "Memory out of bounds");

  return static_cast<uint16_t>(
      static_cast<uint16_t>(bytes_[address]) |
      (static_cast<uint16_t>(bytes_[address + 1]) << 8));
}

uint32_t Memory::read_word(uint32_t address) const {
  assert((address & 3) == 0 && "Unaligned read_word");
  assert(address + 3 < size_bytes() && "Memory out of bounds");

  return static_cast<uint32_t>(bytes_[address]) |
         (static_cast<uint32_t>(bytes_[address + 1]) << 8) |
         (static_cast<uint32_t>(bytes_[address + 2]) << 16) |
         (static_cast<uint32_t>(bytes_[address + 3]) << 24);
}

void Memory::write_byte(uint32_t address, uint8_t value) {
  assert(address < size_bytes() && "Memory out of bounds");
  bytes_[address] = value;
}

void Memory::write_halfword(uint32_t address, uint16_t value) {
  assert((address & 1) == 0 && "Unaligned write_halfword");
  assert(address + 1 < size_bytes() && "Memory out of bounds");

  bytes_[address] = static_cast<uint8_t>(value & 0xFF);
  bytes_[address + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
}

void Memory::write_word(uint32_t address, uint32_t value) {
  assert((address & 3) == 0 && "Unaligned write_word");
  assert(address + 3 < size_bytes() && "Memory out of bounds");

  bytes_[address] = static_cast<uint8_t>(value & 0xFF);
  bytes_[address + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
  bytes_[address + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
  bytes_[address + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
}

size_t Memory::load_file(const std::filesystem::path& path) {
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    std::cerr << "[ERROR] Could not open binary file: " << path << '\n';
    exit(1);
  }

  size_t file_size = file.tellg();
  if (file_size > size_bytes()) {
    std::cerr << "[ERROR] Binary file size (" << file_size
              << " bytes) exceeds memory size (" << size_bytes() << " bytes)\n";
    exit(1);
  }

  file.seekg(0, std::ios::beg);
  file.read(reinterpret_cast<char*>(bytes_.data()),
            static_cast<std::streamsize>(file_size));
  return file_size;
}
void Memory::load_segment(uint32_t addr, const uint8_t* data, size_t size) {
  assert(addr < bytes_.size() && (bytes_.size() - addr) >= size);
  std::memcpy(&bytes_[addr], data, size);
}

void Memory::clear(uint32_t addr, size_t size) {
  assert(addr < this->size_bytes() && (this->size_bytes() - addr) >= size);
  std::memset(&bytes_[addr], 0, size);
}

std::vector<uint8_t> Memory::read_span(uint32_t address, size_t count) const {
  size_t available = (address < bytes_.size()) ? bytes_.size() - address : 0;
  size_t n = std::min(count, available);
  return std::vector<uint8_t>(bytes_.begin() + address,
                              bytes_.begin() + address + n);
}
