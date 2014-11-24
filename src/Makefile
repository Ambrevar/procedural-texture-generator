ROOT ?= ..
include ${ROOT}/config.mk

CPPFLAGS += -DAUTHORS="${authors}" -DVERSION=${version} -DYEAR=${year}
CPPFLAGS += -DHAVE_INLINE
LDLIBS += -lm
LDLIBS += -lSDL

all: ${cmdname} ptx-creator

.PHONY: debug
debug:
	CFLAGS+="-g3 -O0 -DDEBUG=9" ${MAKE}

.PHONY: clean
clean:
	rm -f ${cmdname} *.d *.o ptx-creator

## Generate prerequisites automatically. GNU Make only.
## The 'awk' part is used to add the .d file itself to the target, so that it
## gets updated on changes. The -MQ option is to add full path to object files
## in subfolders. (-MM strips parent directories.)
%.d: %.c
	${CC} -MM -MQ ${<:.c=.o} ${CPPFLAGS} $< | awk -v stem=$* -v target=$@ '{gsub (stem ".o[ :]*", stem ".o " target " : ")} 1' > $@

sources = $(wildcard *.c)
deps = ${sources:.c=.d}
-include ${deps}
