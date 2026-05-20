#include "cpu.h"
#include "instructions.h"


Instruction::Instruction(uint32_t raw) : raw(raw) {}
Instruction::~Instruction() {}

void RTypeInstruction::decode(){
  funct7 = raw >> 25 & 0b1111111;
  rs2 = (raw >> 20) & 0b11111;
  rs1 = (raw >> 15) & 0b11111;
  funct3 = static_cast<Funct3>((raw >> 12) & 0b111);
  rd = (raw >> 7) & 0b11111;
  opcode = static_cast<OpCode>(raw & 0b1111111);
}

RTypeInstruction::RTypeInstruction(uint32_t raw) : Instruction(raw) {
  decode();
}
void RTypeInstruction::execute(CPU &cpu) {
  switch (funct3) {
  case Funct3::addSub:
    if (funct7 == 0) {
      cpu.registers[rd] = cpu.registers[rs1] + cpu.registers[rs2];
    } else
      cpu.registers[rd] = cpu.registers[rs1] - cpu.registers[rs2];
    break;
  case Funct3::sll:
    cpu.registers[rd] = cpu.registers[rs1] << (cpu.registers[rs2] & 0b1111);
    break;
  case Funct3::slt:
    cpu.registers[rd] = static_cast<int32_t>(cpu.registers[rs1]) <
                        static_cast<int32_t>(cpu.registers[rs2]);
    break;
  case Funct3::sltu:
    cpu.registers[rd] = cpu.registers[rs1] < cpu.registers[rs2];
    break;
  case Funct3::xorr:
    cpu.registers[rd] = cpu.registers[rs1] ^ cpu.registers[rs2];
    break;
  case Funct3::sr:
    if (funct7 == 0) {
      cpu.registers[rd] = cpu.registers[rs1] >> (cpu.registers[rs2] & 0b1111);
    } else {
      cpu.registers[rd] = static_cast<int32_t>(cpu.registers[rs1]) >>
                          (cpu.registers[rs2] & 0b1111);
    }
    break;
  case Funct3::orr:
    cpu.registers[rd] = cpu.registers[rs1] | cpu.registers[rs2];
    break;
  case Funct3::andd:
    cpu.registers[rd] = cpu.registers[rs1] & cpu.registers[rs2];
    break;

  default:
    break;
  }
}

std::string RTypeInstruction::toString() const {
  std::stringstream s;
  s << std::hex << std::setw(8) << std::setfill('0') << raw << std::endl;
  s << "opcode = " << static_cast<uint8_t>(opcode) << std::endl;
  s << "funct3 = " << static_cast<uint8_t>(funct3) << std::endl;
  s << "funct7 = " << static_cast<uint8_t>(funct7) << std::endl;
  s << "rs1 = " << rs1 << std::endl;
  s << "rs2 = " << rs2 << std::endl;
  s << "rd = " << rd << std::endl;

  return s.str();
}