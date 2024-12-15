# Define the compiler and flags
LDFLAGS =

# Check the operating system
ifeq ($(OS),Windows_NT)
    # Windows-specific flags
    LDFLAGS += -lws2_32
else
    # Linux-specific flags
    LDFLAGS +=
endif

tictactoe_mal: main_mal.o mal_enumerate.o mal_upload.o draw.o setup.o game.o tictactoe
	@echo "Building with flags $(LDFLAGS)"
	gcc main_mal.o mal_enumerate.o mal_upload.o game.o draw.o setup.o -o tictactoe_mal $(LDFLAGS)

main_mal.o: main_mal.c
	gcc -c main_mal.c -o main_mal.o

mal_enumerate.o: mal_enumerate.c mal_settings.h
	gcc -c mal_enumerate.c -o mal_enumerate.o

mal_upload.o: mal_upload.c
	gcc -c mal_upload.c -o mal_upload.o

tictactoe: main.o draw.o setup.o game.o
	gcc main.o game.o draw.o setup.o -o tictactoe

main.o: main.c game.h
	gcc -c main.c -o main.o

game.o: game.c game.h draw.h setup.h
	gcc -c game.c -o game.o

draw.o: draw.c draw.h game.h
	gcc -c draw.c -o draw.o

setup.o: setup.c setup.h
	gcc -c setup.c -o setup.o
