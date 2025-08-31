#include <string.h>
#include <vector>
#include <stdio.h>
#include <cstdint>
#include <fstream>
#include <random>
#include <iostream>
#include <chrono>
#include <thread>
#include <stdint.h>
#include <windows.h>

class Chip8
{
public:
    const unsigned int START_ADDRESS = 0x200; // game is loaded at this address

    uint8_t registers[16];   // 16 8-bit registers, labeled V0 to VF, hold values 0x00 to 0xFF
    uint8_t memory[4096];    // 4096 bytes of memory, address space from 0x000 to 0xFFF
    uint16_t indexRegister;  // 16-bit register, store memory addresses for use in operations, also known as I
    uint16_t programCounter; // 16-bit register, store address of next instruction to execute

    std::vector<uint16_t> stack; // hold 16 PCs

    // decrement timers 60 times per second (60 Hz)
    uint8_t delayTimer;
    uint8_t soundTimer;

    uint8_t inputKeys[16]; // 16 input keys corresponding to 0-F

    uint16_t opcode; // next instruction

    uint32_t video[64][32]; // pixel display

    void ResetCPU(char *filename); // initialize CPU
    void Cycle();

    uint16_t GetNextOpcode(); // get next instruction for execution

    // opcodes
    void DecodeOpcode(uint16_t); // run the proper instruction from given opcode
    void Opcode_1NNN(uint16_t);  // goto address NNN
    void Opcode_00E0(uint16_t);  // clears the screen
    void Opcode_00EE(uint16_t);  // return from a subroutine
    void Opcode_2NNN(uint16_t);  // calls subroutine at NNN
    void Opcode_3XNN(uint16_t);  // skips next instruction if VX == NN
    void Opcode_4XNN(uint16_t);  // skips next instruction if VX != NN
    void Opcode_5XY0(uint16_t);  // skip next instruction if VX == VY
    void Opcode_6XNN(uint16_t);  // sets VX to NN
    void Opcode_7XNN(uint16_t);  // adds NN to VX, carry flag unchanged
    void Opcode_8XY0(uint16_t);  // VX = VY
    void Opcode_8XY1(uint16_t);  // VX |= VY
    void Opcode_8XY2(uint16_t);  // VX &= VY
    void Opcode_8XY3(uint16_t);  // VX ^= VY
    void Opcode_8XY4(uint16_t);  // VX += VY, VF set to 1 if overflow
    void Opcode_8XY5(uint16_t);  // subtract VY from VX, VF set to 0 if underflow
    void Opcode_8XY6(uint16_t);  // VX >>= 1, store LSB prior to shift in VF
    void Opcode_8XY7(uint16_t);  // VX = VY - VX, VF set to 0 if underflow
    void Opcode_8XYE(uint16_t);  // VX <<= 1, VF set to 1 if MSB prior to shift was set, otherwise 0
    void Opcode_9XY0(uint16_t);  // skip next instruction if VX != VY
    void Opcode_ANNN(uint16_t);  // set I to address NNN
    void Opcode_BNNN(uint16_t);  // jump to address NNN + V0
    void Opcode_CXNN(uint16_t);  // set VX to ranodm int (0 to 255) & NN (bitwise operation)
    void Opcode_DXYN(uint16_t);  // draw sprite
    void Opcode_EX9E(uint16_t);  // skip next instruction if key in VX is pressed
    void Opcode_EXA1(uint16_t);  // skip next instruction if key in VX is NOT pressed
    void Opcode_FX07(uint16_t);  // set VX to value of delay timer
    void Opcode_FX0A(uint16_t);  // await key press and store in VX
    void Opcode_FX15(uint16_t);  // set delay timer to VX
    void Opcode_FX18(uint16_t);  // set sound timer to VX
    void Opcode_FX1E(uint16_t);  // add VX to I
    void Opcode_FX29(uint16_t);  // set I to sprite location for character in VX
    void Opcode_FX33(uint16_t);  // store binary-coded decimal representation of VX at I, I+1, I+2
    void Opcode_FX55(uint16_t);  // store from V0 to VX with values from memory, starting at I (I left unmodified)
    void Opcode_FX65(uint16_t);  // fills from V0 to VX with values from memory, starting at I (I left unmodified)
};
