# Melatonin AudioBlock Test Helpers

This module contains:

1. Catch2 Matchers that operate on `juce::dsp::AudioBlock`
2. A collection of C++ free functions that take an `dsp::AudioBlock<float>`.
   
I'm publishing it, mainly to share how I use Catch2 with JUCE.

## Installing

Prerequisites:

1. Catch2 
2. The [melatonin_audio_sparklines](https://github.com/sudara/melatonin_audio_sparklines) module. It provides Catch2 with fancy descriptions of AudioBlocks when tests fail.

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

...

using namespace melatonin;

```

## Catch2 Matchers

The following matchers all have free function versions that are prepended with `block`.

So for example you can do either the matcher style:

```
REQUIRE_THAT(fundamentalResult, isFilledUntil(256));
```

Or the free function style:

```
REQUIRE(blockIsFilledUntil(fundamentalResult, 256);
```

The matcher style gives much more detail on the AudioBlock when the test fails, including showing sparkline versions of both the expected and actual AudioBlock.

### isFilled

The block is completely filled with data. No more than 1 consecutive zero is allowed. 

### isFilledUntil

```cpp
REQUIRE_THAT(fundamentalResult, isFilledUntil(256));
```

Up to and including sample 256, the block is filled and doesn't contain consecutive zeros.

### isFilledAfter

```cpp
REQUIRE_THAT(fundamentalResult, isFilledAfter(256));
```

Starting with this sample, the block is filled. 

It's ok if sample 256 is zero (eg, start of a sine wave), we are just making sure we don't have consecutive zeros.

### isEmpty

The block only has 0s.

### isEmptyUntil

The block has only zeros up to and including this sample number.

### isEmptyAfter

The block is empty after this sample number.

## Caveats

1. This is just a random assortment of things I actually use for my tests.
2. `AudioBlock` only (I don't use `AudioBuffer` much).
