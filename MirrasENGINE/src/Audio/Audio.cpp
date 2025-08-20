#include "Audio/Audio.h"

#include "raylib/raudio.h"

namespace mirras
{
    struct raudioInstance
    {
        raudioInstance()  { InitAudioDevice(); }
        ~raudioInstance() { CloseAudioDevice(); }
    };

    Sound::Sound() = default;

    Sound::Sound(const fs::path& soundFilepath)
    {
        loadFrom(soundFilepath);
    }

    void Sound::loadFrom(const fs::path& soundFilepath)
    {
        static raudioInstance instance;

        if(sound)
            UnloadSound(*sound);

        sound = instantiate<::Sound>(LoadSound(soundFilepath.string().c_str()));
    }

    void Sound::play()
    {
        if(sound)
            PlaySound(*sound);
    }

    void Sound::pause()
    {
        if(sound)
            PauseSound(*sound);
    }

    void Sound::resume()
    {
        if(sound)
            ResumeSound(*sound);
    }

    void Sound::stop()
    {
        if(sound)
            StopSound(*sound);
    }

    void Sound::loop(bool enable)
    {
        if(sound)
            LoopSound(*sound, enable);
    }

    void Sound::setVolume(float volume)
    {
        if(sound)
            SetSoundVolume(*sound, volume);
    }

    void Sound::setPitch(float pitch)
    {
        if(sound)
            SetSoundPitch(*sound, pitch);
    }

    void Sound::setPan(float pan)
    {
        if(sound)
            SetSoundPan(*sound, pan);
    }

    bool Sound::isPlaying()
    {
        return sound ? IsSoundPlaying(*sound) : false;
    }

    bool Sound::isLooping()
    {
        return sound ? IsSoundLooping(*sound) : false;
    }

    bool Sound::isValid()
    {
        return sound ? IsSoundValid(*sound) : false;
    }

    Sound::~Sound()
    {
        if(sound)
            UnloadSound(*sound);
    }
} // namespace mirras
