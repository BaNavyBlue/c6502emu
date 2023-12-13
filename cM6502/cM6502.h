#ifndef CM6502_H
#define CM6502_H
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>

// http://www.obelisk.me.uk/6502/

namespace M6502
{
    using u8 = unsigned char;
    using u16 = unsigned short;
    using u32 = unsigned int;
    struct Mem;
    struct CPU;
}




struct M6502::Mem{
    static constexpr u32 MAX_MEM = 1024 * 64;
    u8 data[MAX_MEM];
    void Initialize()
    {
        for(u32 i = 0; i < MAX_MEM; i++){
            data[i] = 0;
        }
    }
    
    // Read one Byte
    u8 operator[](u32 address) const
    {
        return data[address];
    }

    // Write one Byte
    u8& operator[](u32 address)
    {
        return data[address];
    }

    // costs 2 cycles
    void WriteWord(u16 value, u16 address)
    {
        data[address] = value & 0xFF;
        data[address + 1] = (value >> 8); 
    }

};

struct M6502::CPU{
    u16 PC;     // program counter
    u16 SP;     // stack pointer

    u8 A, X, Y; // registers

    // Status Flags
    u8 C:1;
    u8 Z:1;
    u8 I:1;
    u8 D:1;
    u8 B:1;
    u8 V:1;
    u8 N:1;

    u8 FetchByte(Mem& mem)
    {
        u8 data = mem[PC];
        PC++;
        return data;
    }

    u8 ReadByteZPage(u8 address, Mem& mem)
    {
        return mem[address];
    }

    u8 ReadByte(u16 address, Mem& mem)
    {
        return mem[address];
    }

    u16 ReadWord(u16 address, Mem& mem)
    {
        u8 lowByte = ReadByte(address, mem);
        u8 highByte = ReadByte(address + 1, mem);
        return (highByte << 8)|lowByte;
    }

    u16 FetchWord(Mem& mem)
    {
        // 6502 is little endian
        u16 data = mem[PC];
        PC++;
        data |= (mem[PC] << 8);
        PC++;
        return data;
    }

    void Reset(Mem& mem)
    {
        PC = 0xFFFC;
        SP = 0x00FF;
        C = Z = I = D = B = V = N = 0;
        A = X = Y = 0;
        mem.Initialize();
    }
    
    // opcodes
    static constexpr u8 
                        // LDA
                        INS_LDA_IM = 0xA9,
                        INS_LDA_ZP = 0xA5,
                        INS_LDA_ZPX = 0xB5,
                        INS_LDA_ABS = 0xAD,
                        INS_LDA_ABSX = 0xBD,
                        INS_LDA_ABSY = 0xB9,
                        INS_LDA_INDX = 0xA1,
                        INS_LDA_INDY = 0xB1,
                        // LDX
                        INS_LDX_IM = 0xA2,
                        INS_LDX_ZP = 0xA6,
                        INS_LDX_ZPY = 0xB6,
                        INS_LDX_ABS = 0xAE,
                        INS_LDX_ABSY = 0xBE,
                        // LDY
                        INS_LDY_IM = 0xA0,
                        INS_LDY_ZP = 0xA4,
                        INS_LDY_ZPX = 0xB4,
                        INS_LDY_ABS = 0xAC,
                        INS_LDY_ABSX = 0xBC,
                        INS_JSR = 0x20;
    void LDASetStatus()
    {
        Z = (A == 0);
        N = (A & 0b10000000) > 0;       
    }

    void LDXSetStatus()
    {
        Z = (X == 0);
        N = (X & 0b10000000) > 0;       
    }

    void LDYSetStatus()
    {
        Z = (Y == 0);
        N = (Y & 0b10000000) > 0;       
    }

    int Execute(int cycles, Mem& mem)
    {
        int requestedCycles = cycles;
        while(cycles > 0){
            u8 ins = FetchByte(mem);
            cycles--;
            switch(ins){
                case INS_LDA_IM:
                {
                    u8 value = FetchByte(mem);
                    cycles--;
                    A = value;
                    LDASetStatus();
                }break;               
                case INS_LDA_ZP:
                {
                    u8 address = FetchByte(mem);
                    cycles--;
                    A = ReadByteZPage(address, mem);
                    cycles--;
                    LDASetStatus();
                }break;
                case INS_LDA_ZPX:
                {
                    u8 zPageAdd = FetchByte(mem);
                    cycles--;
                    zPageAdd += X;
                    cycles--;
                    A = ReadByteZPage(zPageAdd, mem);
                    cycles--;
                    LDASetStatus();
                }break;
                case INS_LDA_ABS:
                {
                    u16 absAddress = FetchWord(mem);
                    cycles -= 2;
                    A = ReadByte(absAddress, mem);
                    cycles--;
                    LDASetStatus();
                }break;
                case INS_LDA_ABSX:
                {
                    u16 absAddress = FetchWord(mem);
                    cycles -= 2;
                    if((absAddress & 0x00FF) + X > 0xFF){
                        cycles--;
                    }
                    absAddress += X;
                    A = ReadByte(absAddress, mem);
                    cycles--;
                    LDASetStatus();
                }break;
                case INS_LDA_ABSY:
                {
                    u16 absAddress = FetchWord(mem);
                    cycles -= 2;
                    if((absAddress & 0x00FF) + Y > 0xFF){
                        cycles--;
                    }
                    absAddress += Y;
                    A = ReadByte(absAddress, mem);
                    cycles--;
                    LDASetStatus();
                }break;
                case INS_LDA_INDX:
                {
                    u16 zPageAddress = FetchByte(mem);
                    cycles--;
                    zPageAddress += X;
                    cycles--;
                    u16 targetAddress = ReadWord(zPageAddress, mem);
                    cycles -= 2;
                    A = ReadByte(targetAddress, mem);
                    cycles--;
                    LDASetStatus();
                }break; 
                case INS_LDA_INDY:
                {
                    u16 zPageAddress = FetchByte(mem);
                    cycles--;
                    u16 targetAddress = ReadWord(zPageAddress, mem);
                    cycles -= 2;
                    if((targetAddress&0x00FF) + Y > 0xFF){
                        cycles--;
                    }
                    targetAddress += Y;
                    A = ReadByte(targetAddress, mem);
                    cycles--;
                    LDASetStatus();
                }break;
                case INS_LDX_IM:
                {
                    u8 value = FetchByte(mem);
                    cycles--;
                    X = value;
                    LDXSetStatus();
                }break;
                case INS_LDX_ZP:
                {
                    u8 address = FetchByte(mem);
                    cycles--;
                    X = ReadByteZPage(address, mem);
                    cycles--;
                    LDXSetStatus();
                }break;
                case INS_LDX_ZPY:
                {
                    u8 zPageAdd = FetchByte(mem);
                    cycles--;
                    zPageAdd += Y;
                    cycles--;
                    X = ReadByteZPage(zPageAdd, mem);
                    cycles--;
                    LDXSetStatus();
                }break;
                case INS_LDX_ABS:
                {
                    u16 absAddress = FetchWord(mem);
                    cycles -= 2;
                    X = ReadByte(absAddress, mem);
                    cycles--;
                    LDXSetStatus();
                }break;
                case INS_LDX_ABSY:
                {
                    u16 absAddress = FetchWord(mem);
                    cycles -= 2;
                    if((absAddress & 0x00FF) + Y > 0xFF){
                        cycles--;
                    }
                    absAddress += Y;
                    X = ReadByte(absAddress, mem);
                    cycles--;
                    LDXSetStatus();
                }break;
                case INS_LDY_IM:
                {
                    u8 value = FetchByte(mem);
                    cycles--;
                    Y = value;
                    LDYSetStatus();
                }break;
                case INS_LDY_ZP:
                {
                    u8 address = FetchByte(mem);
                    cycles--;
                    Y = ReadByteZPage(address, mem);
                    cycles--;
                    LDYSetStatus();
                }break; 
                case INS_LDY_ZPX:
                {
                    u8 zPageAdd = FetchByte(mem);
                    cycles--;
                    zPageAdd += X;
                    cycles--;
                    Y = ReadByteZPage(zPageAdd, mem);
                    cycles--;
                    LDYSetStatus();
                }break;
                case INS_LDY_ABS:
                {
                    u16 absAddress = FetchWord(mem);
                    cycles -= 2;
                    Y = ReadByte(absAddress, mem);
                    cycles--;
                    LDYSetStatus();
                }break;
                case INS_LDY_ABSX:
                {
                    u16 absAddress = FetchWord(mem);
                    cycles -= 2;
                    if((absAddress & 0x00FF) + X > 0xFF){
                        cycles--;
                    }
                    absAddress += X;
                    Y = ReadByte(absAddress, mem);
                    cycles--;
                    LDYSetStatus();
                }break;            
                case INS_JSR:
                {
                    u16 subAddr = FetchWord(mem);
                    cycles--;
                    mem.WriteWord(PC - 1, SP);
                    cycles -= 2;
                    SP++;
                    cycles--;
                    PC = subAddr;
                    cycles--;
                }
                default:
                    std::cout << "Instruction: " << static_cast<int>(ins) << " not handled" << std::endl;
                    cycles = - 1;
                    requestedCycles = 0;
                    break;
            }
        }
        return requestedCycles - cycles;
    }
};
#endif