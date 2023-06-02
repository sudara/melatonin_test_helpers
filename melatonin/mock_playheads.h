#pragma once

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

        void setIsPlaying (bool isPlaying)
        {
            info.setIsPlaying (isPlaying);
        }

        void advancePosition (size_t numSamples)
        {
            auto samplesPerBeat = 44100.f * 60.f / info.getBpm().orFallback (120.f);
            info.setPpqPosition (info.getPpqPosition().orFallback(0) + (float) numSamples / samplesPerBeat);
        }

    private:
        PositionInfo info;
        double sampleRate = 44100.0;
    };
}
