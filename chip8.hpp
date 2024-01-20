#ifndef CHIP8_H
#define CHIP8_H
#include <cstdint>
#include <fstream>
#include <cstring>
#include <ctime>
#define FONTSET_SIZE 80
#define S_WIDTH 64
#define S_HEIGHT 32

#define ROM_INSTR_START 0x200
#define FONTSET_START 0x50

class Chip8
{
public:
    uint8_t sp{};
    uint16_t pc{};
    uint16_t stack[16]{};
    uint8_t memory[4096]{};
    uint8_t keypad[16]{};
    uint8_t delay_timer{};
    uint8_t sound_timer{};
    uint32_t video[64 * 32]{};
    uint16_t index{};
    uint16_t opcode;
    uint16_t registers[16]{};

    Chip8();
    void LoadROM(const char *filename);

    // Opcodes
    void Opcode_00E0();
    void Opcode_00EE();
    void Opcode_1nnn();
    void Opcode_2nnn();
    void Opcode_3xkk();
    void Opcode_4xkk();
    void Opcode_5xy0();
    void Opcode_6xkk();
    void Opcode_7xkk();
    void Opcode_8xy0();
    void Opcode_8xy1();
    void Opcode_8xy2();
    void Opcode_8xy3();
    void Opcode_8xy4();
    void Opcode_8xy5();
    void Opcode_8xy6();
    void Opcode_8xy7();
    void Opcode_8xyE();
    void Opcode_9xy0();
    void Opcode_Annn();
    void Opcode_Bnnn();
    void Opcode_Cxkk();
    void Opcode_Dxyn();
    void Opcode_Ex9E();
    void Opcode_ExA1();
    void Opcode_Fx07();
    void Opcode_Fx0A();
    void Opcode_Fx15();
    void Opcode_Fx18();
    void Opcode_Fx1E();
    void Opcode_Fx29();
    void Opcode_Fx33();
    void Opcode_Fx55();
    void Opcode_Fx65();
    void Opcode_NULL(){}

    void FuncTable0();
    void FuncTable8();
    void FuncTableE();
    void FuncTableF();

    void Cycle();
    
    Chip8Func funcTable[0xf + 1];
    Chip8Func funcTable0[0xe + 1];
    Chip8Func funcTable8[0xe + 1];
    Chip8Func funcTableE[0xe + 1];
    Chip8Func funcTableF[0x65 + 1];
};

typedef void (Chip8::*Chip8Func)();
#endif