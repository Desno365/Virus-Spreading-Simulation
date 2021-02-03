OBJECT = main.o
SOURCE = ./src
## Here select the ID of the processor you are interested in debugging
DEBUGGEDPROCESSOR = 0
REQUIRED = $(wildcard $(SOURCE)/**/*.cpp)
CFLAGS = -ggdb -O0 -lmpi -I/usr/lib/x86_64-linux-gnu/openmpi/include -I${SOURCE}
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