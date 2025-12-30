CXX = g++
CXXFLAGS = -Wall -std=c++17
TARGET = pangolin
SRC_DIR = src
OBJ_DIR = obj

SRC = $(SRC_DIR)/pangolin.cpp $(SRC_DIR)/repl.cpp
OBJ = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile .cpp → obj/*.o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)



.PHONY: all clean