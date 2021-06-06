# Melatonin AudioBlock Test Helpers

This module contains:

1. Catch2 matchers that operate on `juce::dsp::AudioBlock` and rely on...
2. ...a collection of C++ free functions that take an `dsp::dsp::AudioBlock`.
   
I'm publishing it to share how I use Catch2 with JUCE.

## Catch2 Matchers

The Catch2 matchers gives a solid amount of detail on the AudioBlock when the test fails, including showing [sparkline](https://github.com/sudara/melatonin_audio) versions of both the expected and actual AudioBlock. 

### isEqualTo

```cpp
REQUIRE_THAT (partialBlock, isEqualTo (fullBlock));
```

This compares each channel of the two blocks and confirms each sample is within a tolerance of `std::numeric_limits<float>::epsilon() * 100`. Don't @ me, but assuming you are doing things like multiplying a number of floats together to arrive at the end result, you probably don't want this tolerance any tighter.

However, you can loosen or tighten to your desire by passing an absolute tolerance:

```cpp
REQUIRE_THAT (partialBlock, isEqualTo (fullBlock), 0.0005f);
```

### isFilled

```cpp
REQUIRE_THAT(myAudioBlock, isFilledUntil(256));
```

The block is completely filled with data. No more than 1 consecutive zero is allowed. 

### isFilledUntil

```cpp
REQUIRE_THAT(myAudioBlock, isFilledUntil(256));
```

Up to and including sample 256, the block is filled and doesn't contain consecutive zeros.

### isFilledAfter

```cpp
REQUIRE_THAT(myAudioBlock, isFilledAfter(256));
```

Starting with this sample, the block is filled. 

It's ok if sample 256 is zero (eg, start of a sine wave), we are just making sure we don't have consecutive zeros.

### isFilledBetween

```cpp
REQUIRE_THAT(myAudioBlock, isFilledBetween(64, 128));
```
Returns true when sample values 64 and 128 have a non zero value for all channels.


### isEmpty

```cpp
REQUIRE_THAT(myAudioBlock, isEmpty);
```

The block only has 0s.

### isEmptyUntil

```cpp
REQUIRE_THAT(myAudioBlock, isEmptyUntil(256));
```

The block has only zeros up to and including this sample number.

### isEmptyAfter

```cpp
REQUIRE_THAT(myAudioBlock, isEmptyAfter(256));
```

The block is empty after this sample number.


## Other helpers

The matchers above below all depend on free functions test helpers (prepended with `block`) which can be used seperately.

So for example you can do use the Catch2 the matcher style:

```cpp
REQUIRE_THAT(myAudioBlock, isFilledUntil(256));
```

Or the free function style:

```cpp
REQUIRE(blockIsFilledUntil(myAudioBlock, 256);
```

Additionally, there are some other helpers:

### maxMagnitude

Returns the peak absolute value (magnitude) from any channel in the block.

```cpp
REQUIRE (maxMagnitude (myAudioBlock) <= Catch::Approx (1.0f));
```

### magnitudeOfFrequency

```cpp
REQUIRE (magnitudeOfFrequency (myAudioBlock, 440.f, sampleRate) < 0.005);
```

Use this if you know the frequencies you expect to encounter in the AudioBlock. You'll need to pass it the sample rate you are using.

This uses frequency correlation and compares your AudioBlock against sine and cosine probes. It's essentially the same thing as what goes on under the hood with DFT, but for exactly the frequency you specify. 

This is a much more accurate way to confirm the presence of a known frequency than FFT.


### FFT

There are various FFT related functions available which rely on creating an instance of the FFT class.

Tip: Prefer the `magnitudeOfFrequency` helper if you know the frequency you are expecting and wish to somewhat accurately confirm magnitude. FFT is inherently messy. You'll get better results when your expected frequencies are in the middle of FFT bins. 


However, you can still sorta sloppily 
```cpp 
REQUIRE (FFT (myAudioBlock, 44100.0f).strongestFrequencyIs (200.f));
```

```cpp
auto fft = FFT (myAudioBlock, 44100.f);
fft.printDebug(); // prints the bins
REQUIRE (fft.strongestFrequencyBin() == fft.frequencyBinFor (220.f));
```

```cpp
auto fft = FFT (myAudioBlock, 44100.f);
REQUIRE_THAT (fft.strongFrequencyBins(), Catch::Matchers::Contains (fft.frequencyBinFor (220.f)));
```

## Installing

Prerequisites:

1. Catch2 
2. The [melatonin_audio_sparklines](https://github.com/sudara/melatonin_audio_sparklines) JUCE module. It provides Catch2 with fancy descriptions of AudioBlocks when tests fail.

### Add the module

Set up with a git submodule tracking the `main` branch:

```git
git submodule add -b main https://github.com/sudara/melatonin_audio_block_test_helpers modules/melatonin_audio_block_test_helpers
git commit -m "Added melatonin_audio_block_test_helpers submodule."
```

To update these test helpers, you can:
```git
git submodule update --remote --merge modules/melatonin_audio_block_test_helpers
```

If you use CMake, you can inform JUCE about the module in your `CMakeLists.txt`:
```
juce_add_module("modules/melatonin_audio_block_test_helpers")
```

### Include the module and use the melatonin namespace

Add the following to any .cpp you'd like to use the helpers:

```cpp
#include "melatonin_audio_block_test_helpers/melatonin_audio_block_test_helpers.h"

// make your life easier while you are at it...
using namespace melatonin;

```

## Caveats

1. This is just a random assortment of things I actually use for my tests.
2. `AudioBlock` only (I don't use `AudioBuffer` much, but would be willing to look at a PR!).
