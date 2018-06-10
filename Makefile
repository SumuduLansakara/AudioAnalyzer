INC=-I/opt/portaudio/include
LIB=-L/opt/portaudio/lib
LIBS=-lportaudio

SRC=$(wildcard *.cpp)
OBJ=$(patsubst %.cpp,%.o,${SRC})

CFLAGS=-std=c++17 -Wall -Weffc++ -fexceptions -O2

TARGET=aulog

${TARGET}: ${OBJ}
	g++ ${INC} ${LIB} $^ ${LIBS} -o ${TARGET}

%.o: %.cpp
	g++ ${CFLAGS} ${INC} -c $^

clean:
	rm -f ${TARGET} ${OBJ}
