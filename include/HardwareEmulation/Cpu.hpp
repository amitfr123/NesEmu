#pragma once

#include <map>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <functional>

class Cpu {
public:
    using WriteFunction = std::function<void (const uint16_t address, const uint8_t data)>;
    using ReadFunction = std::function<uint8_t (const uint16_t address)>;

    Cpu(WriteFunction mmu_write, ReadFunction mmu_read);

    void CpuReset();

    void CpuExecuteInstruction();

    std::map<uint16_t, std::string> Disassemble();
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
    using InstructionFunction = std::function<void (const AMode addrMode)>;

    uint16_t _pc; //program counter
    uint8_t _sp; //stack
    uint8_t _a; //accumulator
    uint8_t _x; //index
    uint8_t _y; //index
    uint8_t _p; //flags
    size_t _cycles;
    WriteFunction _mmu_write;
    ReadFunction _mmu_read;
    std::unordered_map<AMode, AddressFunction> _address_mode_mapper;
    std::unordered_map<IType, InstructionFunction> _instruction_type_mapper;
    std::unordered_map<IrqType, std::pair<uint16_t, uint16_t>> _irq_vector_map;
    std::vector<Instruction> _opcode_vector;
    bool _loop_running;

    std::vector<std::string> _amode_name_mapper = 
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

    std::vector<std::string> _itype_name_mapper = 
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

    void SetFlag(uint8_t flagMask, bool val);

    void ClearFlag(uint8_t flagMask);

    uint8_t GetFlag(uint8_t flagMask);

    void CpuWrite(const uint16_t address, const uint8_t data);

    uint8_t CpuRead(const uint16_t address);

    // This are the diffrent addresing modes:

    uint16_t accum_addr();

    uint16_t imm_addr();

    uint16_t absolute_addr();

    uint16_t zp_addr();

    uint16_t i_zp_x_addr();

    uint16_t i_zp_y_addr();
 
    uint16_t i_absolute_addr_x();

    uint16_t i_absolute_addr_y();

    uint16_t implied_addr();

    uint16_t relative_addr();

    uint16_t i_indirect_addr();

    uint16_t indirect_i_addr();

    uint16_t indirect_addr(); 

    // Instruction set
    void Adc(const AMode addrMode);
    void And(const AMode addrMode);
    void Asl(const AMode addrMode);
    void Bcc(const AMode addrMode);
    void Bcs(const AMode addrMode);
    void Beq(const AMode addrMode);
    void Bit(const AMode addrMode);
    void Bmi(const AMode addrMode);
    void Bne(const AMode addrMode);
    void Bpl(const AMode addrMode);
    void Brk(const AMode addrMode);
    void Bvc(const AMode addrMode);
    void Bvs(const AMode addrMode);
    void Clc(const AMode addrMode);
    void Cld(const AMode addrMode);
    void Cli(const AMode addrMode);
    void Clv(const AMode addrMode);
    void Cmp(const AMode addrMode);
    void Cpx(const AMode addrMode);
    void Cpy(const AMode addrMode);
    void Dec(const AMode addrMode);
    void Dex(const AMode addrMode);
    void Dey(const AMode addrMode);
    void Eor(const AMode addrMode);
    void Inc(const AMode addrMode);
    void Inx(const AMode addrMode);
    void Iny(const AMode addrMode);
    void Jmp(const AMode addrMode);
    void Jsr(const AMode addrMode);
    void Lda(const AMode addrMode);
    void Ldx(const AMode addrMode);
    void Ldy(const AMode addrMode);
    void Lsr(const AMode addrMode);
    void Nop(const AMode addrMode);
    void Ora(const AMode addrMode);
    void Pha(const AMode addrMode);
    void Php(const AMode addrMode);
    void Pla(const AMode addrMode);
    void Plp(const AMode addrMode);
    void Rol(const AMode addrMode);
    void Ror(const AMode addrMode);
    void Rti(const AMode addrMode);
    void Rts(const AMode addrMode);
    void Sbc(const AMode addrMode);
    void Sec(const AMode addrMode);
    void Sed(const AMode addrMode);
    void Sei(const AMode addrMode);
    void Sta(const AMode addrMode);
    void Stx(const AMode addrMode);
    void Sty(const AMode addrMode);
    void Tax(const AMode addrMode);
    void Tay(const AMode addrMode);
    void Tsx(const AMode addrMode);
    void Txa(const AMode addrMode);
    void Txs(const AMode addrMode);
    void Tya(const AMode addrMode);

    void Irq();
    void Nmi();
};