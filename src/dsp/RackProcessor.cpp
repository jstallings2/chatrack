#include "RackProcessor.h"

RackProcessor::RackProcessor(juce::AudioProcessorValueTreeState& apvts)
    : eq(apvts),
      compressor(apvts),
      saturation(apvts),
      stereoWidth(apvts),
      apvts(apvts)
{
}

void RackProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    eq.prepare(spec);
    compressor.prepare(spec);
    saturation.prepare(spec);
    stereoWidth.prepare(spec);
}

void RackProcessor::process(juce::dsp::AudioBlock<float>& block)
{
    if (masterBypassed.load())
        return;

    eq.process(block);
    compressor.process(block);
    saturation.process(block);
    stereoWidth.process(block);
}

void RackProcessor::reset()
{
    eq.reset();
    compressor.reset();
    saturation.reset();
    stereoWidth.reset();
}

void RackProcessor::setParameter(const juce::String& id, float value)
{
    if (auto* param = apvts.getParameter(id))
    {
        param->setValueNotifyingHost(param->convertTo0to1(value));
    }
}

float RackProcessor::getParameter(const juce::String& id) const
{
    if (auto* param = apvts.getRawParameterValue(id))
    {
        return param->load();
    }
    return 0.0f;
}
