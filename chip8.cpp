#include "chip8.hpp"

uint8_t fontset[FONTSET_SIZE] =
    {
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

void Chip8::Cycle()
{
    // Fetch, decode and execute
    this->opcode = (memory[pc] << 8u) | memory[pc + 1];

    this->pc += 2;
    ((*this).*(funcTable[(opcode & 0xF000u) >> 12u]))();

    if (this->delay_timer > 0)
    {
        --this->delay_timer;
    }

    if (this->sound_timer > 0)
    {
        --this->sound_timer;
    }
}

void Chip8::FuncTable0()
{
    ((*this).*(funcTable0[this->opcode & 0xf]))();
}

void Chip8::FuncTable8()
{
    ((*this).*(funcTable8[this->opcode & 0xf]))();
}

void Chip8::FuncTableE()
{
    ((*this).*(funcTableE[this->opcode & 0xf]))();
}

void Chip8::FuncTableF()
{
    ((*this).*(funcTableF[this->opcode & 0xff]))();
}

Chip8::Chip8()
{
    srand(time(0));
    this->pc = ROM_INSTR_START;
    // Load the fonts into memory 0x50-0xa0
    for (int i = 0; i < FONTSET_SIZE; i++)
    {
        this->memory[FONTSET_START + i] = fontset[i];
    }

    funcTable[0x0] = &Chip8::FuncTable0;
    funcTable[0x1] = &Chip8::Opcode_1nnn;
    funcTable[0x2] = &Chip8::Opcode_2nnn;
    funcTable[0x3] = &Chip8::Opcode_3xkk;
    funcTable[0x4] = &Chip8::Opcode_4xkk;
    funcTable[0x5] = &Chip8::Opcode_5xy0;
    funcTable[0x6] = &Chip8::Opcode_6xkk;
    funcTable[0x7] = &Chip8::Opcode_7xkk;
    funcTable[0x8] = &Chip8::FuncTable8;
    funcTable[0x9] = &Chip8::Opcode_9xy0;
    funcTable[0xA] = &Chip8::Opcode_Annn;
    funcTable[0xB] = &Chip8::Opcode_Bnnn;
    funcTable[0xC] = &Chip8::Opcode_Cxkk;
    funcTable[0xD] = &Chip8::Opcode_Dxyn;
    funcTable[0xE] = &Chip8::FuncTableE;
    funcTable[0xF] = &Chip8::FuncTableF;

    for (int i = 0; i <= 0xe; i++)
    {
        this->funcTable0[i] = &Chip8::Opcode_NULL;
        this->funcTable8[i] = &Chip8::Opcode_NULL;
        this->funcTableE[i] = &Chip8::Opcode_NULL;
    }
    this->funcTable0[0x0] = &Chip8::Opcode_00E0;
    this->funcTable0[0xe] = &Chip8::Opcode_00EE;

    this->funcTable8[0x0] = &Chip8::Opcode_8xy0;
    this->funcTable8[0x1] = &Chip8::Opcode_8xy1;
    this->funcTable8[0x2] = &Chip8::Opcode_8xy2;
    this->funcTable8[0x3] = &Chip8::Opcode_8xy3;
    this->funcTable8[0x4] = &Chip8::Opcode_8xy4;
    this->funcTable8[0x5] = &Chip8::Opcode_8xy5;
    this->funcTable8[0x6] = &Chip8::Opcode_8xy6;
    this->funcTable8[0x7] = &Chip8::Opcode_8xy7;
    this->funcTable8[0xe] = &Chip8::Opcode_8xyE;

    for (int i = 0; i <= 0x65; i++)
    {
        this->funcTableF[i] = &Chip8::Opcode_NULL;
    }
    this->funcTableF[0x7] = &Chip8::Opcode_Fx07;
    this->funcTableF[0xa] = &Chip8::Opcode_Fx0A;
    this->funcTableF[0x15] = &Chip8::Opcode_Fx15;
    this->funcTableF[0x18] = &Chip8::Opcode_Fx18;
    this->funcTableF[0x1e] = &Chip8::Opcode_Fx1E;
    this->funcTableF[0x29] = &Chip8::Opcode_Fx29;
    this->funcTableF[0x33] = &Chip8::Opcode_Fx33;
    this->funcTableF[0x55] = &Chip8::Opcode_Fx55;
    this->funcTableF[0x65] = &Chip8::Opcode_Fx65;
}

void Chip8::LoadROM(const char *filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (file.is_open())
    {
        std::streampos size = file.tellg();
        char *buffer = new char[size];
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();
        for (int i = 0; i < size; i++)
        {
            this->memory[ROM_INSTR_START + i] = buffer[i];
        }
        delete[] buffer;
    }
}

void Chip8::Opcode_00E0()
{
    // CLS
    memset(this->video, 0, sizeof(this->video));
}

void Chip8::Opcode_00EE()
{
    // RET
    this->pc = this->stack[--this->sp];
}

void Chip8::Opcode_1nnn()
{
    // Jump to location nnn.
    uint16_t target_address = this->opcode & 0xfff;
    this->pc = target_address;
}

void Chip8::Opcode_2nnn()
{
    // Call subroutine at nnn.
    uint16_t target_address = this->opcode & 0xfff;
    this->stack[this->sp++] = this->pc;
    this->pc = target_address;
}

void Chip8::Opcode_3xkk()
{
    // Skip next instruction if Vx = kk.
    uint8_t kk = this->opcode & 0xff;
    uint8_t vx = (this->opcode & 0x0f00) >> 8;
    if (this->registers[vx] == kk)
    {
        this->pc += 2;
    }
}

void Chip8::Opcode_4xkk()
{
    // Skip next instruction if Vx!=kk
    uint8_t kk = this->opcode & 0xff;
    uint8_t vx = (this->opcode & 0x0f00) >> 8;
    if (this->registers[vx] != kk)
    {
        this->pc += 2;
    }
}

void Chip8::Opcode_5xy0()
{
    // Skip next instruction if Vx = Vy
    uint8_t vx = (this->opcode & 0x0f00) >> 8;
    uint8_t vy = (this->opcode & 0x00f0) >> 4;
    if (this->registers[vx] == this->registers[vy])
    {
        this->pc += 2;
    }
}

void Chip8::Opcode_6xkk()
{
    // 6xkk - LD Vx, byte
    uint8_t kk = this->opcode & 0xff;
    uint8_t vx = (this->opcode & 0x0f00) >> 8;
    this->registers[vx] = kk;
}

void Chip8::Opcode_7xkk()
{
    // Set Vx = Vx + kk
    uint8_t kk = (this->opcode & 0xff);
    uint8_t vx = (this->opcode & 0x0f00) >> 8;
    this->registers[vx] += kk;
}

void Chip8::Opcode_8xy0()
{
    // Set Vx = Vy
    uint8_t vy = (this->opcode & 0x00f0) >> 4;
    uint8_t vx = (this->opcode & 0x0f00) >> 8;
    this->registers[vx] = this->registers[vy];
}

void Chip8::Opcode_8xy1()
{
    // Set Vx = Vx OR Vy
    uint8_t vy = (this->opcode & 0x00f0) >> 4;
    uint8_t vx = (this->opcode & 0x0f00) >> 8;
    this->registers[vx] |= this->registers[vy];
}

void Chip8::Opcode_8xy2()
{
    // Set Vx = Vx AND Vy
    uint8_t vy = (this->opcode & 0x00f0) >> 4;
    uint8_t vx = (this->opcode & 0x0f00) >> 8;
    this->registers[vx] &= this->registers[vy];
}

void Chip8::Opcode_8xy3()
{
    // Set Vx = Vx XOR Vy
    uint8_t vy = (this->opcode & 0x00f0) >> 4;
    uint8_t vx = (this->opcode & 0x0f00) >> 8;
    this->registers[vx] ^= this->registers[vy];
}

void Chip8::Opcode_8xy4()
{
    // Set Vx = Vx + Vy, set VF = carry
    uint8_t vy = (this->opcode & 0x00f0) >> 4;
    uint8_t vx = (this->opcode & 0x0f00) >> 8;
    uint16_t sum = this->registers[vy] + this->registers[vx];

    if (sum > 255)
    {
        this->registers[0xf] = 1;
    }
    else
    {
        this->registers[0xf] = 0;
    }
    this->registers[vx] = sum & 0xff;
}

void Chip8::Opcode_8xy5()
{
    // Set Vx = Vx - Vy, set VF = NOT borrow.
    uint8_t vy = (this->opcode & 0x00f0) >> 4;
    uint8_t vx = (this->opcode & 0x0f00) >> 8;

    if (this->registers[vx] > this->registers[vy])
    {
        this->registers[0xf] = 1;
    }
    else
    {
        this->registers[0xf] = 0;
    }
    this->registers[vx] -= this->registers[vy];
}

void Chip8::Opcode_8xy6()
{
    // Set Vx = Vx SHR 1 and LS-Bit is saved in VF
    uint8_t vx = (this->opcode & 0x0f00) >> 8;
    this->registers[0xf] = this->registers[vx] & 0x1;
    this->registers[vx] >>= 1;
}

void Chip8::Opcode_8xy7()
{
    // Set Vx = Vy - Vx, set VF = NOT borrow.
    uint8_t vy = (this->opcode & 0x00f0) >> 4;
    uint8_t vx = (this->opcode & 0x0f00) >> 8;

    if (this->registers[vy] > this->registers[vx])
    {
        this->registers[0xf] = 1;
    }
    else
    {
        this->registers[0xf] = 0;
    }
    this->registers[vx] = this->registers[vy] - this->registers[vx];
}

void Chip8::Opcode_8xyE()
{
    // If the MS bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
    uint8_t vx = (this->opcode & 0x0f00) >> 8;
    this->registers[0xf] = (this->registers[vx] & 0x80) >> 7;
    this->registers[vx] <<= 1;
}

void Chip8::Opcode_9xy0()
{
    // Skip next instruction if Vx != Vy.
    uint8_t vy = (this->opcode & 0x00f0) >> 4;
    uint8_t vx = (this->opcode & 0x0f00) >> 8;
    if (this->registers[vx] != this->registers[vy])
    {
        this->pc += 2;
    }
}

void Chip8::Opcode_Annn()
{
    // Set I = nnn.
    uint16_t val = this->opcode & 0x0fff;
    this->index = val;
}

void Chip8::Opcode_Bnnn()
{
    // Jump to location nnn + V0.
    uint16_t val = this->opcode & 0x0fff;
    this->pc = this->registers[0] + val;
}

void Chip8::Opcode_Cxkk()
{
    // Set Vx = random byte AND kk.
    uint8_t kk = this->opcode & 0x00ff;
    uint8_t vx = (this->opcode & 0x0f00) >> 8;
    this->registers[vx] = kk & (rand() % 0xf);
}

void Chip8::Opcode_Dxyn()
{
    uint8_t Vx = (this->opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t height = opcode & 0x000Fu;

    uint8_t xPos = registers[Vx] % S_WIDTH;
    uint8_t yPos = registers[Vy] % S_HEIGHT;

    registers[0xF] = 0;

    for (unsigned int row = 0; row < height; ++row)
    {
        uint8_t spriteByte = memory[index + row];

        for (unsigned int col = 0; col < 8; ++col)
        {
            uint8_t spritePixel = spriteByte & (0x80u >> col);
            uint32_t *screenPixel = &video[(yPos + row) * S_WIDTH + (xPos + col)];

            if (spritePixel)
            {
                if (*screenPixel == 0xFFFFFFFF)
                {
                    registers[0xF] = 1;
                }
                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }
}

void Chip8::Opcode_Ex9E()
{
    // Skip next instruction if key with the value of Vx is pressed
    uint8_t vx = (this->opcode & 0x0f00) >> 8;
    if (this->keypad[this->registers[vx]])
    {
        this->pc += 2;
    }
}

void Chip8::Opcode_ExA1()
{
    // Skip next instruction if key with the value of Vx is not pressed.
    uint8_t vx = (this->opcode & 0x0f00) >> 8;
    if (!this->keypad[this->registers[vx]])
    {
        this->pc += 2;
    }
}

void Chip8::Opcode_Fx07()
{
    // Set Vx = delay timer value.
    uint8_t vx = (this->opcode & 0x0f00) >> 8;
    this->registers[vx] = this->delay_timer;
}

void Chip8::Opcode_Fx0A()
{
    // Wait for a key press, store the value of the key in Vx.
    uint8_t vx = (this->opcode & 0x0f00) >> 8;
    for (int i = 0; i < 16; i++)
    {
        if (this->keypad[i])
        {
            this->registers[vx] = static_cast<uint8_t>(i);
            return;
        }
    }
    this->pc -= 2;
}

void Chip8::Opcode_Fx15()
{
    // Set delay timer = Vx.
    uint8_t vx = (this->opcode & 0x0f00) >> 8;
    this->delay_timer = this->registers[vx];
}

void Chip8::Opcode_Fx18()
{
    // Set sound timer = Vx.
    uint8_t vx = (this->opcode & 0x0f00) >> 8;
    this->sound_timer = this->registers[vx];
}

void Chip8::Opcode_Fx1E()
{
    // Set I = I + Vx.
    uint8_t vx = (this->opcode & 0x0f00) >> 8;
    this->index = this->index + this->registers[vx];
}

void Chip8::Opcode_Fx29()
{
    // Set I = location of sprite for digit Vx.
    uint8_t vx = (this->opcode & 0x0f00) >> 8;
    this->index = FONTSET_START + 5 * this->registers[vx];
}

void Chip8::Opcode_Fx33()
{
    uint8_t Vx = (this->opcode & 0x0F00u) >> 8u;
    uint8_t value = this->registers[Vx];

    memory[index + 2] = value % 10;
    value /= 10;
    this->memory[index + 1] = value % 10;
    value /= 10;
    this->memory[index] = value % 10;
}

void Chip8::Opcode_Fx55()
{
    // Store registers V0 through Vx in memory starting at location I.
    uint8_t vx = (this->opcode & 0x0f00) >> 8;
    for (int i = 0; i < vx; i++)
    {
        this->memory[this->index + i] = this->registers[i];
    }
}

void Chip8::Opcode_Fx65()
{
    // Read registers V0 through Vx from memory starting at location I.
    uint8_t vx = (this->opcode & 0xf00) >> 8;
    for (int i = 0; i < vx; i++)
    {
        this->registers[i] = this->memory[this->index + i];
    }
}