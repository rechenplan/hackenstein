#include "game.h"
#include "player.h"
#include "object.h"
#include "net.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/*

  create_object
  net_send_packet

*/

void game_init(game_t* game, player_t* local_player, net_t net, object_bank_t* objects) {
  game->local_player = local_player;
  game->net = net;
  game->objects = objects;
}

void game_cleanup(game_t* game) {
}

void game_map_load(game_t* game, map_t* map) {
}

void game_player_collide_with_object(game_t* game, player_t* player, object_t* object) {
}

void game_player_update(game_t* game, player_t* player) {
}

void game_key_down(game_t* game, int key) {
}

void game_key_up(game_t* game, int key) {
}

void game_player_init(game_t* game, player_t* player) {
}

void game_player_cleanup(game_t* game, player_t* player) {
}

void game_receive_packet(game_t* game, player_t* source, unsigned char* data, int data_len) {
}
