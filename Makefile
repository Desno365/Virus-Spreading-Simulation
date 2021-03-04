OBJECT = main.o
SOURCE = ./src
OPENMPIPATH = /usr/lib/x86_64-linux-gnu/openmpi/include
#OPENMPIPATH = /usr/local/Cellar/open-mpi/4.0.5/include
# Here select the ID of the processor you are interested in debugging
DEBUGGEDPROCESSOR = 0
REQUIRED = $(wildcard $(SOURCE)/**/*.cpp)
CFLAGS = -std=c++11 -Wall -ggdb -O0 -lmpi -I${OPENMPIPATH} -I${SOURCE}
MPIRUNFLAGS = -np 4
# Notes:
# - A person walking has a speed of ≈1.3 m/s.
#		But in a realistic scenario not everyone is walking and many people are stationary.
#		If we suppose 90% of people are stationary we have an average speed of 0.13m/s.
#		If we suppose 95% of people are stationary we have an average speed of 0.065m/s.
# - The spreading distance d depends on the virus itself and many other conditions.
#		For Covid an almost safe distance is thought to be 2m.
LAUNCHPARAMETERS = -N 2000 -I 20 -W 1000 -L 1000 -w 250 -l 250 -v 0.13 -d 2.0 -t 500 -D 90

all: ${OBJECT}

${OBJECT}:

%.o:${SOURCE}/%.cpp
	mpic++ ${CFLAGS} -o $@ $< ${REQUIRED}

clean:
	rm ${OBJECT}

run:
	mpirun ${MPIRUNFLAGS} ${OBJECT} ${LAUNCHPARAMETERS}

debug:
	mpirun ${MPIRUNFLAGS} ${OBJECT} ${LAUNCHPARAMETERS} --debug ${DEBUGGEDPROCESSOR}

compile-test:
	mpic++ ${CFLAGS} -o ./test.o ./test/test.cpp ${REQUIRED}

run-test:
	./test.o --success

test:
	mpic++ ${CFLAGS} -o ./test.o ./test/test.cpp ${REQUIRED} && ./test.o --success