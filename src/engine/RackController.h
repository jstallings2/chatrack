#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "../dsp/RackProcessor.h"
#include "RecipeEngine.h"
#include "ActionHistory.h"

class RackController
{
public:
    RackController(RackProcessor& rackProcessor,
                   ActionHistory& actionHistory,
                   juce::AudioProcessorValueTreeState& apvts);
    ~RackController() = default;
    
    void applyActions(const ResolvedRecipe& recipe,
                      const juce::String& intentId,
                      float magnitude = 0.6f,
                      const juce::String& title = {});
    void setParameterDirectly(const juce::String& paramId, float value);
    float getParameter(const juce::String& paramId) const;
    
    std::map<juce::String, float> getCurrentState() const;
    std::map<juce::String, float> getStateForParameters(const std::vector<juce::String>& paramIds) const;

private:
    RackProcessor& rackProcessor;
    ActionHistory& actionHistory;
    juce::AudioProcessorValueTreeState& apvts;
    
    int actionCounter = 0;
};
