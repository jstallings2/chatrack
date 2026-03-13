#include "CompressorProcessor.h"

CompressorProcessor::CompressorProcessor(juce::AudioProcessorValueTreeState& apvts)
    : apvts(apvts)
{
    thresholdParam = apvts.getRawParameterValue("comp.threshold");
    ratioParam = apvts.getRawParameterValue("comp.ratio");
    attackParam = apvts.getRawParameterValue("comp.attack");
    releaseParam = apvts.getRawParameterValue("comp.release");
    enabledParam = apvts.getRawParameterValue("comp.enabled");
}

void CompressorProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    compressor.prepare(spec);
    updateParameters();
}

void CompressorProcessor::process(juce::dsp::AudioBlock<float>& block)
{
    if (!enabled || (enabledParam && *enabledParam < 0.5f))
        return;
    
    float threshold = thresholdParam ? thresholdParam->load() : -20.0f;
    float ratio = ratioParam ? ratioParam->load() : 4.0f;
    float attack = attackParam ? attackParam->load() : 10.0f;
    float release = releaseParam ? releaseParam->load() : 100.0f;
    
    if (threshold != lastThreshold || ratio != lastRatio ||
        attack != lastAttack || release != lastRelease)
    {
        updateParameters();
        lastThreshold = threshold;
        lastRatio = ratio;
        lastAttack = attack;
        lastRelease = release;
    }
    
    juce::dsp::ProcessContextReplacing<float> context(block);
    compressor.process(context);
}

void CompressorProcessor::reset()
{
    compressor.reset();
}

void CompressorProcessor::updateParameters()
{
    if (thresholdParam)
        compressor.setThreshold(thresholdParam->load());
    if (ratioParam)
        compressor.setRatio(ratioParam->load());
    if (attackParam)
        compressor.setAttack(attackParam->load());
    if (releaseParam)
        compressor.setRelease(releaseParam->load());
}

void CompressorProcessor::setParameter(const juce::String& id, float value)
{
    if (auto* param = apvts.getParameter(id))
    {
        param->setValueNotifyingHost(param->convertTo0to1(value));
    }
}

float CompressorProcessor::getParameter(const juce::String& id) const
{
    if (auto* param = apvts.getRawParameterValue(id))
    {
        return param->load();
    }
    return 0.0f;
}

bool CompressorProcessor::isEnabled() const
{
    return enabled && (enabledParam == nullptr || *enabledParam > 0.5f);
}

void CompressorProcessor::setEnabled(bool newEnabled)
{
    enabled = newEnabled;
}
