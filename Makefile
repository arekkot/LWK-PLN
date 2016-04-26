CXX         ?= g++
CXXFLAGS     = $(INCLUDES_DIR) -std=c++11 -Wall -Wextra -Wpedantic -march=native -msse3 -O2 -fPIC -pipe -flto  -fomit-frame-pointer -ftree-loop-im -funroll-loops
QT_INC_DIR   = /usr/include/x86_64-linux-gnu/qt5
INCLUDES_DIR = -I/home/arek/opencv-2.4.9/release/bin/include -Isrc -I$(QT_INC_DIR) -I$(QT_INC_DIR)/QtCore -I$(QT_INC_DIR)/QtGui -I$(QT_INC_DIR)/QtWidgets
BIN_DIR      = bin
OBJ_DIR      = obj
SRC_DIR      = src
TARGETS      = recognizer
LIBS         = -lpthread -lQt5Widgets -lQt5Gui -lQt5Core `pkg-config --libs /home/arek/opencv-2.4.9/release/bin/lib/pkgconfig/opencv.pc`

.PHONY: all clean

all: $(TARGETS)

recognizer: $(OBJ_DIR)/recognizer.o $(OBJ_DIR)/window.o $(OBJ_DIR)/matcher.o
	mkdir -p $(BIN_DIR)
	$(CXX) $^ $(LIBS) -o $(BIN_DIR)/$@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $@ -c $^

clean:
	rm -rf $(BIN_DIR)
	rm -rf $(OBJ_DIR)
