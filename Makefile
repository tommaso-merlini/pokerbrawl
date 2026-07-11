SOURCES = src/main.c src/game_state.c src/player.c src/ui.c src/screens/menu.c src/screens/game.c src/json_reader.c src/map_loader.c
HEADERS = src/game_state.h src/player.h src/character.h src/ui.h src/screens/menu.h src/screens/game.h src/json_reader.h src/map_loader.h src/map.h

game: $(SOURCES) $(HEADERS)
	cc $(SOURCES) -o game -Iraylib/src raylib/build/raylib/libraylib.a -lm -ldl -lpthread -lGL -lrt -lX11
