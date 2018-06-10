INC=-I/opt/portaudio/include
LIB=-L/opt/portaudio/lib
LIBS=-lportaudio

SRC=$(wildcard *.cpp)
OBJ=$(patsubst %.cpp,%.o,${SRC})

TARGET=aulog

${TARGET}: ${OBJ}
	g++ ${INC} ${LIB} $^ ${LIBS} -o ${TARGET}

%.o: %.cpp
	g++ ${INC} ${LIB} ${LIBS} -c $^

clean:
	rm -f ${TARGET} ${OBJ}
