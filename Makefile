# CC = gcc
# CFLAGS = -I/opt/homebrew/include -L/opt/homebrew/lib -lraylib -lm -lpthread
# TARGET = main
# SRCS = main.c client.c server.c
#
# all: main client server
#
# main: $(SRCS) 
# 	$(CC) $(SRCS) -o main $(CFLAGS)
#
# client: $(SRCS) 
# 	$(CC) $(SRCS) -o client $(CFLAGS)
#
# server: $(SRCS) 
# 	$(CC) $(SRCS) -o server $(CFLAGS)
#
# # $(TARGET): main.c client.c server.c
# # 	$(CC) main.c client.c server.c -o $(TARGET) $(CFLAGS)
#
# clean:
# 	rm -f main client server
CC = gcc
CFLAGS = -I/opt/homebrew/include -L/opt/homebrew/lib -lraylib -lm -lpthread
SRCS = main.c client.c server.c
TARGET = main

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(SRCS) -o $(TARGET) $(CFLAGS)

clean:
	rm -f $(TARGET)
