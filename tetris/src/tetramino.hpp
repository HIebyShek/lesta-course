#ifndef TETRAMINO_HPP
#define TETRAMINO_HPP

#include <nano/scene.hpp>
#include <nano/sprite.hpp>

#include <vector>

namespace tetris
{

class tetramino : public nano::node
{
public:
    enum class type
    {
        I,
        O,
        T,
        L,
        J,
        S,
        Z
    };
    static type random_type();

    enum color
    {
        blue,
        yellow,
        red,
        green
    };

    enum class state
    {
        falling,
        locked,
    };

    tetramino(type, color, nano::vec2f);

    void start() override;
    void stop() override;

    void pause() override;
    void resume() override;
    void process(delta_t) override;
    void lock();

    void rotate90();
    void rotate270();

    void rshift();
    void lshift();

    void draw(nano::drawable::state) const override;

    static constexpr std::size_t max_positions_num = 4;
    std::vector<nano::vec2i> positions() const;

private:
    mutable nano::sprite block;
    type t;
    state m_state;
    std::vector<nano::vec2i> blocks_positions{};
};

} // namespace tetris

#endif // TETRAMINO_HPP