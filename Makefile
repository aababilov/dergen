#defines
CC=gcc
OBJFILES=main.o dgutils.o parser.o lex.o
BISONFLAGS=-dtv
BISON=bison
EXENAME=dergen

#general
all: $(EXENAME)

clean:
	-$(RM) *~ *.o parser.c parser.h parser.output

#implicit rules	
%.o: %.c parser.h lex.h dgutils.h
	$(CC) -c $< -o $@	

#explicit rules	
$(EXENAME): $(OBJFILES)
	$(CC) -o $@ $^

parser.c: parser.y
	-$(BISON) $(BISONFLAGS) -o $@ $<

parser.h: parser.c