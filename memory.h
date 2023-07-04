/**************************************************************
 *
 *                     memory.h
 *
 *     Assignment: UM
 *     Authors: Adam Weiss and Auriel Wish
 *     Date: 4/5/2023
 *
 *     Purpose: Interface for the UM memory
 *
 **************************************************************/

#ifndef MEMORY_INCLUDED
#define MEMORY_INCLUDED

#include <stdio.h>
#include "seq.h"
#include "mem.h"
#include "bitpack.h"
#include <assert.h>

typedef uint32_t Um_instruction;
typedef struct memoryInfo *memoryInfo;

memoryInfo makeMemoryInfo(uint32_t numInstructions, FILE *commandFile);
Um_instruction getCurrInstruction(memoryInfo memory);
uint32_t getRegisterValue(memoryInfo memory, uint32_t regNum);
void setRegisterValue(memoryInfo memory, uint32_t regNum, uint32_t value);
void loadInitialProgram(memoryInfo memory, int numInstructions,
                                                        FILE *commandFile);
void segLoad(uint32_t commandRegs[], memoryInfo memory);
void segStore(uint32_t commandRegs[], memoryInfo memory);
void mapSeg(uint32_t commandRegs[], memoryInfo memory);
void unmapSeg(uint32_t commandRegs[], memoryInfo memory);
void loadProgram(uint32_t commandRegs[],  memoryInfo memory);
void incrementProgramCounter(memoryInfo memory);
void freeMemory(memoryInfo memory);
void freeSeqMemory(Seq_T seq);

#endif