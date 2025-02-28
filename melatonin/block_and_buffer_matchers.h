#pragma once
#include "melatonin_audio_sparklines/melatonin_audio_sparklines.h"

// This teaches Catch how to convert an AudioBlock to a string
// This allows us to print out detail about the AudioBlock on matcher failure
namespace Catch
{
    template <>
    struct StringMaker<juce::dsp::AudioBlock<float>>
    {
        static std::string convert (juce::dsp::AudioBlock<float> const& value)
        {
            return melatonin::sparkline (value).toStdString();
        }
    };

    template <>
    struct StringMaker<juce::dsp::AudioBlock<double>>
    {
        static std::string convert (juce::dsp::AudioBlock<double> const& value)
        {
            return melatonin::sparkline (value).toStdString();
        }
    };

    template <>
    struct StringMaker<juce::AudioBuffer<float>>
    {
        static std::string convert (juce::AudioBuffer<float> value)
        {
            return melatonin::sparkline (value).toStdString();
        }
    };

    template <>
    struct StringMaker<juce::AudioBuffer<double>>
    {
        static std::string convert (juce::AudioBuffer<double>& value)
        {
            return melatonin::sparkline (value).toStdString();
        }
    };

}

namespace melatonin
{

    struct isValidAudio : Catch::Matchers::MatcherGenericBase
    {
        template <typename SampleType>
        [[nodiscard]] bool match (const AudioBlock<SampleType>& block) const
        {
            return validAudio (block);
        }

        template <typename SampleType>
        [[nodiscard]] bool match (juce::AudioBuffer<SampleType>& buffer) const
        {
            return validAudio (buffer);
        }

        [[nodiscard]] std::string describe() const override
        {
            return "Block is free of NaNs, INFs and subnormals\n";
        }
    };

    struct isFilled : Catch::Matchers::MatcherGenericBase
    {
        template <typename SampleType>
        [[nodiscard]] bool match (const AudioBlock<SampleType>& block) const
        {
            return blockIsFilled (block);
        }

        template <typename SampleType>
        [[nodiscard]] bool match (juce::AudioBuffer<SampleType>& block) const
        {
            return bufferIsFilled (block);
        }

        [[nodiscard]] std::string describe() const override
        {
            return "Block is completely filled";
        }
    };

    struct isEmpty : Catch::Matchers::MatcherGenericBase
    {
        template <typename SampleType>
        [[nodiscard]] bool match (const AudioBlock<SampleType>& block) const
        {
            return blockIsEmpty (block);
        }

        template <typename SampleType>
        [[nodiscard]] bool match (juce::AudioBuffer<SampleType>& block) const
        {
            return bufferIsEmpty (block);
        }

        [[nodiscard]] std::string describe() const override
        {
            return "Block is completely empty";
        }
    };

    struct isFilledUntil : Catch::Matchers::MatcherGenericBase
    {
        size_t boundary = 0;
        explicit isFilledUntil (size_t sampleNum) : boundary (sampleNum) {}

        template <typename SampleType>
        [[nodiscard]] bool match (const AudioBlock<SampleType>& block) const
        {
            return blockIsFilledUntil (block, (int) boundary);
        }

        template <typename SampleType>
        [[nodiscard]] bool match (juce::AudioBuffer<SampleType>& block) const
        {
            return bufferIsFilledUntil (block, (int) boundary);
        }

        [[nodiscard]] std::string describe() const override
        {
            std::ostringstream ss;
            ss << "Block is filled to sample " << boundary;
            return ss.str();
        }
    };

    struct isFilledAfter : Catch::Matchers::MatcherGenericBase
    {
        size_t boundary;
        explicit isFilledAfter (size_t sampleNum) : boundary (sampleNum) {}

        template <typename SampleType>
        [[nodiscard]] bool match (const AudioBlock<SampleType>& block) const
        {
            return blockIsFilledAfter (block, (int) boundary);
        }

        template <typename SampleType>
        [[nodiscard]] bool match (juce::AudioBuffer<SampleType>& block) const
        {
            return bufferIsFilledAfter (block, (int) boundary);
        }

        [[nodiscard]] std::string describe() const override
        {
            std::ostringstream ss;
            ss << "Block is filled after sample " << boundary;
            return ss.str();
        }
    };

    struct isFilledBetween : Catch::Matchers::MatcherGenericBase
    {
        size_t start;
        size_t end;
        isFilledBetween (size_t s, size_t e) : start (s), end (e) {}

        template <typename SampleType>
        [[nodiscard]] bool match (const AudioBlock<SampleType>& block) const
        {
            return blockIsFilledBetween (block, start, end);
        }

        template <typename SampleType>
        [[nodiscard]] bool match (juce::AudioBuffer<SampleType>& block) const
        {
            return bufferIsFilledBetween (block, start, end);
        }

        [[nodiscard]] std::string describe() const override
        {
            std::ostringstream ss;
            ss << "Block is filled between samples " << start << " and " << end;
            return ss.str();
        }
    };

    struct isEmptyAfter : Catch::Matchers::MatcherGenericBase
    {
        size_t boundary = 0;

        explicit isEmptyAfter (size_t sampleNum) : boundary (sampleNum) {}

        template <typename SampleType>
        [[nodiscard]] bool match (const AudioBlock<SampleType>& block) const
        {
            return blockIsEmptyAfter (block, boundary);
        }

        template <typename SampleType>
        [[nodiscard]] bool match (juce::AudioBuffer<SampleType>& block) const
        {
            return bufferIsEmptyAfter (block, boundary);
        }

        [[nodiscard]] std::string describe() const override
        {
            std::ostringstream ss;
            ss << "Block is empty after sample " << boundary;
            return ss.str();
        }
    };

    struct isEmptyUntil : Catch::Matchers::MatcherGenericBase
    {
        size_t boundary = 0;
        explicit isEmptyUntil (size_t sampleNum) : boundary (sampleNum) {}

        template <typename SampleType>
        [[nodiscard]] bool match (const AudioBlock<SampleType>& block) const
        {
            return blockIsEmptyUntil (block, boundary);
        }

        template <typename SampleType>
        [[nodiscard]] bool match (juce::AudioBuffer<SampleType>& block) const
        {
            return bufferIsEmptyUntil (block, boundary);
        }

        [[nodiscard]] std::string describe() const override
        {
            std::ostringstream ss;
            ss << "Block is empty until sample " << boundary;
            return ss.str();
        }
    };

    struct hasRMS : Catch::Matchers::MatcherGenericBase
    {
        double expectedRMS = 0;
        mutable double actualRMS = 0;
        double tolerance = 0;
        explicit hasRMS (double r, double t = 0.0001) : expectedRMS (r), tolerance (t) {}

        template <typename SampleType>
        [[nodiscard]] bool match (const AudioBlock<SampleType>& block) const
        {
            actualRMS = rms (block);
            return std::abs (actualRMS - expectedRMS) < tolerance;
        }

        [[nodiscard]] std::string describe() const override
        {
            std::ostringstream ss;
            ss << "Block RMS of " << actualRMS << " was expected to be within " << tolerance << " of " << expectedRMS;
            return ss.str();
        }
    };

    // supports audioblock and std::vector
    template <typename SampleType>
    struct isEqualTo : Catch::Matchers::MatcherGenericBase
    {
        juce::HeapBlock<char> expectedData;
        AudioBlock<SampleType> expected = {};
        std::vector<SampleType> expectedVector = {};
        mutable std::vector<float> testedVector = {};
        const float tolerance = 0;
        mutable size_t sampleNumber = 0;
        mutable double blockValue = 0;
        mutable double expectedValue = 0;
        mutable std::string descriptionOfOther = "";

        explicit isEqualTo (const AudioBlock<SampleType>& e, float t = std::numeric_limits<float>::epsilon() * 100)
            : expected (e), tolerance (t) {}

        // allow us to easily compare vector to vector
        // needed because Catch::Matchers::Approx<float> for std::vector is broken around 0.0
        // convenient for test writing
        explicit isEqualTo (const std::vector<SampleType>& vector, float t = std::numeric_limits<float>::epsilon() * 100)
            : expected(), expectedVector (vector), tolerance (t)
        {
            // also populate the expected block in case we want to compare incoming vector against blocks
            expected = AudioBlock<SampleType> (expectedData, 1, expectedVector.size());
            for (int i = 0; i < (int) expectedVector.size(); ++i)
                expected.setSample (0, i, expectedVector[(size_t) i]);
        }

        [[nodiscard]] bool match (AudioBlock<SampleType>& block) const
        {
            jassert (expected.getNumSamples() == block.getNumSamples());
            for (int channel = 0; channel < (int) block.getNumChannels(); ++channel)
            {
                // TODO: clean this up!
                for (int i = 0; i < (int) block.getNumSamples(); ++i)
                {
                    testedVector.push_back (block.getSample (channel, i));
                }
                for (int i = 0; i < (int) block.getNumSamples(); ++i)
                {
                    // juce::approximatelyEqual was not quite tolerant enough for my needs
                    // if you are doing things like adding deltas 100 times vs. multiplying a delta by 1000, you'll need more
                    if (!juce::isWithin (expected.getSample (channel, i), block.getSample (channel, i), tolerance))
                    {
                        sampleNumber = (size_t) i;
                        blockValue = block.getSample (channel, i);
                        expectedValue = expected.getSample (channel, i);
                        return false;
                    }
                }
            }
            return true;
        }

        [[nodiscard]] bool
            match (juce::AudioBuffer<SampleType>& block) const
        {
            return match (AudioBlock<SampleType> (block));
        }

        [[nodiscard]] bool match (const std::vector<SampleType>& vector) const
        {
            // hi, you have to write your expectation for the exact number of elements!
            if (vector.size() != expectedVector.size())
                jassertfalse;

            testedVector = vector;

            for (auto& value : vector)
            {
                if (!juce::isWithin (expectedVector[sampleNumber], value, tolerance))
                {
                    expectedValue = expectedVector[sampleNumber];
                    return false;
                }
                sampleNumber++;
            }
            return true;
        }

        std::string describe() const override
        {
            if (descriptionOfOther.empty())
                descriptionOfOther = sparkline (expected).toStdString();

            auto expectedString = expectedVector.empty() ? blockToString(expected) : vectorToString (expectedVector);
            std::ostringstream ss;
            ss << "is equal to \n"
               << descriptionOfOther << "\n";
            ss << "Expected: " << expectedString << "\nActual:   " << melatonin::vectorToString (testedVector);
            return ss.str();
        }
    };
}
