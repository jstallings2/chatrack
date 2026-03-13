#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <memory>
#include <vector>

#include "ChainProcessor.h"
#include "EQProcessor.h"
#include "CompressorProcessor.h"
#include "SaturationProcessor.h"
#include "StereoWidthProcessor.h"

class RackProcessor
{
public:
    explicit RackProcessor(juce::AudioProcessorValueTreeState& apvts);
    ~RackProcessor() = default;
    
    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::dsp::AudioBlock<float>& block);
    void reset();
    
    EQProcessor& getEQ() { return eq; }
    CompressorProcessor& getCompressor() { return compressor; }
    SaturationProcessor& getSaturation() { return saturation; }
    StereoWidthProcessor& getStereoWidth() { return stereoWidth; }
    
    void setParameter(const juce::String& id, float value);
    float getParameter(const juce::String& id) const;

private:
    EQProcessor eq;
    CompressorProcessor compressor;
    SaturationProcessor saturation;
    StereoWidthProcessor stereoWidth;
    
    juce::AudioProcessorValueTreeState& apvts;
};
