#include <nano/resource_loader.hpp>
#include <nano/soundbuf.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>

bool
operator==(const SDL_AudioSpec& lhs, const SDL_AudioSpec& rhs)
{
    // clang-format off
    return lhs.format == rhs.format &&
           lhs.channels == rhs.channels &&
           lhs.freq == rhs.freq &&
           lhs.silence == rhs.silence &&
           lhs.callback == rhs.callback;
    // clang-format on
}

std::ostream&
operator<<(std::ostream& o, const SDL_AudioSpec& spec)
{
    std::string tab(4, ' ');
    o << tab << "freq: " << spec.freq << '\n'
      << tab << "format: " << std::hex << spec.format << '\n'
      << tab << "channels: " << std::dec << int(spec.channels) << '\n'
      << tab << "silence: " << int(spec.silence) << '\n'
      << tab << "samples: " << spec.samples << '\n'
      << tab << "size: " << spec.size << '\n'
      << tab << "callback: " << reinterpret_cast<const void*>(spec.callback)
      << '\n'
      << tab << "userdata: " << spec.userdata << std::endl;
    return o;
}

#pragma pack(push, 1)
struct sound
{
    nano::soundbuf buf;
    size_t position = 0;
};
#pragma pack(pop)

void
audio_callback(void* userdata, std::uint8_t* stream, int len)
{
    auto buf = (sound*)userdata;
    std::fill_n(stream, len, buf->buf.spec().silence);
    int left = buf->buf.size() - buf->position;

    if (left > len)
    {
        SDL_MixAudioFormat(stream,
                           &buf->buf.data()[buf->position],
                           static_cast<SDL_AudioFormat>(buf->buf.spec().fmt),
                           len,
                           SDL_MIX_MAXVOLUME);
        buf->position += len;
    }
    else
    {
        SDL_MixAudioFormat(stream,
                           &buf->buf.data()[buf->position],
                           static_cast<SDL_AudioFormat>(buf->buf.spec().fmt),
                           left,
                           SDL_MIX_MAXVOLUME);
    }
}

int
main()
{
    int err = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO);
    if (EXIT_FAILURE == err)
    {
        SDL_Log("%s", SDL_GetError());
        return err;
    }

    // const char* fn = "../homework/06-and-god-create-sounds/road.wav";
    const char* fn = "../homework/06-and-god-create-sounds/highlands.wav";

    sound sbuf;
    std::ifstream file(fn, std::ios::binary);
    if (not file)
    {
        std::cerr << "ERROR: failed while opening file:\n\t" << fn << std::endl;
        SDL_Quit();
        return EXIT_FAILURE;
    }
    int err_code = nano::wav::load(file, sbuf.buf);
    if (EXIT_SUCCESS != err_code)
    {
        using namespace nano::wav;
        std::cerr << "ERROR: failed while loading file:\n\t" << fn << std::endl;
        std::cerr << error2str(err_code) << std::endl;
        SDL_Quit();
        return EXIT_FAILURE;
    }

    const nano::audio_spec& _spec = sbuf.buf.spec();
    SDL_AudioSpec obtained;
    SDL_AudioSpec spec{ .freq = static_cast<int>(_spec.frequence),
                        .format = static_cast<SDL_AudioFormat>(_spec.fmt),
                        .channels = static_cast<Uint8>(_spec.channel),
                        .silence = _spec.silence };
    spec.callback = audio_callback;
    spec.userdata = &sbuf;
    SDL_AudioDeviceID audio_device =
        SDL_OpenAudioDevice(nullptr, 0, &spec, &obtained, 0);
    if (0 == audio_device)
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO,
                     "Failed to open audio device: %s",
                     SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    if (obtained != spec)
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO,
                     "Obtained spec is not equal to desired");
        std::cerr << "obtained: \n" << obtained << std::endl;
        std::cerr << "desired: \n" << spec << std::endl;
        SDL_Quit();
        return EXIT_FAILURE;
    }

    if (EXIT_SUCCESS != SDL_PlayAudioDevice(audio_device))
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO,
                     "Failed to play audio device %d.\n%s",
                     audio_device,
                     SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    while (true)
    {
        char c;
        std::cin >> c;
        if ('q' == c)
            break;
    }

    SDL_Quit();
    return EXIT_SUCCESS;
}
