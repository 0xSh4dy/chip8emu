Thanks to Austin for the awesome blog on developing Chip-8 emulators. This project is inspired from his blog https://austinmorlan.com/posts/chip8_emulator/

## Structure of CHIP-8
- Sixteen 8-bit registers, labelled from V0 to VF. Each register can hold a value from 0x0 to 0xff. VF is used as the flag register.
- 4096 bytes of memory (4KB), with the address space ranging from 0x000 to 0xfff.
- 0x50 - 0xa0 : Storage space for 16 built-in characters.
- 0x200 - 0xfff : Instructions from the ROM will be stored starting at 0x200.

- 16-bit index register used for storing memory locations.
- 16-bit program counter
- 16 levels of stack, storing entries of 16 bytes each
- 8-bit stack pointer

- A memory buffer used for storing the graphics to display which is 64 pixels wide and 32 pixels high. Each pixel is either on or off.

## Fonts
``` py
>>> x = [0xF0, 0x90, 0xF0, 0x90, 0x90] # A
>>> for i in x:
...     print(bin(i)[2:])

11110000
10010000
11110000
10010000
10010000

This looks like A, doesn't it?
```

- Chip8 has 34 instructions that need to be emulated
