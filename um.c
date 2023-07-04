/**************************************************************
 *
 *                     um.c
 *
 *     Assignment: UM
 *     Authors: Adam Weiss and Auriel Wish
 *     Date: 4/5/2023
 *
 *     Purpose: Driver file to emulate a universal machine.
 *
 **************************************************************/

#include <stdlib.h>
#include <sys/stat.h>
#include "memory.h"
#include "arithmetic.h"

/* Typdefs and Enums */
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;

/* Macro Definitions */
#define INSTR_WIDTH 4
#define INSTR_LSB 28
#define LV_REG_LSB 25
#define REG_WIDTH 3
#define A regsInCommand[0]
#define B regsInCommand[1]
#define C regsInCommand[2]

/* Function Declarations */
int getFileSize(char *filename);
char getOpcode(Um_instruction instruction);
void getThreeRegisters(uint32_t registers[], Um_instruction instruction);

int main(int argc, char *argv[])
{
        if (argc != 2) {
                fprintf(stderr, "Usage: ./um <um-file>");
                return EXIT_FAILURE;
        }
        char *filename = argv[1];
        FILE *commandFile = fopen(filename, "r");
        int numInstructions = getFileSize(filename);
        
        /* Build and initialize program memory */
        memoryInfo memory = makeMemoryInfo(numInstructions, commandFile);
        assert(fclose(commandFile) == 0);

        /* Set up command loop */
        char opcode = 0;
        uint32_t regsInCommand[3] = {0};
        uint32_t currInstruction;
        
        /* Command Loop */
        while(opcode != HALT) {
                /* Fetch and decode instruction */
                currInstruction = getCurrInstruction(memory);
                opcode = getOpcode(currInstruction);
                incrementProgramCounter(memory);

                if (opcode == LV) {
                        regsInCommand[0] = Bitpack_getu(currInstruction,
                                                        REG_WIDTH, LV_REG_LSB);
                }
                else {
                        getThreeRegisters(regsInCommand, currInstruction);
                }

                /* Execute instruction */
                if (opcode == CMOV) {
                        setRegisterValue(memory, A, conditionalMove(
                                getRegisterValue(memory, A),
                                getRegisterValue(memory, B),
                                getRegisterValue(memory, C)));
                } else if (opcode == SLOAD) {
                        segLoad(regsInCommand, memory);
                } else if (opcode == SSTORE) {
                        segStore(regsInCommand, memory);
                } else if (opcode == ADD) {
                        setRegisterValue(memory, A, add(
                                getRegisterValue(memory, B),
                                getRegisterValue(memory, C)));
                } else if (opcode == MUL) {
                        setRegisterValue(memory, A, multiply(
                                getRegisterValue(memory, B),
                                getRegisterValue(memory, C)));
                } else if (opcode == DIV) {
                        setRegisterValue(memory, A, divide(
                                getRegisterValue(memory, B),
                                getRegisterValue(memory, C)));
                } else if (opcode == NAND) {
                        setRegisterValue(memory, A, nand(
                                getRegisterValue(memory, B),
                                getRegisterValue(memory, C)));
                } else if (opcode == HALT) {
                        
                } else if (opcode == ACTIVATE) {
                        mapSeg(regsInCommand, memory);
                } else if (opcode == INACTIVATE) {
                        unmapSeg(regsInCommand, memory);
                } else if (opcode == OUT) {
                        output(getRegisterValue(memory, C));
                } else if (opcode == IN) {
                        setRegisterValue(memory, C, input());
                } else if (opcode == LOADP) {
                        loadProgram(regsInCommand, memory);
                } else if (opcode == LV) {
                        setRegisterValue(memory, A, loadValue(currInstruction));
                }
        }

        /* Free leftover memory */
        freeMemory(memory);

        return EXIT_SUCCESS;
}

/*
 * Name: getFileSize
 * Purpose: Get instruction file length (in bytes)
 * Parameters: Name of file
 * Returns: Size of file
 * Notes: None
 */
int getFileSize(char *filename)
{
        struct stat st;
        stat(filename, &st);
        return st.st_size / 4;
}

/*
 * Name: getOpcode
 * Purpose: Get the opcode from the instruction
 * Parameters: The instruction to pull the instruction code from.
 * Returns: The opcode
 * Notes: None
 */
char getOpcode(Um_instruction instruction)
{
        char opcode = Bitpack_getu(instruction, INSTR_WIDTH, INSTR_LSB);
        return opcode;
}

/*
 * Name: getThreeRegisters
 * Purpose: Get the 3 registers from the instruction
 * Parameters: An array to place the register numbers in
 * Returns: None
 * Notes: None
 */
void getThreeRegisters(uint32_t registers[], Um_instruction instruction)
{
        char reg = 0;
        for (int i = 2; i >= 0; i--) {
                reg = Bitpack_getu(instruction, REG_WIDTH, 6 - 3 * i);
                registers[i] = reg;
        }
}