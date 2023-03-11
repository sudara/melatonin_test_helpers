#pragma once

namespace melatonin
{
    template<typename SampleType>
    class FFT
    {
    public:
        FFT (AudioBlock<SampleType>& b, float rate, bool scale = true, bool debug = false) : block (b), sampleRate (rate)
        {
            // fill up all 1024 samples
            for (size_t i = 0; i < fftSize; i++)
                fftData[i] = block.getSample (0, (int) (i % block.getNumSamples()));

            // Hann is best for sinusoids
            window.multiplyWithWindowingTable (fftData.getData(), fftSize);
            fft.performFrequencyOnlyForwardTransform (fftData.getData());

            SampleType maxValue = 0.0;

            // Normalize from fftSize magnitudes down to a 0-1 magnitude range
            for (size_t i = 0; i < fftSize; i++)
            {
                fftData[i] = fftData[i] / fftSize;
                maxValue = fftData[i] > maxValue ? fftData[i] : maxValue;
                if (debug)
                    DBG (fftData[i]);
            }

            // Different tests use different amplitudes...
            if (scale)
            {
                for (size_t i = 0; i < fftSize; i++)
                {
                    fftData[i] = fftData[i] * (SampleType) 0.5 / maxValue; // 0.5 is our arbitrary max value for a bin
                }
            }
        }

        // VORSICHT!: This will only return the strongest *single* bin
        // But in reality FFT is sloppy and a single freq can easily spread across bins
        size_t frequencyBinFor (float frequency) const
        {
            // add 0.5 so that the implicit truncation behaves as rounding
            return (size_t) ((frequency / sampleRate * fftSize) + 0.5);
        }

        float approxFrequencyForBin (size_t bin) const
        {
            float binWidth = sampleRate / fftSize;
            return (float) (bin + 1) * binWidth;
        }

        [[maybe_unused]] std::vector<size_t> frequencyBinsFor (const std::vector<float>& frequencies) const
        {
            std::vector<size_t> frequencyBins;

            for (const auto freq : frequencies)
                frequencyBins.push_back (frequencyBinFor (freq));

            return frequencyBins;
        }

        bool strongestFrequencyIs (const float frequency) const
        {
            // Ideally we could detect single sine waves of magnitude 1.0 and call it a day.
            // However, FFT is messy. Frequencies might be split between bins
            // We might be ramping up, have multiple frequencies present, etc.
            auto index = frequencyBinFor (frequency);
            return fftData[index] == juce::FloatVectorOperations::findMaximum (fftData, numberOfBins);
        }

        size_t strongestFrequencyBin()
        {
            size_t strongestBin = 0;
            for (size_t i = 0; i < numberOfBins; i++)
            {
                //DBG(fftData[i]);
                if (fftData[i] > fftData[strongestBin])
                {
                    strongestBin = i;
                }
            }
            return strongestBin;
        }

        std::vector<size_t> strongFrequencyBins() const
        {
            std::vector<size_t> strongFrequencyBins;
            for (size_t i = 0; i < numberOfBins; i++)
            {
                //DBG(fftData[i]);
                if (fftData[i] > 0.05)
                    strongFrequencyBins.push_back (i);
            }
            return strongFrequencyBins;
        }

        bool frequencyNotPresent (const float frequency) const
        {
            // Ideally, FFT would be more accurate.
            // However, bleed of 0.01 is often seen 2 bins over
            return fftData[frequencyBinFor (frequency)] < 0.02;
        }

        /* Produces an output like the following:

        FFT bins | freq | signal
        9 | 215.332 | 0.246468
        10 | 236.865 | 0.5
        11 | 258.398 | 0.192688
        12 | 279.932 | 0.118243
        13 | 301.465 | 0.091966
        */
        [[maybe_unused]] void printDebug() const
        {
            DBG ("FFT bins | freq | signal");
            for (auto bin : strongFrequencyBins())
            {
                DBG (bin << " | " << approxFrequencyForBin (bin) << " | " << fftData[bin]);
            }
        }

    private:
        AudioBlock<SampleType>& block;
        float sampleRate;
        const size_t fftSize = 2048;
        const size_t numberOfBins = fftSize / 2;
        juce::HeapBlock<float> fftData { fftSize * 2, true }; // Even though we aren't calculating negative freqs, still needs to be 2x the size
        juce::dsp::WindowingFunction<float> window { fftSize, juce::dsp::WindowingFunction<float>::WindowingMethod::hann };
        juce::dsp::FFT fft { 11 }; // 2^10 (10th order) is the same as a size of 1024
    };
}
