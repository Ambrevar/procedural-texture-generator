################################################################################
# procedural-textrures makefile
# 2013-02-24
################################################################################

include "config.mk"

all:
	@make -C src/

clean:
	@make -C src/ clean


.PHONY = all clean 
