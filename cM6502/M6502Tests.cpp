#include "gtest/gtest.h"
#include "cM6502.h"

class M6502Test1 : public testing::Test
{
public:
    M6502::Mem mem;
    M6502::CPU cpu;
    virtual void SetUp()
    {
        cpu.Reset(mem);
    }

    virtual void TearDownd()
    {

    }
    void TestLoadMessengerImmediate(
        M6502::u8 opCode,
        M6502::u8 M6502::CPU::*Register);
};

TEST_F(M6502Test1, TheCPUDoesNothingWhenExecute0Cycles)
{
    // given:
    constexpr int NUM_CYCLES = 0;

    // when:
    int cyclesUsed = NUM_CYCLES;
    cpu.Execute(NUM_CYCLES, mem);

    //then
    EXPECT_EQ(cyclesUsed, 0);
}

TEST_F(M6502Test1, CPUUnderCycleLDAImmediateCanLoadAValueIntoTheARegister)
{
    // given:

    mem[0xFFFC] = M6502::CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x84;

    //when:
    int cyclesUsed = cpu.Execute(1, mem);

    // then:
    EXPECT_EQ( cpu.A, 0x84);
    EXPECT_EQ(cyclesUsed, 2);
    EXPECT_FALSE(cpu.Z);
    EXPECT_TRUE(cpu.N);
}


TEST_F(M6502Test1, LDAImmediateCanLoadAValueIntoTheARegister)
{
    // UGLEE SYNTAX
    TestLoadMessengerImmediate(M6502::CPU::INS_LDA_IM, &M6502::CPU::A);
}

TEST_F(M6502Test1, LDXImmediateCanLoadAValueIntoTheXRegister)
{
    // UGLEE SYNTAX
    TestLoadMessengerImmediate(M6502::CPU::INS_LDX_IM, &M6502::CPU::X);
}

TEST_F(M6502Test1, LDYImmediateCanLoadAValueIntoTheYRegister)
{
    // UGLEE SYNTAX
    TestLoadMessengerImmediate(M6502::CPU::INS_LDY_IM, &M6502::CPU::X);
}

TEST_F(M6502Test1, LDAZeroPageCanLoadAValueIntoTheARegister)
{
    // given:

    mem[0xFFFC] = M6502::CPU::INS_LDA_ZP;
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0x37;

    //when:
    int cyclesUsed = cpu.Execute(3, mem);

    // then:
    EXPECT_EQ( cpu.A, 0x37);
    EXPECT_EQ(cyclesUsed, 3);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(M6502Test1, LDAZeroPageXCanLoadAValueIntoTheARegister)
{
    // given:
    cpu.X = 0x05;

    mem[0xFFFC] = M6502::CPU::INS_LDA_ZPX;
    mem[0xFFFD] = 0x42;
    mem[0x0047] = 0x37;

    //when:
    int cyclesUsed = cpu.Execute(4, mem);

    // then:
    EXPECT_EQ( cpu.A, 0x37);
    EXPECT_EQ(cyclesUsed, 4);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(M6502Test1, LDAZeroPageXCanLoadAValueIntoTheARegisterWhenItWraps)
{
    // given:
    cpu.X = 0xFF;

    mem[0xFFFC] = M6502::CPU::INS_LDA_ZPX;
    mem[0xFFFD] = 0x80;
    mem[0x007F] = 0x37;

    //when:
    int cyclesUsed = cpu.Execute(4, mem);

    // then:
    EXPECT_EQ( cpu.A, 0x37);
    EXPECT_EQ(cyclesUsed, 4);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(M6502Test1, LDAAbsoluteCanLoadAValueIntoTheARegister)
{
    // given:
    mem[0xFFFC] = M6502::CPU::INS_LDA_ABS;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44;
    mem[0x4480] = 0x37;
    constexpr int EXPECTED_CYCLES = 4;
    //when:
    int cyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);

    // then:
    EXPECT_EQ( cpu.A, 0x37);
    EXPECT_EQ(cyclesUsed, EXPECTED_CYCLES);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(M6502Test1, LDAAbsoluteXCanLoadAValueIntoTheARegister)
{
    // given:
    cpu.X = 0x01;

    mem[0xFFFC] = M6502::CPU::INS_LDA_ABSX;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44;
    mem[0x4481] = 0x37; // 0x4480 + 0x4481
    constexpr int EXPECTED_CYCLES = 4;
    //when:
    int cyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);

    // then:
    EXPECT_EQ( cpu.A, 0x37);
    EXPECT_EQ(cyclesUsed, EXPECTED_CYCLES);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(M6502Test1, LDAAbsoluteXCanLoadAValueIntoTheARegisterWhenCrossPageBoundary)
{
    // given:
    cpu.X = 0xFF;

    mem[0xFFFC] = M6502::CPU::INS_LDA_ABSX;
    mem[0xFFFD] = 0x02;
    mem[0xFFFE] = 0x44; //0x4402
    mem[0x4501] = 0x37; //0x4402 + 0xFF crosses the page boundary
    constexpr int EXPECTED_CYCLES = 5;
    //when:
    int cyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);

    // then:
    EXPECT_EQ( cpu.A, 0x37);
    EXPECT_EQ(cyclesUsed, EXPECTED_CYCLES);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(M6502Test1, LDAAbsoluteYCanLoadAValueIntoTheARegister)
{
    // given:
    cpu.Y = 0x01;

    mem[0xFFFC] = M6502::CPU::INS_LDA_ABSY;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44;
    mem[0x4481] = 0x37; // 0x4480 + 0x4481
    constexpr int EXPECTED_CYCLES = 4;
    //when:
    int cyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);

    // then:
    EXPECT_EQ( cpu.A, 0x37);
    EXPECT_EQ(cyclesUsed, EXPECTED_CYCLES);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(M6502Test1, LDAAbsoluteYCanLoadAValueIntoTheARegisterWhenCrossPageBoundary)
{
    // given:
    cpu.Y = 0xFF;

    mem[0xFFFC] = M6502::CPU::INS_LDA_ABSY;
    mem[0xFFFD] = 0x02;
    mem[0xFFFE] = 0x44; //0x4402
    mem[0x4501] = 0x37; //0x4402 + 0xFF crosses the page boundary
    constexpr int EXPECTED_CYCLES = 5;
    //when:
    int cyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);

    // then:
    EXPECT_EQ( cpu.A, 0x37);
    EXPECT_EQ(cyclesUsed, EXPECTED_CYCLES);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(M6502Test1, LDAIndirectXCanLoadAValueIntoTheARegister)
{
    // given:
    cpu.X = 0x04;

    mem[0xFFFC] = M6502::CPU::INS_LDA_INDX;
    mem[0xFFFD] = 0x02;
    mem[0x0006] = 0x00; //0x4402
    mem[0x0007] = 0x80; //0x4402 + 0xFF crosses the page boundary
    mem[0x8000] = 0x37;
    constexpr int EXPECTED_CYCLES = 6;
    //when:
    int cyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);

    // then:
    EXPECT_EQ( cpu.A, 0x37);
    EXPECT_EQ(cyclesUsed, EXPECTED_CYCLES);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(M6502Test1, LDAIndirectYCanLoadAValueIntoTheARegister)
{
    // given:
    cpu.Y = 0x04;

    mem[0xFFFC] = M6502::CPU::INS_LDA_INDY;
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x00; //0x4402
    mem[0x0003] = 0x80; //0x4402 + 0xFF crosses the page boundary
    mem[0x8004] = 0x37;
    constexpr int EXPECTED_CYCLES = 5;
    //when:
    int cyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);

    // then:
    EXPECT_EQ( cpu.A, 0x37);
    EXPECT_EQ(cyclesUsed, EXPECTED_CYCLES);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(M6502Test1, LDAIndirectYCanLoadAValueIntoTheARegisterWhenCrossPageBoundary)
{
    // given:
    cpu.Y = 0xFF;

    mem[0xFFFC] = M6502::CPU::INS_LDA_INDY;
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x02;
    mem[0x0003] = 0x80;
    mem[0x8101] = 0x37;
    constexpr int EXPECTED_CYCLES = 6;
    //when:
    int cyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);

    // then:
    EXPECT_EQ( cpu.A, 0x37);
    EXPECT_EQ(cyclesUsed, EXPECTED_CYCLES);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
}

TEST_F(M6502Test1, CPUNoCycles)
{
    // given:

    //when:
    int cyclesUsed = cpu.Execute(0, mem);

    // then:
    EXPECT_EQ(cyclesUsed, 0);

}

void M6502Test1::TestLoadMessengerImmediate(
    M6502::u8 opCode,
    M6502::u8 M6502::CPU::*RegisterToTest)
{
    // given:

    mem[0xFFFC] = opCode;
    mem[0xFFFD] = 0x84;

    //when:
    int cyclesUsed = cpu.Execute(2, mem);

    // then:
    EXPECT_EQ( cpu.*RegisterToTest, 0x84);
    EXPECT_EQ(cyclesUsed, 2);
    EXPECT_FALSE(cpu.Z);
    EXPECT_TRUE(cpu.N);
}

#if 0
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
#endif