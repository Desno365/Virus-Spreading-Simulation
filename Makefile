OBJECT = main.o
SOURCE = ./src
OPENMPIPATH = /usr/lib/x86_64-linux-gnu/openmpi/include
#OPENMPIPATH = /usr/local/Cellar/open-mpi/4.0.5/include
## Here select the ID of the processor you are interested in debugging
DEBUGGEDPROCESSOR = 1
REQUIRED = $(wildcard $(SOURCE)/**/*.cpp)
CFLAGS = -std=c++11 -ggdb -O0 -lmpi -I${OPENMPIPATH} -I${SOURCE}
MPIRUNFLAGS = -np 4
LAUNCHPARAMETERS = -N 8 -I 2 -W 200 -L 100 -w 100 -l 50 -v 10 -d 10 -t 1 -D 4

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