/**************************************************************
 *
 *                     arithmetic.c
 *
 *     Assignment: UM
 *     Authors: Adam Weiss and Auriel Wish
 *     Date: 4/5/2023
 *
 *     Purpose: Implementation for UM functions that DO NOT
 *              directly manipulate registers and memory segments.
 *              These are mostly arithmetic operations but also
 *              include other operations like I/O.
 *
 **************************************************************/

#include "arithmetic.h"

#define LV_REG_LSB 25

/*
 * Name: conditionalMove
 * Purpose: Return the correct value based on whether or not the condition is
 *          met
 * Parameters: The integers used to perform the operation
 * Returns: The number that should be placed in the register
 * Notes: None
 */
uint32_t conditionalMove(uint32_t A, uint32_t B, uint32_t C)
{
        if (C != 0) {
                return B;
        }
        return A;
}

/*
 * Name: add
 * Purpose: Add 2 values together
 * Parameters: The integers used to perform the operation
 * Returns: The number that should be placed in the register
 * Notes: In order to mod the values by 2^32, the answer must first be stored in
 *        a 64-bit unsigned integer
 */
uint32_t add(uint32_t B, uint32_t C)
{
        return B + C;
}

/*
 * Name: multiply
 * Purpose: Multiply 2 values together
 * Parameters: The integers used to perform the operation
 * Returns: The number that should be placed in the register
 * Notes: In order to mod the values by 2^32, the answer must first be stored in
 *        a 64-bit unsigned integer
 */
uint32_t multiply(uint32_t B, uint32_t C)
{
        return B * C;
}

/*
 * Name: divide
 * Purpose: Divide 1 value by another
 * Parameters: The integers used to perform the operation
 * Returns: The number that should be placed in the register
 * Notes: None
 */
uint32_t divide(uint32_t B, uint32_t C)
{
        return B / C;
}

/*
 * Name: nand
 * Purpose: Perform bitwise nand on 2 values
 * Parameters: The integers used to perform the operation
 * Returns: The number that should be placed in the register
 * Notes: None
 */
uint32_t nand(uint32_t B, uint32_t C)
{
        return ~(B & C);
}

/*
 * Name: output
 * Purpose: Output a value to standard output
 * Parameters: The value to output
 * Returns: None
 * Notes: The value must be between 0 and 255
 */
void output(uint32_t C)
{
        assert(C < 256);
        putchar(C);
}

/*
 * Name: input
 * Purpose: Take in an input from standard input
 * Parameters: None
 * Returns: The inputted value
 * Notes: None
 */
uint32_t input()
{
        int curr_char = getchar();
        return curr_char;
}

/*
 * Name: loadValue
 * Purpose: Determine the value that should be placed in a register based on the
 *          given instruction
 * Parameters: The instruction
 * Returns: The value to be loaded into the register
 * Notes: None
 */
uint32_t loadValue(uint32_t instruction)
{
        return Bitpack_getu(instruction, LV_REG_LSB, 0);
}