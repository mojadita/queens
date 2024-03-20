CFLAGS ?= -O0 -g

targets = queens knight
toclean += $(targets)

all: $(targets)
clean:
	rm -f $(toclean)

queens: queens.c
