CC = $(CROSS_COMPILE)gcc
CFLAGS = -O3 -march=armv7-a -mcpu=cortex-a9 -mtune=cortex-a9 -mfpu=neon -mfloat-abi=hard
RESULT = ../vna

all: $(RESULT)

$(RESULT): vna.c
	$(CC) $(CFLAGS) -o $@ $^ -lm -lpthread

clean:
	rm -f $(RESULT)
