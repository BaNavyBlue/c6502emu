#include <stdio.h>
#include <stdlib.h>
#include <cstdint>

// http://www.obelisk.me.uk/6502/

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;


struct Mem{
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

struct CPU{
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

    u8 ReadByte(u8 address, Mem& mem)
    {
        return mem[address];
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
    static constexpr u8 INS_LDA_IM = 0xA9,
                        INS_LDA_ZP = 0xA5,
                        INS_LDA_ZPX = 0xB5,
                        INS_JSR = 0x20;
    void LDASetStatus()
    {
        Z = (A == 0);
        N = (A & 0b1000000) > 0;       
    }

    void Execute(u32 cycles, Mem& mem)
    {
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
                    A = ReadByte(address, mem);
                    cycles--;
                    LDASetStatus();
                }break;
                case INS_LDA_ZPX:
                {
                    u8 zPageAdd = FetchByte(mem);
                    cycles--;
                    zPageAdd += X;
                    cycles--;
                    A = ReadByte(zPageAdd, mem);
                    cycles--;
                    LDASetStatus();
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
                // default:
                //     break;
            }
        }
    }
};


int main(){
    // zero page first 256 bytes of memory
    Mem mem;
    CPU cpu;
    cpu.Reset(mem);
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x42;
    cpu.Execute(2, mem);
    printf("cpu.A: 0x%x\r\ncpu.PC: 0x%x\r\n", cpu.A, cpu.PC);
    
    cpu.Reset(mem);
    mem[0xFFFC] = CPU::INS_LDA_ZP;
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0x84;
    cpu.Execute(3, mem);
    printf("cpu.A: 0x%x\r\ncpu.PC: 0x%x\r\n", cpu.A, cpu.PC);

    cpu.Reset(mem);
    mem[0xFFFC] = CPU::INS_JSR;
    mem[0xFFFD] = 0x42;
    mem[0xFFFE] = 0x42;
    mem[0x4242] = CPU::INS_LDA_IM;
    mem[0x4243] = 0x84;
    printf("Before\r\n");
    printf("cpu.SP: 0x%x\r\ncpu.PC: 0x%x\r\n", cpu.SP, cpu.PC);
    cpu.Execute(8, mem);
    printf("After\r\n");
    printf("cpu.A: 0x%x\r\ncpu.PC: 0x%x\r\n", cpu.A, cpu.PC);
    printf("cpu.SP: 0x%x\r\ncpu.PC: 0x%x\r\n", cpu.SP, cpu.PC);

    return 0;
}