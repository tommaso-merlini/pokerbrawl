game: main.c json_reader.c json_reader.h map_loader.c map_loader.h map.h
	cc main.c json_reader.c map_loader.c -o game -Iraylib/src raylib/build/raylib/libraylib.a -lm -ldl -lpthread -lGL -lrt -lX11
