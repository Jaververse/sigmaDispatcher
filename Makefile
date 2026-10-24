CC = gcc
# Añadimos -pthread obligatoriamente para los hilos
CFLAGS = -Wall -Wextra -Iinclude -g -pthread 

SRCS = main.c $(wildcard src/*.c)
OBJ_DIR = obj
OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(SRCS)))

TARGET = sentinel-dispatcher

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Regla para compilar main.c
$(OBJ_DIR)/main.o: main.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Regla para compilar los archivos de src (incluyendo tus nuevos sync.c y threads.c)
$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)