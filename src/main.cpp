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
#include <raylib.h>
#include "chip8.hpp"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Error: No entry ROM path specified.\n";
        std::cout << "Usage: " << argv[0] << " <path_to_rom.ch8>\n";
        return -1;
    }

    std::string romPath = argv[1];

    const int videoScale = 20;
    const int windowWidth = 64 * videoScale;
    const int windowHigh = 32 * videoScale;

    Chip8 chip;

    if (!chip.LoadChip(romPath))
    {
        return -1;
    }

    InitWindow(windowWidth, windowHigh, "Chip8 - Engine Emulator");
    SetTargetFPS(60);
    InitAudioDevice();
    // 1. Audio constants setup
    const int sampleRate = 44100;
    const float frequency = 440.0f;          // Standard A4 tuning note pitch
    const int totalSamples = sampleRate * 1; // 1 second duration buffer loop

    // 2. Allocate and fill a raw memory float buffer with a square wave
    float *audioSamples = new float[totalSamples];
    for (int i = 0; i < totalSamples; ++i)
    {
        // Calculate the position inside the audio wave period
        float time = (float)i / sampleRate;

        // Square wave math: if the phase is positive, output max volume (0.5), else min (-0.5)
        float waveValue = (sinf(2.0f * PI * frequency * time) >= 0.0f) ? 0.5f : -0.5f;

        audioSamples[i] = waveValue;
    }

    // 3. Package the raw samples into Raylib's structural Wave layout
    Wave beepWave = {
        .frameCount = (unsigned int)totalSamples,
        .sampleRate = (unsigned int)sampleRate,
        .sampleSize = 32, // 32-bit floating point format samples
        .channels = 1,    // 1 = Mono sound channel output
        .data = audioSamples};

    // 4. Move the Wave directly into the sound card processor memory
    Sound beepSound = LoadSoundFromWave(beepWave);

    // 5. Clean up your raw allocated RAM copy safely
    delete[] audioSamples;
    SetSoundPitch(beepSound, 1.0f);

    while (!WindowShouldClose())
    {
        chip.keypad[0x1] = IsKeyDown(KEY_ONE);
        chip.keypad[0x2] = IsKeyDown(KEY_TWO);
        chip.keypad[0x3] = IsKeyDown(KEY_THREE);
        chip.keypad[0xC] = IsKeyDown(KEY_FOUR);

        chip.keypad[0x4] = IsKeyDown(KEY_Q);
        chip.keypad[0x5] = IsKeyDown(KEY_W);
        chip.keypad[0x6] = IsKeyDown(KEY_E);
        chip.keypad[0xD] = IsKeyDown(KEY_R);

        chip.keypad[0x7] = IsKeyDown(KEY_A);
        chip.keypad[0x8] = IsKeyDown(KEY_S);
        chip.keypad[0x9] = IsKeyDown(KEY_D);
        chip.keypad[0xE] = IsKeyDown(KEY_F);

        chip.keypad[0xA] = IsKeyDown(KEY_Z);
        chip.keypad[0x0] = IsKeyDown(KEY_X);
        chip.keypad[0xB] = IsKeyDown(KEY_C);
        chip.keypad[0xF] = IsKeyDown(KEY_V);

        for (int i = 0; i < 40; i++)
        {
            chip.Cycle();
        }
        chip.UpdateTimers();

        if (chip.delaySound > 0)
        {
            if (!IsSoundPlaying(beepSound))
            {
                PlaySound(beepSound);
            }
        }
        else
        {
            if (IsSoundPlaying(beepSound))
            {
                StopSound(beepSound);
            }
        }
        BeginDrawing();
        ClearBackground(BLACK);

        for (int y = 0; y < 32; y++)
        {
            for (int x = 0; x < 64; x++)
            {
                if (chip.video[y * 64 + x] != 0)
                {
                    DrawRectangle(x * videoScale, y * videoScale, videoScale, videoScale, LIME);
                }
            }
        }
        EndDrawing();
    }
    CloseAudioDevice();
    CloseWindow();
    return 0;
}