/*
BEGIN_JUCE_MODULE_DECLARATION

 ID:               melatonin_audio_block_test_helpers
 vendor:           Sudara
 version:          1.0.0
 name:             Melatonin AudioBlock Test Helpers
 description:      Nobody Tests Audio Code, but if don't forget to add Catch2 to your build!
 license:          MIT
 dependencies:     juce_dsp, melatonin_audio_sparklines

END_JUCE_MODULE_DECLARATION
*/

#pragma once
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_templated.hpp>

#include <juce_dsp/juce_dsp.h>
#include <melatonin_audio_sparklines/melatonin_audio_sparklines.h>

#include "melatonin/AudioBlockTestHelpers.h"
#include "melatonin/AudioBlockMatchers.h"
#include "melatonin/AudioBlockFFT.h"
