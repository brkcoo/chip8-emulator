#include "chip8.h"

// sprite data representing hexadecimal numbers, 4x5 pixels each
uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void Chip8::ResetCPU()
{
    indexRegister = 0;
    programCounter = START_ADDRESS;          // instructions start here
    memset(registers, 0, sizeof(registers)); // reset registers for use

    FILE *in;
    in = fopen("", "rb");
    fread(&memory[START_ADDRESS], 0xfff, 1, in);
    fclose(in);

    // load fontset into memory, 0x50 to 0x9F popular convention apparently
    for (int i = 0; i < 80; i++)
    {
        memory[0x50 + i] = fontset[i];
    }
}

void Chip8::Cycle()
{
    // fetch opcode
    opcode = GetNextOpcode();

    // decode and execute opcode
    DecodeOpcode(opcode);

    // decrease timers if applicable
    if (delayTimer > 0)
        delayTimer--;
    if (soundTimer > 0)
        soundTimer--;
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

// 5 trillion switches
void Chip8::DecodeOpcode(uint16_t opcode)
{
    // start with first digit of opcode
    switch (opcode & 0xF000)
    {
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
    case 0x1000:
        Opcode_1NNN(opcode);
        break;
    case 0x2000:
        Opcode_2NNN(opcode);
        break;
    case 0x3000:
        Opcode_3XNN(opcode);
        break;
    case 0x4000:
        Opcode_4XNN(opcode);
        break;
    case 0x5000:
        Opcode_5XY0(opcode);
        break;
    case 0x6000:
        Opcode_6XNN(opcode);
        break;
    case 0x7000:
        Opcode_7XNN(opcode);
        break;
    case 0x8000:
        switch (opcode & 0x000F)
        {
        case 0x0000:
            Opcode_8XY0(opcode);
            break;
        case 0x0001:
            Opcode_8XY1(opcode);
            break;
        case 0x0002:
            Opcode_8XY2(opcode);
            break;
        case 0x0003:
            Opcode_8XY3(opcode);
            break;
        case 0x0004:
            Opcode_8XY4(opcode);
            break;
        case 0x0005:
            Opcode_8XY5(opcode);
            break;
        case 0x0006:
            Opcode_8XY6(opcode);
            break;
        case 0x0007:
            Opcode_8XY7(opcode);
            break;
        case 0x000E:
            Opcode_8XYE(opcode);
            break;
        }
        break;
    case 0x9000:
        Opcode_9XY0(opcode);
        break;
    case 0xA000:
        Opcode_ANNN(opcode);
        break;
    case 0xB000:
        Opcode_BNNN(opcode);
        break;
    case 0xC000:
        Opcode_CXNN(opcode);
        break;
    case 0xD000:
        Opcode_DXYN(opcode);
        break;
    case 0xE000:
        switch (opcode & 0x000F)
        {
        case 0x000E:
            Opcode_EX9E(opcode);
            break;
        case 0x0001:
            Opcode_EXA1(opcode);
            break;
        }
        break;
    case 0xF000:
        switch (0x000F)
        {
        case 0x0007:
            Opcode_FX07(opcode);
            break;
        case 0x00A:
            Opcode_FX0A(opcode);
            break;
        case 0x0008:
            Opcode_FX18(opcode);
            break;
        case 0x000E:
            Opcode_FX1E(opcode);
            break;
        case 0x0009:
            Opcode_FX29(opcode);
            break;
        case 0x0003:
            Opcode_FX33(opcode);
            break;
        case 0x0005:
            switch (opcode & 0x00F0)
            {
            case 0x0010:
                Opcode_FX15(opcode);
                break;
            case 0x0050:
                Opcode_FX55(opcode);
                break;
            case 0x0060:
                Opcode_FX65(opcode);
                break;
            }
            break;
        }
        break;
    default: // opcode not found
        break;
    }
}

void Chip8::Opcode_1NNN(uint16_t opcode)
{
    programCounter = opcode & 0x0FFF; // get the back 3 values of the opcode (address)
}

void Chip8::Opcode_00E0(uint16_t opcode)
{
    memset(video, 0, sizeof(video)); // reset video
}

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

void Chip8::Opcode_8XY2(uint16_t opcode)
{
    int regx = opcode & 0x0F00;
    regx >>= 8;
    int regy = opcode & 0x00F0;
    regy >>= 4;
    registers[regx] &= registers[regy];
}
void Chip8::Opcode_8XY3(uint16_t opcode)
{
    int regx = opcode & 0x0F00;
    regx >>= 8;
    int regy = opcode & 0x00F0;
    regy >>= 4;
    registers[regx] ^= registers[regy];
}
void Chip8::Opcode_8XY4(uint16_t opcode)
{
    registers[0xF] = 0;
    int regx = opcode & 0x0F00;
    regx >>= 8;
    int regy = opcode & 0x00F0;
    regy >>= 4;
    registers[regx] += registers[regy];
    if (registers[regx] + registers[regy] > 255)
        registers[0xF] = 1;
}

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

void Chip8::Opcode_8XY6(uint16_t opcode)
{
    int regx = opcode & 0x0F00;
    regx >>= 8;
    uint8_t lsb = registers[regx] & 0x1;
    registers[regx] >>= 1;
    registers[0xF] = lsb;
}

void Chip8::Opcode_8XY7(uint16_t opcode)
{
    registers[0xF] = 1;
    int regx = opcode & 0x0F00;
    regx >>= 8;
    int regy = opcode & 0x00F0;
    regy >>= 4;
    if (registers[regx] > registers[regx]) // check for underflow
        registers[0xF] = 0;
    registers[regx] = registers[regy] - registers[regx];
}

void Chip8::Opcode_8XYE(uint16_t opcode)
{
    int regx = opcode & 0x0F00;
    regx >>= 8;
    registers[0xF] = (registers[regx] >> 8) & 0x1;
    registers[regx] <<= 1;
}

void Chip8::Opcode_9XY0(uint16_t opcode)
{
    int regx = opcode & 0x0F00;
    regx >>= 8;
    int regy = opcode & 0x00F0;
    regy >>= 4;
    if (registers[regx] != registers[regy])
    {
        programCounter += 2;
    }
}
void Chip8::Opcode_ANNN(uint16_t opcode)
{
    indexRegister = opcode & 0x0FFF;
}

void Chip8::Opcode_BNNN(uint16_t opcode)
{
    programCounter = registers[0] + (opcode & 0x0FFF);
}

void Chip8::Opcode_CXNN(uint16_t opcode)
{
    int regx = opcode & 0x0F00;
    regx >>= 8;

    // get random number 0 to 255
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint8_t> distrib(0, 255u);
    uint8_t randomByte = distrib(gen);

    registers[regx] = randomByte & (opcode & 0x00FF);
}

void Chip8::Opcode_DXYN(uint16_t opcode)
{
    // draw N pixels tall sprite from memory location held in index
    // at horizontal coordinate VX and vertical coordinate VY
    // on pixels will flip what is already on the screen , from left to right and MSB to LSB
    // VF = 1 if any pixels were turned off by this
    // a sprite is 8 pixels wide
    int regx = opcode & 0x0F00;
    regx >>= 8;
    int regy = opcode & 0x00F0;
    regy >>= 4;

    int startX = registers[regx];
    int startY = registers[regy];
    int height = opcode & 0x000F;

    registers[0xF] = 0;

    for (int row = 0; row < height; row++)
    {
        uint8_t spriteData = memory[indexRegister + row];
        for (int col = 0; col < 8; col++)
        { // always 8 columns (sprite is 8 pixels wide)
            uint8_t spritePixel = spriteData >> col;
            if (spritePixel == 0x1)
            {
                int x = startX + col;
                int y = startY + row;
                if (video[x][y] == 1) // pixel IS being flipped off
                    registers[0xF] = 1;
                video[x][y] ^= 1; // flip pixel
            }
        }
    }
}

void Chip8::Opcode_EX9E(uint16_t opcode)
{
    int regx = opcode & 0x0F00;
    regx >>= 8;
    if (inputKeys[registers[regx]]) // check if input key stored in VX is pressed
    {
        programCounter += 2;
    }
}

void Chip8::Opcode_EXA1(uint16_t opcode)
{
    int regx = opcode & 0x0F00;
    regx >>= 8;
    if (!inputKeys[registers[regx]]) // check if input key stored in VX is NOT pressed
    {
        programCounter += 2;
    }
}

void Chip8::Opcode_FX07(uint16_t opcode)
{
    int regx = opcode & 0x0F00;
    regx >>= 8;
    registers[regx] = delayTimer;
}

void Chip8::Opcode_FX0A(uint16_t opcode)
{
    int regx = opcode & 0x0F00;
    regx >>= 8;
    for (int i = 0; i < 16; i++)
    {
        if (inputKeys[i])
        {
            registers[regx] = i; // store key in VX
            return;
        }
    }
    // if no key pressed, decrement program counter
    programCounter -= 2;
}

void Chip8::Opcode_FX15(uint16_t opcode)
{
    int regx = opcode & 0x0F00;
    regx >>= 8;
    delayTimer = registers[regx];
}

void Chip8::Opcode_FX18(uint16_t opcode)
{
    int regx = opcode & 0x0F00;
    regx >>= 8;
    soundTimer = registers[regx];
}

void Chip8::Opcode_FX1E(uint16_t opcode)
{
    int regx = opcode & 0x0F00;
    regx >>= 8;
    indexRegister += registers[regx];
}

void Chip8::Opcode_FX29(uint16_t opcode)
{
    int regx = opcode & 0x0F00;
    regx >>= 8;
    int valueX = registers[regx];
    indexRegister = 0x50 + (valueX * 5);
}

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
    for (int i = 0; i <= regx; i++)
    {
        memory[indexRegister + i] = registers[i];
    }
}

void Chip8::Opcode_FX65(uint16_t opcode)
{
    // get final register
    int regx = opcode & 0x0F00;
    regx >>= 8;
    for (int i = 0; i <= regx; i++)
    {
        registers[i] = memory[indexRegister + i];
    }
}