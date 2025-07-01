#pragma once

#include "Core/Types/Reference.h"

#include <filesystem_fs>

struct Sound;
struct Music;

namespace mirras
{
    class Sound
    {
    public:
        Sound();
        Sound(const fs::path& soundFilepath);

        void loadFrom(const fs::path& soundFilepath);
        void play();
        void pause();
        void resume();
        void stop();

        void loop(bool enable);

        void setVolume(float volume);
        void setPitch(float pitch);
        void setPan(float pan);

        bool isPlaying();
        bool isLooping();
        bool isValid();

        ~Sound();

    private:
        single_ref<::Sound> sound;
    };


    class Music
    {
        Music() = default;
        Music(const fs::path& musicFilepath);

        void loadFrom(const fs::path& musicFilepath);
        void play();
        void pause();
        void resume();
        void stop();

        bool isPlaying();
        bool isValid();

        void seek(float position);

        ~Music();

    private:
        single_ref<::Music> music;
    };
} // namespace mirras
