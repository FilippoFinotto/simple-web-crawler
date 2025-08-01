CC = gcc
CFLAGS = -Wall -g -fopenmp
LDFLAGS = -lm -fopenmp
SRC = crawler.c hashtable.c http.c parser.c queue.c parser.c queue.c
OBJ = $(SRC:.c=.o)
DEPS = $(SRC:.c=.d)
EXEC = myprogram

$(EXEC): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS) 

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.d: %.c
	$(CC) -MM $(CFLAGS) $< > $@

-include $(DEPS)

clean:
	rm -f $(OBJ) $(EXEC) $(DEPS)
