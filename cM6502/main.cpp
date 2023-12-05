#include "cM6502.h"

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