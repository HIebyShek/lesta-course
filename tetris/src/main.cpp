#include "menu_scene.hpp"
#include "tetramino.hpp"

#include <nano/engine.hpp>
#include <nano/error.hpp>
#include <nano/scene.hpp>
#include <nano/sound.hpp>
#include <nano/utils.hpp>

#include <imgui.h>

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>

extern "C" int
main()
{
    using namespace nano;
    auto&& e = engine::instance();
    int err_code = e->initialize(engine::flag::all);
    ASSERT_ERROR(err_code, "Fail while initialization of engine");

    auto tex = std::make_shared<texture2D>();
    tex->load("../assets/blue-J.png");
    nano::sprite block(tex);

    bool is_running{ true };
    auto time_point = std::chrono::system_clock::now();
    auto menu = std::make_shared<tetris::menu_scene>(is_running);
    e->scenarist.push(menu);

    auto program = std::make_shared<shader>();
    err_code = shaders::transform_texture(program);
    ASSERT_ERROR(err_code, "Fail while creating default shader");

    while (is_running)
    {
        event ev;
        while (poll_event(&ev))
        {
            e->supplier.deliver(ev);
        }

        auto cur_time_point = std::chrono::system_clock::now();
        if (e->scenarist.top())
        {
            e->scenarist.top()->process(cur_time_point - time_point);
        }
        time_point = cur_time_point;

        if (e->scenarist.top())
        {
            e->scenarist.top()->draw({ program });
        }

        engine::instance()->swap_buffers();
    }
    return EXIT_SUCCESS;
}