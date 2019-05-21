EXEC=controller
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)
BUILD_OBJ=$(patsubst %,$(BUILD_DIR)/%,$(OBJ))


all: $(EXEC)


$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -lpthread $(BUILD_OBJ) -o $(BUILD_DIR)/$@


%.o: %.c
	$(CC) $(CFLAGS) -c -o $(BUILD_DIR)/$@ $<

clean:
	rm -f *.o $(EXEC)
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/$(EXEC)
