
#include "Game.h"

int main(int /*argc*/, char */*argv*/[]) {
    Game game;

    if (!game.initialize()) {
        return EXIT_FAILURE;
    }

    game.loop();
}
