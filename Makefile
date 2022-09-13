NAME = ircserv
TEST_NAME = $(NAME)_test

# Paths

SRC_PATH = ./src/
INC_PATH = ./inc/
OBJ_PATH = ./obj/
TEST_PATH = ./test/

# Names

SRC_NAME =	main.cpp\

TEST_SRC_NAME =	test_main.cpp \

TEST_HEADER_NAME =	testUtils.hpp \

# production code files
SRC = $(addprefix $(SRC_PATH),$(SRC_NAME))
INC_PARAMS=$(INC_PATH:%=-I%)
OBJ_NAME = $(SRC_NAME:.cpp=.o)
OBJ = $(addprefix $(OBJ_PATH),$(OBJ_NAME))
DEP_NAME := $(patsubst %.o,%.d,$(OBJ))

# test code files
TEST_SRC = $(addprefix $(TEST_PATH),$(TEST_SRC_NAME))
TEST_HEADER = $(addprefix $(TEST_PATH), $(TEST_HEADER_NAME))
TEST_OBJ_NAME = $(TEST_SRC_NAME:.cpp=.o)
TEST_OBJ = $(addprefix $(OBJ_PATH),$(TEST_OBJ_NAME))
TEST_DEP_NAME := $(patsubst %.o,%.d,$(TEST_OBJ))
OBJ_NO_MAIN := $(filter-out $(OBJ_PATH)main.o, $(OBJ))

# Flags
CXX = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -g
MAKE_DEP_FLAGS = -MMD
TEST_FLAGS	= -pthread -lgtest -std=c++98 -g

ifeq ($(MAKECMDGOALS),test)
	CPPFLAGS += -g -D TESTING
endif

all: $(NAME)

$(OBJ_PATH)%.o: $(SRC_PATH)%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(INC_PARAMS) $(MAKE_DEP_FLAGS) -c $< -o $@

$(OBJ_PATH)%.o: $(TEST_PATH)%.cpp
	@mkdir -p $(@D)
	$(CXX) -D TESTING -g -I $(TEST_PATH) $(INC_PARAMS) $(MAKE_DEP_FLAGS) -c $< -o $@

$(NAME): $(OBJ)
	@echo "Build $(NAME)"
	@$(CXX) $(CPPFLAGS) $(OBJ) -o $(NAME)

test: $(TEST_NAME)

$(TEST_NAME): $(OBJ_NO_MAIN) $(TEST_OBJ)
	$(CXX) -D TESTING $(OBJ_NO_MAIN) $(TEST_OBJ) $(TEST_FLAGS) $(INC_PARAMS) -I./$(TEST_PATH) $(TEST_FLAGS) -lm -o $(TEST_NAME)

clean:
	@echo "Delete $(OBJ_PATH)"
	@rm -rf $(OBJ_PATH) db

fclean:	clean
	@echo "Delete $(NAME)"
	@rm -f $(NAME) db

runTest: test
	./$(TEST_NAME)

-include $(DEP_NAME)
-include $(TEST_DEP_NAME)

re:	fclean all

.PHONY: all clean fclean re mlx libft db
