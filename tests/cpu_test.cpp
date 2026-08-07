#include "core/cpu.h"
#include "core/registers.h"
#include "loader/elf_loader.h"
#include "memory/memory.h"
#include "system/syscall.h"

#include <elf.h>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace {
std::filesystem::path write_temp_elf(const std::vector<uint8_t>& code) {
  auto path = std::filesystem::temp_directory_path() /
              ("riscemu_cpu_" + std::to_string(std::rand()) + ".elf");

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

// The component stack a Simulator would own, assembled directly so the test
// can reach the CPU's RISCEMU_TESTING accessors.
struct Machine {
  Memory mem{4 * 1024 * 1024};
  Syscall sys{mem};
  CPU cpu{mem, sys};

  bool load(const std::filesystem::path& path) {
    auto res = ELFLoader::load(path, mem);
    if (!res) {
      return false;
    }
    cpu.initialize(res->entry_point, res->max_addr,
                   static_cast<uint32_t>(mem.size_bytes()) - 16);
    return true;
  }
};
} // namespace

TEST(CpuRunTest, ExecutesLinearProgramAndStopsAtProgramEnd) {
  // addi x1, x0, 1
  // addi x2, x0, 2
  // add  x3, x1, x2
  // (end of program — PC walks off the end)
  std::vector<uint8_t> bin = {
      0x93, 0x00, 0x10, 0x00, 0x13, 0x01, 0x20, 0x00, 0xB3, 0x81, 0x20, 0x00,
  };
  auto path = write_temp_elf(bin);
  Machine machine;
  ASSERT_TRUE(machine.load(path));
  std::filesystem::remove(path);

  machine.cpu.run();

  EXPECT_EQ(machine.cpu.regs().read(1), 1u);
  EXPECT_EQ(machine.cpu.regs().read(2), 2u);
  EXPECT_EQ(machine.cpu.regs().read(3), 3u);
  EXPECT_EQ(machine.cpu.regs().pc, 12u); // 3 instructions × 4 bytes
}

TEST(CpuRunTest, ForwardJalSkipsOverInstructions) {
  // addi x1, x0, 1
  // addi x2, x0, 2
  // add  x3, x1, x2
  // jal  x0, +12  ; skip to PC=0x18 (the next JAL, which jumps out)
  // addi x4, x0, 99  ; skipped
  // addi x4, x0, 100 ; skipped
  // jal  x0, +8   ; target PC=0x20, out of program bounds (28 bytes) → exits
  std::vector<uint8_t> bin = {
      0x93,
      0x00,
      0x10,
      0x00,
      0x13,
      0x01,
      0x20,
      0x00,
      0xB3,
      0x81,
      0x20,
      0x00,
      // jal x0, +12 = 0x00C0006F
      0x6F,
      0x00,
      0xC0,
      0x00,
      0x93,
      0x01,
      0x30,
      0x06,
      0x93,
      0x01,
      0x40,
      0x06,
      // jal x0, +8 = 0x0080006F
      0x6F,
      0x00,
      0x80,
      0x00,
  };
  auto path = write_temp_elf(bin);
  Machine machine;
  ASSERT_TRUE(machine.load(path));
  std::filesystem::remove(path);

  machine.cpu.run();

  EXPECT_EQ(machine.cpu.regs().read(1), 1u);
  EXPECT_EQ(machine.cpu.regs().read(2), 2u);
  EXPECT_EQ(machine.cpu.regs().read(3), 3u);
  EXPECT_EQ(machine.cpu.regs().read(4), 0u); // both writes were skipped
  EXPECT_GE(machine.cpu.regs().pc,
            28u); // ran off the end (28 bytes = program size)
}

TEST(CpuRunTest, BackwardJalRepeatsLastTwoInstructions) {
  // The run loop has no iteration cap; this test bounds itself by setting a
  // tight max-iteration safety counter in the test driver. The point of the
  // test is to verify the fetch loop honours regs_.pc updates from JAL, not
  // to verify a fixed iteration count.
  //
  // Program layout (4 instrs, 16 bytes):
  //   PC=0x00: addi x5, x0, 0
  //   PC=0x04: addi x5, x5, 1
  //   PC=0x08: addi x5, x5, 1
  //   PC=0x0C: jal  x0, -8        ; target = 0x04 → loop
  //
  // Bounded by the run loop's external timeout; the executor unit tests cover
  // the correctness of the JAL target computation.
  std::vector<uint8_t> bin = {
      // addi x5, x0, 0
      0x93,
      0x02,
      0x00,
      0x00,
      // addi x5, x5, 1
      0x93,
      0x82,
      0x12,
      0x00,
      // addi x5, x5, 1
      0x93,
      0x82,
      0x12,
      0x00,
      // jal x0, -8  → 0xFF9FF06F
      0x6F,
      0xF0,
      0x9F,
      0xFF,
  };
  auto path = write_temp_elf(bin);
  Machine machine;
  ASSERT_TRUE(machine.load(path));
  std::filesystem::remove(path);

  // Run a fixed number of steps to bound the test.
  for (int i = 0; i < 5; ++i) {
    machine.cpu.step();
  }

  // After 5 steps starting at PC=0x00:
  //   step 0: addi x5=0
  //   step 1: addi x5=1
  //   step 2: addi x5=2
  //   step 3: jal back to 0x04
  //   step 4: addi x5=3
  EXPECT_EQ(machine.cpu.regs().read(5), 3u);
  EXPECT_EQ(machine.cpu.regs().pc, 0x08u); // PC after the most recent addi
}

TEST(CpuRunTest, JalrExitsProgramByJumpingOutOfBounds) {
  // Program layout (3 instrs, 12 bytes):
  //   PC=0x00: lui   x1, 0x10     ; x1 = 0x10000 (way out of bounds)
  //   PC=0x04: jalr  x0, x1, 0    ; target = x1 & ~1 = 0x10000, exits run
  std::vector<uint8_t> bin = {
      // lui x1, 0x10   → opcode 0x37, rd=1, imm20=0x10
      //               → 0x10 << 12 | (1 << 7) | 0x37 = 0x000100B7
      // little-endian bytes: B7 00 01 00
      0xB7,
      0x00,
      0x01,
      0x00,
      // jalr x0, x1, 0  → opcode 0x67, rd=0, funct3=0, rs1=1, imm=0
      //                → (1 << 15) | 0x67 = 0x00008067
      // little-endian bytes: 67 80 00 00
      0x67,
      0x80,
      0x00,
      0x00,
  };
  auto path = write_temp_elf(bin);
  Machine machine;
  ASSERT_TRUE(machine.load(path));
  std::filesystem::remove(path);

  machine.cpu.run();

  EXPECT_EQ(machine.cpu.regs().read(1), 0x10000u);
  // PC was set to 0x10000 by the JALR, exiting the run loop.
  EXPECT_GE(machine.cpu.regs().pc, 12u);
}

TEST(CpuRunTest, BneLoopCalculatesSum) {
  // Simple countdown loop that sums values from 3 down to 1.
  //   addi x1, x0, 3    ; n = 3
  //   addi x2, x0, 0    ; sum = 0
  // loop:
  //   add  x2, x2, x1   ; sum += n
  //   addi x1, x1, -1   ; n--
  //   bne  x1, x0, loop ; if n != 0, goto loop (-8 bytes)
  //   jal  x0, +8       ; jump out of program to end test
  std::vector<uint8_t> bin = {
      0x93, 0x00,
      0x30, 0x00, // addi x1, x0, 3 (0x00300093)
      0x13, 0x01,
      0x00, 0x00, // addi x2, x0, 0 (0x00000113)
      0x33, 0x01,
      0x11, 0x00, // loop: add x2, x2, x1 (rd=2, rs1=2, rs2=1) -> 0x00110133
      0x93, 0x80,
      0xF0, 0xFF, // addi x1, x1, -1 (rd=1, rs1=1, imm=-1) -> 0xFFF08093
      0xE3, 0x9C,
      0x00, 0xFE, // bne x1, x0, loop (-8) -> 0xFE009CE3
      0x6F, 0x00,
      0x80, 0x00, // jal x0, +8
  };
  auto path = write_temp_elf(bin);
  Machine machine;
  ASSERT_TRUE(machine.load(path));
  std::filesystem::remove(path);

  machine.cpu.run();

  EXPECT_EQ(machine.cpu.regs().read(2), 6u); // 3+2+1 = 6
  EXPECT_EQ(machine.cpu.regs().read(1), 0u);
}

TEST(CpuRunTest, EcallExitHaltsWithExitCode) {
  // addi  a0, x0, 7    ; exit code 7
  // addi  a7, x0, 93    ; SYS_exit
  // ecall                ; 0x00000073
  std::vector<uint8_t> bin = {
      // addi a0(x10), x0, 7  -> 0x00700513
      0x13,
      0x05,
      0x70,
      0x00,
      // addi a7(x17), x0, 93 -> 0x05D00893
      0x93,
      0x08,
      0xD0,
      0x05,
      // ecall -> 0x00000073
      0x73,
      0x00,
      0x00,
      0x00,
  };
  auto path = write_temp_elf(bin);
  Machine machine;
  ASSERT_TRUE(machine.load(path));
  std::filesystem::remove(path);

  machine.cpu.run();

  EXPECT_TRUE(machine.cpu.halted());
  EXPECT_EQ(machine.cpu.exit_code(), 7);
  EXPECT_EQ(machine.cpu.regs().read(10), 7u);
  EXPECT_EQ(machine.cpu.regs().pc,
            8u); // halt happens without advancing past ECALL
}
