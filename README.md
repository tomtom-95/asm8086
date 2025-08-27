# asm8086

A simple educational **8086 assembler** written in C.
It takes 8086 assembly code as input, tokenizes it, parses instructions, and generates the corresponding machine code.

## Project Structure

* **`asm8086.c`** – main entry point (compiled into the `asm8086` executable).
* **`build.sh`** – build script (replacement for the Makefile).
  Provides targets for building, running, testing, and generating preprocessor output.
* **Core modules**:

  * `tokenizer.[ch]` – breaks assembly source into tokens.
  * `parser.[ch]` – parses tokens into structured instructions and operands.
  * `codegen.[ch]` – converts parsed instructions into machine encodings.
  * `tables.h` – lookup tables for mnemonics, registers, encodings.
  * `hashmap.[ch]` – custom hash map implementation (used for label and symbol handling).

* **Supporting modules**:

  * `arena.*`, `utils.*`, `string.*` – utilities for memory management and strings.

## Features

* Supports a subset of **8086 instructions** (see `tables.h`).
* Tokenization of registers, mnemonics, labels, immediates, and memory operands.
* Parsing of instruction formats with one or two operands.
* Encoding of instructions into binary format (close to NASM output).
* Basic label management and address patching.
* Designed with **clarity and modularity** in mind (each phase of the assembler is separated).

## Build

Requirements:

* `clang` or `gcc`
* `nasm` (for cross-checking test binaries)

To build the assembler:

```bash
./build.sh build
```

The executable will be created under `./build/asm8086`.

## Run

To assemble a test file:

```bash
./build.sh run
```

This uses the default test file at `./resources/test_one_instruction.asm`.

## Test

You can compare the assembler’s output against NASM:

```bash
./build.sh test
```

This assembles the test input with both your assembler and NASM, then diffs the resulting binaries.

## Development Notes

* Written in **C99**, with extra warnings enabled and sanitizers (`-fsanitize=address`, `-fsanitize=undefined`).
* Still under active development:

  * Instruction coverage is partial.
  * Displacement handling and label patching logic are work-in-progress.
  * Some TODOs are marked in the source (e.g. `parser.c`, `codegen.c`).
