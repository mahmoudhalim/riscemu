#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>

class CPU;
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

class Instruction {
public:
  uint32_t raw;
  Instruction(uint32_t raw);

  virtual void decode() = 0;
  virtual std::string toString() const = 0;
  virtual void execute(CPU &cpu) = 0;
  virtual ~Instruction() = 0;
};
class RTypeInstruction : public Instruction {
public:
  uint8_t funct7;
  uint8_t rs2;
  uint8_t rs1;
  Funct3 funct3;
  uint8_t rd;
  OpCode opcode;

  RTypeInstruction(uint32_t raw);

  void decode() override;

  void execute(CPU &cpu) override;
  std::string toString() const override; 

  ~RTypeInstruction() {}
};
#endif // INSTRUCTIONS_H