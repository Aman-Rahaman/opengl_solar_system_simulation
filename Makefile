OPENGLLIB= -lGL
GLEWLIB= -lGLEW
GLFWLIB = -lglfw
LIBS=$(GLEWLIB) $(GLFWLIB)
LDFLAGS=-L/opt/homebrew/lib
CPPFLAGS=-I/opt/homebrew/include
FRAMEWORK=-framework OpenGL

NAME=main

BIN=$(NAME).out
SRCS=$(NAME).cpp

# INCLUDES=

all: $(BIN) 

clean:
	rm -f *~ *.o *.out *.exe $(BIN)

run: $(BIN)
	./$(BIN)


$(BIN): $(SRCS) #$(INCLUDES)
	g++ -std=c++11 $(SRCS) ../glad/src/glad.c -o $(BIN) $(CPPFLAGS) $(LDFLAGS) $(LIBS) $(FRAMEWORK) -Wdeprecated






# gcc main.c src/glad.c ../glad/src/glad.c -o app -Iinclude -lglfw -framework Cocoa -framework OpenGL -framework IOKit
