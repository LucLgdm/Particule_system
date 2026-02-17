# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/11/18 10:18:17 by lde-merc          #+#    #+#              #
#    Updated: 2026/02/16 15:36:12 by lde-merc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Compiler
CPP      = g++
FLAGS    = -MMD -g -std=c++17 -I includes/ -I$(HOME)/local/include -I includes/backends/
LDFLAGS  = -L/usr/lib/x86_64-linux-gnu -l:libOpenCL.so.1 -lglfw -lGL

# Sources
vpath %.cpp srcs/ srcs/imGui/
vpath %.c srcs/

SRC      = main.cpp Application.cpp ParticleSystem.cpp CameraFps.cpp ImGuiLayer.cpp \
		   CameraOrbit.cpp AxisGizmo.cpp \
		   imgui_draw.cpp imgui_tables.cpp imgui_widgets.cpp imgui_impl_glfw.cpp \
		   imgui_impl_opengl3.cpp imgui.cpp
SRCC     = glad.c

# Object directories
OBJDIR   = objs
OBJ      = $(SRC:%.cpp=$(OBJDIR)/%.o)
OBJGLAD  = $(OBJDIR)/glad.o
DEP      = $(OBJ:.o=.d)

# Target executable
NAME     = Particle_system


all: $(NAME) ## Build the project

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


clean: ## Clean object files
	@rm -rf $(OBJDIR)
	@echo "\033[34mDeleted object files!\033[0m"
	@rm imgui.ini || true 


fclean: clean ## Clean object files + executable
	@rm -f $(NAME)
	@echo "\033[35mDeleted everything!\033[0m"


re: fclean all ## Rebuild
	@echo "\033[33mRebuild done!\033[0m"


val: all ## Run with Valgrind
	valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=definite ./$(NAME) resources/42.obj || true


help: ## Display the help
	@awk 'BEGIN {FS = ":.*##"} \
		/^[a-zA-Z_-]+:.*##/ { \
			printf "  %-10s %s\n", $$1, $$2 \
		}' $(MAKEFILE_LIST)

# Include dependencies
-include $(DEP)

# Phony targets
.PHONY: all clean fclean re help val