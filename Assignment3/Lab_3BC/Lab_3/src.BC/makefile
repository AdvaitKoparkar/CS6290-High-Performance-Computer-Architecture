SIM_SRC  = rat.cpp rest.cpp rob.cpp pipeline.cpp sim.cpp exeq.cpp 
SIM_OBJS = $(SIM_SRC:.cpp=.o)

all: $(SIM_SRC) sim

%.o: %.cpp
	g++ -Wall -c -o $@ $<  

sim: $(SIM_OBJS) 
	g++ -Wall -o $@ $^

clean: 
	rm sim *.o
