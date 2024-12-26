CC=clang -Wall

TARGET=life

all: ${TARGET}

${TARGET}: ${TARGET}.o inout.o

inout.o: inout.c

clean:
	rm -f *.o *~ ${TARGET}
