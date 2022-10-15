#pragma once

#include <map>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <functional>

class Cpu {
public:
    using WriteFunction = std::function<void (uint16_t address, uint8_t data)>;
    using ReadFunction = std::function<uint8_t (uint16_t address)>;

    Cpu(WriteFunction bus_write, ReadFunction bus_read);

    void cpuReset();

    void cpuExecuteInstruction();

    void Irq();
    void Nmi();

    std::map<uint16_t, std::string> disassemble();
private:
    #ifdef NESTEST_DEBUG
    friend class NestestLogTester;
    #endif // NESTEST_DEBUG

    static constexpr uint32_t STACK_OFFSET = 0x100;
    static constexpr uint32_t STACK_SIZE = 0x100;

    static constexpr uint8_t CARRY_FLAG_MASK = 0x01;
    static constexpr uint8_t ZERO_FLAG_MASK = 0x02;
    static constexpr uint8_t INTERRUPT_DISABLE_FLAG_MASK = 0x04;
    static constexpr uint8_t DECIMAL_MODE_FLAG_MASK = 0x08;
    static constexpr uint8_t BREAK_COMMAND_FLAG_MASK = 0x10;
    static constexpr uint8_t RESERVED_FLAG_MASK = 0x20;
    static constexpr uint8_t OVERFLOW_FLAG_MASK = 0x40;
    static constexpr uint8_t NEGATIVE_FLAG_MASK = 0x80;

    static constexpr uint16_t ABORT_LSB = 0xfff8;
    static constexpr uint16_t ABORT_MSB = 0xfff9;
    static constexpr uint16_t COP_LSB = 0xfff4;
    static constexpr uint16_t COP_MSB = 0xfff5;
    static constexpr uint16_t NORMAL_IRQ_LSB = 0xfffe;
    static constexpr uint16_t NORMAL_IRQ_MSB = 0xffff;
    static constexpr uint16_t BRK_LSB = NORMAL_IRQ_LSB;
    static constexpr uint16_t BRK_MSB = NORMAL_IRQ_MSB;
    static constexpr uint16_t NMI_LSB = 0xfffa;
    static constexpr uint16_t NMI_MSB = 0xfffb;
    static constexpr uint16_t RESET_LSB = 0xfffc;
    static constexpr uint16_t RESET_MSB = 0xfffd;

    enum class IrqType
    {
        ABORT,
        COP,
        NORMAL_IRQ,
        BRK,
        NMI,
        RESET
    };

    enum class AMode
    {
        ACCUM, //Operation on the accumulator
        IMM, //Second byte contains the opeand with no further memory addresing required
        ABSOLUTE, //Second and third bytes contain a full 16 bit address
        ZP, //Zero page instruction
        I_ZP_X, //Uses the zero page addressing and the x registers
        I_ZP_Y, //Uses the zero page addressing and the y registers
        I_ABSOLUTE_X, //Absolute addressing that also uses the x registers
        I_ABSOLUTE_Y, //Absolute addressing that also uses the y registers
        IMPLIED, //The address containing the operand is implicitly stated in the operation code
        RELATIVE, //Used only in branch instructions and establishes a destination for the conditional branch
        I_INDIRECT, //The second byte is added to the x register and the result is a zero page address. From the zero page you take the effective address is the next 2 bytes.
        INDIRECT_I, //The second byte is added to the y register and the result is a zero page address. From the zero page you take the effective address is the next 2 bytes.
        INDIRECT //The second and third contain a full 16 bit address. From this address you take the effective address is the next 2 bytes.
    };

    enum class IType
    {
        ADC,
        AND,
        ASL,
        BCC,
        BCS,
        BEQ,
        BIT,
        BMI,
        BNE,
        BPL,
        BRK,
        BVC,
        BVS,
        CLC, 
        CLD, 
        CLI,
        CLV,
        CMP,
        CPX,
        CPY,
        DEC,
        DEX,
        DEY,
        EOR,
        INC,
        INX,
        INY,
        JMP,
        JSR,
        LDA,
        LDX,
        LDY,
        LSR,
        NOP,
        ORA,
        PHA,
        PHP,
        PLA,
        PLP,
        ROL,
        ROR,
        RTI,
        RTS,
        SBC,
        SEC,
        SED,
        SEI,
        STA,
        STX,
        STY,
        TAX,
        TAY,
        TSX,
        TXA,
        TXS,
        TYA,
        MIA // MIA represents every illegal instruction that wasn't implemented
    };

    struct Instruction 
    {
        IType type;
        AMode addrMode;
        uint8_t cycles;
    };

    using AddressFunction = std::function<uint16_t ()>;
    using InstructionFunction = std::function<void (AMode addrMode)>;

    uint16_t _pc; //program counter
    uint8_t _sp; //stack
    uint8_t _a; //accumulator
    uint8_t _x; //index
    uint8_t _y; //index
    uint8_t _p; //flags
    size_t _cycles;
    WriteFunction _busWrite;
    ReadFunction _busRead;
    std::unordered_map<AMode, AddressFunction> _addressModeMapper;
    std::unordered_map<IType, InstructionFunction> _instructionTypeMapper;
    std::unordered_map<IrqType, std::pair<uint16_t, uint16_t>> _irqVectorMap;
    std::vector<Instruction> _opcodeVector;
    bool _loop_running;

    std::vector<std::string> _aModeNameMapper = 
    {
        "ACCUM",
        "IMM",
        "ABSOLUTE",
        "ZP",
        "I_ZP_X",
        "I_ZP_Y",
        "I_ABSOLUTE_X",
        "I_ABSOLUTE_Y",
        "IMPLIED",
        "RELATIVE",
        "I_INDIRECT",
        "INDIRECT_I",
        "INDIRECT"
    };

    std::vector<std::string> _iTypeNameMapper = 
    {
        "ADC",
        "AND",
        "ASL",
        "BCC",
        "BCS",
        "BEQ",
        "BIT",
        "BMI",
        "BNE",
        "BPL",
        "BRK",
        "BVC",
        "BVS",
        "CLC",
        "CLD",
        "CLI",
        "CLV",
        "CMP",
        "CPX",
        "CPY",
        "DEC",
        "DEX",
        "DEY",
        "EOR",
        "INC",
        "INX",
        "INY",
        "JMP",
        "JSR",
        "LDA",
        "LDX",
        "LDY",
        "LSR",
        "NOP",
        "ORA",
        "PHA",
        "PHP",
        "PLA",
        "PLP",
        "ROL",
        "ROR",
        "RTI",
        "RTS",
        "SBC",
        "SEC",
        "SED",
        "SEI",
        "STA",
        "STX",
        "STY",
        "TAX",
        "TAY",
        "TSX",
        "TXA",
        "TXS",
        "TYA",
        "MIA" 
    };

    void setFlag(uint8_t flagMask, bool val);

    void clearFlag(uint8_t flagMask);

    uint8_t getFlag(uint8_t flagMask);

    void cpuWrite(uint16_t address, uint8_t data);

    uint8_t cpuRead(uint16_t address);

    // This are the diffrent addresing modes:

    uint16_t accumAddr();

    uint16_t immAddr();

    uint16_t absoluteAddr();

    uint16_t zpAddr();

    uint16_t iZpXAddr();

    uint16_t iZpYAddr();
 
    uint16_t iAbsoluteAddrX();

    uint16_t iAbsoluteAddrY();

    uint16_t impliedAddr();

    uint16_t relativeAddr();

    uint16_t iIndirectAddr();

    uint16_t indirectIAddr();

    uint16_t indirectAddr(); 

    // Instruction set
    void Adc(AMode addrMode);
    void And(AMode addrMode);
    void Asl(AMode addrMode);
    void Bcc(AMode addrMode);
    void Bcs(AMode addrMode);
    void Beq(AMode addrMode);
    void Bit(AMode addrMode);
    void Bmi(AMode addrMode);
    void Bne(AMode addrMode);
    void Bpl(AMode addrMode);
    void Brk(AMode addrMode);
    void Bvc(AMode addrMode);
    void Bvs(AMode addrMode);
    void Clc(AMode addrMode);
    void Cld(AMode addrMode);
    void Cli(AMode addrMode);
    void Clv(AMode addrMode);
    void Cmp(AMode addrMode);
    void Cpx(AMode addrMode);
    void Cpy(AMode addrMode);
    void Dec(AMode addrMode);
    void Dex(AMode addrMode);
    void Dey(AMode addrMode);
    void Eor(AMode addrMode);
    void Inc(AMode addrMode);
    void Inx(AMode addrMode);
    void Iny(AMode addrMode);
    void Jmp(AMode addrMode);
    void Jsr(AMode addrMode);
    void Lda(AMode addrMode);
    void Ldx(AMode addrMode);
    void Ldy(AMode addrMode);
    void Lsr(AMode addrMode);
    void Nop(AMode addrMode);
    void Ora(AMode addrMode);
    void Pha(AMode addrMode);
    void Php(AMode addrMode);
    void Pla(AMode addrMode);
    void Plp(AMode addrMode);
    void Rol(AMode addrMode);
    void Ror(AMode addrMode);
    void Rti(AMode addrMode);
    void Rts(AMode addrMode);
    void Sbc(AMode addrMode);
    void Sec(AMode addrMode);
    void Sed(AMode addrMode);
    void Sei(AMode addrMode);
    void Sta(AMode addrMode);
    void Stx(AMode addrMode);
    void Sty(AMode addrMode);
    void Tax(AMode addrMode);
    void Tay(AMode addrMode);
    void Tsx(AMode addrMode);
    void Txa(AMode addrMode);
    void Txs(AMode addrMode);
    void Tya(AMode addrMode);
};