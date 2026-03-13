#pragma once

#include "ChainProcessor.h"

class SaturationProcessor : public ChainProcessor
{
public:
    SaturationProcessor(juce::AudioProcessorValueTreeState& apvts);
    ~SaturationProcessor() override = default;
    
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(juce::dsp::AudioBlock<float>& block) override;
    void reset() override;
    
    void setParameter(const juce::String& id, float value) override;
    float getParameter(const juce::String& id) const override;
    
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;

private:
    static float softClipFunction(float x);
    
    juce::dsp::WaveShaper<float> waveshaper;
    juce::dsp::Gain<float> inputGain;
    juce::dsp::Gain<float> outputGain;
    
    std::atomic<float>* driveParam = nullptr;
    std::atomic<float>* mixParam = nullptr;
    std::atomic<float>* enabledParam = nullptr;
    
    float lastDrive = 0.0f;
    bool enabled = true;
    
    juce::AudioProcessorValueTreeState& apvts;
};
