#include <nano/event.hpp>

#include <SDL3/SDL_events.h>

#include <algorithm>
#include <chrono>
#include <ratio>

#include <imgui_impl_sdl3.h>

namespace nano
{

void
convert_sdl_event(SDL_Event* sdl_ev, event* ev)
{
    if (not sdl_ev or not ev)
        return;

    using ev_t = event::type;
    ev->t = static_cast<ev_t>(sdl_ev->type);
    ev->kb.timestamp = event::timestamp_t(sdl_ev->key.timestamp);
    switch (ev->t)
    {
    case ev_t::key_down:
    case ev_t::key_up:
        ev->kb.key.mod = sdl_ev->key.keysym.mod;
        ev->kb.key.scancode =
            static_cast<scancode_t>(sdl_ev->key.keysym.scancode);
        ev->kb.key.keycode = static_cast<keycode_t>(sdl_ev->key.keysym.sym);
        ev->kb.repeat = sdl_ev->key.repeat;
        break;

    case ev_t::mouse_key_down:
    case ev_t::mouse_key_up:
        ev->mouse.mouseID = sdl_ev->button.which;
        ev->mouse.button =
            static_cast<event::mouse_button::button_t>(sdl_ev->button.button);
        ev->mouse.clicks = sdl_ev->button.clicks;
        ev->mouse.state =
            static_cast<event::button_state>(sdl_ev->button.state);
        ev->mouse.x = sdl_ev->button.x;
        ev->mouse.y = sdl_ev->button.y;
        break;

    case ev_t::mouse_motion:
        ev->motion.state =
            static_cast<event::button_state>(sdl_ev->motion.state);
        ev->motion.mouseID = sdl_ev->motion.which;
        ev->motion.windowID = sdl_ev->motion.windowID;
        ev->motion.x = sdl_ev->motion.x;
        ev->motion.y = sdl_ev->motion.y;
        ev->motion.xrel = sdl_ev->motion.xrel;
        ev->motion.yrel = sdl_ev->motion.yrel;
        break;

    case ev_t::mouse_wheel:
        ev->wheel.mouseID = sdl_ev->wheel.which;
        ev->wheel.direction = static_cast<event::mouse_wheel::direction_t>(
            sdl_ev->wheel.direction);
        ev->wheel.x = sdl_ev->wheel.x;
        ev->wheel.y = sdl_ev->wheel.y;
        ev->wheel.mouseX = sdl_ev->wheel.mouseX;
        ev->wheel.mouseY = sdl_ev->wheel.mouseY;
        break;

    case ev_t::text_edit:
        std::ranges::copy(sdl_ev->edit.text, ev->edit.text);
        ev->edit.length = sdl_ev->edit.length;
        ev->edit.start = sdl_ev->edit.start;
        ev->edit.windowID = sdl_ev->edit.windowID;
        break;

    case ev_t::text_input:
        std::ranges::copy(sdl_ev->text.text, ev->input.text);
        ev->input.windowID = sdl_ev->text.windowID;
        break;

    case ev_t::quit:
        break;

    case ev_t::window_close_request:
    case ev_t::window_expose:
    case ev_t::window_focus_gain:
    case ev_t::window_focus_lost:
    case ev_t::window_hide:
    case ev_t::window_show:
    case ev_t::window_maxmimize:
    case ev_t::window_minimize:
    case ev_t::window_mouse_enter:
    case ev_t::window_mouse_leave:
    case ev_t::window_move:
    case ev_t::window_resize:
    case ev_t::window_restore:
    case ev_t::window_pixel_size_change:
        ev->win.windowID = sdl_ev->window.windowID;
        break;

    default:
        ev->t = ev_t::unknown;
        break;
    }
}

int
poll_event(event* ev)
{
    if (nullptr == ev)
        return 0;

    static SDL_Event sdl_ev;
    int result = SDL_PollEvent(&sdl_ev);
    if (SDL_FALSE == result)
        return 0;

    convert_sdl_event(&sdl_ev, ev);
    ImGui_ImplSDL3_ProcessEvent(&sdl_ev);

    return 1;
}

} // namespace nano
