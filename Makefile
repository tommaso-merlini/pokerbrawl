game: main.c
	cc main.c -o game -Iraylib/src raylib/build/raylib/libraylib.a -lm -ldl -lpthread -lGL -lrt -lX11
