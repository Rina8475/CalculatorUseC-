#
# Makefile that builds calculator
# 
CC = g++
CFLAGS = -Og -Wall

ODIR = target
OBJS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
COBJS = $(filter-out test.o, $(OBJS))
_OBJS = $(patsubst %.o, $(ODIR)/%.o, $(COBJS))

calculator: $(_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(_OBJS): $(ODIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

test: $(_OBJS) $(ODIR)/test.o
	$(CC) $(CFLAGS) $^ -o $@

$(ODIR)/test.o: test.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(ODIR)/*.o calculator test


