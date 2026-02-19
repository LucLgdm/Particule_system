# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/11/18 10:18:17 by lde-merc          #+#    #+#              #
#    Updated: 2026/02/19 17:12:24 by lde-merc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Compiler
CPP      = g++
FLAGS    = -MMD -g -std=c++17 -I includes/ -I includes/backends/
LDFLAGS     = -lGL -lGLU -lglfw -lGLEW -lOpenCL

# Docker
COMPOSE     = docker compose
SERVICE     = builder

# Sources
vpath %.cpp srcs/ srcs/imGui/
vpath %.c srcs/

SRC      = $(wildcard srcs/*.cpp srcs/imGui/*.cpp)
SRCC     = glad.c

# Object directories
OBJDIR   = objs
OBJ      = $(SRC:%.cpp=$(OBJDIR)/%.o)
OBJGLAD  = $(OBJDIR)/glad.o
DEP      = $(OBJ:.o=.d)

# Target executable
NAME     = Particle_system

# ─── Regles HOST (lancent le docker) ─────────────────────────────────────────

all: ## Build the project
	$(COMPOSE) run --rm $(SERVICE) make _build

clean: ## Clean object files via docker
	$(COMPOSE) run --rm $(SERVICE) make _clean


fclean: clean ## Clean object files and executable and docker volumes
	@rm -f $(NAME)
	$(COMPOSE) run --rm $(SERVICE) make _clean
	$(COMPOSE) down --volumes --rmi local
	$(COMPOSE) rm -f
	@echo "\033[35mDeleted everything!\033[0m"
	
re: fclean all ## Rebuild
	@echo "\033[33mRebuild done!\033[0m"


val: all ## Run with Valgrind
	valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=definite \
		./$(NAME) resources/42.obj || true


help: ## Display the help
	@awk 'BEGIN {FS = ":.*##"} \
		/^[a-zA-Z_-]+:.*##/ { \
			printf "\033[34m  %-10s\033[0m \033[32m%s\033[0m\n", $$1, $$2 \
		}' $(MAKEFILE_LIST)

# ─── Regles INTERNES (appelees dans le container) ────────────────────────────

_build: $(NAME) ## Compile and link in the container

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

_clean: ## Clean object files in the container
	@rm -rf $(OBJDIR)
	@rm imgui.ini || true 
	@echo "\033[34mDeleted object files!\033[0m"

_fclean: _clean ## Clean object and executable in the container
	@rm -f $(NAME)
	
# Include dependencies
-include $(DEP)

# Phony targets
.PHONY: all clean fclean re help val _build _clean _fclean