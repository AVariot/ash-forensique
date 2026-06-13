NAME	= getLostData
LNAME	= liblostdata.so

CC	= cc
CFLAGS	= -Wall -Wextra -Werror -fPIC -g

SRCS	= $(shell find . -name '*.c')
OBJS	= $(SRCS:.c=.o)

INCS	= $(addprefix -I, $(sort $(dir $(shell find . -name '*.h'))))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) -lcapstone

lib: $(LNAME)

$(LNAME): $(OBJS)
	$(CC) -shared $(OBJS) -o $(LNAME) -lcapstone

# add -s to strip the exe / elf
%.o: %.c
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME) $(LNAME)

re: fclean all

.PHONY: all lib clean fclean re
