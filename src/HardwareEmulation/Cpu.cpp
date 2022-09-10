#include "NumToHexStringConvertor.hpp"

#include "HardwareEmulation/Cpu.hpp"

#ifdef NESTEST_DEBUG
#include "HardwareEmulation/NestestLogTester.hpp"
#endif // NESTEST_DEBUG

Cpu::Cpu(WriteFunction mmu_write, ReadFunction mmu_read) : 
    _mmu_write(std::move(mmu_write)),
    _mmu_read(std::move(mmu_read)),
    _address_mode_mapper({
        {AMode::ACCUM, std::bind(&Cpu::accum_addr, this)},
        {AMode::IMM, std::bind(&Cpu::imm_addr, this)},
        {AMode::ABSOLUTE, std::bind(&Cpu::absolute_addr, this)},
        {AMode::ZP, std::bind(&Cpu::zp_addr, this)},
        {AMode::I_ZP_X, std::bind(&Cpu::i_zp_x_addr, this)},
        {AMode::I_ZP_Y, std::bind(&Cpu::i_zp_y_addr, this)},
        {AMode::I_ABSOLUTE_X, std::bind(&Cpu::i_absolute_addr_x, this)},
        {AMode::I_ABSOLUTE_Y, std::bind(&Cpu::i_absolute_addr_y, this)},
        {AMode::IMPLIED, std::bind(&Cpu::implied_addr, this)},
        {AMode::RELATIVE, std::bind(&Cpu::relative_addr, this)},
        {AMode::I_INDIRECT, std::bind(&Cpu::i_indirect_addr, this)},
        {AMode::INDIRECT_I, std::bind(&Cpu::indirect_i_addr, this)},
        {AMode::INDIRECT, std::bind(&Cpu::indirect_addr, this)}
    }),
    _instruction_type_mapper({
        {IType::ADC, std::bind(&Cpu::Adc, this, std::placeholders::_1)},
        {IType::AND, std::bind(&Cpu::And, this, std::placeholders::_1)},
        {IType::ASL, std::bind(&Cpu::Asl, this, std::placeholders::_1)},
        {IType::BCC, std::bind(&Cpu::Bcc, this, std::placeholders::_1)},
        {IType::BCS, std::bind(&Cpu::Bcs, this, std::placeholders::_1)},
        {IType::BEQ, std::bind(&Cpu::Beq, this, std::placeholders::_1)},
        {IType::BIT, std::bind(&Cpu::Bit, this, std::placeholders::_1)},
        {IType::BMI, std::bind(&Cpu::Bmi, this, std::placeholders::_1)},
        {IType::BNE, std::bind(&Cpu::Bne, this, std::placeholders::_1)},
        {IType::BPL, std::bind(&Cpu::Bpl, this, std::placeholders::_1)},
        {IType::BRK, std::bind(&Cpu::Brk, this, std::placeholders::_1)},
        {IType::BVC, std::bind(&Cpu::Bvc, this, std::placeholders::_1)},
        {IType::BVS, std::bind(&Cpu::Bvs, this, std::placeholders::_1)},
        {IType::CLC, std::bind(&Cpu::Clc, this, std::placeholders::_1)},
        {IType::CLD, std::bind(&Cpu::Cld, this, std::placeholders::_1)},
        {IType::CLI, std::bind(&Cpu::Cli, this, std::placeholders::_1)},
        {IType::CLV, std::bind(&Cpu::Clv, this, std::placeholders::_1)},
        {IType::CMP, std::bind(&Cpu::Cmp, this, std::placeholders::_1)},
        {IType::CPX, std::bind(&Cpu::Cpx, this, std::placeholders::_1)},
        {IType::CPY, std::bind(&Cpu::Cpy, this, std::placeholders::_1)},
        {IType::DEC, std::bind(&Cpu::Dec, this, std::placeholders::_1)},
        {IType::DEX, std::bind(&Cpu::Dex, this, std::placeholders::_1)},
        {IType::DEY, std::bind(&Cpu::Dey, this, std::placeholders::_1)},
        {IType::EOR, std::bind(&Cpu::Eor, this, std::placeholders::_1)},
        {IType::INC, std::bind(&Cpu::Inc, this, std::placeholders::_1)},
        {IType::INX, std::bind(&Cpu::Inx, this, std::placeholders::_1)},
        {IType::INY, std::bind(&Cpu::Iny, this, std::placeholders::_1)},
        {IType::JMP, std::bind(&Cpu::Jmp, this, std::placeholders::_1)},
        {IType::JSR, std::bind(&Cpu::Jsr, this, std::placeholders::_1)},
        {IType::LDA, std::bind(&Cpu::Lda, this, std::placeholders::_1)},
        {IType::LDX, std::bind(&Cpu::Ldx, this, std::placeholders::_1)},
        {IType::LDY, std::bind(&Cpu::Ldy, this, std::placeholders::_1)},
        {IType::LSR, std::bind(&Cpu::Lsr, this, std::placeholders::_1)},
        {IType::NOP, std::bind(&Cpu::Nop, this, std::placeholders::_1)},
        {IType::ORA, std::bind(&Cpu::Ora, this, std::placeholders::_1)},
        {IType::PHA, std::bind(&Cpu::Pha, this, std::placeholders::_1)},
        {IType::PHP, std::bind(&Cpu::Php, this, std::placeholders::_1)},
        {IType::PLA, std::bind(&Cpu::Pla, this, std::placeholders::_1)},
        {IType::PLP, std::bind(&Cpu::Plp, this, std::placeholders::_1)},
        {IType::ROL, std::bind(&Cpu::Rol, this, std::placeholders::_1)},
        {IType::ROR, std::bind(&Cpu::Ror, this, std::placeholders::_1)},
        {IType::RTI, std::bind(&Cpu::Rti, this, std::placeholders::_1)},
        {IType::RTS, std::bind(&Cpu::Rts, this, std::placeholders::_1)},
        {IType::SBC, std::bind(&Cpu::Sbc, this, std::placeholders::_1)},
        {IType::SEC, std::bind(&Cpu::Sec, this, std::placeholders::_1)},
        {IType::SED, std::bind(&Cpu::Sed, this, std::placeholders::_1)},
        {IType::SEI, std::bind(&Cpu::Sei, this, std::placeholders::_1)},
        {IType::STA, std::bind(&Cpu::Sta, this, std::placeholders::_1)},
        {IType::STX, std::bind(&Cpu::Stx, this, std::placeholders::_1)},
        {IType::STY, std::bind(&Cpu::Sty, this, std::placeholders::_1)},
        {IType::TAX, std::bind(&Cpu::Tax, this, std::placeholders::_1)},
        {IType::TAY, std::bind(&Cpu::Tay, this, std::placeholders::_1)},
        {IType::TSX, std::bind(&Cpu::Tsx, this, std::placeholders::_1)},
        {IType::TXA, std::bind(&Cpu::Txa, this, std::placeholders::_1)},
        {IType::TXS, std::bind(&Cpu::Txs, this, std::placeholders::_1)},
        {IType::TYA, std::bind(&Cpu::Tya, this, std::placeholders::_1)},
        {IType::MIA, std::bind(&Cpu::Nop, this, std::placeholders::_1)}
    }),
    _irq_vector_map({
        {IrqType::ABORT, std::pair<uint32_t,uint32_t>(ABORT_LSB, ABORT_MSB)},
        {IrqType::COP, std::pair<uint32_t,uint32_t>(COP_LSB, COP_MSB)},
        {IrqType::NORMAL_IRQ, std::pair<uint32_t,uint32_t>(NORMAL_IRQ_LSB, NORMAL_IRQ_MSB)},
        {IrqType::BRK, std::pair<uint32_t,uint32_t>(BRK_LSB, BRK_MSB)},
        {IrqType::NMI, std::pair<uint32_t,uint32_t>(NMI_LSB, NMI_MSB)},
        {IrqType::RESET, std::pair<uint32_t,uint32_t>(RESET_LSB, RESET_MSB)},
    }),
    _opcode_vector({
        /*          0                                       1                                   2                                    3                               4                              5                                6                              7                               8                                  9                                       A                              B                                        C                                       D                                      E                                        F
/*0*/   {IType::BRK, AMode::IMPLIED, 7},    {IType::ORA, AMode::I_INDIRECT, 6}, {IType::MIA, AMode::IMPLIED, 2},    {IType::MIA, AMode::I_INDIRECT, 2}, {IType::MIA, AMode::ZP, 3},     {IType::ORA, AMode::ZP, 3},     {IType::ASL, AMode::ZP, 5},     {IType::MIA, AMode::ZP, 5},     {IType::PHP, AMode::IMPLIED, 3}, {IType::ORA, AMode::IMM, 2},           {IType::ASL, AMode::ACCUM, 2}, {IType::MIA, AMode::IMM, 2},           {IType::MIA, AMode::ABSOLUTE, 4},       {IType::ORA, AMode::ABSOLUTE, 4},       {IType::ASL, AMode::ABSOLUTE, 6},       {IType::MIA, AMode::ABSOLUTE, 6},
/*1*/   {IType::BPL, AMode::RELATIVE, 2},   {IType::ORA, AMode::INDIRECT_I, 5}, {IType::MIA, AMode::IMPLIED, 2},    {IType::MIA, AMode::INDIRECT_I, 8}, {IType::MIA, AMode::I_ZP_X, 4}, {IType::ORA, AMode::I_ZP_X, 4}, {IType::ASL, AMode::I_ZP_X, 6}, {IType::MIA, AMode::I_ZP_X, 6}, {IType::CLC, AMode::IMPLIED, 2}, {IType::ORA, AMode::I_ABSOLUTE_Y, 4},  {IType::MIA, AMode::IMPLIED, 2}, {IType::MIA, AMode::I_ABSOLUTE_Y, 7},  {IType::MIA, AMode::I_ABSOLUTE_X, 4},   {IType::ORA, AMode::I_ABSOLUTE_X, 4},   {IType::ASL, AMode::I_ABSOLUTE_X, 7},   {IType::MIA, AMode::I_ABSOLUTE_X, 7},
/*2*/   {IType::JSR, AMode::ABSOLUTE, 6},   {IType::AND, AMode::I_INDIRECT, 6}, {IType::MIA, AMode::IMPLIED, 2},    {IType::MIA, AMode::I_INDIRECT, 8}, {IType::BIT, AMode::ZP, 3},     {IType::AND, AMode::ZP, 3},     {IType::ROL, AMode::ZP, 5},     {IType::MIA, AMode::ZP, 5},     {IType::PLP, AMode::IMPLIED, 4}, {IType::AND, AMode::IMM, 2},           {IType::ROL, AMode::ACCUM, 2}, {IType::MIA, AMode::IMM, 2},           {IType::BIT, AMode::ABSOLUTE, 4},       {IType::AND, AMode::ABSOLUTE, 4},       {IType::ROL, AMode::ABSOLUTE, 6},       {IType::MIA, AMode::ABSOLUTE, 6},
/*3*/   {IType::BMI, AMode::RELATIVE, 2},   {IType::AND, AMode::INDIRECT_I, 5}, {IType::MIA, AMode::IMPLIED, 2},    {IType::MIA, AMode::I_INDIRECT, 8}, {IType::MIA, AMode::I_ZP_X, 4}, {IType::AND, AMode::I_ZP_X, 4}, {IType::ROL, AMode::I_ZP_X, 6}, {IType::MIA, AMode::I_ZP_X, 6}, {IType::SEC, AMode::IMPLIED, 2}, {IType::AND, AMode::I_ABSOLUTE_Y, 4},  {IType::MIA, AMode::IMPLIED, 2}, {IType::MIA, AMode::I_ABSOLUTE_Y, 7},  {IType::MIA, AMode::I_ABSOLUTE_X, 4},   {IType::AND, AMode::I_ABSOLUTE_X, 4},   {IType::ROL, AMode::I_ABSOLUTE_X, 7},   {IType::MIA, AMode::I_ABSOLUTE_X, 7},
/*4*/   {IType::RTI, AMode::IMPLIED, 6},    {IType::EOR, AMode::I_INDIRECT, 6}, {IType::MIA, AMode::IMPLIED, 2},    {IType::MIA, AMode::I_INDIRECT, 8}, {IType::MIA, AMode::ZP, 3},     {IType::EOR, AMode::ZP, 3},     {IType::LSR, AMode::ZP, 5},     {IType::MIA, AMode::ZP, 5},     {IType::PHA, AMode::IMPLIED, 3}, {IType::EOR, AMode::IMM, 2},           {IType::LSR, AMode::ACCUM, 2}, {IType::MIA, AMode::IMM, 2},           {IType::JMP, AMode::ABSOLUTE, 3},       {IType::EOR, AMode::ABSOLUTE, 4},       {IType::LSR, AMode::ABSOLUTE, 6},       {IType::MIA, AMode::ABSOLUTE, 6},
/*5*/   {IType::BVC, AMode::RELATIVE, 2},   {IType::EOR, AMode::INDIRECT_I, 5}, {IType::MIA, AMode::IMPLIED, 2},    {IType::MIA, AMode::INDIRECT_I, 8}, {IType::MIA, AMode::I_ZP_X, 4}, {IType::EOR, AMode::I_ZP_X, 4}, {IType::LSR, AMode::I_ZP_X, 6}, {IType::MIA, AMode::I_ZP_X, 6}, {IType::CLI, AMode::IMPLIED, 2}, {IType::EOR, AMode::I_ABSOLUTE_Y, 4},  {IType::MIA, AMode::IMPLIED, 2}, {IType::MIA, AMode::I_ABSOLUTE_Y, 7},  {IType::MIA, AMode::I_ABSOLUTE_X, 4},   {IType::EOR, AMode::I_ABSOLUTE_X, 4},   {IType::LSR, AMode::I_ABSOLUTE_X, 7},   {IType::MIA, AMode::I_ABSOLUTE_X, 7},
/*6*/   {IType::RTS, AMode::IMPLIED, 6},    {IType::ADC, AMode::I_INDIRECT, 6}, {IType::MIA, AMode::IMPLIED, 2},    {IType::MIA, AMode::I_INDIRECT, 8}, {IType::MIA, AMode::ZP, 3},     {IType::ADC, AMode::ZP, 3},     {IType::ROR, AMode::ZP, 5},     {IType::MIA, AMode::ZP, 5},     {IType::PLA, AMode::IMPLIED, 4}, {IType::ADC, AMode::IMM, 2},           {IType::ROR, AMode::ACCUM, 2}, {IType::MIA, AMode::IMM, 2},           {IType::JMP, AMode::INDIRECT, 5},       {IType::ADC, AMode::ABSOLUTE, 4},       {IType::ROR, AMode::ABSOLUTE, 6},       {IType::MIA, AMode::ABSOLUTE, 6},
/*7*/   {IType::BVS, AMode::RELATIVE, 2},   {IType::ADC, AMode::INDIRECT_I, 5}, {IType::MIA, AMode::IMPLIED, 2},    {IType::MIA, AMode::INDIRECT_I, 8}, {IType::MIA, AMode::I_ZP_X, 4}, {IType::ADC, AMode::I_ZP_X, 4}, {IType::ROR, AMode::I_ZP_X, 6}, {IType::MIA, AMode::I_ZP_X, 6}, {IType::SEI, AMode::IMPLIED, 2}, {IType::ADC, AMode::I_ABSOLUTE_Y, 4},  {IType::MIA, AMode::IMPLIED, 2}, {IType::MIA, AMode::I_ABSOLUTE_Y, 7},  {IType::MIA, AMode::I_ABSOLUTE_X, 4},   {IType::ADC, AMode::I_ABSOLUTE_X, 4},   {IType::ROR, AMode::I_ABSOLUTE_X, 7},   {IType::MIA, AMode::I_ABSOLUTE_X, 7},
/*8*/   {IType::MIA, AMode::IMM, 2},        {IType::STA, AMode::I_INDIRECT, 6}, {IType::MIA, AMode::IMM, 2},        {IType::MIA, AMode::I_INDIRECT, 6}, {IType::STY, AMode::ZP, 3},     {IType::STA, AMode::ZP, 3},     {IType::STX, AMode::ZP, 3},     {IType::MIA, AMode::ZP, 3},     {IType::DEY, AMode::IMPLIED, 2}, {IType::MIA, AMode::IMM, 2},           {IType::TXA, AMode::IMPLIED, 2}, {IType::MIA, AMode::IMM, 2},           {IType::STY, AMode::ABSOLUTE, 4},       {IType::STA, AMode::ABSOLUTE, 4},       {IType::STX, AMode::ABSOLUTE, 4},       {IType::MIA, AMode::ABSOLUTE, 4},
/*9*/   {IType::BCC, AMode::RELATIVE, 2},   {IType::STA, AMode::INDIRECT_I, 6}, {IType::MIA, AMode::IMPLIED, 2},    {IType::MIA, AMode::INDIRECT_I, 6}, {IType::STY, AMode::I_ZP_X, 4}, {IType::STA, AMode::I_ZP_X, 4}, {IType::STX, AMode::I_ZP_Y, 4}, {IType::MIA, AMode::I_ZP_Y, 4}, {IType::TYA, AMode::IMPLIED, 2}, {IType::STA, AMode::I_ABSOLUTE_Y, 5},  {IType::TXS, AMode::IMPLIED, 2}, {IType::MIA, AMode::I_ABSOLUTE_Y, 5},  {IType::MIA, AMode::I_ABSOLUTE_X, 5},   {IType::STA, AMode::I_ABSOLUTE_X, 5},   {IType::MIA, AMode::I_ABSOLUTE_Y, 5},   {IType::MIA, AMode::I_ABSOLUTE_Y, 5},
/*A*/   {IType::LDY, AMode::IMM, 2},        {IType::LDA, AMode::I_INDIRECT, 6}, {IType::LDX, AMode::IMM, 2},        {IType::MIA, AMode::I_INDIRECT, 6}, {IType::LDY, AMode::ZP, 3},     {IType::LDA, AMode::ZP, 3},     {IType::LDX, AMode::ZP, 3},     {IType::MIA, AMode::ZP, 3},     {IType::TAY, AMode::IMPLIED, 2}, {IType::LDA, AMode::IMM, 2},           {IType::TAX, AMode::IMPLIED, 2}, {IType::MIA, AMode::IMM, 2},           {IType::LDY, AMode::ABSOLUTE, 4},       {IType::LDA, AMode::ABSOLUTE, 4},       {IType::LDX, AMode::ABSOLUTE, 4},       {IType::MIA, AMode::ABSOLUTE, 4},
/*B*/   {IType::BCS, AMode::RELATIVE, 2},   {IType::LDA, AMode::INDIRECT_I, 5}, {IType::MIA, AMode::IMPLIED, 2},    {IType::MIA, AMode::INDIRECT_I, 5}, {IType::LDY, AMode::I_ZP_X, 4}, {IType::LDA, AMode::I_ZP_X, 4}, {IType::LDX, AMode::I_ZP_Y, 4}, {IType::MIA, AMode::I_ZP_Y, 4}, {IType::CLV, AMode::IMPLIED, 2}, {IType::LDA, AMode::I_ABSOLUTE_Y, 4},  {IType::TSX, AMode::IMPLIED, 2}, {IType::MIA, AMode::I_ABSOLUTE_Y, 4},  {IType::LDY, AMode::I_ABSOLUTE_X, 4},   {IType::LDA, AMode::I_ABSOLUTE_X, 4},   {IType::LDX, AMode::I_ABSOLUTE_Y, 4},   {IType::MIA, AMode::I_ABSOLUTE_Y, 4},
/*C*/   {IType::CPY, AMode::IMM, 2},        {IType::CMP, AMode::I_INDIRECT, 6}, {IType::MIA, AMode::IMM, 2},        {IType::MIA, AMode::I_INDIRECT, 8}, {IType::CPY, AMode::ZP, 3},     {IType::CMP, AMode::ZP, 3},     {IType::DEC, AMode::ZP, 5},     {IType::MIA, AMode::ZP, 5},     {IType::INY, AMode::IMPLIED, 2}, {IType::CMP, AMode::IMM, 2},           {IType::DEX, AMode::IMPLIED, 2}, {IType::MIA, AMode::IMM, 2},           {IType::CPY, AMode::ABSOLUTE, 4},       {IType::CMP, AMode::ABSOLUTE, 4},       {IType::DEC, AMode::ABSOLUTE, 6},       {IType::MIA, AMode::ABSOLUTE, 6},
/*D*/   {IType::BNE, AMode::RELATIVE, 2},   {IType::CMP, AMode::INDIRECT_I, 5}, {IType::MIA, AMode::IMPLIED, 2},    {IType::MIA, AMode::INDIRECT_I, 8}, {IType::MIA, AMode::I_ZP_X, 4}, {IType::CMP, AMode::I_ZP_X, 4}, {IType::DEC, AMode::I_ZP_X, 6}, {IType::MIA, AMode::I_ZP_X, 6}, {IType::CLD, AMode::IMPLIED, 2}, {IType::CMP, AMode::I_ABSOLUTE_Y, 4},  {IType::MIA, AMode::IMPLIED, 2}, {IType::MIA, AMode::I_ABSOLUTE_Y, 7},  {IType::MIA, AMode::I_ABSOLUTE_X, 4},   {IType::CMP, AMode::I_ABSOLUTE_X, 4},   {IType::DEC, AMode::I_ABSOLUTE_X, 7},   {IType::MIA, AMode::I_ABSOLUTE_X, 7},
/*E*/   {IType::CPX, AMode::IMM, 2},        {IType::SBC, AMode::I_INDIRECT, 6}, {IType::MIA, AMode::IMM, 2},        {IType::MIA, AMode::I_INDIRECT, 8}, {IType::CPX, AMode::ZP, 3},     {IType::SBC, AMode::ZP, 3},     {IType::INC, AMode::ZP, 5},     {IType::MIA, AMode::ZP, 5},     {IType::INX, AMode::IMPLIED, 2}, {IType::SBC, AMode::IMM, 2},           {IType::NOP, AMode::IMPLIED, 2}, {IType::MIA, AMode::IMM, 2},           {IType::CPX, AMode::ABSOLUTE, 4},       {IType::SBC, AMode::ABSOLUTE, 4},       {IType::INC, AMode::ABSOLUTE, 6},       {IType::MIA, AMode::ABSOLUTE, 6},
/*F*/   {IType::BEQ, AMode::RELATIVE, 2},   {IType::SBC, AMode::INDIRECT_I, 6}, {IType::MIA, AMode::IMPLIED, 2},    {IType::MIA, AMode::INDIRECT_I, 8}, {IType::MIA, AMode::I_ZP_X, 4}, {IType::SBC, AMode::I_ZP_X, 4}, {IType::INC, AMode::I_ZP_X, 6}, {IType::MIA, AMode::I_ZP_X, 6}, {IType::SED, AMode::IMPLIED, 2}, {IType::SBC, AMode::I_ABSOLUTE_Y, 4},  {IType::MIA, AMode::IMPLIED, 2}, {IType::MIA, AMode::I_ABSOLUTE_Y, 7},  {IType::MIA, AMode::I_ABSOLUTE_X, 4},   {IType::SBC, AMode::I_ABSOLUTE_X, 4},   {IType::INC, AMode::I_ABSOLUTE_X, 7},   {IType::MIA, AMode::I_ABSOLUTE_X, 7}
    })
{
    _loop_running = false;
}

void Cpu::CpuExecuteInstruction()
{
    static uint32_t index = 0;
    
    uint8_t opcode = CpuRead(_pc++);

    #ifdef NESTEST_DEBUG
    NestestLogTester::GetInstance()->DebugInstruction(*this, opcode, index);
    #endif // NESTEST_DEBUG
    _instruction_type_mapper[_opcode_vector[opcode].type](_opcode_vector[opcode].addrMode);
    index++;
}

void Cpu::CpuReset()
{
    _a = 0;
    _x = 0;
    _y = 0;
    _p = INTERRUPT_DISABLE_FLAG_MASK;
    _sp = 0xfd; // this is done to simulate the hacked stack insertions

    #ifdef NESTEST_DEBUG
    _pc = 0xc000;
    #else
    _pc = CpuRead(_irq_vector_map[IrqType::RESET].first) | (CpuRead(_irq_vector_map[IrqType::RESET].second) << 8);
    #endif // NESTEST_DEBUG

    _cycles = 7;
}

void Cpu::Irq()
{
    if (!GetFlag(INTERRUPT_DISABLE_FLAG_MASK))
    {
        SetFlag(INTERRUPT_DISABLE_FLAG_MASK, true);
        CpuWrite(STACK_OFFSET + _sp--, (_pc >> 8) & 0xff);
        CpuWrite(STACK_OFFSET + _sp--, _pc & 0xff);
        CpuWrite(STACK_OFFSET + _sp--, _p | RESERVED_FLAG_MASK);
        _pc = CpuRead(_irq_vector_map[IrqType::NORMAL_IRQ].first) | (CpuRead(_irq_vector_map[IrqType::NORMAL_IRQ].second) << 8);
        _cycles += 7;
    }
}

void Cpu::Nmi()
{
    SetFlag(INTERRUPT_DISABLE_FLAG_MASK, true);
    CpuWrite(STACK_OFFSET + _sp--, (_pc >> 8) & 0xff);
    CpuWrite(STACK_OFFSET + _sp--, _pc & 0xff);
    CpuWrite(STACK_OFFSET + _sp--, _p | RESERVED_FLAG_MASK);
    _pc = CpuRead(_irq_vector_map[IrqType::NMI].first) | (CpuRead(_irq_vector_map[IrqType::NMI].second) << 8);
    _cycles += 7;
}

void Cpu::SetFlag(uint8_t flagMask, bool val)
{
    if (val)
    {
        _p |= flagMask;
    }
    else
    {
        ClearFlag(flagMask);
    }
}

void Cpu::ClearFlag(uint8_t flagMask)
{
    _p &= ~(flagMask);
}

uint8_t Cpu::GetFlag(uint8_t flagMask)
{
    return (_p & flagMask)? 1 : 0;
}

void Cpu::CpuWrite(const uint16_t address, const uint8_t data)
{
    _mmu_write(address, data);
}

uint8_t Cpu::CpuRead(const uint16_t address)
{
    return _mmu_read(address);
}

uint16_t Cpu::accum_addr()
{
    return 0;
}

uint16_t Cpu::imm_addr()
{
    return _pc++;
}

uint16_t Cpu::absolute_addr()
{
    uint16_t lo = CpuRead(_pc++);
    uint16_t hi = CpuRead(_pc++);
    return lo | (hi << 8);
}

uint16_t Cpu::zp_addr()
{
    return CpuRead(_pc++);
}

uint16_t Cpu::i_zp_x_addr()
{
    return (CpuRead(_pc++) + _x) & 0x00ff;
}

uint16_t Cpu::i_zp_y_addr()
{
    return (CpuRead(_pc++) + _y) & 0x00ff;
}

uint16_t Cpu::i_absolute_addr_x()
{
    uint16_t lo = CpuRead(_pc++);
    uint16_t hi = CpuRead(_pc++);
    uint16_t addr = lo;
    addr |= (hi << 8);
    addr += _x;
    if ((addr & 0xff00) != hi)
    {
        _cycles++;
    }
    return addr;
}

uint16_t Cpu::i_absolute_addr_y()
{
    uint16_t lo = CpuRead(_pc++);
    uint16_t hi = CpuRead(_pc++);
    uint16_t addr = lo;
    addr |= (hi << 8);
    addr += _y;
    if ((addr & 0xff00) != hi)
    {
        _cycles++;
    }
    return addr;
}

uint16_t Cpu::implied_addr()
{
    return 0;
}

uint16_t Cpu::relative_addr()
{
    int8_t offset = CpuRead(_pc++);
    return _pc + offset;
}

uint16_t Cpu::i_indirect_addr()
{
    uint16_t zpg_addr = CpuRead(_pc++);
    uint16_t addr = 0;
    addr |= CpuRead((zpg_addr++ + _x) & 0xff)  & 0xff;
    addr |= (CpuRead((zpg_addr + _x) & 0xff) << 8);
    return addr;
}

uint16_t Cpu::indirect_i_addr()
{
    uint16_t zpg_addr = CpuRead(_pc++);
    uint16_t addr = 0;
    addr |= CpuRead(zpg_addr++);
    addr |= (CpuRead(zpg_addr & 0xff) << 8);
    if ((addr + _y) & 0xff00 != addr & 0xff00)
    {
        _cycles++;
    }
    return addr + _y;
}

uint16_t Cpu::indirect_addr()
{
    uint16_t ind_addr = 0;
    uint16_t addr = 0;
    ind_addr |= CpuRead(_pc++);
    ind_addr |= CpuRead(_pc++) << 8;
    addr |= CpuRead(ind_addr);
    // This line is in order to emulate one of the 6502 bugs
    // In the 6502 advencing the ind_addr to read the hi bits of the address did not effect the hi bits of the ind_addr
    ind_addr = (ind_addr & 0xff00) | ((ind_addr + 1) & 0xff); 
    addr |= (CpuRead(ind_addr) << 8);
    return addr;
}

void Cpu::Adc(const AMode addrMode)
{
    uint16_t addr = _address_mode_mapper[addrMode]();
    uint8_t data = CpuRead(addr);
    uint16_t temp = GetFlag(CARRY_FLAG_MASK) + data + _a;
    SetFlag(CARRY_FLAG_MASK, temp & 0xff00);
    SetFlag(ZERO_FLAG_MASK, !(temp & 0xff));
    SetFlag(OVERFLOW_FLAG_MASK, (_a ^ temp) & (data ^ temp) & 0x80);
    SetFlag(NEGATIVE_FLAG_MASK, temp & 0x80);
    _a = temp & 0xff;
}

void Cpu::And(const AMode addrMode)
{
    uint16_t addr = _address_mode_mapper[addrMode]();
    uint8_t data = CpuRead(addr);
    _a &= data;
    SetFlag(ZERO_FLAG_MASK, !(_a));
    SetFlag(NEGATIVE_FLAG_MASK, _a & 0x80);
}

void Cpu::Asl(const AMode addrMode)
{
    uint8_t data;
    uint16_t addr;
    if (addrMode == AMode::ACCUM)
    {
        data = _a;
    }
    else
    {
        addr = _address_mode_mapper[addrMode]();
        data = CpuRead(addr);
    }
    uint16_t temp = data << 1;
    SetFlag(CARRY_FLAG_MASK, temp & 0xff00);
    SetFlag(ZERO_FLAG_MASK, !(temp & 0xff));
    SetFlag(NEGATIVE_FLAG_MASK, temp & 0x80);
    if (addrMode == AMode::ACCUM)
    {
        _a = temp & 0xff;
    }
    else
    {
        CpuWrite(addr, temp & 0xff);
    }
}

void Cpu::Bcc(const AMode addrMode)
{
    
    uint16_t addr = _address_mode_mapper[addrMode]();
    if (!GetFlag(CARRY_FLAG_MASK))
    {
        if (addr & 0xff != _pc % 0xff)
        {
            _cycles++;
        }
        _pc = addr;
    }
}

void Cpu::Bcs(const AMode addrMode)
{
    
    uint16_t addr = _address_mode_mapper[addrMode]();
    if (GetFlag(CARRY_FLAG_MASK))
    {
        if (addr & 0xff != _pc % 0xff)
        {
            _cycles++;
        }
        _pc = addr;
    }
}

void Cpu::Beq(const AMode addrMode)
{
    
    uint16_t addr = _address_mode_mapper[addrMode]();
    if (GetFlag(ZERO_FLAG_MASK))
    {
        if (addr & 0xff != _pc % 0xff)
        {
            _cycles++;
        }
        _pc = addr;
    }
}

void Cpu::Bit(const AMode addrMode)
{
    uint16_t addr = _address_mode_mapper[addrMode]();
    uint8_t data = CpuRead(addr);
    uint16_t temp = _a & data;
    SetFlag(ZERO_FLAG_MASK, !(temp & 0xff));
    SetFlag(OVERFLOW_FLAG_MASK, data & 1 << 6);
    SetFlag(NEGATIVE_FLAG_MASK, data & 1 << 7);
}

void Cpu::Bmi(const AMode addrMode)
{
    
    uint16_t addr = _address_mode_mapper[addrMode]();
    if (GetFlag(NEGATIVE_FLAG_MASK))
    {
        if (addr & 0xff != _pc % 0xff)
        {
            _cycles++;
        }
        _pc = addr;
    }
}

void Cpu::Bne(const AMode addrMode)
{
    
    uint16_t addr = _address_mode_mapper[addrMode]();
    if (!GetFlag(ZERO_FLAG_MASK))
    {
        if (addr & 0xff != _pc % 0xff)
        {
            _cycles++;
        }
        _pc = addr;
    }
}

void Cpu::Bpl(const AMode addrMode)
{
    
    uint16_t addr = _address_mode_mapper[addrMode]();
    if (!GetFlag(NEGATIVE_FLAG_MASK))
    {
        if (addr & 0xff != _pc % 0xff)
        {
            _cycles++;
        }
        _pc = addr;
    }
}

void Cpu::Brk(const AMode addrMode)
{
    _pc++;
    SetFlag(INTERRUPT_DISABLE_FLAG_MASK, true);
    CpuWrite(STACK_OFFSET + _sp--, (_pc >> 8) & 0xff);
    CpuWrite(STACK_OFFSET + _sp--, _pc & 0xff);
    CpuWrite(STACK_OFFSET + _sp--, _p | BREAK_COMMAND_FLAG_MASK);
    _pc = CpuRead(_irq_vector_map[IrqType::BRK].first) | (CpuRead(_irq_vector_map[IrqType::BRK].second) << 8);
}

void Cpu::Bvc(const AMode addrMode)
{
    
    uint16_t addr = _address_mode_mapper[addrMode]();
    if (!GetFlag(OVERFLOW_FLAG_MASK))
    {
        if (addr & 0xff != _pc % 0xff)
        {
            _cycles++;
        }
        _pc = addr;
    }
}

void Cpu::Bvs(const AMode addrMode)
{
    
    uint16_t addr = _address_mode_mapper[addrMode]();
    if (GetFlag(OVERFLOW_FLAG_MASK))
    {
        if (addr & 0xff != _pc % 0xff)
        {
            _cycles++;
        }
        _pc = addr;
    }
}

void Cpu::Clc(const AMode addrMode)
{
    ClearFlag(CARRY_FLAG_MASK);
}

void Cpu::Cld(const AMode addrMode)
{
    ClearFlag(DECIMAL_MODE_FLAG_MASK);
}

void Cpu::Cli(const AMode addrMode)
{
    ClearFlag(INTERRUPT_DISABLE_FLAG_MASK);
}

void Cpu::Clv(const AMode addrMode)
{
    ClearFlag(OVERFLOW_FLAG_MASK);
}

void Cpu::Cmp(const AMode addrMode)
{
    uint16_t addr = _address_mode_mapper[addrMode]();
    uint8_t data = CpuRead(addr);
    uint16_t temp = _a - data;
    SetFlag(CARRY_FLAG_MASK,(_a >= data));
    SetFlag(ZERO_FLAG_MASK, !(temp & 0xff));
    SetFlag(NEGATIVE_FLAG_MASK, temp & 0x80);
}

void Cpu::Cpx(const AMode addrMode)
{
    uint16_t addr = _address_mode_mapper[addrMode]();
    uint8_t data = CpuRead(addr);
    uint16_t temp = _x - data;
    SetFlag(CARRY_FLAG_MASK,(_x >= data));
    SetFlag(ZERO_FLAG_MASK, !(temp & 0xff));
    SetFlag(NEGATIVE_FLAG_MASK, temp & 0x80);
}

void Cpu::Cpy(const AMode addrMode)
{
    uint16_t addr = _address_mode_mapper[addrMode]();
    uint8_t data = CpuRead(addr);
    uint16_t temp = _y - data;
    SetFlag(CARRY_FLAG_MASK,(_y >= data));
    SetFlag(ZERO_FLAG_MASK, !(temp & 0xff));
    SetFlag(NEGATIVE_FLAG_MASK, temp & 0x80);
}

void Cpu::Dec(const AMode addrMode)
{
    uint16_t addr = _address_mode_mapper[addrMode]();
    uint8_t data = CpuRead(addr) - 1;
    SetFlag(ZERO_FLAG_MASK, !(data));
    SetFlag(NEGATIVE_FLAG_MASK, data & 0x80);
    CpuWrite(addr, data);
}

void Cpu::Dex(const AMode addrMode)
{
    _x--;
    SetFlag(ZERO_FLAG_MASK, !(_x));
    SetFlag(NEGATIVE_FLAG_MASK, _x & 0x80);
}

void Cpu::Dey(const AMode addrMode)
{
    _y--;
    SetFlag(ZERO_FLAG_MASK, !(_y));
    SetFlag(NEGATIVE_FLAG_MASK, _y & 0x80);
}

void Cpu::Eor(const AMode addrMode)
{
    uint16_t addr = _address_mode_mapper[addrMode]();
    uint8_t data = CpuRead(addr);
    _a ^= data;
    SetFlag(ZERO_FLAG_MASK, !(_a));
    SetFlag(NEGATIVE_FLAG_MASK, _a & 0x80);
}

void Cpu::Inc(const AMode addrMode)
{
    uint16_t addr = _address_mode_mapper[addrMode]();
    uint8_t data = CpuRead(addr) + 1;
    SetFlag(ZERO_FLAG_MASK, !(data));
    SetFlag(NEGATIVE_FLAG_MASK, data & 0x80);
    CpuWrite(addr, data);
}

void Cpu::Inx(const AMode addrMode)
{
    _x++;
    SetFlag(ZERO_FLAG_MASK, !(_x));
    SetFlag(NEGATIVE_FLAG_MASK, _x & 0x80);
}

void Cpu::Iny(const AMode addrMode)
{
    _y++;
    SetFlag(ZERO_FLAG_MASK, !(_y));
    SetFlag(NEGATIVE_FLAG_MASK, _y & 0x80);
}

void Cpu::Jmp(const AMode addrMode)
{
    uint16_t addr = _address_mode_mapper[addrMode]();
    _pc = addr;
}

void Cpu::Jsr(const AMode addrMode)
{
    uint16_t addr = _address_mode_mapper[addrMode]();
    _pc--; // pc has jumped 3 times and now we need to go back once
    CpuWrite(STACK_OFFSET + _sp--, (_pc >> 8) & 0xff);
    CpuWrite(STACK_OFFSET + _sp--, _pc & 0xff);
    _pc = addr;
}

void Cpu::Lda(const AMode addrMode)
{
    uint16_t addr = _address_mode_mapper[addrMode]();
    _a = CpuRead(addr);
    SetFlag(ZERO_FLAG_MASK, !(_a));
    SetFlag(NEGATIVE_FLAG_MASK, _a & 0x80);
}

void Cpu::Ldx(const AMode addrMode)
{
    uint16_t addr = _address_mode_mapper[addrMode]();
    _x = CpuRead(addr);
    SetFlag(ZERO_FLAG_MASK, !(_x));
    SetFlag(NEGATIVE_FLAG_MASK, _x & 0x80);
}

void Cpu::Ldy(const AMode addrMode)
{
    uint16_t addr = _address_mode_mapper[addrMode]();
    _y = CpuRead(addr);
    SetFlag(ZERO_FLAG_MASK, !(_y));
    SetFlag(NEGATIVE_FLAG_MASK, _y & 0x80);
}

void Cpu::Lsr(const AMode addrMode)
{
    uint8_t data;
    uint16_t addr;
    if (addrMode == AMode::ACCUM)
    {
        data = _a;
    }
    else
    {
        addr = _address_mode_mapper[addrMode]();
        data = CpuRead(addr);
    }
    uint16_t temp = data >> 1;
    SetFlag(CARRY_FLAG_MASK, data & 0x1);
    SetFlag(ZERO_FLAG_MASK, !(temp & 0xff));
    ClearFlag(NEGATIVE_FLAG_MASK);
    if (addrMode == AMode::ACCUM)
    {
        _a = temp & 0xff;
    }
    else
    {
        CpuWrite(addr, temp & 0xff);
    }
}

void Cpu::Nop(const AMode addrMode)
{
    // This line is used to handle illegal nops that need to preform pc advencments  
    _address_mode_mapper[addrMode]();
}

void Cpu::Ora(const AMode addrMode)
{
    uint16_t addr = _address_mode_mapper[addrMode]();
    uint8_t data = CpuRead(addr);
    _a |= data;
    SetFlag(ZERO_FLAG_MASK, !(_a));
    SetFlag(NEGATIVE_FLAG_MASK, _a & 0x80);
}

void Cpu::Pha(const AMode addrMode)
{
    CpuWrite(STACK_OFFSET + _sp--, _a);
}

void Cpu::Php(const AMode addrMode)
{
    CpuWrite(STACK_OFFSET + _sp--, _p | BREAK_COMMAND_FLAG_MASK | RESERVED_FLAG_MASK);
}

void Cpu::Pla(const AMode addrMode)
{
    _a = CpuRead(STACK_OFFSET + ++_sp);
    SetFlag(ZERO_FLAG_MASK, !(_a));
    SetFlag(NEGATIVE_FLAG_MASK, _a & 0x80);
}

void Cpu::Plp(const AMode addrMode)
{
    _p = CpuRead(STACK_OFFSET + ++_sp);
    ClearFlag(BREAK_COMMAND_FLAG_MASK);
    ClearFlag(RESERVED_FLAG_MASK);
}

void Cpu::Rol(const AMode addrMode)
{
    uint8_t data;
    uint16_t addr;
    if (addrMode == AMode::ACCUM)
    {
        data = _a;
    }
    else
    {
        addr = _address_mode_mapper[addrMode]();
        data = CpuRead(addr);
    }
    uint16_t temp = data << 1;
    temp |= GetFlag(CARRY_FLAG_MASK);
    SetFlag(CARRY_FLAG_MASK, temp & 0xff00);
    SetFlag(ZERO_FLAG_MASK, !(temp & 0xff));
    SetFlag(NEGATIVE_FLAG_MASK, temp & 0x80);
    if (addrMode == AMode::ACCUM)
    {
        _a = temp & 0xff;
    }
    else
    {
        CpuWrite(addr, temp & 0xff);
    }
}
void Cpu::Ror(const AMode addrMode)
{
    uint8_t data;
    uint16_t addr;
    if (addrMode == AMode::ACCUM)
    {
        data = _a;
    }
    else
    {
        addr = _address_mode_mapper[addrMode]();
        data = CpuRead(addr);
    }
    uint16_t temp = data >> 1;
    temp |= GetFlag(CARRY_FLAG_MASK) << 7;
    SetFlag(CARRY_FLAG_MASK, data & 0x1);
    SetFlag(ZERO_FLAG_MASK, !(temp & 0xff));
    SetFlag(NEGATIVE_FLAG_MASK, temp & 0x80);
    if (addrMode == AMode::ACCUM)
    {
        _a = temp & 0xff;
    }
    else
    {
        CpuWrite(addr, temp & 0xff);
    }
}

void Cpu::Rti(const AMode addrMode)
{
    _p = CpuRead(STACK_OFFSET + ++_sp);
    _pc = CpuRead(STACK_OFFSET + ++_sp);
    _pc |= CpuRead(STACK_OFFSET + ++_sp) << 8;
    ClearFlag(BREAK_COMMAND_FLAG_MASK);
    ClearFlag(RESERVED_FLAG_MASK);
}

void Cpu::Rts(const AMode addrMode)
{
    _pc = CpuRead(STACK_OFFSET + ++_sp);
    _pc |= CpuRead(STACK_OFFSET + ++_sp) << 8;
    _pc++;
}

void Cpu::Sbc(const AMode addrMode)
{
    uint16_t addr = _address_mode_mapper[addrMode]();
    uint8_t data = CpuRead(addr);
    uint16_t temp = _a + (data ^ 0xff) + GetFlag(CARRY_FLAG_MASK);
    SetFlag(CARRY_FLAG_MASK, temp & 0xff00);
    SetFlag(ZERO_FLAG_MASK, !(temp & 0xff));
    SetFlag(OVERFLOW_FLAG_MASK, (_a ^ temp) & ((data ^ 0xff) ^ temp) & 0x80);
    SetFlag(NEGATIVE_FLAG_MASK, temp & 0x80);
    _a = temp & 0xff;
}

void Cpu::Sec(const AMode addrMode)
{
    SetFlag(CARRY_FLAG_MASK, true);
}

void Cpu::Sed(const AMode addrMode)
{
    SetFlag(DECIMAL_MODE_FLAG_MASK, true);
}

void Cpu::Sei(const AMode addrMode)
{
    SetFlag(INTERRUPT_DISABLE_FLAG_MASK, true);
}

void Cpu::Sta(const AMode addrMode)
{
    uint16_t addr = _address_mode_mapper[addrMode]();
    CpuWrite(addr, _a);
}

void Cpu::Stx(const AMode addrMode)
{
    uint16_t addr = _address_mode_mapper[addrMode]();
    CpuWrite(addr, _x);
}

void Cpu::Sty(const AMode addrMode)
{
    uint16_t addr = _address_mode_mapper[addrMode]();
    CpuWrite(addr, _y);
}

void Cpu::Tax(const AMode addrMode)
{
    _x = _a;
    SetFlag(ZERO_FLAG_MASK, !(_x));
    SetFlag(NEGATIVE_FLAG_MASK, _x & 0x80);
}

void Cpu::Tay(const AMode addrMode)
{
    _y = _a;
    SetFlag(ZERO_FLAG_MASK, !(_y));
    SetFlag(NEGATIVE_FLAG_MASK, _y & 0x80);
}

void Cpu::Tsx(const AMode addrMode)
{
    _x = _sp;
    SetFlag(ZERO_FLAG_MASK, !(_x));
    SetFlag(NEGATIVE_FLAG_MASK, _x & 0x80);
}

void Cpu::Txa(const AMode addrMode)
{
    _a = _x;
    SetFlag(ZERO_FLAG_MASK, !(_a));
    SetFlag(NEGATIVE_FLAG_MASK, _a & 0x80);
}

void Cpu::Txs(const AMode addrMode)
{
    _sp = _x;
}

void Cpu::Tya(const AMode addrMode)
{
    _a = _y;
    SetFlag(ZERO_FLAG_MASK, !(_a));
    SetFlag(NEGATIVE_FLAG_MASK, _a & 0x80);
}

std::map<uint16_t, std::string> Cpu::Disassemble()
{
    std::map<uint16_t, std::string> diss_map;
    uint16_t addr = 0xc000;
    uint16_t orig_addr = addr;
    uint16_t temp_addr;
    int8_t temp_data;
    uint8_t data;
    while (addr < 0xffff && orig_addr <= addr)
    {
        std::string asm_line;
        asm_line = "Address: " + NumToHexStringConvertor::Convert(addr, 4) + ",   ";
        orig_addr = addr;
        data = _mmu_read(addr++);
        if (_opcode_vector[data].type == IType::BRK)
        {
            addr++;
        }
        asm_line += "Instruction opcode:" + NumToHexStringConvertor::Convert(data, 2);
        asm_line += ",  Instruction:" + _itype_name_mapper[static_cast<int>(_opcode_vector[data].type)];
        switch (_opcode_vector[data].addrMode)
        {
        case AMode::ACCUM :
        break;
        case AMode::IMM :
            temp_addr = _mmu_read(addr++);
            asm_line += "  " + NumToHexStringConvertor::Convert(temp_addr, 4);
                
        break;
        case AMode::IMPLIED :
        break;
        case AMode::ABSOLUTE :
            temp_addr = _mmu_read(addr++);
            temp_addr |= (_mmu_read(addr++) << 8);
            asm_line += "  " + NumToHexStringConvertor::Convert(temp_addr, 4);
                
        break;
        case AMode::ZP:
            temp_addr = _mmu_read(addr++);
            asm_line += "  " + NumToHexStringConvertor::Convert(temp_addr, 4);
                
        break;
        case AMode::I_ZP_X:
            temp_addr = _mmu_read(addr++);
            asm_line += "  " + NumToHexStringConvertor::Convert(temp_addr, 4) + ", x";
                
        break;
        case AMode::I_ZP_Y:
            temp_addr = _mmu_read(addr++);
            asm_line += "  " + NumToHexStringConvertor::Convert(temp_addr, 4) + ", y";
                
        break;
        case AMode::I_ABSOLUTE_X:
            temp_addr = _mmu_read(addr++);
            temp_addr |= (_mmu_read(addr++) << 8);
            asm_line += "  " + NumToHexStringConvertor::Convert(temp_addr, 4) + ", x";
                
        break;
        case AMode::I_ABSOLUTE_Y:
            temp_addr = _mmu_read(addr++);
            temp_addr |= (_mmu_read(addr++) << 8);
            asm_line += "  " +  NumToHexStringConvertor::Convert(temp_addr, 4) + ", y";
                
        break;
        case AMode::RELATIVE:
            temp_data = _mmu_read(addr++);
            temp_addr = temp_data;
            asm_line += "  " + NumToHexStringConvertor::Convert(temp_addr, 4) + " [Rel Address:" + NumToHexStringConvertor::Convert(temp_data + addr, 4) + "]";
                
        break;
        case AMode::I_INDIRECT:
            temp_addr = _mmu_read(addr++);
            asm_line += "  " + NumToHexStringConvertor::Convert(temp_addr, 4) + ", x";
                
        break;
        case AMode::INDIRECT_I:
            temp_addr = _mmu_read(addr++);
            asm_line += "  " + NumToHexStringConvertor::Convert(temp_addr, 4) + ", y";
                
        break;
        case AMode::INDIRECT:
            temp_addr = _mmu_read(addr++);
            temp_addr |= (_mmu_read(addr++) << 8);
            asm_line += "  " + NumToHexStringConvertor::Convert(temp_addr, 4);
                
        break;
        default:
            break;
        }
        asm_line += ",  Address mode:" + _amode_name_mapper[static_cast<int>(_opcode_vector[data].addrMode)];
        diss_map[orig_addr] = asm_line;
    }
    return std::move(diss_map);
}