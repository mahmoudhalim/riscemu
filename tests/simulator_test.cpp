#include "riscv/simulator.h"

#include <elf.h>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace {
std::filesystem::path write_temp_elf(const std::vector<uint8_t>& code) {
  auto path = std::filesystem::temp_directory_path() /
              ("riscemu_sim_" + std::to_string(std::rand()) + ".elf");

  Elf32_Ehdr ehdr{};
  ehdr.e_ident[EI_MAG0] = ELFMAG0;
  ehdr.e_ident[EI_MAG1] = 'E';
  ehdr.e_ident[EI_MAG2] = 'L';
  ehdr.e_ident[EI_MAG3] = 'F';
  ehdr.e_ident[EI_CLASS] = ELFCLASS32;
  ehdr.e_ident[EI_DATA] = ELFDATA2LSB;
  ehdr.e_ident[EI_VERSION] = EV_CURRENT;
  ehdr.e_type = ET_EXEC;
  ehdr.e_machine = EM_RISCV;
  ehdr.e_entry = 0;
  ehdr.e_phoff = sizeof(Elf32_Ehdr);
  ehdr.e_ehsize = sizeof(Elf32_Ehdr);
  ehdr.e_phentsize = sizeof(Elf32_Phdr);
  ehdr.e_phnum = 1;

  Elf32_Phdr phdr{};
  phdr.p_type = PT_LOAD;
  phdr.p_offset = sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr);
  phdr.p_vaddr = 0;
  phdr.p_paddr = 0;
  phdr.p_filesz = code.size();
  phdr.p_memsz = code.size();
  phdr.p_flags = PF_R | PF_X;
  phdr.p_align = 0x1000;

  std::ofstream f(path, std::ios::binary);
  f.write(reinterpret_cast<const char*>(&ehdr), sizeof(ehdr));
  f.write(reinterpret_cast<const char*>(&phdr), sizeof(phdr));
  f.write(reinterpret_cast<const char*>(code.data()), code.size());
  return path;
}
} // namespace

TEST(SimulatorTest, LoadAndRunPropagatesExitCode) {
  // addi a0, x0, 42 (0x02A00513); addi a7, x0, 93 (0x05D00893); ecall
  std::vector<uint8_t> bin = {
      0x13, 0x05, 0xA0, 0x02, 0x93, 0x08, 0xD0, 0x05, 0x73, 0x00, 0x00, 0x00,
  };
  auto path = write_temp_elf(bin);
  Simulator sim;
  auto result = sim.load(path);
  std::filesystem::remove(path);

  ASSERT_TRUE(result.has_value());
  auto outcome = sim.run();
  ASSERT_TRUE(outcome.has_value());
  EXPECT_EQ(*outcome, 42);
  EXPECT_TRUE(sim.halted());
  EXPECT_EQ(sim.exit_code(), 42);
}

TEST(SimulatorTest, StepExecutesSingleInstruction) {
  // addi x1, x0, 1; (run loop would fall off the end after 1 more)
  std::vector<uint8_t> bin = {0x93, 0x00, 0x10, 0x00};
  auto path = write_temp_elf(bin);
  Simulator sim;
  ASSERT_TRUE(sim.load(path));
  std::filesystem::remove(path);

  sim.step();

  EXPECT_EQ(sim.instruction_count(), 1u);
  EXPECT_FALSE(sim.halted());
}

TEST(SimulatorTest, RunCountsExecutedInstructions) {
  // addi x1, x0, 1; addi x2, x0, 2; add x3, x1, x2
  std::vector<uint8_t> bin = {
      0x93, 0x00, 0x10, 0x00, 0x13, 0x01, 0x20, 0x00, 0xB3, 0x81, 0x20, 0x00,
  };
  auto path = write_temp_elf(bin);
  Simulator sim;
  ASSERT_TRUE(sim.load(path));
  std::filesystem::remove(path);

  auto outcome = sim.run();
  EXPECT_FALSE(outcome.has_value()); // fetch walks off the 12-byte image

  EXPECT_EQ(sim.instruction_count(), 3u);
  EXPECT_FALSE(sim.halted());
}

TEST(SimulatorTest, LoadMissingFileReturnsError) {
  Simulator sim;
  auto result = sim.load("/nonexistent/riscemu_missing.elf");
  ASSERT_FALSE(result.has_value());
  EXPECT_FALSE(result.error().empty());
}

TEST(SimulatorTest, LoadGarbageFileReturnsError) {
  auto path = std::filesystem::temp_directory_path() /
              ("riscemu_garbage_" + std::to_string(std::rand()) + ".bin");
  std::ofstream f(path, std::ios::binary);
  f.write("not an elf file at all", 22);
  f.close();

  Simulator sim;
  auto result = sim.load(path);
  std::filesystem::remove(path);

  ASSERT_FALSE(result.has_value());
  EXPECT_FALSE(result.error().empty());
}