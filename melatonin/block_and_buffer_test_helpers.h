#pragma once

namespace melatonin
{
    template <typename SampleType>
    using AudioBlock = juce::dsp::AudioBlock<SampleType>;

    // Ensures there's no INF, NaN or subnormals in the block
    template <typename SampleType>
    static inline bool validAudio (const AudioBlock<SampleType>& block)
    {
        for (size_t c = 0; c < block.getNumChannels(); ++c)
            for (size_t i = 0; i < block.getNumSamples(); ++i)
            {
                auto value = std::fpclassify (block.getSample ((int) c, (int) i));
                if (value == FP_SUBNORMAL || value == FP_INFINITE || value == FP_NAN)
                    return false;
            }
        return true;
    }

    template <typename SampleType>
    static inline bool validAudio (juce::AudioBuffer<SampleType>& buffer)
    {
        auto block = AudioBlock<SampleType> (buffer);
        return validAudio (block);
    }

    // returns the number of full cycles of a waveform contained by a block
    template <typename SampleType>
    static inline int numberOfCycles (const AudioBlock<SampleType>& block)
    {
        auto waveform = block.getChannelPointer (0);
        int numberOfZeroCrossings = 0;
        for (size_t i = 0; i < block.getNumSamples(); ++i)
        {
            if (waveform[i] == 0 && ((i == 0) || waveform[i - 1] != 0))
                numberOfZeroCrossings++;
            if ((i > 0) && ((waveform[i - 1] < 0) != (waveform[i] < 0)))
                numberOfZeroCrossings++;
        }
        return numberOfZeroCrossings / 2;
    }

    template <typename SampleType>
    static inline bool numberOfCycles (juce::AudioBuffer<SampleType>& buffer)
    {
        const auto block = AudioBlock<SampleType> (buffer);
        return numberOfCycles (block);
    }

    template <typename SampleType>
    static inline bool channelsAreIdentical (const AudioBlock<SampleType>& block)
    {
        jassert (block.getNumChannels() > 1);
        for (size_t i = 0; i < block.getNumSamples(); ++i)
        {
            float channelZeroValue = block.getSample (0, (int) i);
            for (size_t c = 0; c < block.getNumChannels(); ++c)
            {
                if (block.getSample ((int) c, (int) i) != channelZeroValue)
                    return false;
            }
        }
        return true;
    }

    template <typename SampleType>
    static inline bool channelsAreIdentical (juce::AudioBuffer<SampleType>& buffer)
    {
        const auto block = AudioBlock<SampleType> (buffer);
        return channelsAreIdentical (block);
    }

    template <typename SampleType>
    static inline SampleType maxMagnitude (const AudioBlock<SampleType>& block)
    {
        float max = 0.0;
        for (size_t c = 0; c < block.getNumChannels(); ++c)
        {
            auto channel_max = juce::FloatVectorOperations::findMaximum (block.getChannelPointer (c), block.getNumSamples());
            auto channel_abs_min = abs (juce::FloatVectorOperations::findMinimum (block.getChannelPointer (c), block.getNumSamples()));
            if (channel_max > max)
                max = channel_max;
            else if (channel_abs_min > max)
                max = channel_abs_min;
        }
        return max;
    }

    template <typename SampleType>
    static inline SampleType minMagnitude (const AudioBlock<SampleType>& block)
    {
        float min = 0.0;
        for (size_t c = 0; c < block.getNumChannels(); ++c)
        {
            auto channel_min = juce::FloatVectorOperations::findMinimum (block.getChannelPointer (c), block.getNumSamples());
            auto channel_abs_max = abs (juce::FloatVectorOperations::findMaximum (block.getChannelPointer (c), block.getNumSamples()));
            if (channel_min < min)
                min = channel_min;
            else if (channel_abs_max < min)
                min = channel_abs_max;
        }
        return min;
    }

    template <typename SampleType>
    static inline SampleType maxMagnitude (juce::AudioBuffer<SampleType>& buffer)
    {
        const auto block = AudioBlock<SampleType> (buffer);
        return maxMagnitude (block);
    }

    template <typename SampleType>
    static inline SampleType rms (const AudioBlock<SampleType>& block)
    {
        float sum = 0.0;
        for (int c = 0; c < (int) block.getNumChannels(); ++c)
        {
            for (int i = 0; i < (int) block.getNumSamples(); ++i)
            {
                sum += (float) block.getSample (c, i) * (float) block.getSample (c, i);
            }
        }

        return static_cast<SampleType> (std::sqrt (sum / float (block.getNumSamples() * block.getNumChannels())));
    }

    template <typename SampleType>
    static inline SampleType rms (juce::AudioBuffer<SampleType>& buffer)
    {
        const auto block = AudioBlock<SampleType> (buffer);
        return rms (block);
    }

    template <typename SampleType>
    static inline SampleType rmsInDB (const AudioBlock<SampleType>& block)
    {
        return static_cast<SampleType> (juce::Decibels::gainToDecibels (rms (block)));
    }

    template <typename SampleType>
    static inline SampleType rmsInDB (juce::AudioBuffer<SampleType>& buffer)
    {
        const auto block = AudioBlock<SampleType> (buffer);
        return rmsInDB (block);
    }

    template <typename SampleType>
    static inline AudioBlock<SampleType>& fillBlockWithFunction (AudioBlock<SampleType>& block, const std::function<float (float)>& function, float frequency, float sampleRate, float gain = 1.0f, bool accumulate = false)
    {
        auto angleDelta = juce::MathConstants<float>::twoPi * frequency / sampleRate;
        for (size_t c = 0; c < block.getNumChannels(); ++c)
        {
            auto currentAngle = 0.0f;
            for (size_t i = 0; i < block.getNumSamples(); ++i)
            {
                auto sampleValue = gain * function (currentAngle);
                block.setSample ((int) c, (int) i, accumulate ? block.getSample (c, i) + gain + sampleValue : sampleValue);
                currentAngle += angleDelta;
                if (currentAngle >= juce::MathConstants<float>::pi)
                    currentAngle -= juce::MathConstants<float>::twoPi;
            }
        }
        return block;
    }

    template <typename SampleType>
    static inline juce::AudioBuffer<SampleType>& fillBufferWithFunction (juce::AudioBuffer<SampleType>& buffer, const std::function<float (float)>& function, float frequency, float sampleRate, float gain = 1.0f)
    {
        const auto block = AudioBlock<SampleType> (buffer);
        return fillBlockWithFunction (block, function, frequency, sampleRate, gain);
    }

    // assumes mono for now
    template <typename SampleType>
    static inline AudioBlock<SampleType>& fillWithSine (AudioBlock<SampleType>& block, float frequency, float sampleRate, float gain = 1.0f)
    {
        return fillBlockWithFunction (
            block, [] (float angle) { return juce::dsp::FastMathApproximations::sin (angle); }, frequency, sampleRate, gain);
    }

    template <typename SampleType>
    static inline AudioBlock<SampleType>& addSineToBlock (AudioBlock<SampleType>& block, float frequency, float sampleRate, float gain = 1.0f)
    {
        return fillBlockWithFunction (
            block, [] (float angle) { return juce::dsp::FastMathApproximations::sin (angle); }, frequency, sampleRate, gain, true);
    }

    template <typename SampleType>
    static inline juce::AudioBuffer<SampleType>& fillBufferWithSine (juce::AudioBuffer<SampleType>& buffer, float frequency, float sampleRate, float gain = 1.0f)
    {
        const auto block = AudioBlock<SampleType> (buffer);
        return fillWithSine (block, frequency, sampleRate, gain);
    }

    // assumes mono for now
    template <typename SampleType>
    static inline AudioBlock<SampleType>& fillWithCosine (AudioBlock<SampleType>& block, float frequency, float sampleRate, float gain = 1.0f)
    {
        return fillBlockWithFunction (
            block, [] (float angle) { return juce::dsp::FastMathApproximations::cos (angle); }, frequency, sampleRate, gain);
    }

    template <typename SampleType>
    static inline juce::AudioBuffer<SampleType>& fillBufferWithCosine (juce::AudioBuffer<SampleType>& buffer, float frequency, float sampleRate, float gain = 1.0f)
    {
        const auto block = AudioBlock<SampleType> (buffer);
        return fillWithCosine (block, frequency, sampleRate, gain);
    }

    // all zeros
    template <typename SampleType>
    static inline bool blockIsEmpty (const AudioBlock<SampleType>& block)
    {
        for (size_t i = 0; i < block.getNumChannels(); ++i)
        {
            if (!juce::FloatVectorOperations::findMinAndMax (block.getChannelPointer (i), (int) block.getNumSamples()).isEmpty())
            {
                return false;
            }
        }
        return true;
    }

    template <typename SampleType>
    static inline bool bufferIsEmpty (juce::AudioBuffer<SampleType>& buffer)
    {
        const auto block = AudioBlock<SampleType> (buffer);
        return blockIsEmpty (block);
    }

    template <typename SampleType>
    static inline bool blockIsEmptyUntil (const AudioBlock<SampleType>& block, size_t numSamples)
    {
        jassert (block.getNumSamples() >= numSamples);

        for (size_t i = 0; i < block.getNumChannels(); ++i)
        {
            if (!juce::FloatVectorOperations::findMinAndMax (block.getChannelPointer (i), (int) numSamples).isEmpty())
            {
                return false;
            }
        }
        return true;
    }

    template <typename SampleType>
    static inline bool bufferIsEmptyUntil (juce::AudioBuffer<SampleType>& buffer, size_t numSamples)
    {
        const auto block = AudioBlock<SampleType> (buffer);
        return blockIsEmptyUntil (block, numSamples);
    }

    template <typename SampleType>
    static inline bool blockIsEmptyAfter (const AudioBlock<SampleType>& block, size_t firstZeroAt)
    {
        jassert (block.getNumSamples() >= firstZeroAt);

        if (block.getNumSamples() == firstZeroAt)
            return true;

        auto numSamples = block.getNumSamples() - firstZeroAt;
        auto subBlock = block.getSubBlock (firstZeroAt, numSamples);
        for (size_t i = 0; i < subBlock.getNumChannels(); ++i)
        {
            if (!juce::FloatVectorOperations::findMinAndMax (subBlock.getChannelPointer (i), (int) numSamples).isEmpty())
            {
                return false;
            }
        }
        return true;
    }

    template <typename SampleType>
    static inline bool bufferIsEmptyAfter (juce::AudioBuffer<SampleType>& buffer, size_t firstZeroAt)
    {
        const auto block = AudioBlock<SampleType> (buffer);
        return blockIsEmptyAfter (block, firstZeroAt);
    }

    template <typename SampleType>
    static inline bool blockIsFilled (const AudioBlock<SampleType>& block)
    {
        for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
        {
            auto channelBlock = block.getSingleChannelBlock (ch);
            if (numberOfConsecutiveZeros (channelBlock) > 0)
                return false;
        }
        return true;
    }

    template <typename SampleType>
    static inline bool bufferIsFilled (juce::AudioBuffer<SampleType>& buffer)
    {
        const auto block = AudioBlock<SampleType> (buffer);
        return blockIsFilled (block);
    }

    template <typename SampleType>
    static inline bool blockIsFilledUntil (const AudioBlock<SampleType>& block, int sampleNum)
    {
        jassert ((int) block.getNumSamples() >= sampleNum);

        for (int c = 0; c < (int) block.getNumChannels(); ++c)
        {
            for (int i = 0; i < sampleNum; ++i)
            {
                if (i > 0 && ((block.getSample (c, i) == 0) && (block.getSample (c, i - 1) == 0)))
                    return false;
            }
        }
        return true;
    }

    template <typename SampleType>
    static inline bool bufferIsFilledUntil (juce::AudioBuffer<SampleType>& buffer, int sampleNum)
    {
        const auto block = AudioBlock<SampleType> (buffer);
        return blockIsFilledUntil (block, sampleNum);
    }

    template <typename SampleType>
    static inline bool blockIsFilledAfter (const AudioBlock<SampleType>& block, int sampleNum)
    {
        jassert ((int) block.getNumSamples() >= sampleNum);

        if ((int) block.getNumSamples() == sampleNum)
            return false;

        for (int c = 0; c < (int) block.getNumChannels(); ++c)
        {
            for (int i = sampleNum; i < (int) block.getNumSamples(); ++i)
            {
                if (i > sampleNum && ((block.getSample (c, i) == 0) && (block.getSample (c, i - 1) == 0)))
                    return false;
            }
        }
        return true;
    }

    template <typename SampleType>
    static inline bool bufferIsFilledAfter (juce::AudioBuffer<SampleType>& buffer, int sampleNum)
    {
        const auto block = AudioBlock<SampleType> (buffer);
        return blockIsFilledAfter (block, sampleNum);
    }

    template <typename SampleType>
    static inline bool blockIsFilledBetween (const AudioBlock<SampleType>& block, int start, int end)
    {
        jassert (end > start);
        for (int c = 0; c < (int) block.getNumChannels(); ++c)
        {
            for (int i = start; i < (end - start); ++i)
            {
                if (i > start && ((block.getSample (c, i) == 0) && (block.getSample (c, i - 1) == 0)))
                    return false;
            }
        }
        return true;
    }

    template <typename SampleType>
    static inline bool bufferIsFilledBetween (juce::AudioBuffer<SampleType>& buffer, int start, int end)
    {
        const auto block = AudioBlock<SampleType> (buffer);
        return blockIsFilledBetween (block, start, end);
    }

    // MONO FOR NOW
    // Manual frequency correlation using a known frequency
    // https://github.com/juce-framework/JUCE/blob/master/modules/juce_dsp/frequency/juce_FFT_test.cpp#L59-L82
    template <typename SampleType>
    static inline float magnitudeOfFrequency (const AudioBlock<SampleType>& block, float freq, float sampleRate)
    {
        // happy to allocate memory here, this is a test, not real time audio
        juce::HeapBlock<char> sineData;
        juce::HeapBlock<char> cosineData;

        const size_t length = block.getNumSamples();

        // we can get more accurate results by assuming the block is full with the frequency
        // and only taking an integer number of cycles out of the block
        const int lastFullCycle = (int) length - ((int) length % (int) (sampleRate / freq));

        auto sineBlock = juce::dsp::AudioBlock<SampleType> (sineData, block.getNumChannels(), length);
        auto cosineBlock = juce::dsp::AudioBlock<SampleType> (cosineData, block.getNumChannels(), length);

        fillWithSine (sineBlock, freq, sampleRate).multiplyBy (block);
        fillWithCosine (cosineBlock, freq, sampleRate).multiplyBy (block);

        float sineSum = 0;
        float cosineSum = 0;

        for (int i = 0; i < lastFullCycle; ++i)
        {
            sineSum += sineBlock.getSample (0, i);
            cosineSum += cosineBlock.getSample (0, i);
        }
        return std::sqrt ((float) juce::square (sineSum / (float) lastFullCycle) + juce::square (cosineSum / (float) lastFullCycle)) * 2.0f;
    }

    template <typename SampleType>
    static inline float magnitudeOfFrequency (juce::AudioBuffer<SampleType>& buffer, float freq, float sampleRate)
    {
        const auto block = AudioBlock<SampleType> (buffer);
        return magnitudeOfFrequency (block, freq, sampleRate);
    }

    template <typename SampleType>
    static inline AudioBlock<SampleType>& normalized (AudioBlock<SampleType>& block)
    {
        for (auto channel = 0; channel < block.getNumChannels(); ++channel)
        {
            float absMax = abs (juce::FloatVectorOperations::findMaximum (block.getChannelPointer (channel), block.getNumSamples()));
            float absMin = abs (juce::FloatVectorOperations::findMinimum (block.getChannelPointer (channel), block.getNumSamples()));
            float max = juce::jmax (absMin, absMax);
            if (max > 0.0)
            {
                juce::FloatVectorOperations::multiply (block.getChannelPointer (channel), 1 / max, block.getNumSamples());
            }
        }
        return block;
    }

    template <typename SampleType>
    static inline AudioBlock<SampleType>& normalized (juce::AudioBuffer<SampleType>& buffer)
    {
        const auto block = AudioBlock<SampleType> (buffer);
        return normalized (block);
    }

    template <typename SampleType>
    static inline AudioBlock<SampleType>& reverse (AudioBlock<SampleType>& block)
    {
        juce::HeapBlock<char> tempHeap;
        auto reversedBlock = AudioBlock<SampleType> (tempHeap, block.getNumChannels(), block.getNumSamples());

        for (int channel = 0; channel < (int) block.getNumChannels(); ++channel)
        {
            for (int i = 0; i < (int) block.getNumSamples(); ++i)
                reversedBlock.setSample (channel, i, block.getSample (channel, (int) block.getNumSamples() - 1 - i));
        }
        block.copyFrom (reversedBlock);
        return block;
    }

    template <typename SampleType>
    static inline AudioBlock<SampleType>& reverse (juce::AudioBuffer<SampleType>& buffer)
    {
        const auto block = AudioBlock<SampleType> (buffer);
        return reverse (block);
    }

    template <typename SampleType>
    void printHistogram (AudioBlock<SampleType>& block)
    {
        // Calculate the range of the samples
        float rangeStart = juce::FloatVectorOperations::findMinimum (block.getChannelPointer (0), block.getNumSamples());
        double rangeEnd = juce::FloatVectorOperations::findMaximum (block.getChannelPointer (0), block.getNumSamples());

        // Calculate the histogram of the samples
        const size_t numBins = 10; // Number of bins for histogram
        std::vector<size_t> histogram (numBins, 0);
        const double binSize = (rangeEnd - rangeStart) / numBins;

        for (auto i = 0; i < block.getNumSamples(); ++i)
        {
            size_t binIndex = static_cast<size_t> ((block.getSample (0, i) - rangeStart) / binSize);
            histogram[binIndex]++;
        }

        // Print the histogram
        for (auto i = 0; i < numBins; ++i)
        {
            std::cout << "[" << rangeStart + i * binSize << ", " << rangeStart + (i + 1) * binSize << "): ";
            for (auto j = 0; j < histogram[i]; ++j)
                std::cout << "|";
            std::cout << std::endl;
        }
    }

    template <typename SampleType>
    bool isUniformlyDistributed (AudioBlock<SampleType>& block)
    {
        const double epsilon = 0.3; // lol, this is pretty accepting...

        // Calculate the range of the samples
        float rangeStart = juce::FloatVectorOperations::findMinimum (block.getChannelPointer (0), block.getNumSamples());
        double rangeEnd = juce::FloatVectorOperations::findMaximum (block.getChannelPointer (0), block.getNumSamples());

        // Calculate the histogram of the samples
        const size_t numBins = 10; // Number of bins for histogram
        std::vector<size_t> histogram (numBins, 0);
        const double binSize = (rangeEnd - rangeStart) / numBins;

        for (auto i = 0; i < block.getNumSamples(); ++i)
        {
            size_t binIndex = static_cast<size_t> ((block.getSample (0, i) - rangeStart) / binSize);
            histogram[binIndex]++;
        }

        // Check if the histogram values are relatively equal
        const double expectedCount = block.getNumSamples() / static_cast<double> (numBins);
        for (const size_t& count : histogram)
        {
            if (std::abs (count - expectedCount) > epsilon * expectedCount)
            {
                return false;
            }
        }
        return true;
    }

    template <typename SampleType>
    float average (AudioBlock<SampleType>& block)
    {
        float sum = 0;
        for (int i = 0; i < block.getNumSamples(); ++i)
            sum += block.getSample (0, i);
        return sum / (float) block.getNumSamples();
    }

}
