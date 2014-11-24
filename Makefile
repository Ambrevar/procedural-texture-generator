ROOT = .
include ${ROOT}/config.mk

################################################################################
## Build.

.PHONY: all
all: app

.PHONY: app
app:
	${MAKE} -C ${srcdir}

.PHONY: debug
debug:
	CFLAGS+="-g3 -O0 -DDEBUG=9" ${MAKE}

.PHONY: clean
clean:
	${MAKE} -C ${srcdir} clean
