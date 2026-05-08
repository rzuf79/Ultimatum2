#include <stdio.h>
#include <string.h>
#include "resources.h"
#include "chao/chao.h"
#include "ultimatum/game.h"

int main() {
    chao_run("Ultimatum 2", U2_VIEW_WIDTH, U2_VIEW_HEIGHT, &game_init, &game_update);
    game_final();
    return 0;
}
