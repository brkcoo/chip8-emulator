#include <stdio.h>
#include <string.h>
#include <cstdint>
#include <fstream>
#include <vector>

class Chip8
{
public:
    const unsigned int START_ADDRESS = 0x200; // game is loaded at this address

    uint8_t registers[16];   // 16 8-bit registers, labeled V0 to VF, hold values 0x00 to 0xFF
    uint8_t memory[4096];    // 4096 bytes of memory, address space from 0x000 to 0xFFF
    uint16_t indexRegister;  // 16-bit register, store memory addresses for use in operations, also known as I
    uint16_t programCounter; // 16-bit register, store address of next instruction to execute

    std::vector<uint16_t> stack; // hold 16 PCs

    uint8_t delayTimer;
    uint8_t soundTimer;

    uint8_t inputKeys[16]; // 16 input keys corresponding to 0-F

    uint16_t opcode; // next instruction

    uint32_t video[64 * 32]; // pixel display

    void ResetCPU();          // initialize CPU
    uint16_t GetNextOpcode(); // get next instruction for execution

    // opcodes
    void DecodeOpcode(uint16_t);
    void Opcode_1NNN(uint16_t); // goto address NNN
    void Opcode_00E0(uint16_t); // clears the screen
    void Opcode_00EE(uint16_t); // return from a subroutine
    void Opcode_2NNN(uint16_t); // calls subroutine at NNN
    void Opcode_3XNN(uint16_t); // skips next instruction if VX == NN
    void Opcode_4XNN(uint16_t); // skips next instruction if VX != NN
    void Opcode_5XY0(uint16_t); // skip next instruction if VX == VY
    void Opcode_6XNN(uint16_t); // sets VX to NN
    void Opcode_7XNN(uint16_t); // adds NN to VX, carry flag unchanged
    void Opcode_8XY0(uint16_t); // sets value of VX to VY
    void Opcode_8XY1(uint16_t); // sets VX to (VX | VY)
    void Opcode_8XY2(uint16_t);
    void Opcode_8XY3(uint16_t);
    void Opcode_8XY4(uint16_t);
    void Opcode_8XY5(uint16_t); // subtract VY from VX, VF set to 0 if underflow
    void Opcode_8XY6(uint16_t);
    void Opcode_8XY7(uint16_t);
    void Opcode_8XYE(uint16_t);
    void Opcode_9XY0(uint16_t);
    void Opcode_ANNN(uint16_t);
    void Opcode_BNNN(uint16_t);
    void Opcode_CXNN(uint16_t);
    void Opcode_DXYN(uint16_t);
    void Opcode_EX9E(uint16_t);
    void Opcode_EXA1(uint16_t);
    void Opcode_FX07(uint16_t);
    void Opcode_FX0A(uint16_t);
    void Opcode_FX15(uint16_t);
    void Opcode_FX18(uint16_t);
    void Opcode_FX1E(uint16_t);
    void Opcode_FX29(uint16_t);
    void Opcode_FX33(uint16_t); // store binary-coded decimal representation of VX at I, I+1, I+2
    void Opcode_FX55(uint16_t); // store from V0 to VX with values from memory, starting at I (I left unmodified)
    void Opcode_FX65(uint16_t);
};

void Chip8::ResetCPU()
{
    indexRegister = 0;
    programCounter = START_ADDRESS;          // instructions start here
    memset(registers, 0, sizeof(registers)); // reset registers for use

    FILE *in;
    in = fopen("/invaders", "rb");
    fread(&memory[START_ADDRESS], 0xfff, 1, in);
    fclose(in);
}

uint16_t Chip8::GetNextOpcode()
{
    uint16_t next;
    next = memory[programCounter];      // get first byte of next instruction
    next <<= 8;                         // shift over to receive next byte
    next |= memory[programCounter + 1]; // get second byte of next instruction
    programCounter += 2;                // next instruction is now 2 bytes over
    return next;
}

void Chip8::DecodeOpcode(uint16_t opcode)
{
    switch (opcode & 0xF000)
    {            // get first digit of opcode
    case 0x1000: // jump
        Opcode_1NNN(opcode);
        break;
    case 0x0000:
        switch (opcode & 0x000F)
        {
        case 0x0000:
            Opcode_00E0(opcode);
            break;
        case 0x000E:
            Opcode_00EE(opcode);
            break;
        }
        break;
    default: // opcode tbd
        break;
    }
}

void Chip8::Opcode_1NNN(uint16_t opcode)
{
    programCounter = opcode & 0x0FFF; // get the back 3 values of the opcode (address)
}

void Chip8::Opcode_00E0(uint16_t opcode) {}

void Chip8::Opcode_00EE(uint16_t opcode)
{
    stack.pop_back();              // finished
    programCounter = stack.back(); // return to previous address
}

void Chip8::Opcode_2NNN(uint16_t opcode)
{
    stack.push_back(programCounter);  // save program counter
    programCounter = opcode & 0x0FFF; // goto next address
}

void Chip8::Opcode_3XNN(uint16_t opcode)
{
    int regx = opcode & 0x0F00;
    regx >>= 8;
    int byte = opcode & 0x00FF;
    if (registers[regx] == byte)
    {
        programCounter += 2; // skip next instruction
    }
}

void Chip8::Opcode_4XNN(uint16_t opcode)
{
    // basically the same as 3XNN
    int regx = opcode & 0x0F00;
    regx >>= 8;
    int byte = opcode & 0x00FF;
    if (registers[regx] != byte)
    {
        programCounter += 2; // skip next instruction
    }
}

void Chip8::Opcode_5XY0(uint16_t opcode)
{
    // get the values in the registers
    int regx = opcode & 0x0F00;
    regx >>= 8; // shift x over 8 bits
    int regy = opcode & 0x00F0;
    regy >>= 4; // shift y over 4 bits
    if (registers[regx] == registers[regy])
    {
        programCounter += 2; // skip next instruction
    }
}

void Chip8::Opcode_6XNN(uint16_t opcode)
{
    int regx = opcode & 0x0F00;
    regx >>= 8;
    int byte = opcode & 0x00FF;
    registers[regx] = byte;
}

void Chip8::Opcode_7XNN(uint16_t opcode)
{
    int regx = opcode & 0x0F00;
    regx >>= 8;
    int byte = opcode & 0x00FF;
    registers[regx] += byte;
}

void Chip8::Opcode_8XY0(uint16_t opcode)
{
    int regx = opcode & 0x0F00;
    regx >>= 8;
    int regy = opcode & 0x00F0;
    regy >>= 4;
    registers[regx] = registers[regy];
}

void Chip8::Opcode_8XY1(uint16_t opcode)
{
    int regx = opcode & 0x0F00;
    regx >>= 8;
    int regy = opcode & 0x00F0;
    regy >>= 4;
    registers[regx] |= registers[regy];
}

void Chip8::Opcode_8XY2(uint16_t opcode) {}
void Chip8::Opcode_8XY3(uint16_t opcode) {}
void Chip8::Opcode_8XY4(uint16_t opcode) {}

void Chip8::Opcode_8XY5(uint16_t opcode)
{
    // get the values in the registers
    registers[0xF] = 1;
    int regx = opcode & 0x0F00;
    regx >>= 8;
    int regy = opcode & 0x00F0;
    regy >>= 4;
    int valueX = registers[regx];
    int valueY = registers[regy];
    // check if there will be underflow, store result in VF
    if (valueY > valueX)
        registers[0xF] = 0;
    // perform subtraction
    registers[regx] -= registers[regy];
}

void Chip8::Opcode_8XY6(uint16_t opcode) {}
void Chip8::Opcode_8XY7(uint16_t opcode) {}
void Chip8::Opcode_8XYE(uint16_t opcode) {}
void Chip8::Opcode_9XY0(uint16_t opcode) {}
void Chip8::Opcode_ANNN(uint16_t opcode) {}
void Chip8::Opcode_BNNN(uint16_t opcode) {}
void Chip8::Opcode_CXNN(uint16_t opcode) {}
void Chip8::Opcode_DXYN(uint16_t opcode) {}
void Chip8::Opcode_EX9E(uint16_t opcode) {}
void Chip8::Opcode_EXA1(uint16_t opcode) {}
void Chip8::Opcode_FX07(uint16_t opcode) {}
void Chip8::Opcode_FX0A(uint16_t opcode) {}
void Chip8::Opcode_FX15(uint16_t opcode) {}
void Chip8::Opcode_FX18(uint16_t opcode) {}
void Chip8::Opcode_FX1E(uint16_t opcode) {}
void Chip8::Opcode_FX29(uint16_t opcode) {}

void Chip8::Opcode_FX33(uint16_t opcode)
{
    // get the value in the register
    int regx = opcode & 0x0F00;
    regx >>= 8;
    int value = registers[regx];

    // get each digit of the register separately
    int hundreds = value / 100;
    int tens = (value / 10) % 10;
    int ones = value % 10;

    // store in desired location
    memory[indexRegister] = hundreds;
    memory[indexRegister + 1] = tens;
    memory[indexRegister + 2] = ones;
}

void Chip8::Opcode_FX55(uint16_t opcode)
{
    // get final register
    int regx = opcode & 0x0F00;
    regx >>= 8;
    for (int i = 0; i < regx; i++)
    {
        memory[indexRegister + i] = registers[i];
    }
}
void Chip8::Opcode_FX65(uint16_t opcode) {}
