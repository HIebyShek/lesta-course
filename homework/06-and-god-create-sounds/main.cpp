#include <nano/engine.hpp>
#include <nano/error.hpp>
#include <nano/resource_loader.hpp>
#include <nano/sound.hpp>
#include <nano/soundbuf.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

#define TEST_ERROR(err, msg)                                                   \
    {                                                                          \
        if (EXIT_FAILURE == (err))                                             \
        {                                                                      \
            LOG_DEBUG((msg));                                                  \
            return EXIT_FAILURE;                                               \
        }                                                                      \
        err_code = EXIT_FAILURE;                                               \
    }

int
main()
{
    nano::engine& eng = nano::engine::instance();
    int err_code = eng.initialize(nano::engine::flag::audio);
    TEST_ERROR(err_code, "Fail while initialising engine");

    std::filesystem::path fn(
        "../homework/06-and-god-create-sounds/highlands.wav");
    nano::sound music;
    err_code = music.load(fn);
    TEST_ERROR(err_code, "Fail while loading sound from: " + fn.string());

    std::filesystem::path fn2("../homework/06-and-god-create-sounds/road.wav");
    nano::sound road_music;
    err_code = road_music.load(fn2);
    TEST_ERROR(err_code, "Fail while loading sound from: " + fn2.string());

    music.volume(50);
    road_music.volume(50);
    music.play();

    using namespace std::chrono_literals;
    bool is_running{ true };
    while (is_running)
    {
        char c;
        std::cin >> c;
        switch (c)
        {
        case 'r':
            road_music.toggle();
            break;

        case 'h':
            music.toggle();
            break;

        case 'q':
            is_running = false;
            break;

        case 's':
            music.stop();
            road_music.stop();
            break;

        default:
            break;
        }
    }

    eng.finalize();
    return EXIT_SUCCESS;
}
