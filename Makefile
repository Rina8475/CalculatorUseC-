#
# Makefile that builds calculator
# 
CC = g++
CFLAGS = -Og -Wall

all: buildin.cpp calculate.cpp parse.cpp table.cpp tokens.cpp
	$(CC) $(CFLAGS) -o calculator buildin.cpp calculate.cpp parse.cpp table.cpp tokens.cpp

test: table.cpp test.cpp buildin.cpp
	$(CC) $(CFLAGS) -o test table.cpp buildin.cpp test.cpp calculate.cpp parse.cpp tokens.cpp
clean:
	rm -f *.o calculator


