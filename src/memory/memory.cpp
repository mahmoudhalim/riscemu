#include "memory.h"

#include <cassert>
#include <cstdlib>
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

size_t Memory::load_file(const std::filesystem::path &path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    std::cerr << "[ERROR] Could not open hex file: " << path << '\n';
    exit(1);
  }
  std::string hex;
  size_t addr = 0;
  while (file >> hex) {
    if (addr + 3 >= size_bytes()) {
      std::cerr << "[ERROR] Hex file exceeds memory size at address 0x"
                << std::hex << addr << '\n';
      return addr;
    }
    try {
      this->write_word(addr, std::stoul(hex, nullptr, 16));
      addr += 4;
    } catch (const std::exception &e) {
      std::cerr << "[ERROR] Malformed hex string '" << hex << "' at address 0x"
                << std::hex << addr << '\n';
      return addr;
    }
  }

  return addr;
}
