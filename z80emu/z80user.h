/* z80user.h
 * Add your code here to interface the emulated system with z80emu. See towards
 * the end of the file for an example for running zextest.
 *
 * Copyright (c) 2016 Lin Ke-Fong
 *
 * This code is free, do whatever you want with it.
 */

#ifndef __Z80USER_INCLUDED__
#define __Z80USER_INCLUDED__

#include <stdint.h>
#include <cstdlib>
#include <cstdio>

extern uint8_t z80_memory_read(uint16_t address);
extern void z80_memory_write(uint16_t address, uint8_t val);

 /* Write the following macros for memory access and input/output on the Z80.
  *
  * Z80_FETCH_BYTE() and Z80_FETCH_WORD() are used by the emulator to read the
  * code (opcode, constants, displacement, etc). The upper 16-bit of the address
  * parameters is undefined and must be reset to zero before actually reading
  * memory (use & 0xffff). The value x read, must be an unsigned 8-bit or 16-bit
  * value in the endianness of the host processor.
  *
  * Z80_READ_BYTE(), Z80_WRITE_BYTE(), Z80_READ_WORD(), and Z80_WRITE_WORD()
  * are used for general memory access. They obey the same rules as the code
  * reading macros. The upper bits of the value x to write may be non-zero.
  * Z80_WRITE_WORD_INTERRUPT() is same as Z80_WRITE_WORD(), except it is only
  * used for interrupt generation.
  *
  * Z80_INPUT_BYTE() and Z80_OUTPUT_BYTE() are for input and output. The upper
  * bits of the port number to read or write are always zero. The input byte x
  * must be an unsigned 8-bit value. The value x to write is an unsigned 8-bit
  * with its upper bits zeroed.
  *
  * All macros have access to the following three variables:
  *
  *      state           Pointer to the current Z80_STATE. Because the
  *			instruction is currently executing, its members may not
  *			be fully up to date, depending on when the macro is
  *			called in the process. It is rather suggested to access
  *			the state only when the emulator is stopped.
  *
  *      elapsed_cycles  Number of cycles emulated. If needed, you may add wait
  *			states to it for slow memory accesses. Because the
  *			macros are called during the execution of the current
  *			instruction, this number is only precise up to the
  *			previous one.
  *
  *	context 	This is the (void *) context passed to the emulation
  *			functions.
  *
  * Except for Z80_WRITE_WORD_INTERRUPT, all macros also have access to:
  *
  *      number_cycles   Number of cycles to emulate. After executing each
  *			instruction, the emulator checks if elapsed_cycles is
  *			greater or equal to number_cycles, and will stops if
  *			so. Hence you may decrease or increase the value of
  *			number_cycles to stop the emulation earlier or later.
  * 			In particular, if you set it to zero, the emulator will
  * 			stop after completion of the current instruction.
  *
  *      registers       Current register decoding table, use it to determine if
  * 			the current instruction is prefixed. It points on:
  *
  *				state->dd_register_table for 0xdd prefixes;
  *                      	state->fd_register_table for 0xfd prefixes;
  *				state->register_table otherwise.
  *
  *      pc              Current PC register (upper bits are undefined), points
  *                      on the opcode, the displacement or constant to read for
  *                      Z80_FETCH_BYTE() and Z80_FETCH_WORD(), or on the next
  *                      instruction otherwise.
  *
  * Except for Z80_FETCH_BYTE(), Z80_FETCH_WORD(), and Z80_WRITE_WORD_INTERRUPT,
  * all other macros can know which instruction is currently executing:
  *
  *      opcode          Opcode of the currently executing instruction.
  *
  *      instruction     Type of the currently executing instruction, see
  *                      instructions.h for a list.
  */

  /* Here are macros for emulating zexdoc and zexall. Read/write memory macros
   * are written for a linear 64k RAM. Input/output port macros are used as
   * "traps" to simulate system calls.
   */

//#include "zextest.h"

#define Z80_READ_BYTE(address, x)                                       \
{                                                                       \
	(x) = z80_memory_read((address) & 0xffff);                          \
}

#define Z80_FETCH_BYTE(address, x)		Z80_READ_BYTE((address), (x))

#define Z80_READ_WORD(address, x)                                       \
{                                                                       \
	(x) = z80_memory_read((address) & 0xffff) |                         \
		   (z80_memory_read(((address)+1) & 0xffff) << 8);              \
}

#define Z80_FETCH_WORD(address, x)		Z80_READ_WORD((address), (x))

#define Z80_WRITE_BYTE(address, x)                                      \
{                                                                       \
	z80_memory_write((address) & 0xffff, (uint8_t)x);                   \
}

#define Z80_WRITE_WORD(address, x)                                      \
{                                                                       \
	z80_memory_write((address) & 0xffff, (uint8_t)(x));                 \
	z80_memory_write(((address) + 1) & 0xffff, (uint8_t)((x) >> 8));    \
}

#define Z80_WRITE_WORD_INTERRUPT(address, x)	Z80_WRITE_WORD((address), (x))

#define Z80_INPUT_BYTE(port, x)                                         \
{                                                                       \
		if (state->registers.byte[Z80_C] == 2)                  \
				printf("%c", state->registers.byte[Z80_E]);     \
		else if (state->registers.byte[Z80_C] == 9) {           \
				int     i, c;                                           \
				for (i = state->registers.word[Z80_DE], c = 0;  \
						z80_memory_read(i) != '$';                      \
						i++) {                                          \
						printf("%c", z80_memory_read(i & 0xffff));      \
						if (c++ > 100) {              \
								fprintf(stderr,                         \
										"String to print is too long!\n"); \
								exit(EXIT_FAILURE);                     \
						}                                               \
				}                                                       \
		}                                                               \
}

#define Z80_OUTPUT_BYTE(port, x)                                        \
{                                                                       \
		/*((ZEXTEST *) context)->is_done = !0;*/ 				        \
		number_cycles = 0;                                              \
}

#endif
