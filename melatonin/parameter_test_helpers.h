#pragma once

namespace melatonin
{
    // You'll want to use this anytime you call someParam->setValueNotifyingHost (1.0f);
    static inline void waitForParameterChange()
    {
        juce::MessageManager::getInstance()->runDispatchLoopUntil (1);
        juce::Timer::callPendingTimersSynchronously();
    }

    // the apvts can take up to half a second (!) due to back off to flush its state
    // the only way to circumvent and get flushParameterValuesToValueTree to fire is by copying state
    // so we (pointlessly) copy the state and then run the dispatch loop
    static inline void flushAPVTS (juce::AudioProcessorValueTreeState& apvts)
    {
        auto unusedState = apvts.copyState();
        waitForParameterChange();
    }
}
