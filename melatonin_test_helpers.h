/*
BEGIN_JUCE_MODULE_DECLARATION

 ID:               melatonin_test_helpers
 vendor:           Sudara
 version:          1.0.0
 name:             Melatonin Catch2 Test Helpers
 description:      Nobody Tests Audio Code (but don't forget to add Catch2 to your build!)
 license:          MIT
 dependencies:     juce_audio_processors,juce_dsp,melatonin_audio_sparklines

END_JUCE_MODULE_DECLARATION
*/

#pragma once
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_templated.hpp>

#include "juce_audio_processors/juce_audio_processors.h"
#include <juce_dsp/juce_dsp.h>
#include <melatonin_audio_sparklines/melatonin_audio_sparklines.h>

#include "melatonin/AudioBlockFFT.h"
#include "melatonin/block_and_buffer_test_helpers.h"
#include "melatonin/block_and_buffer_matchers.h"
#include "melatonin/vector_matchers.h"
#include "melatonin/mock_playheads.h"
#include "melatonin/parameter_test_helpers.h"
