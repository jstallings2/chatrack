#include "StereoWidthProcessor.h"

StereoWidthProcessor::StereoWidthProcessor(juce::AudioProcessorValueTreeState& apvts)
    : apvts(apvts)
{
    widthParam = apvts.getRawParameterValue("stereo.width");
    enabledParam = apvts.getRawParameterValue("stereo.enabled");
}

void StereoWidthProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    juce::ignoreUnused(spec);
}

void StereoWidthProcessor::process(juce::dsp::AudioBlock<float>& block)
{
    if (!enabled || (enabledParam && *enabledParam < 0.5f))
        return;
    
    if (block.getNumChannels() < 2)
        return;
    
    float width = widthParam ? widthParam->load() : 1.0f;
    
    if (std::abs(width - 1.0f) < 0.001f)
        return;
    
    auto* leftChannel = block.getChannelPointer(0);
    auto* rightChannel = block.getChannelPointer(1);
    
    for (size_t i = 0; i < block.getNumSamples(); ++i)
    {
        float left = leftChannel[i];
        float right = rightChannel[i];
        
        float mid = (left + right) * 0.5f;
        float side = (left - right) * 0.5f;
        
        side *= width;
        
        leftChannel[i] = mid + side;
        rightChannel[i] = mid - side;
    }
}

void StereoWidthProcessor::reset()
{
}

void StereoWidthProcessor::setParameter(const juce::String& id, float value)
{
    if (auto* param = apvts.getParameter(id))
    {
        param->setValueNotifyingHost(param->convertTo0to1(value));
    }
}

float StereoWidthProcessor::getParameter(const juce::String& id) const
{
    if (auto* param = apvts.getRawParameterValue(id))
    {
        return param->load();
    }
    return 0.0f;
}

bool StereoWidthProcessor::isEnabled() const
{
    return enabled && (enabledParam == nullptr || *enabledParam > 0.5f);
}

void StereoWidthProcessor::setEnabled(bool newEnabled)
{
    enabled = newEnabled;
}
