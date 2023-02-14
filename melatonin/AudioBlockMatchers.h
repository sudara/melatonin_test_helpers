#pragma once
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_templated.hpp>

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
}

namespace melatonin
{
    struct isValidAudio : Catch::Matchers::MatcherGenericBase
    {
        template <typename SampleType>
        bool match (const AudioBlock<SampleType>& block) const
        {
            return validAudio (block);
        }

        std::string describe() const override
        {
            return "Block is free of NaNs, INFs and subnormals";
        }
    };

    struct isFilled : Catch::Matchers::MatcherGenericBase
    {
        template <typename SampleType>
        bool match (const AudioBlock<SampleType>& block) const
        {
            return blockIsFilled (block);
        }

        std::string describe() const override
        {
            return "Block is completely filled";
        }
    };

    struct isEmpty : Catch::Matchers::MatcherGenericBase
    {
        template <typename SampleType>
        bool match (const AudioBlock<SampleType>& block) const
        {
            return blockIsEmpty (block);
        }

        std::string describe() const override
        {
            return "Block is completely empty";
        }
    };

    struct isFilledUntil : Catch::Matchers::MatcherGenericBase
    {
        size_t boundary;
        isFilledUntil (size_t sampleNum) : boundary (sampleNum) {}

        template <typename SampleType>
        bool match (const AudioBlock<SampleType>& block) const
        {
            return blockIsFilledUntil (block, (int) boundary);
        }

        std::string describe() const override
        {
            std::ostringstream ss;
            ss << "Block is filled to sample " << boundary;
            return ss.str();
        }
    };

    struct isFilledAfter : Catch::Matchers::MatcherGenericBase
    {
        size_t boundary;
        isFilledAfter (size_t sampleNum) : boundary (sampleNum) {}

        template <typename SampleType>
        bool match (const AudioBlock<SampleType>& block) const
        {
            return blockIsFilledAfter (block, (int) boundary);
        }

        std::string describe() const override
        {
            std::ostringstream ss;
            ss << "Block is filled after sample " << boundary;
            return ss.str();
        }
    };

    struct isFilledBetween : Catch::Matchers::MatcherGenericBase
    {
        int start;
        int end;
        isFilledBetween (size_t s, size_t e) : start (s), end (e) {}

        template <typename SampleType>
        bool match (const AudioBlock<SampleType>& block) const
        {
            return blockIsFilledBetween (block, start, end);
        }

        std::string describe() const override
        {
            std::ostringstream ss;
            ss << "Block is filled between samples " << start << " and " << end;
            return ss.str();
        }
    };

    struct isEmptyAfter : Catch::Matchers::MatcherGenericBase
    {
        size_t boundary;

        isEmptyAfter (size_t sampleNum) : boundary (sampleNum) {}

        template <typename SampleType>
        bool match (const AudioBlock<SampleType>& block) const
        {
            return blockIsEmptyAfter (block, boundary);
        }

        std::string describe() const override
        {
            std::ostringstream ss;
            ss << "Block is empty after sample " << boundary;
            return ss.str();
        }
    };

    struct isEmptyUntil : Catch::Matchers::MatcherGenericBase
    {
        size_t boundary;
        isEmptyUntil (size_t sampleNum) : boundary (sampleNum) {}

        template <typename SampleType>
        bool match (const AudioBlock<SampleType>& block) const
        {
            return blockIsEmptyUntil (block, boundary);
        }

        std::string describe() const override
        {
            std::ostringstream ss;
            ss << "Block is empty until sample " << boundary;
            return ss.str();
        }
    };

    template <typename SampleType>
    struct isEqualTo : Catch::Matchers::MatcherGenericBase
    {
        const AudioBlock<SampleType>& other;
        const float tolerance;
        mutable int sampleNumber;
        mutable double blockValue;
        mutable double otherValue;
        mutable std::string descriptionOfOther;

        isEqualTo (const AudioBlock<SampleType>& o, float t = std::numeric_limits<float>::epsilon() * 100)
            : other (o), tolerance (t) {}

        bool match (AudioBlock<SampleType>& block) const
        {
            for (auto channel = 0; channel < (int) block.getNumChannels(); ++channel)
            {
                for (auto i = 0; i < (int) block.getNumSamples(); ++i)
                {
                    // juce::approximatelyEqual was not quite tolerant enough for my needs
                    // if you are doing things like adding deltas 100 times vs. multiplying a delta by 1000, you'll need more
                    if (!juce::isWithin (other.getSample (channel, i), block.getSample (channel, i), tolerance))
                    {
                        sampleNumber = i;
                        blockValue = block.getSample (channel, i);
                        otherValue = other.getSample (channel, i);
                        return false;
                    }
                }
            }
            return true;
        }

        std::string describe() const override
        {
            if (descriptionOfOther.empty())
                descriptionOfOther = sparkline (other).toStdString();

            std::ostringstream ss;
            ss << "is equal to \n"
               << descriptionOfOther << "\n";
            ss << "Sample " << sampleNumber << " expected to be " << otherValue << " but was " << blockValue << " a difference of " << otherValue - blockValue;
            return ss.str();
        }
    };
}