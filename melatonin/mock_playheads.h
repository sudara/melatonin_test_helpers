#pragma once
#include <juce_audio_basics/juce_audio_basics.h>

namespace melatonin
{
    class NoPlayhead : public juce::AudioPlayHead
    {
    public:
        [[nodiscard]] juce::Optional<PositionInfo> getPosition() const override
        {
            return {};
        }
    };

    class PlayingPlayhead : public juce::AudioPlayHead
    {
    public:
        PlayingPlayhead()
        {
            info.setIsPlaying (true);
        }

        void setTempo (double bpm)
        {
            info.setBpm (bpm);
        }

        [[nodiscard]] juce::Optional<PositionInfo> getPosition() const override
        {
            return info;
        }

    private:
        PositionInfo info;
    };
}
