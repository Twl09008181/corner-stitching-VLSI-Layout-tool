

CXX = g++
DEBUG = -g   
CXXFLAGS = -std=c++11 ${DEBUG}

OBJ = main.o tile.o   
TARGET = Lab1

$(TARGET) : $(OBJ) 
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) 


main.o : tile.hpp
tile.o : tile.hpp


.PHONY : clean
clean:
	rm *.o temp
