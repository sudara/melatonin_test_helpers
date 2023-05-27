#pragma once

namespace melatonin
{
    struct isBetween : Catch::Matchers::MatcherGenericBase
    {
        float min;
        float max;
        float margin;

        isBetween (float m, float x, float g = std::numeric_limits<float>::epsilon() * 100) : min (m), max (x), margin (g) {}

        template <typename SampleType>
        [[nodiscard]] bool match (std::vector<SampleType> array) const
        {
            jassert (min < max);

            for (auto& item : array)
                if (item < (min - margin) || item > (max + margin))
                    return false;
            return true;
        }

        template <typename SampleType>
        [[nodiscard]] bool match (juce::dsp::AudioBlock<SampleType>& block) const
        {
            jassert (min < max);

            for (size_t i = 0; i < block.getNumSamples(); ++i)
                if (block.getSample (0, i) < min - margin || block.getSample (0, i) > max + margin)
                    return false;
            return true;
        }

        [[nodiscard]] std::string describe() const override
        {
            std::ostringstream ss;
            ss << "\n items are between "
               << min << " and " << max << "\n";
            return ss.str();
        }
    };
}
