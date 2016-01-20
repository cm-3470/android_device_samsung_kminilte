OBJ = audio_config.o converter.o
CFLAGS  = -Wall -g

%.o: %.c
	$(CC) $(CFLAGS) -c $<

.PHONY: all
all: converter

converter: $(OBJ)
	$(CC) $(CFLAGS) -o converter $(OBJ)

.PHONY: clean
clean:
	rm -f converter $(OBJ)
