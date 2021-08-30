CXX = g++
CXXFLAGS = -std=c++14 -Wall

OUT = proj
OBJS = main.o board.o history.o

all: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(OUT)

main.o: board.o
board.o: history.o 
history.o:


.PHONY: clean
clean:
	rm -rf *.o $(OUT)