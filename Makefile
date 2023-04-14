
# Compilation
NAME		=	Scop
CC			=	g++
FLAGS		=	-std=c++14 -Wall -Wextra -g3
HEADER		=	-I$(PATH_INC)

# Color Code and template code
GREY = \033[2:49;39m
RED = \033[1;31m
GREEN = \033[3;32m
YELLOW = \033[3;49;33m
BLUE = \033[3;4;34m
BLUEE = \033[3;34m
PURPLE = \033[3;35m
CYAN = \033[3;36m
RESET = \033[0m
ICONOK = \033[1;32m
ICONKO = \033[1;31m
CO_DELET = \033[3;31m
CO_A_OUT = \033[96m
WHITE = \033[1;37m


# Path directories
PATH_INC	=	include
PATH_SRC	=	src
PATH_OBJ	=	obj

# Source
SRC			=	$(addprefix $(PATH_SRC)/, main.cxx \
				obj.cxx \
				rendering.cxx \
				)

OBJ			=	$(addprefix $(PATH_OBJ)/, $(SRC:.cxx=.o))

# Headers
INC_FILES	= $(wildcard $(PATH_INC)/*.hpp)

INC			=	$(INC_FILES)

# Rules
all			:	$(NAME)

$(NAME)	:	$(OBJ) $(INC)
	@ $(CC) $(FLAGS) $(HEADER) $(OBJ) -o $(NAME) -lglfw -lGL -lGLEW
	@ printf "\033[2K\r$(BLUE)$(NAME)$(RESET)$(BLUEE): $(ICONOK)Compiled [√]$(RESET)\n"

$(PATH_OBJ)/%.o		: 	%.cxx    $(INC)
	@ mkdir -p $(dir $@)
	@ printf "\033[2K\r$(PURPLE)$<: $(CYAN)loading..$(RESET)"
	@ $(CC) $(FLAGS) $(HEADER) -c  -o $@ $<

clean			:
				@ rm -rf ${obj}
				@ rm -rf obj

fclean	:	clean
	@ rm -rf $(NAME)
	@ printf "$(CYAN)'$(NAME)'$(RESET) and all .o has been $(RED)deleted. 🗑️\n$(RESET)"

re		:	fclean all

run : all
	 ./$(NAME)

help	:
	@ printf "$(RESET)Welcom to Scop a project of graphic computing. \n$(RESET)"
	@ printf "$(RESET)\nCommand of the Makefile : \n\n$(RESET)"
	@ printf "$(RESET) - clean :        clean objs \n$(RESET)"
	@ printf "$(RESET) - fclean :       clean exec and objs\n$(RESET)"
	@ printf "$(RESET) - re :           clean and recompile\n$(RESET)"

.PHONY: all compil clean fclean re leaks run help
