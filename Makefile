CC = g++
CFLAGS = -Wall -ansi -pedantic -I include
LDFLAGS = -lSDL -lGLU -lGL -lm -lSDL_image

SRC_PATH = src
BIN_PATH = bin

EXEC = trifocalTensor

SRC_FILES = $(shell find $(SRC_PATH) -type f -name '*.cpp')
OBJ_FILES = $(patsubst $(SRC_PATH)/%.cpp, $(SRC_PATH)/%.o, $(SRC_FILES))

all: $(BIN_PATH)/$(EXEC)

$(BIN_PATH)/$(EXEC): $(OBJ_FILES)
	$(CC) -o $@ $^ $(LDFLAGS)

$(SRC_PATH)/%.o: $(SRC_PATH)/%.cpp
	$(CC) -c -o $@ $(CFLAGS) $^ 

clean:
	rm $(OBJ_FILES)

cleanall:
	rm $(BIN_PATH)/$(EXEC) $(OBJ_FILES)
