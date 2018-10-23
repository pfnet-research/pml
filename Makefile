TARGET_NAME = pml
CXX = clang++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2

LDFLAGS =
LIBS =
INCLUDE = -I./include

TEST_TARGET_NAME = test
TESTFLAGS = -DPML_TEST_BUILD
TEST_TARGET = $(BUILD_DIR)/$(TEST_TARGET_NAME)

SRCDIR = ./src
SRC = $(wildcard $(SRCDIR)/*.cpp)

BUILD_DIR = ./build
TARGET = $(BUILD_DIR)/$(TARGET_NAME)

OBJ = $(addprefix $(BUILD_DIR)/obj/, $(notdir $(SRC:.cpp=.o)))
DEPEND = $(OBJ:.o=.d)

.PHONY: all
all: $(TARGET)

-include $(DEPEND)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS) $(LIBS)

$(BUILD_DIR)/obj/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE)  -o $@ -c -MMD -MP $<

.PHONY: test
test:
	$(CXX) $(CXXFLAGS) $(TESTFLAGS) $(INCLUDE) $(SRC) -o $(TEST_TARGET) $(TESTFLAGS)
	$(TEST_TARGET)

.PHONY: clean
clean:
	-rm -f $(OBJ) $(DEPEND) $(TARGET)

.PHONY: run
run: $(TARGET)
	$(TARGET)

