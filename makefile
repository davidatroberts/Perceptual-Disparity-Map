TARGET := pdisparity
CC := clang++

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
IMG_DIR = images

SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
SOURCES := $(filter-out $(wildcard $(SRC_DIR)/test*.cpp), $(SOURCES))
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

INCLUDE_DIR = -Iinclude -I/usr/local/include/
LIBRARY_DIR = -Llib -L/usr/local/lib/
LIBRARIES = -lopencv_core -lopencv_highgui -lopencv_imgproc
LD_FLAGS = -O2 -Wall -stdlib=libc++ -std=c++11

$(BIN_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	@mkdir -p $(IMG_DIR)
	@$(CC) -o $@ $(LD_FLAGS) $(LD_FLAGS) $(LIBRARY_DIR) $(LIBRARIES)  $(OBJECTS)
	@echo "Linked OK"

$(OBJECTS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE_DIR) $(LD_FLAGS)
	@echo "Compiled OK"

.PHONEY: clean
clean:
	@rm -rf $(BIN_DIR)
	@rm -rf $(OBJ_DIR)