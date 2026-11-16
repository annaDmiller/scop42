NAME = scop

INCLUDES_DIR = ./includes

SRCS_DIR = ./srcs

MATH_DIR = ./math

SRCS_FILES = main.cpp window_utils.cpp Shader.cpp

SRCS = 

MATH_FILES = Mat4.cpp Vec3.cpp

FLAGS = -std=c++17

all: $(NAME)

$(NAME): $(SRCS) $(INCLUDES_DIR)

clean:

fclean: clean

re: fclean all

.PHONY all clean fclean re