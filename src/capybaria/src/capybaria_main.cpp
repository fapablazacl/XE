
#include "Game.h"

#include <filesystem>

int main(int /*argc*/, char */*argv*/[]) {
#if defined(CAPYBARIA_ROOT_PATH)
    std::printf("Changing current work directory to %s\n", CAPYBARIA_ROOT_PATH);
    std::filesystem::current_path(CAPYBARIA_ROOT_PATH);
#endif

    Game game;

    if (!game.initialize()) {
        return EXIT_FAILURE;
    }

    game.loop();

    return 0;
}
