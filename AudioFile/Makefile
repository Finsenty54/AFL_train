objects :=examples.o

CC=gcc
CXX=g++
LD=g++
CFLAGS+=-g
EXE=first_try

first_try: $(objects) 
	$(CXX) -o $(EXE) $(objects) 

examples.o: examples.cpp AudioFile.h
	$(CXX) $(CFLAGS) -c examples.cpp


.PHONY : clean
clean :
	rm $(objects) first_try
