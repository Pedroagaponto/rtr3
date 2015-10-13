CC = clang++

SDIR = src
HDIR = headers
ODIR = obj

_OBJ = main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

CFLAGS = -I$(HDIR) -Wall -Wextra -pedantic
LIBS = -lm -lSDL2 -lGL -lGLU

all: peggle

debug: CFLAGS += -g
debug: all

$(ODIR)/%.o: $(SDIR)/%.c
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)

pegglee: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean
clean:
	-rm -r $(ODIR) wave

