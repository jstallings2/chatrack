#pragma once

#include "ChainProcessor.h"

class CompressorProcessor : public ChainProcessor
{
public:
    CompressorProcessor(juce::AudioProcessorValueTreeState& apvts);
    ~CompressorProcessor() override = default;
    
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(juce::dsp::AudioBlock<float>& block) override;
    void reset() override;
    
    void setParameter(const juce::String& id, float value) override;
    float getParameter(const juce::String& id) const override;
    
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;

private:
    void updateParameters();
    
    juce::dsp::Compressor<float> compressor;
    
    std::atomic<float>* thresholdParam = nullptr;
    std::atomic<float>* ratioParam = nullptr;
    std::atomic<float>* attackParam = nullptr;
    std::atomic<float>* releaseParam = nullptr;
    std::atomic<float>* enabledParam = nullptr;
    
    float lastThreshold = 0.0f;
    float lastRatio = 0.0f;
    float lastAttack = 0.0f;
    float lastRelease = 0.0f;
    
    bool enabled = true;
    
    juce::AudioProcessorValueTreeState& apvts;
};
