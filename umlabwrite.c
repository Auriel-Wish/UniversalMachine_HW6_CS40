#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "fmt.h"
#include "seq.h"

extern void Um_write_sequence(FILE *output, Seq_T instructions);
extern void halt_test(Seq_T instructions);
extern void lv_test(Seq_T instructions);
extern void out_test(Seq_T instructions);
extern void add_test(Seq_T instructions);
extern void mult_test(Seq_T instructions);
extern void add_mod32_test(Seq_T instructions);
extern void mult_mod32_test(Seq_T instructions);
extern void div_test(Seq_T instructions);
extern void nand_test(Seq_T instructions);
extern void cmove_move_test(Seq_T instructions);
extern void cmove_no_move_test(Seq_T instructions);
extern void input_normal_test(Seq_T instructions);
extern void input_end_of_input_test(Seq_T instructions);
extern void seg_load_seg0_test(Seq_T stream);
extern void seg_store_seg0_test(Seq_T stream);
extern void map_seg_test(Seq_T stream);
extern void unmap_seg_test(Seq_T stream);
extern void test_sload(Seq_T stream);
extern void test_sstore_and_sload(Seq_T stream);
extern void loadp_test(Seq_T stream);
extern void loadp_seg0_test(Seq_T stream);


/* The array `tests` contains all unit tests for the lab. */

static struct test_info {
        const char *name;
        const char *test_input;          /* NULL means no input needed */
        const char *expected_output;
        /* writes instructions into sequence */
        void (*build_test)(Seq_T stream);
} tests[] = {
        {"halt_test", NULL, "", halt_test},
        {"lv_test", NULL, "", lv_test},
        {"out_test", NULL, "A",  out_test},
        {"add_test", NULL, "A",  add_test},
        {"mult_test", NULL, "B",  mult_test},
        {"add_mod32_test", NULL, "A",  add_mod32_test},
        {"mult_mod32_test", NULL, "B",  mult_mod32_test},
        {"div_test", NULL, "A",  div_test},
        {"nand_test", NULL, "C", nand_test},
        {"cmove_move_test", NULL, "A",  cmove_move_test},
        {"cmove_no_move_test", NULL, "A",  cmove_no_move_test},
        {"seg_load_seg0_test", NULL, "A",  seg_load_seg0_test},
        {"seg_store_seg0_test", NULL, "A",  seg_store_seg0_test},
        {"map_seg_test", NULL, "AA",  map_seg_test},
        {"unmap_seg_test", NULL, "",  unmap_seg_test},
        {"test_sload", NULL, "",  test_sload},
        {"test_sstore_and_sload", NULL, "{",  test_sstore_and_sload},
        {"loadp_test", NULL, "", loadp_test},
        {"loadp_seg0_test", NULL, "", loadp_seg0_test},
        {"input_normal_test", "A", "K",  input_normal_test}
};

  
#define NTESTS (sizeof(tests)/sizeof(tests[0]))

/*
 * open file 'path' for writing, then free the pathname;
 * if anything fails, checked runtime error
 */
static FILE *open_and_free_pathname(char *path);

/*
 * if contents is NULL or empty, remove the given 'path', 
 * otherwise write 'contents' into 'path'.  Either way, free 'path'.
 */
static void write_or_remove_file(char *path, const char *contents);
static void write_test_files(struct test_info *test);


int main (int argc, char *argv[])
{
        bool failed = false;
        if (argc == 1)
                for (unsigned i = 0; i < NTESTS; i++) {
                        printf("***** Writing test '%s'.\n", tests[i].name);
                        write_test_files(&tests[i]);
                }
        else
                for (int j = 1; j < argc; j++) {
                        bool tested = false;
                        for (unsigned i = 0; i < NTESTS; i++)
                                if (!strcmp(tests[i].name, argv[j])) {
                                        tested = true;
                                        write_test_files(&tests[i]);
                                }
                        if (!tested) {
                                failed = true;
                                fprintf(stderr,
                                        "***** No test named %s *****\n",
                                        argv[j]);
                        }
                }
        return failed; /* failed nonzero == exit nonzero == failure */
}


static void write_test_files(struct test_info *test)
{
        FILE *binary = open_and_free_pathname(Fmt_string("%s.um", test->name));
        Seq_T instructions = Seq_new(0);
        test->build_test(instructions);
        Um_write_sequence(binary, instructions);
        Seq_free(&instructions);
        fclose(binary);

        write_or_remove_file(Fmt_string("%s.0", test->name),
                             test->test_input);
        write_or_remove_file(Fmt_string("%s.1", test->name),
                             test->expected_output);
}


static void write_or_remove_file(char *path, const char *contents)
{
        if (contents == NULL || *contents == '\0') {
                remove(path);
        } else {
                FILE *input = fopen(path, "wb");
                assert(input != NULL);

                fputs(contents, input);
                fclose(input);
        }
        free(path);
}


static FILE *open_and_free_pathname(char *path)
{
        FILE *fp = fopen(path, "wb");
        assert(fp != NULL);

        free(path);
        return fp;
}