##==============================================================================
## procedural-textures make configuration
##==============================================================================

APPNAME = "procedural-textures"
BIN = procedural-textures
VERSION = 1.0
AUTHOR = "Pierre Neidhardt"
# MAIL <ambrevar [at] gmail [dot] com>"
YEAR = "2013"

## Paths
PREFIX ?= /usr/local
BINDIR ?= ${PREFIX}/bin
DATADIR ?= ${PREFIX}/share
MANDIR ?= ${DATADIR}/man
MAN1DIR ?= ${MANDIR}/man1

## Tools
CC=gcc

## Customize below to fit your system
CFLAGS ?= -g3 -O0
# CFLAGS ?= -Os
# LDFLAGS ?= -s
