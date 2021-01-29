OBJECT = main.o
SOURCE = ./src/
REQUIRED = ${SOURCE}user/user.cpp ${SOURCE}user/position.cpp
CFLAGS = -lmpi -I/usr/lib/x86_64-linux-gnu/openmpi/include -I${SOURCE}user
MPIRUNFLAGS = -np 4
LAUNCHPARAMETERS = -N 10 -I 2 -W 200 -L 100 -w 100 -l 50 -v 10 -d 10 -t 1

all: ${OBJECT}

${OBJECT}:

%.o:${SOURCE}%.cpp
	mpic++ -o $@ $< ${CFLAGS} ${REQUIRED}

clean:
	rm ${OBJECT}

run:
	mpirun ${MPIRUNFLAGS} ${OBJECT} ${LAUNCHPARAMETERS}
