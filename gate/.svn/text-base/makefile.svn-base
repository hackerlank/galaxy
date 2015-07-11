############################## DIRECTIONS ##############################

SOLUTION_DIR := ..
OBJ_DIR := ./obj
BIN_DIR := .

INC_DIRS := . ../common ../net ../game_def/ ..
SRC_DIRS := . ../common ../net

############################## OPTIONS ##############################

CC := g++
CCFLAGS := -O -g3
LDFLAGS := -lboost_system -lboost_thread -lboost_filesystem -lboost_date_time -lz
INCFLAGS := -I/usr/local/include -I/usr/local/include/mongo 

############################## TARGET ##############################

TARGET := gt

.PHONY : GATE
GATE : $(BIN_DIR)/$(TARGET)

############################## BUILD RULE ##############################

vpath %.h $(INC_DIRS)
vpath %.cpp $(SRC_DIRS)

SRCS := $(shell find $(SRC_DIRS) -name "*.cpp")
SRCS := $(foreach V,$(SRCS),$(shell readlink -f $(V)))
SOLUTION_DIR := $(shell readlink -f $(SOLUTION_DIR))
SRCS := $(subst $(SOLUTION_DIR),$(OBJ_DIR),$(SRCS))
SRCS := $(sort $(SRCS))
OBJS := $(SRCS:%.cpp=%.o)
DEPS := $(OBJS:%.o=%.d)
BASE_DIR := $(dir $(OBJS))
BASE_DIR := $(sort $(BASE_DIR))
INCFLAGS += $(addprefix -I,$(INC_DIRS))

$(BIN_DIR)/$(TARGET) : $(OBJS) 
	@echo Link..
	@$(CC) -o $(BIN_DIR)/$(TARGET) $^ $(LDFLAGS);
	@echo Build: $(TARGET)

$(OBJ_DIR)/%.d : $(SOLUTION_DIR)/%.cpp
	@set -e;
	@rm -f $@;
	@$(CC) -MM $< $(INCFLAGS) > $@.temp;
	@sed "s,^[^\ ].*\.o,$@ ,g" < $@.temp >> $@;
	@sed "s,^[^\ ].*\.o,$(subst .d,.o,$@) ,g" < $@.temp >> $@;
	@echo -e "\t"$(CC) -c $< -o $(subst .d,.o,$@) $(INCFLAGS) $(CCFLAGS) >> $@;
	@rm -f $@.temp;
	@echo Build: $@;

ifeq ($(MAKECMDGOALS), )
sinclude $(DEPS)
endif

.PHONY : info
info :
	@echo $(OBJS)
	@echo $(DEPS)
	@echo $(BASE_DIR)

.PHONY : dir
dir :
	@mkdir -p $(BASE_DIR) $(BIN_DIR)
	@echo Create: $(OBJ_DIR) $(BIN_DIR)

.PHONY : clean
clean :
	@rm -rf $(BIN_DIR)/$(TARGET) $(OBJ_DIR)
	@echo Clean: $(TARGET)	

############################## END ############################## 
