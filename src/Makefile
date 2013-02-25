################################################################################
# procedural-textures makefile
# 2013-02-24
################################################################################

include ../config.mk

## Appends user flags.
CFLAGS_ALL += ${CFLAGS}
LDFLAGS_ALL += ${LDFLAGS}

## Application information.
CFLAGS_ALL += -DAPPNAME=\"${APPNAME}\"
CFLAGS_ALL += -DVERSION=\"${VERSION}\"
CFLAGS_ALL += -DAUTHOR=\"${AUTHOR}\"
CFLAGS_ALL += -DYEAR=\"${YEAR}\"

CFLAGS_ALL += -Wall -Wextra -Wshadow -Winline -pedantic -std=c99

## For use of getopt() with -pedantic.
CFLAGS_ALL += -D_XOPEN_SOURCE=700

## SDL
LDFLAGS_ALL += -lSDL

## Math
LDFLAGS_ALL += -lm

SRC += main.c
# OBJ = ${SRC:.c=.o}

##==============================================================================
## Rules
##==============================================================================

all: options ${BIN} creator

options:
	@echo ${BIN} build options:
	@echo "CFLAGS  = ${CFLAGS_ALL}"
	@echo "LDFLAGS = ${LDFLAGS_ALL}"
	@echo "CC      = ${CC}"

creator: creator.c
	@${CC} ${CFLAGS_ALL} $^ -o $@

${BIN}: ${SRC}
	@${CC} ${LDFLAGS_ALL} $^ -o $@

%.o: %.c
	@${CC} ${CFLAGS_ALL} $< -c -o $@

clean:
	@rm -f ${BIN} ${OBJ} creator

.PHONY=all options clean

