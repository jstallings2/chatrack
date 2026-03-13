#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>

class ChainProcessor
{
public:
    virtual ~ChainProcessor() = default;
    
    virtual void prepare(const juce::dsp::ProcessSpec& spec) = 0;
    virtual void process(juce::dsp::AudioBlock<float>& block) = 0;
    virtual void reset() = 0;
    
    virtual void setParameter(const juce::String& id, float value) = 0;
    virtual float getParameter(const juce::String& id) const = 0;
    
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
};
