Adam Weiss (aweiss15) and Auriel Wish (awish01)


Acknowledgements:
        Stack Overflow showed us how to get the size of a file using stat


To our knowledge, everything has been correctly implemented


Changes to Design:
        Memory segments that were arrays of uint_32t's were placed in a struct
        that contained the segment array as well as the number of words in the
        array. We also created a struct that holds all of the structures and
        variables that have to do with the UM memory. This is an incomplete
        struct and is completed only in memory.c.


Architecture:
        Module 1 - um
                * Runs the command loop - calls functions from both other
                  modules
                * Has no direct access to registers, memory segments, or the
                  segment structs. Only has access to incomplete structs
                  regarding UM memory.
        Module 2 - memory
                * Contains functions that directly manipulate memory. Therefore,
                  it has direct access to the registers and memory segments.
                * Provides setter and getter functions for registers, allowing
                  the other modules to manipulate them through abstraction.
                * Provides memory allocation and freeing for UM allocated
                  memory.
                * Uses incomplete structs to allow other modules to perform
                  necessary UM operations while maintaining the structs'
                  secrets.
        Module 3 - arithmetic
                * Contains arithmetic operations, I/O, and load value.
                * Has no direct access to registers, memory segments, or the
                  segment structs. Only has access to incomplete structs
                  regarding UM memory.


50 Million Instructions takes 2.34 seconds. This is because midmark is about 80
million instructions and midmark takes 3.74 seconds. 


UM Unit Tests:
  halt_test: halts the program and then tries to print.

  lv_test: loads a value into a register and then halts.

  output_test: loads a value into a register and prints it to see if it was 
  correct. Then halts.

  add_test: loads a value into 2 registers. Then adds the value and prints the
  result. Since we know lv and output work, this test isolates the add function

  mult_test: Same as the add test but with multiply instead.

  add_mod32_test: Tests that if adding 2 numbers will be greater than a 32 bit
  number, the result will wrap around to 0.

  mult_mod32_test: same as add_mod32 but multiplies numbers instead
  
  div_test: same as add_test but with divide.

  nand_test: loads values into multiple registers and then performs some 
  arithmatic on them. Then nands two registers together and prints the result.
  
  cmove_move_test: tests the positive conditional move case by loading 1 into a
  register and performing a move. The value is then outputted.

  cmove_no_move_test: tests the negative conditional move case by loading a 
  value into a register and trying to move another value into that register.
  This move should fail and the printed value of the original register should
  be unchanged.

  input_normal_test: gets input from the user and alters it using the add 
  function. Then prints the result.

  seg_load_seg0_test: load a value from segment 0 into a register and print
  the result.

  seg_store_seg0_test: store a value in a register into segment 0 and then pull
  the value back out of segment 0 and print the result.

  map_seg_test: map a new segment and add two numbers before putting that
  value in the segment. Load the value back into a register and print the result

  unmap_seg_test: map a few segments and load value into them. Perform 
  computation on the values and then unmap the segments and assure no memory
  leaks.

  test_sstore/sload: load a value into a register and then map and segment with 
  that value. Load the value into a register and print that value.

  loadp_test: map a segment and load a halt instruction into it. Then do a lp
  on that segment and see if the program halts.

  loadp_seg0_test: Load segment 0 which will just reset the program counter and
  output the value in a register.


Hours Spent:
        Analyzing the assignment: 5
        Preparing the design: 4
        Solving the problems after our analysis: 13