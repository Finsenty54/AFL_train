objects :=examples.o

CC=gcc
CXX=g++
LD=g++

first_try: $(objects) 
	$(CXX) -o first_try $(objects) 

examples.o: examples.cpp AudioFile.h
	$(CXX) -c examples.cpp


.PHONY : clean
clean :
	rm $(objects) first_try
