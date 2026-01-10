CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -MMD -MP
LDFLAGS = -lreadline -lncurses
TARGET = pangolin

SRC_DIR = src
OBJ_DIR = obj

SRC = $(SRC_DIR)/pangolin.cpp \
      $(SRC_DIR)/repl.cpp \
      $(SRC_DIR)/commands.cpp

OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEP = $(OBJ:.o=.d)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEP)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: clean