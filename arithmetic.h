/**************************************************************
 *
 *                     arithmetic.h
 *
 *      Assignment: UM
 *      Authors: Adam and Auriel Wish
 *      Date: 4/5/2023
 *
 *      Purpose: Interface for arithmetic operations (and some
 *               others)
 *
 *
 **************************************************************/

#ifndef ARITHMETIC_INCLUDED
#define ARITHMETIC_INCLUDED

#include <stdio.h>
#include "assert.h"
#include "bitpack.h"

uint32_t conditionalMove(uint32_t A, uint32_t B, uint32_t C);
uint32_t add(uint32_t B, uint32_t C);
uint32_t multiply(uint32_t B, uint32_t C);
uint32_t divide(uint32_t B, uint32_t C);
uint32_t nand(uint32_t B, uint32_t C);
void output(uint32_t C);
uint32_t input();
uint32_t loadValue(uint32_t instruction);

#endif