OBJECT = main.o
SOURCE = ./src
REQUIRED = $(wildcard $(SOURCE)/**/*.cpp)
CFLAGS = -ggdb -O0 -lmpi -I/usr/lib/x86_64-linux-gnu/openmpi/include -I${SOURCE}
MPIRUNFLAGS = -np 4
LAUNCHPARAMETERS = -N 10 -I 2 -W 200 -L 100 -w 100 -l 50 -v 10 -d 10 -t 1

all: ${OBJECT}

${OBJECT}:

%.o:${SOURCE}/%.cpp
	mpic++ ${CFLAGS} -o $@ $< ${REQUIRED}

clean:
	rm ${OBJECT}

run:
	mpirun ${MPIRUNFLAGS} ${OBJECT} ${LAUNCHPARAMETERS}

debug:
	mpirun ${MPIRUNFLAGS} xterm -e gdb ./${OBJECT}