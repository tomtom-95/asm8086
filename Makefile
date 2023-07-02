.RECIPEPREFIX = <
.PHONY = build run test clean

SOURCENAME = asm8086.c
EXECNAME = ${basename ${SOURCENAME}}

DEGUGEXTENSION = .dSYM
DEBUGNAME = ${EXECNAME}${DEGUGEXTENSION}

TESTFILENAME = test_minimal.asm
BASEFILENAME = ${basename ${TESTFILENAME}}

NASMFILENAME = ${BASEFILENAME}_nasm

CC = clang
CFLAGS = -Wall -Wextra -Wpedantic -Wconversion \
         -Wformat=2 -Wno-unused-parameter -Wshadow -Wvla \
         -Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
         -Wredundant-decls -Wnested-externs -Wmissing-include-dirs \
		 -Werror=cast-qual -Werror=constant-conversion \

# GCC warnings that Clang doesn't provide:
ifeq (${CC}, gcc)
    CFLAGS += -Wjump-misses-init -Wlogical-op
endif

# exclude some annoying warning:
CFLAGS += -Wno-gnu-binary-literal \
		  -Wno-c11-extensions

# only preprocessor output
# CFLAGS += -E

CFLAGS += -O0
CFLAGS += -std=c99

INCLUDE = -I.

build:
< ${CC} ${CFLAGS} ${INCLUDE} -g -o ${EXECNAME} ${SOURCENAME}

run:
< ./${EXECNAME} ${TESTFILENAME}

test:
< nasm ${TESTFILENAME} -o ${NASMFILENAME}
< diff ${BASEFILENAME} ${NASMFILENAME} 

clean:
< rm -rf ${NASMFILENAME} ${BASEFILENAME} ${EXECNAME} ${DEBUGNAME}
