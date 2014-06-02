#CC        := g++ -I /usr/include/boost -Wall -std=c++0x
#LD        := g++ -I /usr/include/boost -std=c++0x

CC        := g++ -std=c++11 -Wall
LD        := g++ -std=c++11

MODULES   := main
SRC_DIR   := $(addprefix src/,$(MODULES))
BUILD_DIR := $(addprefix build/,$(MODULES))

SRC       := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
OBJ       := $(patsubst src/%.cpp,build/%.o,$(SRC))
INCLUDES  := $(addprefix -I,$(SRC_DIR))

vpath %.cpp $(SRC_DIR)

define make-goal
$1/%.o: %.cpp
	$(CC) $(INCLUDES) -c $$< -o $$@ 
endef

.PHONY: all checkdirs clean

all: checkdirs build/test

build/test: $(OBJ)
	$(LD) $^ -o $@ 


checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))
