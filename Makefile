SOURCES = main.c game_state.c player.c screens/menu.c screens/game.c json_reader.c map_loader.c
HEADERS = game_state.h player.h screens/menu.h screens/game.h json_reader.h map_loader.h map.h

game: $(SOURCES) $(HEADERS)
	cc $(SOURCES) -o game -Iraylib/src raylib/build/raylib/libraylib.a -lm -ldl -lpthread -lGL -lrt -lX11
