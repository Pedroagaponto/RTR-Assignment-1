CC = clang

SDIR = src
HDIR = headers
ODIR = obj

_OBJ = main.o core.o input.o draw.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

CFLAGS = -I$(HDIR) -Wall -Wextra -pedantic
LIBS = -lm -lSDL2 -lGL -lGLU

all: wave

debug: CFLAGS += -g
debug: all

$(ODIR)/%.o: $(SDIR)/%.c
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)

wave: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean
clean:
	-rm -r $(ODIR) wave

