test:
	x86_64-w64-mingw32-gcc -O2 -Wall -pedantic -std=c89 src/main.c src/map.c src/lfb.c src/sdl_render.c src/caster.c src/sdl_input.c src/player.c src/phy.c src/sprite.c src/weapon.c src/game.c src/enet_net.c -L./SDL2-2.28.1/x86_64-w64-mingw32/lib -I./SDL2-2.28.1/x86_64-w64-mingw32/include -lSDL2 -lm -o bin/hackenstein.exe
	bin/hackenstein.exe
clean:
	rm -f bin/test.exe
