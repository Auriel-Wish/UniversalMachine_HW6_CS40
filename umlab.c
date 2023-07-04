/*
 * umlab.c
 *
 * Functions to generate UM unit tests. Once complete, this module
 * should be augmented and then linked against umlabwrite.c to produce
 * a unit test writing program.
 *  
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.  
 * 
 * Any additional functions and unit tests written for the lab go
 * here. 
 *  
 */


#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>


typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


/* Functions that return the two instruction types */

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc);
Um_instruction lv(unsigned ra, unsigned val);

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
        Um_instruction new_instruction = 0;
        new_instruction = Bitpack_newu(new_instruction, 4, 28, op);
        new_instruction = Bitpack_newu(new_instruction, 3, 6, ra);
        new_instruction = Bitpack_newu(new_instruction, 3, 3, rb);
        new_instruction = Bitpack_newu(new_instruction, 3, 0, rc);
        return new_instruction;
}

Um_instruction lv(unsigned ra, unsigned val)
{
        Um_instruction new_instruction = 0;
        new_instruction = Bitpack_newu(new_instruction, 4, 28, LV);
        new_instruction = Bitpack_newu(new_instruction, 3, 25, ra);
        new_instruction = Bitpack_newu(new_instruction, 25, 0, val);
        return new_instruction;
}

/* Wrapper functions for each of the instructions */

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

static inline Um_instruction cmov(Um_register a, Um_register b, Um_register c)
{
        return three_register(CMOV, a, b, c);
}

static inline Um_instruction sload(Um_register a, Um_register b, Um_register c)
{
        return three_register(SLOAD, a, b, c);
}

static inline Um_instruction sstore(Um_register a, Um_register b, Um_register c)
{
        return three_register(SSTORE, a, b, c);
}

static inline Um_instruction add(Um_register a, Um_register b, Um_register c) 
{
        return three_register(ADD, a, b, c);
}

static inline Um_instruction mul(Um_register a, Um_register b, Um_register c) 
{
        return three_register(MUL, a, b, c);
}

static inline Um_instruction div(Um_register a, Um_register b, Um_register c) 
{
        return three_register(DIV, a, b, c);
}

static inline Um_instruction nand(Um_register a, Um_register b, Um_register c) 
{
        return three_register(NAND, a, b, c);
}

static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}

static inline Um_instruction activate(Um_register b, Um_register c) 
{
        return three_register(ACTIVATE, 0, b, c);
}

static inline Um_instruction inactivate(Um_register c) 
{
        return three_register(INACTIVATE, 0, 0, c);
}

static inline Um_instruction output(Um_register c) 
{
        return three_register(OUT, 0, 0, c);
}

static inline Um_instruction input(Um_register c) 
{
        return three_register(IN, 0, 0, c);
}

static inline Um_instruction loadp(Um_register b, Um_register c) 
{
        return three_register(LOADP, 0, b, c);
}

/* Functions for working with streams */

static inline void append(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
        assert(output != NULL && stream != NULL);
        int stream_length = Seq_length(stream);
        for (int i = 0; i < stream_length; i++) {
                Um_instruction inst = (uintptr_t)Seq_remlo(stream);
                for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
                        fputc(Bitpack_getu(inst, 8, lsb), output);
                }
        }
      
}


/* Unit tests for the UM */

void build_verbose_halt_test(Seq_T stream)
{
        append(stream, halt());
        append(stream, lv(r1, 'B'));
        append(stream, output(r1));
        append(stream, lv(r1, 'a'));
        append(stream, output(r1));
        append(stream, lv(r1, 'd'));
        append(stream, output(r1));
        append(stream, lv(r1, '!'));
        append(stream, output(r1));
        append(stream, lv(r1, '\n'));
        append(stream, output(r1));
}

void build_add_test(Seq_T stream)
{
        append(stream, add(r1, r2, r3));
        append(stream, halt());
}

void build_print_six_test(Seq_T stream)
{
        append(stream, lv(r1, 48));
        append(stream, lv(r2, 6));
        append(stream, add(r3, r1, r2));
        append(stream, output(r3));
        append(stream, halt());
}

/* Real Tests */

/* Input: None */
/* Output: None */
void halt_test(Seq_T stream)
{
        append(stream, halt());
        append(stream, output(r0));
}

/* Input: None */
/* Output: None */
void lv_test(Seq_T stream)
{
        append(stream, lv(r0, 1));
        append(stream, halt());
}

/* Input: None */
/* Output: 65=A */
void out_test(Seq_T stream)
{
        append(stream, lv(r0, 65));
        append(stream, output(r0));
        append(stream, halt());
}

/* Input: None */
/* Output: 65=A */
void add_test(Seq_T stream)
{
        append(stream, lv(r1, 33));
        append(stream, lv(r2, 32));
        append(stream, add(r0, r1, r2));
        append(stream, output(r0));
        append(stream, halt());
}

/* Input: None */
/* Output: 66=B */
void mult_test(Seq_T stream)
{
        append(stream, lv(r1, 33));
        append(stream, lv(r2, 2));
        append(stream, mul(r0, r1, r2));
        append(stream, output(r0));
        append(stream, halt());
}

/* Input: None */
/* Output: 65=A */
void add_mod32_test(Seq_T stream)
{
        append(stream, lv(r0, 2));
        append(stream, lv(r1, 1));
        append(stream, lv(r2, 1));
        append(stream, lv(r3, 65));
        for (int i = 0; i < 31; i++) {
                append(stream, mul(r1, r1, r0));
                append(stream, mul(r2, r2, r0));
        }

        append(stream, add(r2, r2, r3));
        append(stream, add(r4, r1, r2));

        append(stream, output(r4));
        append(stream, halt());
}

/* Input: None */
/* Output: 66=B */
void mult_mod32_test(Seq_T stream)
{
        append(stream, lv(r0, 2));
        append(stream, lv(r1, 1));
        append(stream, lv(r3, 33));
        for (int i = 0; i < 31; i++) {
                append(stream, mul(r1, r1, r0));
        }
        append(stream, add(r1, r1, r3));
        
        append(stream, lv(r2, 2));
        append(stream, mul(r4, r1, r2));

        append(stream, output(r4));
        append(stream, halt());
}

/* Input: None */
/* Output: 65=A */
void div_test(Seq_T stream)
{
        append(stream, lv(r1, 130));
        append(stream, lv(r2, 2));
        append(stream, div(r0, r1, r2));
        append(stream, output(r0));
        append(stream, halt());
}

/* Input: None */
/* Output: 67=C */
void nand_test(Seq_T stream)
{
        append(stream, lv(r0, 33554431));
        append(stream, lv(r1, 33554431));
        append(stream, lv(r2, 128));
        append(stream, lv(r4, 60));
        append(stream, mul(r0, r0, r2));
        append(stream, mul(r1, r1, r2));
        append(stream, add(r0, r0, r4));
        append(stream, add(r1, r1, r4));
        append(stream, nand(r3, r0, r1));
        append(stream, output(r3));
        append(stream, halt());
}

/* Input: None */
/* Output: 65=A */
void cmove_move_test(Seq_T stream)
{
        append(stream, lv(r1, 1));
        append(stream, lv(r2, 65));
        append(stream, lv(r3, 3));
        append(stream, cmov(r3, r2, r1));
        append(stream, output(r3));
        append(stream, halt());
}

/* Input: None */
/* Output: 65=A */
void cmove_no_move_test(Seq_T stream)
{
        append(stream, lv(r2, 2));
        append(stream, lv(r3, 65));
        append(stream, cmov(r3, r2, r1));
        append(stream, output(r3));
        append(stream, halt());
}

/* Input: A=65 */
/* Output: 75=K */
void input_normal_test(Seq_T stream)
{
        append(stream, input(r0));
        append(stream, lv(r1, 10));
        append(stream, add(r2, r1, r0));
        append(stream, output(r2));
        append(stream, halt());
}

/* Input: A=65 */
/* Output: 65=A */
void input_end_of_input_test(Seq_T stream)
{
        /* all bits in r0 should be 1s */
        append(stream, input(r0));
        append(stream, lv(r1, 66));
        append(stream, add(r2, r1, r0));
        append(stream, output(r2));
        append(stream, halt());
}

/* Input: None */
/* Output: 65=A */
void seg_load_seg0_test(Seq_T stream)
{
        append(stream, lv(r1, 0));
        append(stream, lv(r7, 0));
        append(stream, lv(r2, 1));
        append(stream, sload(r0, r1, r2));
        append(stream, lv(r3, 128));
        append(stream, lv(r4, 65));
        append(stream, mul(r0, r0, r3));
        append(stream, add(r0, r0, r4));
        append(stream, output(r0));
        append(stream, halt());
}

/* Input: None */
/* Output: 65=A */
void seg_store_seg0_test(Seq_T stream)
{
        append(stream, lv(r1, 0));
        append(stream, lv(r7, 0));
        append(stream, lv(r2, 1));
        append(stream, sload(r0, r1, r2));
        append(stream, lv(r3, 128));
        append(stream, lv(r4, 65));
        append(stream, mul(r0, r0, r3));
        append(stream, add(r0, r0, r4));
        append(stream, sstore(r1, r7, r0));
        append(stream, sload(r5, r1, r7));
        append(stream, output(r5));
        append(stream, halt());
}

/* Input: None */
/* Output: 65=A */
void map_seg_test(Seq_T stream)
{
        append(stream, lv(r1, 5));
        append(stream, lv(r2, 64));
        append(stream, activate(r0, r1));
        append(stream, add(r0, r0, r2));
        append(stream, output(r0));

        append(stream, lv(r2, 2));
        append(stream, lv(r3, 1));
        append(stream, sstore(r3, r2, r0));
        append(stream, sload(r0, r3, r2));
        append(stream, output(r0));
        append(stream, halt());
}

/* Input: None */
/* Output: None */
void unmap_seg_test(Seq_T stream)
{
        append(stream, lv(r1, 5));
        append(stream, lv(r2, 64));
        append(stream, activate(r0, r1));
        append(stream, add(r0, r0, r2));
        append(stream, lv(r2, 2));
        append(stream, lv(r3, 1));
        append(stream, sstore(r3, r2, r0));
        append(stream, sload(r0, r3, r2));
        append(stream, lv(r0, 1));
        append(stream, inactivate(r0));
        
        append(stream, lv(r1, 5));
        append(stream, lv(r2, 64));
        append(stream, activate(r0, r1));
        append(stream, activate(r0, r2));
        append(stream, activate(r0, r1));
        append(stream, activate(r0, r2));
        append(stream, add(r0, r0, r2));
        append(stream, lv(r2, 2));
        append(stream, lv(r3, 1));
        append(stream, sstore(r3, r2, r0));
        append(stream, sload(r0, r3, r2));
        append(stream, lv(r0, 1));
        append(stream, inactivate(r0));

        append(stream, lv(r1, 5));
        append(stream, lv(r2, 64));
        append(stream, activate(r0, r1));
        append(stream, add(r0, r0, r2));
        append(stream, lv(r2, 2));
        append(stream, lv(r3, 1));
        append(stream, sstore(r3, r2, r0));
        append(stream, sload(r0, r3, r2));
        append(stream, lv(r0, 1));
        append(stream, inactivate(r0));
        append(stream, halt());
}

void test_nand(Seq_T stream)
{
    append(stream, lv(r1, 0xFFFFFFFF));
    append(stream, lv(r2, 0x0000FFFF));
    append(stream, nand(r3, r1, r2));
    append(stream, halt());
}

void test_map(Seq_T stream)
{
    append(stream, lv(r1, 4));
    append(stream, lv(r2, 1));
    append(stream, activate(r2, r1));
    append(stream, lv(r3, 1));
    append(stream, sstore(r2, r0, r3));
    append(stream, sload(r4, r2, r0));
    append(stream, inactivate(r2));
    append(stream, halt());
}

void test_loadp(Seq_T stream)
{
    append(stream, lv(r1, 3));
    append(stream, lv(r2, 1));
    append(stream, lv(r3, 10));
    append(stream, lv(r4, 5));
    append(stream, lv(r5, 5));
    append(stream, lv(r6, 1));
    append(stream, activate(r5, r4));
    append(stream, sstore(r5, r2, r3));
    append(stream, loadp(r5, r1));
    append(stream, sload(r6, r0, r3));
    append(stream, inactivate(r5));
    append(stream, halt());
}

void test_sload(Seq_T stream)
{
    append(stream, lv(r1, 2));
    append(stream, lv(r2, 1));
    append(stream, activate(r2, r1));
    append(stream, lv(r3, 1));
    append(stream, lv(r4, 10));
    append(stream, lv(r5, 1));
    append(stream, sstore(r2, r3, r4));
    append(stream, sload(r5, r2, r3));
    append(stream, inactivate(r2));
    append(stream, halt());
}

void test_sstore_and_sload(Seq_T stream) {
    append(stream, lv(r2, 50));
    append(stream, activate(r1, r2));
    append(stream, lv(r3, 123));
    append(stream, lv(r4, 20));
    append(stream, sstore(r1, r4, r3));
    append(stream, sload(r5, r1, r4));
    append(stream, output(r5));
    append(stream, inactivate(r1));
    append(stream, halt());
}

void loadp_test(Seq_T stream)
{
        append(stream, lv(r0, 5));
        append(stream, lv(r2, 0));
        append(stream, activate(r1, r0));
        append(stream, lv(r3, 10));
        append(stream, sload(r4, r2, r3));
        append(stream, sstore(r1, r2, r4));
        append(stream, loadp(r1, r2));
        append(stream, lv(r7, 5));
        append(stream, lv(r7, 5));
        append(stream, lv(r7, 5));
        append(stream, halt());
}

void loadp_seg0_test(Seq_T stream)
{
        append(stream, lv(r0, 5));
        append(stream, lv(r6, 0));
        append(stream, loadp(r6, r0));
        append(stream, lv(r7, 65));
        append(stream, output(r7));
        append(stream, halt());
}