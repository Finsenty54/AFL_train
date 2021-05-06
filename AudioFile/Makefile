objects :=examples.o


first_try: $(objects) 
	g++ -o first_try $(objects) 

examples.o: examples.cpp AudioFile.h
	g++ -c examples.cpp


.PHONY : clean
clean :
	rm $(objects) first_try
