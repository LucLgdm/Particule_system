# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/11/18 10:18:17 by lde-merc          #+#    #+#              #
#    Updated: 2025/12/18 11:03:44 by lde-merc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Compiler
CPP      = g++
FLAGS    = -MMD -g -std=c++17 -I includes/ -I$(HOME)/local/include
LDFLAGS  = -L/usr/lib/x86_64-linux-gnu -l:libOpenCL.so.1 -lglfw -lGL

# Sources
vpath %.cpp srcs/
vpath %.c srcs/

SRC      = main.cpp Application.cpp ParticleSystem.cpp
SRCC     = glad.c

# Object directories
OBJDIR   = objs
OBJ      = $(SRC:%.cpp=$(OBJDIR)/%.o)
OBJGLAD  = $(OBJDIR)/glad.o
DEP      = $(OBJ:.o=.d)

# Target executable
NAME     = Particule_system

# Default target
all: $(NAME)

# Link
$(NAME): $(OBJ) $(OBJGLAD)
	@$(CPP) $(OBJ) $(OBJGLAD) $(LDFLAGS) -o $@
	@echo "\033[32m$(NAME) created!\033[0m"

# Compile C++ sources
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	@$(CPP) $(FLAGS) -c $< -o $@

# Compile GLAD (C source)
$(OBJDIR)/glad.o: $(SRCC)
	@mkdir -p $(@D)
	@$(CPP) $(FLAGS) -c $< -o $@

# Clean object files
clean:
	@rm -rf $(OBJDIR)
	@echo "\033[34mDeleted object files!\033[0m"

# Clean object files + executable
fclean: clean
	@rm -f $(NAME)
	@echo "\033[35mDeleted everything!\033[0m"

# Rebuild
re: fclean all
	@echo "\033[33mRebuild done!\033[0m"

val: all
	valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=definite ./$(NAME) resources/42.obj || true

# Include dependencies
-include $(DEP)

# Phony targets
.PHONY: all clean fclean re