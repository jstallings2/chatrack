#include "EQProcessor.h"

EQProcessor::EQProcessor(juce::AudioProcessorValueTreeState& apvts)
    : apvts(apvts)
{
    for (int i = 0; i < NUM_BANDS; ++i)
    {
        juce::String prefix = "eq.band" + juce::String(i) + ".";
        bands[i].freqParam = apvts.getRawParameterValue(prefix + "freq");
        bands[i].gainParam = apvts.getRawParameterValue(prefix + "gain");
        bands[i].qParam = apvts.getRawParameterValue(prefix + "q");
        bands[i].enabledParam = apvts.getRawParameterValue(prefix + "enabled");
    }
}

void EQProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    
    for (auto& band : bands)
    {
        band.filterL.prepare(spec);
        band.filterR.prepare(spec);
        band.lastFreq = 0.0f;
        band.lastGain = 0.0f;
        band.lastQ = 0.0f;
    }
    
    for (int i = 0; i < NUM_BANDS; ++i)
    {
        updateCoefficients(i);
    }
}

void EQProcessor::process(juce::dsp::AudioBlock<float>& block)
{
    if (!enabled)
        return;
    
    for (int i = 0; i < NUM_BANDS; ++i)
    {
        if (bands[i].enabledParam == nullptr)
            continue;
            
        bool bandEnabled = *bands[i].enabledParam > 0.5f;
        if (!bandEnabled)
            continue;
        
        float freq = *bands[i].freqParam;
        float gain = *bands[i].gainParam;
        float q = *bands[i].qParam;
        
        if (freq != bands[i].lastFreq || gain != bands[i].lastGain || q != bands[i].lastQ)
        {
            updateCoefficients(i);
            bands[i].lastFreq = freq;
            bands[i].lastGain = gain;
            bands[i].lastQ = q;
        }
        
        if (block.getNumChannels() >= 1)
        {
            auto channelBlock = block.getSingleChannelBlock(0);
            juce::dsp::ProcessContextReplacing<float> context(channelBlock);
            bands[i].filterL.process(context);
        }
        
        if (block.getNumChannels() >= 2)
        {
            auto channelBlock = block.getSingleChannelBlock(1);
            juce::dsp::ProcessContextReplacing<float> context(channelBlock);
            bands[i].filterR.process(context);
        }
    }
}

void EQProcessor::reset()
{
    for (auto& band : bands)
    {
        band.filterL.reset();
        band.filterR.reset();
    }
}

void EQProcessor::updateCoefficients(int bandIndex)
{
    if (bandIndex < 0 || bandIndex >= NUM_BANDS)
        return;
    
    auto& band = bands[bandIndex];
    
    if (band.freqParam == nullptr || band.gainParam == nullptr || band.qParam == nullptr)
        return;
    
    float freq = *band.freqParam;
    float gain = *band.gainParam;
    float q = *band.qParam;
    
    freq = juce::jlimit(20.0f, static_cast<float>(sampleRate * 0.49), freq);
    q = juce::jmax(0.1f, q);
    
    auto coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRate, freq, q, juce::Decibels::decibelsToGain(gain)
    );
    
    *band.filterL.coefficients = *coefficients;
    *band.filterR.coefficients = *coefficients;
}

void EQProcessor::setParameter(const juce::String& id, float value)
{
    if (auto* param = apvts.getParameter(id))
    {
        param->setValueNotifyingHost(param->convertTo0to1(value));
    }
}

float EQProcessor::getParameter(const juce::String& id) const
{
    if (auto* param = apvts.getRawParameterValue(id))
    {
        return param->load();
    }
    return 0.0f;
}

bool EQProcessor::isEnabled() const
{
    return enabled;
}

void EQProcessor::setEnabled(bool newEnabled)
{
    enabled = newEnabled;
}
