#ifndef ERROR_HPP
#define ERROR_HPP

#include <glad/glad.h>

#include <filesystem>
#include <iosfwd>
#include <string_view>

#if defined(NANO_DEBUG) || defined(DEBUG)
#define GL_CHECK(expr)                                                         \
    {                                                                          \
        (expr);                                                                \
        gl_check(__FILE__, __LINE__, #expr);                                   \
    }

#define LOG_DEBUG(...)                                                         \
    {                                                                          \
        /* fprintf(nano::err_os, "[DEBUG: %s: %d]:\n    ", __FILE__,           \
        __LINE__); fprintf(nano::err_os, __VA_ARGS__); */                      \
        nano::log_debug(__LINE__, __FILE__, __VA_ARGS__);                      \
    }

#else
#define GL_CHECK(expr) (expr);
#define LOG_DEBUG(...)
#endif

#define GL_ASSERT(expr)                                                        \
    {                                                                          \
        (expr);                                                                \
        if (EXIT_SUCCESS != gl_check(__FILE__, __LINE__, #expr))               \
        {                                                                      \
            return EXIT_FAILURE;                                               \
        }                                                                      \
    }

#define ASSERT_ERROR(err, ...)                                                 \
    {                                                                          \
        if (EXIT_SUCCESS != (err))                                             \
        {                                                                      \
            LOG_DEBUG(__VA_ARGS__);                                            \
            return EXIT_FAILURE;                                               \
        }                                                                      \
    }

#define ASSERT_SDL_ERROR(expr, ret)                                            \
    {                                                                          \
        if (!(expr))                                                           \
        {                                                                      \
            LOG_DEBUG("ASSERT: Internal SDL error: %s\n", SDL_GetError());     \
            return (ret);                                                      \
        }                                                                      \
    }

#define ASSERT_IMG_ERROR(expr, ret)                                            \
    {                                                                          \
        if (!(expr))                                                           \
        {                                                                      \
            LOG_DEBUG("Internal IMG error: %s\n", IMG_GetError());             \
            return (ret);                                                      \
        }                                                                      \
    }

#define TEST_SDL_ERROR(expr)                                                   \
    {                                                                          \
        if (!(expr))                                                           \
        {                                                                      \
            LOG_DEBUG("TEST: Internal SDL error: %s\n", SDL_GetError());       \
        }                                                                      \
    }

namespace nano
{

void log_debug(int line, const char* fn, const char* fmt, ...);
int gl_check(const std::filesystem::path& filename,
             unsigned int line,
             std::string_view expr);

} // namespace nano

#endif // ERROR_HPP
