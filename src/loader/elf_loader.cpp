#include "elf_loader.h"
#include "memory/memory.h"
#include <algorithm>
#include <cstring>
#include <elf.h>
#include <expected>
#include <fstream>
#include <vector>

LoadResult ELFLoader::load(const std::filesystem::path& filename,
                           Memory& memory) {
  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    return std::unexpected("Cannot open file: " + filename.string());
  }

  Elf32_Ehdr elf_header;
  if (!file.read(reinterpret_cast<char*>(&elf_header), sizeof(elf_header))) {
    return std::unexpected("Failed to read ELF header");
  }

  if (std::memcmp(elf_header.e_ident, ELFMAG, SELFMAG) != 0) {
    return std::unexpected("Not a valid ELF file");
  }

  if (elf_header.e_ident[EI_CLASS] != ELFCLASS32) {
    return std::unexpected("Not a 32-bit ELF file");
  }

  if (elf_header.e_ident[EI_DATA] != ELFDATA2LSB) {
    return std::unexpected("Not a little-endian ELF file");
  }

  if (elf_header.e_type != ET_EXEC) {
    return std::unexpected("Not an Executable ELF file");
  }

  if (elf_header.e_machine != EM_RISCV) {
    return std::unexpected("Not a RISC-V binary");
  }

  file.seekg(elf_header.e_phoff, std::ios::beg);

  std::vector<Elf32_Phdr> program_header(elf_header.e_phnum);
  file.read(reinterpret_cast<char*>(program_header.data()),
            program_header.size() * sizeof(Elf32_Phdr));
  if (!file) {
    return std::unexpected("Failed to read program headers");
  }

  uint32_t max_addr = 0;

  for (const auto& ph : program_header) {
    if (ph.p_type != PT_LOAD) {
      continue;
    }

    uint32_t seg_end = ph.p_vaddr + ph.p_memsz;
    max_addr = std::max(max_addr, seg_end);

    if (ph.p_filesz > 0) {
      std::vector<uint8_t> segment_data(ph.p_filesz);
      file.seekg(ph.p_offset, std::ios::beg);
      file.read(reinterpret_cast<char*>(segment_data.data()), ph.p_filesz);
      if (!file) {
        return std::unexpected("Failed to read segment data from file.");
      }
      memory.load_segment(ph.p_vaddr, segment_data.data(), ph.p_filesz);
    }

    if (ph.p_memsz > ph.p_filesz) {
      uint32_t bss_size = ph.p_memsz - ph.p_filesz;
      uint32_t bss_start = ph.p_vaddr + ph.p_filesz;
      memory.clear(bss_start, bss_size);
    }
  }
  return LoadInfo{elf_header.e_entry, max_addr};
}
