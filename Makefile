CC = clang++

SDIR = src
HDIR = headers
ODIR = obj

_OBJ = main.o renderer.o input.o collision.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

CFLAGS = -I$(HDIR) -Wall -Wextra -pedantic
LIBS = -lm -lGL -lGLU -lglut

all: peggle

debug: CFLAGS += -g
debug: all

$(ODIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)

peggle: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean
clean:
	-rm -r $(ODIR) peggle

