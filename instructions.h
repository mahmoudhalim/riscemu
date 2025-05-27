#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H
#include <cstdint>
#include <iomanip>
#include <iostream>
typedef enum class OpCode : uint8_t { Rtype = 0x33 } OpCode;
typedef enum class Funct3 : uint8_t {
  addSub = 0x0,
  sll = 0x1,
  slt = 0x2,
  sltu = 0x3,
  xorr = 0x4,
  sr = 0x5,
  orr = 0x6,
  andd = 0x7
} Funct3;


struct Rinstruction {
  uint32_t raw;
  uint8_t funct7;
  uint8_t rs2;
  uint8_t rs1;
  Funct3 funct3;
  uint8_t rd;
  OpCode opcode;

  Rinstruction(uint32_t raw) : raw(raw) { decode(); }
  void decode() {
    funct7 = raw >> 25 & 0b1111111;
    rs2 = (raw >> 20) & 0b11111;
    rs1 = (raw >> 15) & 0b11111;
    funct3 = static_cast<Funct3>((raw >> 12) & 0b111);
    rd = (raw >> 7) & 0b11111;
    opcode = static_cast<OpCode>(raw & 0b1111111);
  }
  void print() {
    std::cout << std::hex << std::setw(8) << std::setfill('0') << raw
              << std::endl;
    std::cout << "opcode = " << static_cast<uint8_t>(opcode) << std::endl;
    std::cout << "funct3 = " << static_cast<uint8_t>(funct3) << std::endl;
    std::cout << "funct7 = " << static_cast<uint8_t>(funct7) << std::endl;
    std::cout << "rs1 = " << rs1 << std::endl;
    std::cout << "rs2 = " << rs2 << std::endl;
    std::cout << "rd = " << rd << std::endl;
    
  }
};
#endif // INSTRUCTIONS_H