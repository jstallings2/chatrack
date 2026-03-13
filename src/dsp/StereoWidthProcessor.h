#pragma once

#include "ChainProcessor.h"

class StereoWidthProcessor : public ChainProcessor
{
public:
    StereoWidthProcessor(juce::AudioProcessorValueTreeState& apvts);
    ~StereoWidthProcessor() override = default;
    
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(juce::dsp::AudioBlock<float>& block) override;
    void reset() override;
    
    void setParameter(const juce::String& id, float value) override;
    float getParameter(const juce::String& id) const override;
    
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;

private:
    std::atomic<float>* widthParam = nullptr;
    std::atomic<float>* enabledParam = nullptr;
    
    bool enabled = true;
    
    juce::AudioProcessorValueTreeState& apvts;
};
