windows:
	x86_64-w64-mingw32-gcc -c -O2 -Wall -pedantic -std=c89 src/main.c src/game.c src/hud.c src/map.c src/lfb.c src/sdl_render.c src/caster.c src/sdl_input.c src/player.c src/physics.c src/object.c src/client.c src/enet_net.c -I./SDL2-2.28.1/x86_64-w64-mingw32/include -I./enet-1.3.17/include -I./lua-5.4.6/src
	x86_64-w64-mingw32-gcc *.o -L./SDL2-2.28.1/x86_64-w64-mingw32/lib -lws2_32 -lwinmm -lSDL2 -lm -o bin/hackenstein.exe
	#x86_64-w64-mingw32-gcc -O2 -Wall -pedantic -std=c89 src/server.c -I./enet-1.3.17/include ./enet-1.3.17/callbacks.c ./enet-1.3.17/compress.c ./enet-1.3.17/host.c ./enet-1.3.17/list.c ./enet-1.3.17/packet.c ./enet-1.3.17/peer.c ./enet-1.3.17/protocol.c ./enet-1.3.17/win32.c -lws2_32 -lwinmm -lm -o bin/server.exe
libs:
	x86_64-w64-mingw32-gcc -c -O2 -Wall lua-5.4.6/src/*.c
	x86_64-w64-mingw32-gcc -c -O2 -Wall ./enet-1.3.17/callbacks.c ./enet-1.3.17/compress.c ./enet-1.3.17/host.c ./enet-1.3.17/list.c ./enet-1.3.17/packet.c ./enet-1.3.17/peer.c ./enet-1.3.17/protocol.c ./enet-1.3.17/win32.c -I./enet-1.3.17/include
