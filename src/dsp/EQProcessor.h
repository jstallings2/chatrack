#pragma once

#include "ChainProcessor.h"
#include <array>

class EQProcessor : public ChainProcessor
{
public:
    static constexpr int NUM_BANDS = 4;
    
    EQProcessor(juce::AudioProcessorValueTreeState& apvts);
    ~EQProcessor() override = default;
    
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(juce::dsp::AudioBlock<float>& block) override;
    void reset() override;
    
    void setParameter(const juce::String& id, float value) override;
    float getParameter(const juce::String& id) const override;
    
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;

private:
    void updateCoefficients(int band);
    
    struct Band
    {
        std::atomic<float>* freqParam = nullptr;
        std::atomic<float>* gainParam = nullptr;
        std::atomic<float>* qParam = nullptr;
        std::atomic<float>* enabledParam = nullptr;
        
        juce::dsp::IIR::Filter<float> filterL;
        juce::dsp::IIR::Filter<float> filterR;
        
        float lastFreq = 0.0f;
        float lastGain = 0.0f;
        float lastQ = 0.0f;
    };
    
    std::array<Band, NUM_BANDS> bands;
    double sampleRate = 44100.0;
    bool enabled = true;
    
    juce::AudioProcessorValueTreeState& apvts;
};
