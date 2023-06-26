#include "game_scene.hpp"

#include "tetramino.hpp"

#include <nano/engine.hpp>
#include <nano/error.hpp>

#include <algorithm>
#include <chrono>
#include <climits>
#include <cstdlib>
#include <functional>
#include <memory>
#include <vector>

namespace tetris
{

game_scene::game_scene(float p_width, bool& p_is_running)
    : is_running(p_is_running)
    , node()
{
    int err_code = bg_beat.load(nano::engine::instance()->assets_path() /
                                "8bit-music.wav");
    if (EXIT_SUCCESS != err_code)
    {
        LOG_DEBUG("Fail while loading background sound for game scene");
    }
    else
    {
        bg_beat.volume(50);
        bg_beat.loop = true;
    }

    pixels_size = { p_width, p_width * height / width };
    block_size = pixels_size / nano::vec2f{ width, height };
    pixels_size_visible = pixels_size;
    pixels_size_visible.y -= 4 * block_size.y;

    subscribe_on_events();
}

void
game_scene::subscribe_on_events() const
{
    auto&& e = nano::engine::instance();
    using ev_t = nano::event::type_t;
    nano::event ev;
    ev.type = ev_t::quit;
    e->supplier.subscribe({ ev, id }, [this]() { this->is_running = false; });

    ev.type = ev_t::window_close_request;
    e->supplier.subscribe({ ev, id }, [this]() { this->is_running = false; });

    ev.type = ev_t::key_down;
    ev.kb.repeat = false;
    ev.kb.key.keycode = nano::keycode_t::kb_d;
    e->supplier.subscribe({ ev, id }, [this]() { rshift_falling(); });

    ev.kb.repeat = true;
    ev.kb.key.keycode = nano::keycode_t::kb_d;
    e->supplier.subscribe({ ev, id }, [this]() { rshift_falling(); });

    ev.kb.repeat = false;
    ev.kb.key.keycode = nano::keycode_t::kb_a;
    e->supplier.subscribe({ ev, id }, [this]() { lshift_falling(); });

    ev.kb.repeat = true;
    ev.kb.key.keycode = nano::keycode_t::kb_a;
    e->supplier.subscribe({ ev, id }, [this]() { lshift_falling(); });

    ev.kb.repeat = false;
    ev.kb.key.keycode = nano::keycode_t::kb_h;
    e->supplier.subscribe({ ev, id }, [this]() { rot270_falling(); });

    ev.kb.key.keycode = nano::keycode_t::kb_l;
    e->supplier.subscribe({ ev, id }, [this]() { rot90_falling(); });

    ev.kb.key.keycode = nano::keycode_t::kb_j;
    ev.kb.repeat = false;
    e->supplier.subscribe({ ev, id }, [this]() { shift_down(); });

    ev.kb.key.keycode = nano::keycode_t::kb_j;
    ev.kb.repeat = true;
    e->supplier.subscribe({ ev, id }, [this]() { shift_down(); });
}

void
game_scene::add(std::shared_ptr<tetramino> block)
{
    falling = block;
    block->start();
}

void
game_scene::start()
{
    add(std::make_shared<tetramino>(tetramino::random_type(), block_size));
    bg_beat.play();
}

void
game_scene::stop()
{
    bg_beat.stop();
}

void
game_scene::pause()
{
    bg_beat.pause();
}

void
game_scene::resume()
{
    bg_beat.play();
}

void
game_scene::rshift_falling() const
{
    if (not falling)
        return;

    falling->xshift(1);
    if (is_collide(falling))
    {
        falling->xshift(-1);
        return;
    }

    for (auto&& block : blocks)
    {
        if (is_collide(falling, block))
        {
            falling->xshift(-1);
            return;
        }
    }
}

void
game_scene::lshift_falling() const
{
    if (not falling)
    {
        return;
    }

    falling->xshift(-1);
    if (is_collide(falling))
    {
        falling->xshift(1);
        return;
    }

    for (auto&& block : blocks)
    {
        if (is_collide(falling, block))
        {
            falling->xshift(1);
            return;
        }
    }
}

void
game_scene::shift_down() const
{
    if (not falling)
    {
        return;
    }

    delay += max_delay;
}

void
game_scene::rot90_falling() const
{
    if (not falling)
    {
        return;
    }

    falling->rot90();
    if (is_collide(falling))
    {
        return;
    }

    for (auto&& block : blocks)
    {
        if (is_collide(falling, block))
        {
            falling->rot270();
            return;
        }
    }
}

void
game_scene::rot270_falling() const
{
    if (not falling)
    {
        return;
    }

    falling->rot270();
    if (is_collide(falling))
    {
        falling->rot90();
        return;
    }

    for (auto&& block : blocks)
    {
        if (is_collide(falling, block))
        {
            falling->rot90();
            return;
        }
    }
}

bool
game_scene::is_game_over() const
{
    for (auto&& block : blocks)
    {
        for (auto&& [_, y] : block->positions())
        {
            if (y >= height_visible)
            {
                return true;
            }
        }
    }

    return false;
}

void
game_scene::delete_full_rows()
{
    for (int i{ 0 }; i < height_visible; ++i)
    {
        if (is_full_row(i))
        {
            delete_row(i);
            --i;
        }
    }
}

void
game_scene::delete_row(const int row)
{
    for (auto&& block : blocks)
    {
        const auto v_positions = block->positions();
        for (const auto pos : v_positions)
        {
            if (pos.y == row)
            {
                block->remove(pos);
            }
        }
    }
    score += 10;
    using namespace std::chrono_literals;
    max_delay -= max_delay > 100ms ? 100ms : 0ms;
    shift_down_all_higher(row);
}

void
game_scene::shift_down_all_higher(const int row)
{
    if (row < 0 or row > height)
    {
        return;
    }

    for (auto&& block : blocks)
    {
        for (auto& [x, y] : block->positions())
        {
            if (y > row)
            {
                --y;
            }
        }
    }
}

bool
game_scene::is_full_row(const int row) const
{
    if (row < 0 or row > height_visible)
    {
        return false;
    }

    int real_row_sum{ 0 };
    static int max_row_sum = ariphmetic_progression_sum(1, width - 1, 1);

    bool is_full_0Y{ false };
    for (auto&& block : blocks)
    {
        for (auto [x, y] : block->positions())
        {
            if (y == row)
            {
                if (0 == x)
                {
                    is_full_0Y = true;
                }
                else
                {
                    real_row_sum += x;
                }
            }
        }
    }

    return is_full_0Y and real_row_sum == max_row_sum;
}

int
game_scene::ariphmetic_progression_sum(const int begin,
                                       const int end,
                                       const int step)
{
    return (begin + end) / 2 * (end - begin + 1) / step;
}

void
game_scene::lock_falling()
{
    falling->lock();
    blocks.push_back(falling);
}

void
game_scene::process(delta_t delta)
{
    delay += delta;

    if (delay < max_delay)
    {
        return;
    }

    delay -= max_delay;

    falling->process(delta);

    bool is_collision{ false };
    if (is_collide(falling))
    {
        is_collision = true;
        falling->yshift(1);
        lock_falling();
    }
    else
    {
        for (auto&& block : blocks)
        {
            if (is_collide(falling, block))
            {
                is_collision = true;
                falling->yshift(1);
                lock_falling();
                break;
            }
        }
    }

    if (is_collision)
    {
        if (is_game_over())
        {
            nano::engine::instance()->scenarist.pop();
            return;
        }

        delete_full_rows();

        add(std::make_shared<tetramino>(tetramino::random_type(), block_size));
    }
}

void
game_scene::draw(nano::drawable::state s) const
{
    auto&& win = nano::engine::instance()->window;
    nano::transform2D tr;
    tr.move(-pixels_size_visible / win.size / nano::vec2f{ 1, win.ratio });
    s.transform.combine(tr);

    falling->draw(s);
    using namespace std::placeholders;
    std::for_each(
        begin(blocks), end(blocks), std::bind(&tetramino::draw, _1, s));
}

} // namespace tetris
