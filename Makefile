CC = g++

CPPFLAGS = -fPIC
LDLIBS = -shared

SOURCES = $(wildcard *.cpp)
APP = codegen

all: $(APP)

clean:
	rm libCodeGen.so *.o

$(APP): $(SOURCES:%.cpp=%.o)
	$(LINK.o) $^ $(LDLIBS) -o libCodeGen.so
	strip libCodeGen.so
	cp libCodeGen.so /home/dilma/workspace/hiasm5/packs/fpc/
	cp libCodeGen.so /home/dilma/workspace/hiasm5/packs/lazarus/
