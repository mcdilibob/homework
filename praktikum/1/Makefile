CPPFLAGS = -O3 -mfpmath=sse -fstack-protector-all -g -W -Wall -Wextra -Wunused -Wcast-align -Werror -pedantic -pedantic-errors -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long -Woverloaded-virtual -Wnon-virtual-dtor -Wcast-qual -Wno-suggest-attribute=format

all: a.out

main.o: main.cpp matrix.h matrix_io.h
	g++ -c $(CPPFLAGS) $<

matrix.o: matrix.cpp matrix.h
	g++ -c $(CPPFLAGS) $<

matrix_io.o: matrix_io.cpp matrix.h matrix_io.h
	g++ -c $(CPPFLAGS) $<

a.out: main.o matrix.o matrix_io.o
	g++ $^ $(CPPFLAGS)

clean:
	rm -f *.o *.out
