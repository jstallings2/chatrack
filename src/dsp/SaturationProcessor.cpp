#include "SaturationProcessor.h"
#include <cmath>

SaturationProcessor::SaturationProcessor(juce::AudioProcessorValueTreeState& apvts)
    : apvts(apvts)
{
    driveParam = apvts.getRawParameterValue("sat.drive");
    mixParam = apvts.getRawParameterValue("sat.mix");
    enabledParam = apvts.getRawParameterValue("sat.enabled");
    
    waveshaper.functionToUse = softClipFunction;
}

float SaturationProcessor::softClipFunction(float x)
{
    return std::tanh(x);
}

void SaturationProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    waveshaper.prepare(spec);
    inputGain.prepare(spec);
    outputGain.prepare(spec);
    
    inputGain.setGainLinear(1.0f);
    outputGain.setGainLinear(1.0f);
}

void SaturationProcessor::process(juce::dsp::AudioBlock<float>& block)
{
    if (!enabled || (enabledParam && *enabledParam < 0.5f))
        return;
    
    float drive = driveParam ? driveParam->load() : 0.0f;
    float mix = mixParam ? mixParam->load() : 1.0f;
    
    if (drive < 0.001f)
        return;
    
    if (drive != lastDrive)
    {
        float inputGainDb = drive * 24.0f;
        float outputGainDb = -drive * 12.0f;
        
        inputGain.setGainDecibels(inputGainDb);
        outputGain.setGainDecibels(outputGainDb);
        
        lastDrive = drive;
    }
    
    juce::AudioBuffer<float> dryBuffer(static_cast<int>(block.getNumChannels()),
                                        static_cast<int>(block.getNumSamples()));
    
    for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
    {
        dryBuffer.copyFrom(static_cast<int>(ch), 0,
                           block.getChannelPointer(ch),
                           static_cast<int>(block.getNumSamples()));
    }
    
    juce::dsp::ProcessContextReplacing<float> context(block);
    inputGain.process(context);
    waveshaper.process(context);
    outputGain.process(context);
    
    if (mix < 0.999f)
    {
        for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
        {
            auto* wetData = block.getChannelPointer(ch);
            auto* dryData = dryBuffer.getReadPointer(static_cast<int>(ch));
            
            for (size_t i = 0; i < block.getNumSamples(); ++i)
            {
                wetData[i] = dryData[i] * (1.0f - mix) + wetData[i] * mix;
            }
        }
    }
}

void SaturationProcessor::reset()
{
    waveshaper.reset();
    inputGain.reset();
    outputGain.reset();
}

void SaturationProcessor::setParameter(const juce::String& id, float value)
{
    if (auto* param = apvts.getParameter(id))
    {
        param->setValueNotifyingHost(param->convertTo0to1(value));
    }
}

float SaturationProcessor::getParameter(const juce::String& id) const
{
    if (auto* param = apvts.getRawParameterValue(id))
    {
        return param->load();
    }
    return 0.0f;
}

bool SaturationProcessor::isEnabled() const
{
    return enabled && (enabledParam == nullptr || *enabledParam > 0.5f);
}

void SaturationProcessor::setEnabled(bool newEnabled)
{
    enabled = newEnabled;
}
