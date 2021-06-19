SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LIB := 
PARAMETERS := -Wall
APP_NAME := compressor

all: clean build
rebuild: cleaner build

$(OBJ_FILES): | $(OBJ_DIR)

$(OBJ_DIR):
	@echo Creating $(OBJ_DIR) directory
	@mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	@echo Creating $(BIN_DIR) directory
	@mkdir -p $(BIN_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo Compiling $<
	@g++ $< -o $@ -c $(LIB) $(PARAMETERS)

build: $(OBJ_FILES) | $(BIN_DIR)
	@echo Creating bin/$(APP_NAME)
	@g++ -o $(BIN_DIR)/$(APP_NAME) $(OBJ_FILES) $(LIB) $(PARAMETERS)

clean:
	@echo Cleaning $(BIN_DIR)
	@rm -f $(BIN_DIR)/*

cleaner:
	@echo Cleaning $(BIN_DIR)
	@rm -f $(BIN_DIR)/*
	@echo Cleaning $(OBJ_DIR)
	@rm -f $(OBJ_DIR)/*
