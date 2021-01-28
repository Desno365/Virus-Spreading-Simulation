OBJECT = main.o
SOURCE = ./src/
CFLAGS = -lmpi -I/usr/lib/x86_64-linux-gnu/openmpi/include
MPIRUNFLAGS = -np 4
LAUNCHPARAMETERS = -N 10 -I 2 -W 200 -L 100 -w 100 -l 50 -v 10 -d 10 -t 1

all: ${OBJECT}

%.o:${SOURCE}%.cpp
	mpic++ -o $@ $< 

clean:
	rm ${OBJECT}

run:
	mpirun ${MPIRUNFLAGS} ${OBJECT} ${LAUNCHPARAMETERS}
