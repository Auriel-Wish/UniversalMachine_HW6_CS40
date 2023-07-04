/**************************************************************
 *
 *                     memory.c
 *
 *     Assignment: UM
 *     Authors: Adam Weiss and Auriel Wish
 *     Date: 4/5/2023
 *
 *     Purpose: Implementation for manipulation of
 *              the UM memory.
 *
 **************************************************************/

#include "memory.h"

#define A regsInCommand[0]
#define B regsInCommand[1]
#define C regsInCommand[2]
#define NUM_REGS 8
#define MEMORY_INFO_SIZE 64
#define INIT_SEQ_SIZE 100

/*
 * Name: segmentInfo
 * Purpose: struct for memory segment
 * Members: length - The number of words in the segment
 *          segData - An array of the words in the segment
 */
typedef struct segmentInfo {
        uint32_t length;
        Um_instruction segData[];
} *segmentInfo;

/*
 * Name: memoryInfo
 * Purpose: Contain all information having to do with UM memory
 * Members: mainMemory - sequence containing all segments (besides segment 0)
 *          recentlyUnmapped - sequence containing all unmapped IDs
 *          program - struct representing segment 0. This is separate from
 *                    mainMemory to make it more likely that it will be placed
 *                    in a register
 *          programCounter - keeps track of which instruction program is on
 *          maxSegementID - the number of the highest ID ever used
 *          allRegs - the emulated registers
 */
struct memoryInfo {
        Seq_T mainMemory;
        Seq_T recentlyUnmapped;
        segmentInfo program;
        uint32_t programCounter;
        uint32_t maxSegmentID;
        uint32_t allRegs[NUM_REGS];
};

/*
 * Name: makeMemoryInfo
 * Purpose: Initialize the memory structures and variables for the UM
 * Parameters: The number of instructions for segment 0, the input command file
 * Returns: A struct containing the memory structures and variables
 * Notes: memory is freed after halt by freeMemory
 */
memoryInfo makeMemoryInfo(uint32_t numInstructions, FILE *commandFile)
{
        /* Allocate space for memoryInfo. All bits in memory are set to 0 */
        memoryInfo memory = CALLOC(MEMORY_INFO_SIZE, 1);
        memory->recentlyUnmapped = Seq_new(INIT_SEQ_SIZE);
        memory->mainMemory = Seq_new(INIT_SEQ_SIZE);
        memory->maxSegmentID = 1;

        /* Create and fill segment 0 with instructions */
        loadInitialProgram(memory, numInstructions, commandFile);
        
        return memory;
}

/*
 * Name: loadInitialProgram
 * Purpose: Read in instructions from command file
 * Parameters: The struct containing the memory structures and variables, the
 *             number of instructions in command file, a pointer to command file
 * Returns: None
 * Notes: Segment 0 (program) will be filled with instructions. The memory
 *        allocated for program is freed either when loadProgam is called or
 *        after halt
 */
void loadInitialProgram(memoryInfo memory, int numInstructions,
                                                        FILE *commandFile)
{
        /*
         * Allocate memory for number of instructions plus the int to hold the
         * number of instructions
         */
        segmentInfo program = CALLOC(numInstructions + 1, sizeof(uint32_t));
        
        /*
         * Keep track of the current byte and the current instruction being
         * built 
         */
        Um_instruction currInstruction;
        uint32_t currByte;
        
        /*
         * Build each instruction by reading a byte at a time from the command
         * file and adding it to a 32 bit instruction. Then add that instruction
         * to the segment 0's array
         */
        for (int i = 0; i < numInstructions; i++) {
                currInstruction = 0;
                for (int j = 3; j >= 0; j--) {
                        currByte = fgetc(commandFile);
                        currByte = currByte << 8 * j;
                        currInstruction |= currByte;
                }
                (program->segData)[i] = currInstruction;
        }

        memory->program = program;
}

/*
 * Name: getCurrInstruction
 * Purpose: Get the instruction at the index specified by the program counter
 * Parameters: The struct containing the memory structures and variables
 * Returns: The instruction
 * Notes: None
 */
Um_instruction getCurrInstruction(memoryInfo memory)
{
        return (memory->program->segData)[memory->programCounter];
}

/*
 * Name: getRegisterValue
 * Purpose: Get the value in a register
 * Parameters: The struct containing the memory structures and variables,
 *             the desired register
 * Returns: The value in the requested register
 * Notes: None
 */
uint32_t getRegisterValue(memoryInfo memory, uint32_t regNum)
{
        return (memory->allRegs)[regNum];
}

/*
 * Name: setRegisterValue
 * Purpose: Set the value in a register
 * Parameters: The struct containing the memory structures and variables, the
 *             register number to change, the value to put in the register
 * Returns: None
 * Notes: None
 */
void setRegisterValue(memoryInfo memory, uint32_t regNum, uint32_t value)
{
        (memory->allRegs)[regNum] = value;
}

/*
 * Name: segLoad
 * Purpose: Store an instruction (in one register) from a place in memory
 *          specified by two other registers
 * Parameters: The registers in the instruction, the struct containing the
 *             memory structures and variables
 * Returns: None
 * Notes: Segment 0 is not in mainMemory so it has a special case
 */
void segLoad(uint32_t regsInCommand[], memoryInfo memory)
{
        if ((memory->allRegs)[B] == 0) {
                (memory->allRegs)[A] =
                        (memory->program->segData)[(memory->allRegs)[C]];
        }
        else {
                segmentInfo segment = Seq_get(memory->mainMemory,
                                                (memory->allRegs)[B] - 1);
                (memory->allRegs)[A] = (segment->segData)[(memory->allRegs)[C]];
        }
}

/*
 * Name: segStore
 * Purpose: Load a single instruction provided by one register and place it into
 *          a different segment location specific by two other registers
 * Parameters: The registers in the instruction, the struct containing the
 *             memory structures and variables
 * Returns: None
 * Notes: Segment 0 is not in mainMemory so it has a special case
 */
void segStore(uint32_t regsInCommand[], memoryInfo memory)
{
        if ((memory->allRegs)[A] == 0){
                (memory->program->segData)[(memory->allRegs)[B]] =
                                                        (memory->allRegs)[C];
        }
        else {
                segmentInfo segment = Seq_get(memory->mainMemory,
                                                (memory->allRegs)[A] - 1);
                (segment->segData)[(memory->allRegs)[B]] = (memory->allRegs)[C];
        }
}

/*
 * Name: mapSeg
 * Purpose: Create a new segment in memory
 * Parameters: The registers in the instruction, the struct containing the
 *             memory structures and variables
 * Returns: None
 * Notes: None
 */
void mapSeg(uint32_t regsInCommand[], memoryInfo memory)
{
        /*
         * Create new segment - each element is a uint32_t and the number of
         * these is allRegs[C] + 1 to account for the "length" variable in the
         * struct
         */
        segmentInfo newSeg = CALLOC((memory->allRegs)[C] + 1, sizeof(uint32_t));
        newSeg->length = (memory->allRegs)[C];

        /*
         * Determine the segment ID. If there are any IDs that can be reused,
         * use one of them. Otherwise use the ID that is 1 higher than the
         * highest one in use
         */
        uint32_t newID;
        if (Seq_length(memory->recentlyUnmapped) > 0) {
                uint32_t *newIDp = Seq_remhi(memory->recentlyUnmapped);
                newID = *newIDp;
                FREE(newIDp);
                Seq_put(memory->mainMemory, newID - 1, (void *) newSeg);
        }
        else {
                newID = memory->maxSegmentID;
                Seq_addhi(memory->mainMemory, (void *) newSeg);
                (memory->maxSegmentID)++;
        }
        assert(newID != 0);

        /* Save the new ID in a register */
        (memory->allRegs)[B] = newID;
}

/*
 * Name: unmapSeg
 * Purpose: Delete all data associated with the given segment
 * Parameters: The registers in the instruction, the struct containing the
 *             memory structures and variables
 * Returns: None
 * Notes: Adds the removed segment's ID to a sequence so it can be reused.
 *        Memory must be allocated for this number and it will be free either
 *        when a segment uses it as an ID or after halt
 */
void unmapSeg(uint32_t regsInCommand[],  memoryInfo memory)
{
        /* Free the segment memory */
        segmentInfo unmap = Seq_get(memory->mainMemory,
                                                (memory->allRegs)[C] - 1);
        FREE(unmap);
        Seq_put(memory->mainMemory, (memory->allRegs)[C] - 1, NULL);

        /* Add the ID to a sequence so it can be reused */
        uint32_t *unmappedID = ALLOC(sizeof(uint32_t));
        *unmappedID = (memory->allRegs)[C];
        Seq_addhi(memory->recentlyUnmapped, (void *) unmappedID);
}

/*
 * Name: loadProgram
 * Purpose: Replace the running program in segment 0 with a program from a
 *          different specified segment
 * Parameters: The registers in the instruction, the struct containing the
 *             memory structures and variables
 * Returns: None
 * Notes: None
 */
void loadProgram(uint32_t regsInCommand[],  memoryInfo memory)
{
        /*
         * If the new program is being loaded from segment 0, then only change
         * the program counter
         */
        uint32_t newCounter = (memory->allRegs)[C];
        if ((memory->allRegs)[B] == 0) {
                memory->programCounter = newCounter;
                return;
        }

        FREE(memory->program);

        /*
         * Allocate memory for the new program and perform a deep copy from the
         * desired segment into segment 0
         */
        segmentInfo incomingProgram = Seq_get(memory->mainMemory,
                                                (memory->allRegs)[B] - 1);
        int newProgramLength = incomingProgram->length;
        segmentInfo newProgram = CALLOC(newProgramLength + 1, 4);
        newProgram->length = newProgramLength;
        for (int i = 0; i < newProgramLength; i++) {
                (newProgram->segData)[i] = (incomingProgram->segData)[i];
        }
        memory->program = newProgram;

        /* Set the program counter */
        memory->programCounter = newCounter;
}

/*
 * Name: incrementProgramCounter
 * Purpose: Increments the program counter
 * Parameters: The struct containing the memory structures and variables
 * Returns: None
 * Notes: None
 */
void incrementProgramCounter(memoryInfo memory)
{
        (memory->programCounter)++;
}

/*
 * Name: freeMemory
 * Purpose: Free the memory of the UM
 * Parameters: The struct containing the memory structures and variables
 * Returns: None
 * Notes: None
 */
void freeMemory(memoryInfo memory)
{
        freeSeqMemory(memory->recentlyUnmapped);
        freeSeqMemory(memory->mainMemory);
        FREE(memory->program);
        FREE(memory);
}

/*
 * Name: freeSeqMemory
 * Purpose: Free memory of elements in the sequence and the sequence itself
 * Parameters: The sequence to free
 * Returns: None
 * Notes: None
 */
void freeSeqMemory(Seq_T seq)
{
        /* If the entry in the sequence is not null, free its memory */
        void *currEntry;
        for(int i = 0; i < Seq_length(seq); i++) {
                currEntry = Seq_get(seq, i);
                if (currEntry != NULL) {
                        FREE(currEntry);
                }
        }

        Seq_free(&seq);
}