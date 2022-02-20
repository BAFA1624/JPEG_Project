CXX = g++
SRC = ${wildcard ./src/*.cpp}
INCLUDES = ${wildcard ./include/*.hpp ./include/*.h}
INC = -I ${INCLUDES}
OBJ = ${SRC:.cpp=.o}
BUILD = ./build
STD = -std=c++20
COMPILE_FLAGS = -Werror -Wall -Wextra -pedantic -O3
NAME = test

executable: ${OBJ}
	$(CXX) $(COMPILE_FLAGS) $(INC) $(STD) $(OBJ) -o $(BUILD)/$(NAME)

all: $(OBJS)
	$(CXX) $(COMPILE_FLAGS) $(INC) $(STD) -c $(SRC)
#executable: main.o PNG.o
#	g++ main.o PNG.o -o $(BUILD)/$(NAME)

#main.o: src/main.cpp includes/PNG.hpp
#	g++ $(STD) $(COMPILE_FLAGS) -c src/main.cpp

#PNG.o: src/PNG.cpp includes/PNG.hpp
#	g++ $(STD) $(COMPILE_FLAGS) $(INC) -c src/PNG.cpp

.PHONY: executable all clean

clean:
	rm {wildcard ./src/*.o} $(BUILD)/$(NAME)
