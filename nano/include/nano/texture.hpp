#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <nano/canvas.hpp>
#include <nano/vec.hpp>

#include <cstdint>
#include <filesystem>

namespace nano
{

class texture2D
{
public:
    texture2D() = default;
    texture2D(const canvas&);

    texture2D(texture2D&&) noexcept;
    texture2D& operator=(texture2D&&) noexcept;

    ~texture2D();

    texture2D(const texture2D&) = delete;
    texture2D& operator=(const texture2D&) = delete;

    int load(const canvas&);
    int load(const std::filesystem::path&);

    vec2s size() const;
    std::size_t width() const;
    std::size_t height() const;

    void set_size(vec2s);
    void set_size(vec2s::type, vec2s::type);

    static void remove(const texture2D& t);
    static bool exist(const texture2D& t);

private:
    std::uint32_t m_handle{ 0 };
    vec2s m_size{ 0, 0 };
};

} // namespace nano

#endif // TEXTURE_HPP
