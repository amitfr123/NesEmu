#include "NumToHexStringConvertor.hpp"

#include "HardwareEmulation/Cpu.hpp"

#ifdef NESTEST_DEBUG
#include "HardwareEmulation/NestestLogTester.hpp"
#endif // NESTEST_DEBUG

Cpu::Cpu(WriteFunction mmu_write, ReadFunction mmu_read) : 
    _busWrite(std::move(mmu_write)),
    _busRead(std::move(mmu_read)),
    _addressModeMapper({
        {AMode::ACCUM, std::bind(&Cpu::accumAddr, this)},
        {AMode::IMM, std::bind(&Cpu::immAddr, this)},
        {AMode::ABSOLUTE, std::bind(&Cpu::absoluteAddr, this)},
        {AMode::ZP, std::bind(&Cpu::zpAddr, this)},
        {AMode::I_ZP_X, std::bind(&Cpu::iZpXAddr, this)},
        {AMode::I_ZP_Y, std::bind(&Cpu::iZpYAddr, this)},
        {AMode::I_ABSOLUTE_X, std::bind(&Cpu::iAbsoluteAddrX, this)},
        {AMode::I_ABSOLUTE_Y, std::bind(&Cpu::iAbsoluteAddrY, this)},
        {AMode::IMPLIED, std::bind(&Cpu::impliedAddr, this)},
        {AMode::RELATIVE, std::bind(&Cpu::relativeAddr, this)},
        {AMode::I_INDIRECT, std::bind(&Cpu::iIndirectAddr, this)},
        {AMode::INDIRECT_I, std::bind(&Cpu::indirectIAddr, this)},
        {AMode::INDIRECT, std::bind(&Cpu::indirectAddr, this)}
    }),
    _instructionTypeMapper({
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
    _irqVectorMap({
        {IrqType::ABORT, std::pair<uint32_t,uint32_t>(ABORT_LSB, ABORT_MSB)},
        {IrqType::COP, std::pair<uint32_t,uint32_t>(COP_LSB, COP_MSB)},
        {IrqType::NORMAL_IRQ, std::pair<uint32_t,uint32_t>(NORMAL_IRQ_LSB, NORMAL_IRQ_MSB)},
        {IrqType::BRK, std::pair<uint32_t,uint32_t>(BRK_LSB, BRK_MSB)},
        {IrqType::NMI, std::pair<uint32_t,uint32_t>(NMI_LSB, NMI_MSB)},
        {IrqType::RESET, std::pair<uint32_t,uint32_t>(RESET_LSB, RESET_MSB)},
    }),
    _opcodeVector({
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

void Cpu::cpuExecuteInstruction()
{
    #ifdef NESTEST_DEBUG
    static uint32_t index = 0;
    #endif // NESTEST_DEBUG
    uint8_t opcode = cpuRead(_pc++);
    
    #ifdef NESTEST_DEBUG
    NestestLogTester::GetInstance()->DebugInstruction(*this, opcode, index);
    index++;
    #endif // NESTEST_DEBUG
    _instructionTypeMapper[_opcodeVector[opcode].type](_opcodeVector[opcode].addrMode);
}

void Cpu::cpuReset()
{
    #ifdef NESTEST_DEBUG
    //_pc = 0xc000;
    _pc = cpuRead(_irqVectorMap[IrqType::RESET].first) | (cpuRead(_irqVectorMap[IrqType::RESET].second) << 8);
    #else
    _pc = cpuRead(_irqVectorMap[IrqType::RESET].first) | (cpuRead(_irqVectorMap[IrqType::RESET].second) << 8);
    #endif // NESTEST_DEBUG
    _a = 0;
    _x = 0;
    _y = 0;
    _p = INTERRUPT_DISABLE_FLAG_MASK;
    _sp = 0xfd; // this is done to simulate the hacked stack insertions
    _cycles = 7;
}

void Cpu::Irq()
{
    if (!getFlag(INTERRUPT_DISABLE_FLAG_MASK))
    {
        setFlag(INTERRUPT_DISABLE_FLAG_MASK, true);
        cpuWrite(STACK_OFFSET + _sp--, (_pc >> 8) & 0xff);
        cpuWrite(STACK_OFFSET + _sp--, _pc & 0xff);
        cpuWrite(STACK_OFFSET + _sp--, _p | RESERVED_FLAG_MASK);
        _pc = cpuRead(_irqVectorMap[IrqType::NORMAL_IRQ].first) | (cpuRead(_irqVectorMap[IrqType::NORMAL_IRQ].second) << 8);
        _cycles += 7;
    }
}

void Cpu::Nmi()
{
    setFlag(INTERRUPT_DISABLE_FLAG_MASK, true);
    cpuWrite(STACK_OFFSET + _sp--, (_pc >> 8) & 0xff);
    cpuWrite(STACK_OFFSET + _sp--, _pc & 0xff);
    cpuWrite(STACK_OFFSET + _sp--, _p | RESERVED_FLAG_MASK);
    _pc = cpuRead(_irqVectorMap[IrqType::NMI].first) | (cpuRead(_irqVectorMap[IrqType::NMI].second) << 8);
    _cycles += 7;
}

void Cpu::setFlag(uint8_t flagMask, bool val)
{
    if (val)
    {
        _p |= flagMask;
    }
    else
    {
        clearFlag(flagMask);
    }
}

void Cpu::clearFlag(uint8_t flagMask)
{
    _p &= ~(flagMask);
}

uint8_t Cpu::getFlag(uint8_t flagMask)
{
    return (_p & flagMask)? 1 : 0;
}

void Cpu::cpuWrite(uint16_t address, uint8_t data)
{
    _busWrite(address, data);
}

uint8_t Cpu::cpuRead(uint16_t address)
{
    return _busRead(address);
}

uint16_t Cpu::accumAddr()
{
    return 0;
}

uint16_t Cpu::immAddr()
{
    return _pc++;
}

uint16_t Cpu::absoluteAddr()
{
    uint16_t lo = cpuRead(_pc++);
    uint16_t hi = cpuRead(_pc++);
    return lo | (hi << 8);
}

uint16_t Cpu::zpAddr()
{
    return cpuRead(_pc++);
}

uint16_t Cpu::iZpXAddr()
{
    return (cpuRead(_pc++) + _x) & 0x00ff;
}

uint16_t Cpu::iZpYAddr()
{
    return (cpuRead(_pc++) + _y) & 0x00ff;
}

uint16_t Cpu::iAbsoluteAddrX()
{
    uint16_t lo = cpuRead(_pc++);
    uint16_t hi = cpuRead(_pc++);
    uint16_t addr = lo;
    addr |= (hi << 8);
    addr += _x;
    if ((addr & 0xff00) != hi)
    {
        _cycles++;
    }
    return addr;
}

uint16_t Cpu::iAbsoluteAddrY()
{
    uint16_t lo = cpuRead(_pc++);
    uint16_t hi = cpuRead(_pc++);
    uint16_t addr = lo;
    addr |= (hi << 8);
    addr += _y;
    if ((addr & 0xff00) != hi)
    {
        _cycles++;
    }
    return addr;
}

uint16_t Cpu::impliedAddr()
{
    return 0;
}

uint16_t Cpu::relativeAddr()
{
    int8_t offset = cpuRead(_pc++);
    return _pc + offset;
}

uint16_t Cpu::iIndirectAddr()
{
    uint16_t zpg_addr = cpuRead(_pc++);
    uint16_t addr = 0;
    addr |= cpuRead((zpg_addr++ + _x) & 0xff)  & 0xff;
    addr |= (cpuRead((zpg_addr + _x) & 0xff) << 8);
    return addr;
}

uint16_t Cpu::indirectIAddr()
{
    uint16_t zpg_addr = cpuRead(_pc++);
    uint16_t addr = 0;
    addr |= cpuRead(zpg_addr++);
    addr |= (cpuRead(zpg_addr & 0xff) << 8);
    if ((addr + _y) & 0xff00 != addr & 0xff00)
    {
        _cycles++;
    }
    return addr + _y;
}

uint16_t Cpu::indirectAddr()
{
    uint16_t ind_addr = 0;
    uint16_t addr = 0;
    ind_addr |= cpuRead(_pc++);
    ind_addr |= cpuRead(_pc++) << 8;
    addr |= cpuRead(ind_addr);
    // This line is in order to emulate one of the 6502 bugs
    // In the 6502 advencing the ind_addr to read the hi bits of the address did not effect the hi bits of the ind_addr
    ind_addr = (ind_addr & 0xff00) | ((ind_addr + 1) & 0xff); 
    addr |= (cpuRead(ind_addr) << 8);
    return addr;
}

void Cpu::Adc(AMode addrMode)
{
    uint16_t addr = _addressModeMapper[addrMode]();
    uint8_t data = cpuRead(addr);
    uint16_t temp = getFlag(CARRY_FLAG_MASK) + data + _a;
    setFlag(CARRY_FLAG_MASK, temp & 0xff00);
    setFlag(ZERO_FLAG_MASK, !(temp & 0xff));
    setFlag(OVERFLOW_FLAG_MASK, (_a ^ temp) & (data ^ temp) & 0x80);
    setFlag(NEGATIVE_FLAG_MASK, temp & 0x80);
    _a = temp & 0xff;
}

void Cpu::And(AMode addrMode)
{
    uint16_t addr = _addressModeMapper[addrMode]();
    uint8_t data = cpuRead(addr);
    _a &= data;
    setFlag(ZERO_FLAG_MASK, !(_a));
    setFlag(NEGATIVE_FLAG_MASK, _a & 0x80);
}

void Cpu::Asl(AMode addrMode)
{
    uint8_t data;
    uint16_t addr;
    if (addrMode == AMode::ACCUM)
    {
        data = _a;
    }
    else
    {
        addr = _addressModeMapper[addrMode]();
        data = cpuRead(addr);
    }
    uint16_t temp = data << 1;
    setFlag(CARRY_FLAG_MASK, temp & 0xff00);
    setFlag(ZERO_FLAG_MASK, !(temp & 0xff));
    setFlag(NEGATIVE_FLAG_MASK, temp & 0x80);
    if (addrMode == AMode::ACCUM)
    {
        _a = temp & 0xff;
    }
    else
    {
        cpuWrite(addr, temp & 0xff);
    }
}

void Cpu::Bcc(AMode addrMode)
{
    
    uint16_t addr = _addressModeMapper[addrMode]();
    if (!getFlag(CARRY_FLAG_MASK))
    {
        if (addr & 0xff != _pc % 0xff)
        {
            _cycles++;
        }
        _pc = addr;
    }
}

void Cpu::Bcs(AMode addrMode)
{
    
    uint16_t addr = _addressModeMapper[addrMode]();
    if (getFlag(CARRY_FLAG_MASK))
    {
        if (addr & 0xff != _pc % 0xff)
        {
            _cycles++;
        }
        _pc = addr;
    }
}

void Cpu::Beq(AMode addrMode)
{
    
    uint16_t addr = _addressModeMapper[addrMode]();
    if (getFlag(ZERO_FLAG_MASK))
    {
        if (addr & 0xff != _pc % 0xff)
        {
            _cycles++;
        }
        _pc = addr;
    }
}

void Cpu::Bit(AMode addrMode)
{
    uint16_t addr = _addressModeMapper[addrMode]();
    uint8_t data = cpuRead(addr);
    uint16_t temp = _a & data;
    setFlag(ZERO_FLAG_MASK, !(temp & 0xff));
    setFlag(OVERFLOW_FLAG_MASK, data & 1 << 6);
    setFlag(NEGATIVE_FLAG_MASK, data & 1 << 7);
}

void Cpu::Bmi(AMode addrMode)
{
    
    uint16_t addr = _addressModeMapper[addrMode]();
    if (getFlag(NEGATIVE_FLAG_MASK))
    {
        if (addr & 0xff != _pc % 0xff)
        {
            _cycles++;
        }
        _pc = addr;
    }
}

void Cpu::Bne(AMode addrMode)
{
    
    uint16_t addr = _addressModeMapper[addrMode]();
    if (!getFlag(ZERO_FLAG_MASK))
    {
        if (addr & 0xff != _pc % 0xff)
        {
            _cycles++;
        }
        _pc = addr;
    }
}

void Cpu::Bpl(AMode addrMode)
{
    
    uint16_t addr = _addressModeMapper[addrMode]();
    if (!getFlag(NEGATIVE_FLAG_MASK))
    {
        if (addr & 0xff != _pc % 0xff)
        {
            _cycles++;
        }
        _pc = addr;
    }
}

void Cpu::Brk(AMode addrMode)
{
    _pc++;
    setFlag(INTERRUPT_DISABLE_FLAG_MASK, true);
    cpuWrite(STACK_OFFSET + _sp--, (_pc >> 8) & 0xff);
    cpuWrite(STACK_OFFSET + _sp--, _pc & 0xff);
    cpuWrite(STACK_OFFSET + _sp--, _p | BREAK_COMMAND_FLAG_MASK);
    _pc = cpuRead(_irqVectorMap[IrqType::BRK].first) | (cpuRead(_irqVectorMap[IrqType::BRK].second) << 8);
}

void Cpu::Bvc(AMode addrMode)
{
    
    uint16_t addr = _addressModeMapper[addrMode]();
    if (!getFlag(OVERFLOW_FLAG_MASK))
    {
        if (addr & 0xff != _pc % 0xff)
        {
            _cycles++;
        }
        _pc = addr;
    }
}

void Cpu::Bvs(AMode addrMode)
{
    
    uint16_t addr = _addressModeMapper[addrMode]();
    if (getFlag(OVERFLOW_FLAG_MASK))
    {
        if (addr & 0xff != _pc % 0xff)
        {
            _cycles++;
        }
        _pc = addr;
    }
}

void Cpu::Clc(AMode addrMode)
{
    clearFlag(CARRY_FLAG_MASK);
}

void Cpu::Cld(AMode addrMode)
{
    clearFlag(DECIMAL_MODE_FLAG_MASK);
}

void Cpu::Cli(AMode addrMode)
{
    clearFlag(INTERRUPT_DISABLE_FLAG_MASK);
}

void Cpu::Clv(AMode addrMode)
{
    clearFlag(OVERFLOW_FLAG_MASK);
}

void Cpu::Cmp(AMode addrMode)
{
    uint16_t addr = _addressModeMapper[addrMode]();
    uint8_t data = cpuRead(addr);
    uint16_t temp = _a - data;
    setFlag(CARRY_FLAG_MASK,(_a >= data));
    setFlag(ZERO_FLAG_MASK, !(temp & 0xff));
    setFlag(NEGATIVE_FLAG_MASK, temp & 0x80);
}

void Cpu::Cpx(AMode addrMode)
{
    uint16_t addr = _addressModeMapper[addrMode]();
    uint8_t data = cpuRead(addr);
    uint16_t temp = _x - data;
    setFlag(CARRY_FLAG_MASK,(_x >= data));
    setFlag(ZERO_FLAG_MASK, !(temp & 0xff));
    setFlag(NEGATIVE_FLAG_MASK, temp & 0x80);
}

void Cpu::Cpy(AMode addrMode)
{
    uint16_t addr = _addressModeMapper[addrMode]();
    uint8_t data = cpuRead(addr);
    uint16_t temp = _y - data;
    setFlag(CARRY_FLAG_MASK,(_y >= data));
    setFlag(ZERO_FLAG_MASK, !(temp & 0xff));
    setFlag(NEGATIVE_FLAG_MASK, temp & 0x80);
}

void Cpu::Dec(AMode addrMode)
{
    uint16_t addr = _addressModeMapper[addrMode]();
    uint8_t data = cpuRead(addr) - 1;
    setFlag(ZERO_FLAG_MASK, !(data));
    setFlag(NEGATIVE_FLAG_MASK, data & 0x80);
    cpuWrite(addr, data);
}

void Cpu::Dex(AMode addrMode)
{
    _x--;
    setFlag(ZERO_FLAG_MASK, !(_x));
    setFlag(NEGATIVE_FLAG_MASK, _x & 0x80);
}

void Cpu::Dey(AMode addrMode)
{
    _y--;
    setFlag(ZERO_FLAG_MASK, !(_y));
    setFlag(NEGATIVE_FLAG_MASK, _y & 0x80);
}

void Cpu::Eor(AMode addrMode)
{
    uint16_t addr = _addressModeMapper[addrMode]();
    uint8_t data = cpuRead(addr);
    _a ^= data;
    setFlag(ZERO_FLAG_MASK, !(_a));
    setFlag(NEGATIVE_FLAG_MASK, _a & 0x80);
}

void Cpu::Inc(AMode addrMode)
{
    uint16_t addr = _addressModeMapper[addrMode]();
    uint8_t data = cpuRead(addr) + 1;
    setFlag(ZERO_FLAG_MASK, !(data));
    setFlag(NEGATIVE_FLAG_MASK, data & 0x80);
    cpuWrite(addr, data);
}

void Cpu::Inx(AMode addrMode)
{
    _x++;
    setFlag(ZERO_FLAG_MASK, !(_x));
    setFlag(NEGATIVE_FLAG_MASK, _x & 0x80);
}

void Cpu::Iny(AMode addrMode)
{
    _y++;
    setFlag(ZERO_FLAG_MASK, !(_y));
    setFlag(NEGATIVE_FLAG_MASK, _y & 0x80);
}

void Cpu::Jmp(AMode addrMode)
{
    uint16_t addr = _addressModeMapper[addrMode]();
    _pc = addr;
}

void Cpu::Jsr(AMode addrMode)
{
    uint16_t addr = _addressModeMapper[addrMode]();
    _pc--; // pc has jumped 3 times and now we need to go back once
    cpuWrite(STACK_OFFSET + _sp--, (_pc >> 8) & 0xff);
    cpuWrite(STACK_OFFSET + _sp--, _pc & 0xff);
    _pc = addr;
}

void Cpu::Lda(AMode addrMode)
{
    uint16_t addr = _addressModeMapper[addrMode]();
    _a = cpuRead(addr);
    setFlag(ZERO_FLAG_MASK, !(_a));
    setFlag(NEGATIVE_FLAG_MASK, _a & 0x80);
}

void Cpu::Ldx(AMode addrMode)
{
    uint16_t addr = _addressModeMapper[addrMode]();
    _x = cpuRead(addr);
    setFlag(ZERO_FLAG_MASK, !(_x));
    setFlag(NEGATIVE_FLAG_MASK, _x & 0x80);
}

void Cpu::Ldy(AMode addrMode)
{
    uint16_t addr = _addressModeMapper[addrMode]();
    _y = cpuRead(addr);
    setFlag(ZERO_FLAG_MASK, !(_y));
    setFlag(NEGATIVE_FLAG_MASK, _y & 0x80);
}

void Cpu::Lsr(AMode addrMode)
{
    uint8_t data;
    uint16_t addr;
    if (addrMode == AMode::ACCUM)
    {
        data = _a;
    }
    else
    {
        addr = _addressModeMapper[addrMode]();
        data = cpuRead(addr);
    }
    uint16_t temp = data >> 1;
    setFlag(CARRY_FLAG_MASK, data & 0x1);
    setFlag(ZERO_FLAG_MASK, !(temp & 0xff));
    clearFlag(NEGATIVE_FLAG_MASK);
    if (addrMode == AMode::ACCUM)
    {
        _a = temp & 0xff;
    }
    else
    {
        cpuWrite(addr, temp & 0xff);
    }
}

void Cpu::Nop(AMode addrMode)
{
    // This line is used to handle illegal nops that need to preform pc advencments  
    _addressModeMapper[addrMode]();
}

void Cpu::Ora(AMode addrMode)
{
    uint16_t addr = _addressModeMapper[addrMode]();
    uint8_t data = cpuRead(addr);
    _a |= data;
    setFlag(ZERO_FLAG_MASK, !(_a));
    setFlag(NEGATIVE_FLAG_MASK, _a & 0x80);
}

void Cpu::Pha(AMode addrMode)
{
    cpuWrite(STACK_OFFSET + _sp--, _a);
}

void Cpu::Php(AMode addrMode)
{
    cpuWrite(STACK_OFFSET + _sp--, _p | BREAK_COMMAND_FLAG_MASK | RESERVED_FLAG_MASK);
}

void Cpu::Pla(AMode addrMode)
{
    _a = cpuRead(STACK_OFFSET + ++_sp);
    setFlag(ZERO_FLAG_MASK, !(_a));
    setFlag(NEGATIVE_FLAG_MASK, _a & 0x80);
}

void Cpu::Plp(AMode addrMode)
{
    _p = cpuRead(STACK_OFFSET + ++_sp);
    clearFlag(BREAK_COMMAND_FLAG_MASK);
    clearFlag(RESERVED_FLAG_MASK);
}

void Cpu::Rol(AMode addrMode)
{
    uint8_t data;
    uint16_t addr;
    if (addrMode == AMode::ACCUM)
    {
        data = _a;
    }
    else
    {
        addr = _addressModeMapper[addrMode]();
        data = cpuRead(addr);
    }
    uint16_t temp = data << 1;
    temp |= getFlag(CARRY_FLAG_MASK);
    setFlag(CARRY_FLAG_MASK, temp & 0xff00);
    setFlag(ZERO_FLAG_MASK, !(temp & 0xff));
    setFlag(NEGATIVE_FLAG_MASK, temp & 0x80);
    if (addrMode == AMode::ACCUM)
    {
        _a = temp & 0xff;
    }
    else
    {
        cpuWrite(addr, temp & 0xff);
    }
}
void Cpu::Ror(AMode addrMode)
{
    uint8_t data;
    uint16_t addr;
    if (addrMode == AMode::ACCUM)
    {
        data = _a;
    }
    else
    {
        addr = _addressModeMapper[addrMode]();
        data = cpuRead(addr);
    }
    uint16_t temp = data >> 1;
    temp |= getFlag(CARRY_FLAG_MASK) << 7;
    setFlag(CARRY_FLAG_MASK, data & 0x1);
    setFlag(ZERO_FLAG_MASK, !(temp & 0xff));
    setFlag(NEGATIVE_FLAG_MASK, temp & 0x80);
    if (addrMode == AMode::ACCUM)
    {
        _a = temp & 0xff;
    }
    else
    {
        cpuWrite(addr, temp & 0xff);
    }
}

void Cpu::Rti(AMode addrMode)
{
    _p = cpuRead(STACK_OFFSET + ++_sp);
    _pc = cpuRead(STACK_OFFSET + ++_sp);
    _pc |= cpuRead(STACK_OFFSET + ++_sp) << 8;
    clearFlag(BREAK_COMMAND_FLAG_MASK);
    clearFlag(RESERVED_FLAG_MASK);
}

void Cpu::Rts(AMode addrMode)
{
    _pc = cpuRead(STACK_OFFSET + ++_sp);
    _pc |= cpuRead(STACK_OFFSET + ++_sp) << 8;
    _pc++;
}

void Cpu::Sbc(AMode addrMode)
{
    uint16_t addr = _addressModeMapper[addrMode]();
    uint8_t data = cpuRead(addr);
    uint16_t temp = _a + (data ^ 0xff) + getFlag(CARRY_FLAG_MASK);
    setFlag(CARRY_FLAG_MASK, temp & 0xff00);
    setFlag(ZERO_FLAG_MASK, !(temp & 0xff));
    setFlag(OVERFLOW_FLAG_MASK, (_a ^ temp) & ((data ^ 0xff) ^ temp) & 0x80);
    setFlag(NEGATIVE_FLAG_MASK, temp & 0x80);
    _a = temp & 0xff;
}

void Cpu::Sec(AMode addrMode)
{
    setFlag(CARRY_FLAG_MASK, true);
}

void Cpu::Sed(AMode addrMode)
{
    setFlag(DECIMAL_MODE_FLAG_MASK, true);
}

void Cpu::Sei(AMode addrMode)
{
    setFlag(INTERRUPT_DISABLE_FLAG_MASK, true);
}

void Cpu::Sta(AMode addrMode)
{
    uint16_t addr = _addressModeMapper[addrMode]();
    cpuWrite(addr, _a);
}

void Cpu::Stx(AMode addrMode)
{
    uint16_t addr = _addressModeMapper[addrMode]();
    cpuWrite(addr, _x);
}

void Cpu::Sty(AMode addrMode)
{
    uint16_t addr = _addressModeMapper[addrMode]();
    cpuWrite(addr, _y);
}

void Cpu::Tax(AMode addrMode)
{
    _x = _a;
    setFlag(ZERO_FLAG_MASK, !(_x));
    setFlag(NEGATIVE_FLAG_MASK, _x & 0x80);
}

void Cpu::Tay(AMode addrMode)
{
    _y = _a;
    setFlag(ZERO_FLAG_MASK, !(_y));
    setFlag(NEGATIVE_FLAG_MASK, _y & 0x80);
}

void Cpu::Tsx(AMode addrMode)
{
    _x = _sp;
    setFlag(ZERO_FLAG_MASK, !(_x));
    setFlag(NEGATIVE_FLAG_MASK, _x & 0x80);
}

void Cpu::Txa(AMode addrMode)
{
    _a = _x;
    setFlag(ZERO_FLAG_MASK, !(_a));
    setFlag(NEGATIVE_FLAG_MASK, _a & 0x80);
}

void Cpu::Txs(AMode addrMode)
{
    _sp = _x;
}

void Cpu::Tya(AMode addrMode)
{
    _a = _y;
    setFlag(ZERO_FLAG_MASK, !(_a));
    setFlag(NEGATIVE_FLAG_MASK, _a & 0x80);
}

std::map<uint16_t, std::string> Cpu::disassemble()
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
        data = _busRead(addr++);
        if (_opcodeVector[data].type == IType::BRK)
        {
            addr++;
        }
        asm_line += "Instruction opcode:" + NumToHexStringConvertor::Convert(data, 2);
        asm_line += ",  Instruction:" + _iTypeNameMapper[static_cast<int>(_opcodeVector[data].type)];
        switch (_opcodeVector[data].addrMode)
        {
        case AMode::ACCUM :
        break;
        case AMode::IMM :
            temp_addr = _busRead(addr++);
            asm_line += "  " + NumToHexStringConvertor::Convert(temp_addr, 4);
                
        break;
        case AMode::IMPLIED :
        break;
        case AMode::ABSOLUTE :
            temp_addr = _busRead(addr++);
            temp_addr |= (_busRead(addr++) << 8);
            asm_line += "  " + NumToHexStringConvertor::Convert(temp_addr, 4);
                
        break;
        case AMode::ZP:
            temp_addr = _busRead(addr++);
            asm_line += "  " + NumToHexStringConvertor::Convert(temp_addr, 4);
                
        break;
        case AMode::I_ZP_X:
            temp_addr = _busRead(addr++);
            asm_line += "  " + NumToHexStringConvertor::Convert(temp_addr, 4) + ", x";
                
        break;
        case AMode::I_ZP_Y:
            temp_addr = _busRead(addr++);
            asm_line += "  " + NumToHexStringConvertor::Convert(temp_addr, 4) + ", y";
                
        break;
        case AMode::I_ABSOLUTE_X:
            temp_addr = _busRead(addr++);
            temp_addr |= (_busRead(addr++) << 8);
            asm_line += "  " + NumToHexStringConvertor::Convert(temp_addr, 4) + ", x";
                
        break;
        case AMode::I_ABSOLUTE_Y:
            temp_addr = _busRead(addr++);
            temp_addr |= (_busRead(addr++) << 8);
            asm_line += "  " +  NumToHexStringConvertor::Convert(temp_addr, 4) + ", y";
                
        break;
        case AMode::RELATIVE:
            temp_data = _busRead(addr++);
            temp_addr = temp_data;
            asm_line += "  " + NumToHexStringConvertor::Convert(temp_addr, 4) + " [Rel Address:" + NumToHexStringConvertor::Convert(temp_data + addr, 4) + "]";
                
        break;
        case AMode::I_INDIRECT:
            temp_addr = _busRead(addr++);
            asm_line += "  " + NumToHexStringConvertor::Convert(temp_addr, 4) + ", x";
                
        break;
        case AMode::INDIRECT_I:
            temp_addr = _busRead(addr++);
            asm_line += "  " + NumToHexStringConvertor::Convert(temp_addr, 4) + ", y";
                
        break;
        case AMode::INDIRECT:
            temp_addr = _busRead(addr++);
            temp_addr |= (_busRead(addr++) << 8);
            asm_line += "  " + NumToHexStringConvertor::Convert(temp_addr, 4);
                
        break;
        default:
            break;
        }
        asm_line += ",  Address mode:" + _aModeNameMapper[static_cast<int>(_opcodeVector[data].addrMode)];
        diss_map[orig_addr] = asm_line;
    }
    return std::move(diss_map);
}