# Melatonin AudioBlock Test Helpers

This module contains two things

1. A collection of C++ free functions that take an `dsp::dsp::AudioBlock` or a `juce::AudioBuffer`
2. [Catch2](https://github.com/catchorg/Catch2) matchers using the free functions that make it convenient to do things like `REQUIRE_THAT (result, isValidAudio())`


## Catch2 Matchers

The Catch2 matchers gives a solid amount of detail on AudioBlocks when a test fails. This includes displaying summary stats and [sparkline](https://github.com/sudara/melatonin_audio_sparklines) for any AudioBlock:

```
REQUIRE_THAT(myAudioBlock, isEqualTo (someOtherBlock))
with expansion:

Block is 1 channel, 480 samples, min -0.766242, max 0.289615, 100% filled
[0—⎻—x—⎼⎽_⎽⎼—]
 is equal to 
Block is 1 channel, 480 samples, min -1, max 1, 100% filled
[0—⎻⎺‾⎺⎻—x—⎼⎽_⎽⎼—]
```

### isEqualTo

```cpp
REQUIRE_THAT (myAudioBlock, isEqualTo (someOtherBlock));
```

This compares each channel of the two blocks and confirms each sample is within a tolerance of `std::numeric_limits<float>::epsilon() * 100`. Don't @ me, but assuming you are doing things like multiplying a number of floats together to arrive at the end result, you probably don't want this tolerance any tighter.

However, you can loosen or tighten to your desire by passing an absolute tolerance:

```cpp
REQUIRE_THAT (myAudioBlock, isEqualTo (someOtherBlock), 0.0005f));
```

### isFilled

```cpp
REQUIRE_THAT (myAudioBlock, isFilled);
```

Passes when the block is completely filled. No more than 1 consecutive zero is allowed. 

### isFilledUntil

```cpp
REQUIRE_THAT (myAudioBlock, isFilledUntil(256));
```

Passes when the block is filled (doesn't contain consecutive zeros) up to to and including sample 256.

### isFilledAfter

```cpp
REQUIRE_THAT (myAudioBlock, isFilledAfter(256));
```

Passes if, starting with this sample, the block is filled and doesn't contain consecutive zeros.

If the sample specified is zero (eg, start of a sine wave) but there are no consecutive zeros, it'll pass. 

Fails if the block ends at the sample specified.

### isFilledBetween

```cpp
REQUIRE_THAT (myAudioBlock, isFilledBetween(64, 128));
```
Passes when sample values 64 and 128 have a non zero value for all channels.

### isEmpty

```cpp
REQUIRE_THAT (myAudioBlock, isEmpty);
```

The block only has 0s.

### isEmptyUntil

```cpp
REQUIRE_THAT (myAudioBlock, isEmptyUntil(256));
```

Passes when the block has only zeros up to and including this sample number.

### isEmptyAfter

```cpp
REQUIRE_THAT (myAudioBlock, isEmptyAfter(256));
```

Passes when the block only contains zeros after this sample number, or when the block ends at this point.

## Other helpers

The matchers above call out to free functions test helpers (prepended with `block`) which can be used seperately.

So for example you can do use the Catch2 matcher style:

```cpp
REQUIRE_THAT (myAudioBlock, isFilledUntil(256));
```

Or the free function style:

```cpp
REQUIRE (blockIsFilledUntil(myAudioBlock, 256);
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

This one is my favorite.

Use this if you know the frequencies you expect to encounter in the AudioBlock. You'll need to pass it the sample rate you are using.

This uses frequency correlation and compares your AudioBlock against sine and cosine probes. It's essentially the same thing as what goes on under the hood with DFT, but for exactly the frequency you specify. 

This is a much more accurate way to confirm the presence of a known frequency than FFT.

### FFT

There are various FFT related functions available which rely on creating an instance of the FFT class.

Tip: Prefer the `magnitudeOfFrequency` helper if you know the frequency you are expecting and wish to somewhat accurately confirm magnitude. FFT is inherently messy. You'll get better results when your expected frequencies are in the middle of FFT bins. 


However, you can still sorta sloppily check for the strongest frequency:

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

If you use CMake, inform JUCE about the module in your `CMakeLists.txt`:
```cmake
juce_add_module("modules/melatonin_audio_block_test_helpers")
```

And link your target against it (using `PRIVATE`, as juce recommends for modules):

```cmake
target_link_libraries(my_target PRIVATE melatonin_audio_block_test_helpers)
```

### Include the module and use the melatonin namespace

Add the following to any .cpp you'd like to use the helpers:

```cpp
#include "melatonin_audio_block_test_helpers/melatonin_audio_block_test_helpers.h"

// make your life easier while you are at it...
using namespace melatonin;

```

## Caveats

1. The matchers are the "new style" and require Catch2 v3.x.
2. This is just a random assortment of things I actually use for my tests! I'm open to more things being added, submit a PR!

## Acknowledgements

* Thanks to @chrhaase for improving module compatibility and performance. 
