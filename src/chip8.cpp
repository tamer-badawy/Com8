/*
 * Copyright [2026] Tamer Badawy
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <raylib.h>

#include "chip8.hpp"

Chip8::Chip8()
    : gen(std::random_device{}()),
      rand(0, 255)
{
    Reset();
}

void Chip8::Reset()
{
    pc = 0x200;
    sp = 0;
    index_reg = 0;

    delaySound = 0;
    delayTimer = 0;

    std::fill(std::begin(registers), std::end(registers), 0);
    std::fill(std::begin(memory), std::end(memory), 0);
    std::fill(std::begin(stack), std::end(stack), 0);
    std::fill(std::begin(video), std::end(video), 0);
    std::fill(std::begin(keypad), std::end(keypad), 0);

    for (int i = 0; i < FONTSET_SIZE; i++)
    {
        memory[i] = FONTSET[i];
    }
}

void Chip8::Cycle()
{
    uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
    pc += 2;

    uint16_t NNN = opcode & 0x0FFF;
    uint8_t NN = opcode & 0x00FF;
    uint8_t N = opcode & 0x000F;
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t Y = (opcode & 0x00F0) >> 4;

    switch (opcode & 0xf000)
    {
    case 0x0000:
        if (opcode == 0x00E0)
        { // Clear screen
            std::fill(std::begin(video), std::end(video), 0);
        }
        else if (opcode == 0x00EE)
        { // Return from subroutin
            assert(sp != 0 && "stack underflow");
            --sp;
            pc = stack[sp];
        }
        break;
    case 0x1000:
        // jump
        pc = NNN;
        break;
    case 0x2000:
        assert(sp < 16 && "stack overflow");
        // call subroutine
        stack[sp] = pc;
        ++sp;
        pc = NNN;
        break;
    case 0x3000:
        if (registers[X] == NN)
        {
            pc += 2;
        }
        break;
    case 0x4000:
        if (registers[X] != NN)
        {
            pc += 2;
        }
        break;
    case 0x5000:
        if ((opcode & 0x000F) == 0x0)
        {
            if (registers[X] == registers[Y])
            {
                pc += 2;
            }
        }
        break;
    case 0x6000:
        registers[X] = NN;
        break;
    case 0x7000:
        registers[X] += NN;
        break;
    case 0x8000:
        switch (opcode & 0x000F)
        {
        case 0x0:
            registers[X] = registers[Y];
            break;
        case 0x1:
        {
            uint8_t value = registers[Y];
            registers[X] |= value;
            registers[0xF] = 0;
        }
        break;
        case 0x2:
        {
            uint8_t value = registers[Y];
            registers[X] &= value;
            registers[0xF] = 0;
        }
        break;
        case 0x3:
        {
            uint8_t value = registers[Y];
            registers[X] ^= value;
            registers[0xF] = 0;
        }
        break;
        case 0x4: // Sum
        {
            int sum = registers[X] + registers[Y];
            registers[X] = sum & 0xFF;
            registers[0xF] = sum > 255;
        }
        break;
        case 0x5: // Sub
        {
            int sub = registers[X] - registers[Y];
            registers[0xF] = registers[X] > registers[Y];
            registers[X] = sub & 0xFF;
        }
        break;
        case 0x6: // SHR
        {
            uint8_t yValue = registers[Y];
            uint8_t dropped_bit = yValue & 0x1;
            registers[X] = yValue >> 1;
            registers[0xF] = dropped_bit;
        }
        break;
        case 0x7:
        {
            uint8_t vx = registers[X];
            uint8_t vy = registers[Y];
            registers[X] = vy - vx;
            registers[0xF] = vy > vx;
        }
        break;
        case 0xE: // SHL
        {
            uint8_t yValue = registers[Y];
            uint8_t dropped_bit = yValue & 0x80;
            registers[X] = yValue << 1;
            registers[0xF] = dropped_bit;
        }
        break;
        }
        break;
    case 0x9000:
        if (registers[X] != registers[Y])
        {
            pc += 2;
        }
        break;
    case 0xA000:
        index_reg = NNN;
        break;
    case 0xB000:
        pc = NNN + registers[0];
        break;
    case 0xC000: // Random number

        registers[X] = rand(gen) & NN;
        break;
    case 0xD000: // Draw vx, vy, nibble
    {
        uint8_t xCoord = registers[X] % 64;
        uint8_t yCoord = registers[Y] % 32;

        registers[0xF] = 0;
        for (unsigned int row = 0; row < N; row++)
        {
            uint8_t spriteByte = memory[index_reg + row];
            for (unsigned int col = 0; col < 8; col++)
            {
                uint8_t spiritePixel = spriteByte & (0x80 >> col);
                if ((xCoord + col) < 64 && (yCoord + row) < 32)
                {
                    uint32_t *screenPixel = &video[(yCoord + row) * 64 + (xCoord + col)];
                    if (spiritePixel)
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
    }
    break;
    case 0xE000:
        if ((opcode & 0x00FF) == 0x9E && keypad[registers[X]])
        {
            pc += 2;
        }
        else if ((opcode & 0x00FF) == 0xA1 && !keypad[registers[X]])
        {
            pc += 2;
        }
        break;
    case 0xF000:
        switch (opcode & 0x00FF)
        {
        case 0x07:
            registers[X] = delayTimer;
            break;
        case 0x0A:
        {
            bool keyreleased = false;

            if (IsKeyReleased(KEY_ONE))
            {
                registers[X] = 0x1;
                keyreleased = true;
            }
            if (IsKeyReleased(KEY_TWO))
            {
                registers[X] = 0x2;
                keyreleased = true;
            }
            if (IsKeyReleased(KEY_THREE))
            {
                registers[X] = 0x3;
                keyreleased = true;
            }
            if (IsKeyReleased(KEY_FOUR))
            {
                registers[X] = 0xC;
                keyreleased = true;
            }
            if (IsKeyReleased(KEY_Q))
            {
                registers[X] = 0x4;
                keyreleased = true;
            }
            if (IsKeyReleased(KEY_W))
            {
                registers[X] = 0x5;
                keyreleased = true;
            }
            if (IsKeyReleased(KEY_E))
            {
                registers[X] = 0x6;
                keyreleased = true;
            }
            if (IsKeyReleased(KEY_R))
            {
                registers[X] = 0xD;
                keyreleased = true;
            }
            if (IsKeyReleased(KEY_A))
            {
                registers[X] = 0x7;
                keyreleased = true;
            }
            if (IsKeyReleased(KEY_S))
            {
                registers[X] = 0x8;
                keyreleased = true;
            }

            if (IsKeyReleased(KEY_D))
            {
                registers[X] = 0x9;
                keyreleased = true;
            }
            if (IsKeyReleased(KEY_F))
            {
                registers[X] = 0xE;
                keyreleased = true;
            }
            if (IsKeyReleased(KEY_Z))
            {
                registers[X] = 0xA;
                keyreleased = true;
            }
            if (IsKeyReleased(KEY_X))
            {
                registers[X] = 0x0;
                keyreleased = true;
            }
            if (IsKeyReleased(KEY_C))
            {
                registers[X] = 0xB;
                keyreleased = true;
            }
            if (IsKeyReleased(KEY_V))
            {
                registers[X] = 0xF;
                keyreleased = true;
            }

            if (!keyreleased)
            {
                pc -= 2;
            }
        }
        break;
        case 0x15:
            delayTimer = registers[X];
            break;
        case 0x18:
            delaySound = registers[X];
            break;
        case 0x1E:
            index_reg += registers[X];
            break;
        case 0x29:
        {
            uint8_t character = registers[X] & 0x0F;
            index_reg = character * 5;
        }
        break;
        case 0x33:
        {
            uint8_t value = registers[X];
            memory[index_reg] = value / 100;
            memory[index_reg + 1] = (value / 10) % 10;
            memory[index_reg + 2] = value % 10;
        }
        break;
        case 0x55:

            for (int i = 0; i <= X; i++)
            {

                memory[index_reg] = registers[i];
                ++index_reg;
            }
            break;
        case 0x65:
            for (int i = 0; i <= X; i++)
            {

                registers[i] = memory[index_reg];
                ++index_reg;
            }
            break;
        }
        break;
    }
}

bool Chip8::LoadChip(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "failed to open ROM: " << filename << std::endl;
        return false;
    }
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();

    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);

    file.read(buffer.data(), size);
    file.close();

    for (long i = 0; i < size; i++)
    {
        memory[0x200 + i] = static_cast<uint8_t>(buffer[i]);
    }
    return true;
}

void Chip8::UpdateTimers()
{
    if (delayTimer > 0)
        --delayTimer;
    if (delaySound > 0)
        --delaySound;
}