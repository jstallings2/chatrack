#include "RackController.h"

RackController::RackController(
    RackProcessor& rackProcessor,
    ActionHistory& actionHistory,
    juce::AudioProcessorValueTreeState& apvts)
    : rackProcessor(rackProcessor),
      actionHistory(actionHistory),
      apvts(apvts)
{
}

void RackController::applyActions(const ResolvedRecipe& recipe,
                                  const juce::String& intentId,
                                  float magnitude,
                                  const juce::String& title)
{
    std::vector<juce::String> paramIds;
    for (const auto& action : recipe.actions)
    {
        paramIds.push_back(action.parameterId);
    }
    
    std::map<juce::String, float> previousValues = getStateForParameters(paramIds);
    
    std::map<juce::String, float> newValues;
    for (const auto& action : recipe.actions)
    {
        setParameterDirectly(action.parameterId, action.value);
        newValues[action.parameterId] = action.value;
    }
    
    juce::String actionId = "action_" + juce::String(++actionCounter);
    actionHistory.pushSnapshot(actionId, intentId, previousValues, newValues, magnitude, title);
}

void RackController::setParameterDirectly(const juce::String& paramId, float value)
{
    if (auto* param = apvts.getParameter(paramId))
    {
        DBG("Setting parameter " << paramId << " to " << value << " (normalized: " << param->convertTo0to1(value) << ")");
        param->setValueNotifyingHost(param->convertTo0to1(value));
    }
    else
    {
        DBG("WARNING: Parameter not found: " << paramId);
    }
}

float RackController::getParameter(const juce::String& paramId) const
{
    if (auto* param = apvts.getRawParameterValue(paramId))
    {
        return param->load();
    }
    return 0.0f;
}

std::map<juce::String, float> RackController::getCurrentState() const
{
    std::map<juce::String, float> state;
    
    for (auto* param : apvts.processor.getParameters())
    {
        if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param))
        {
            state[rangedParam->getParameterID()] = 
                rangedParam->convertFrom0to1(rangedParam->getValue());
        }
    }
    
    return state;
}

std::map<juce::String, float> RackController::getStateForParameters(
    const std::vector<juce::String>& paramIds) const
{
    std::map<juce::String, float> state;
    
    for (const auto& paramId : paramIds)
    {
        if (auto* param = apvts.getRawParameterValue(paramId))
        {
            state[paramId] = param->load();
        }
    }
    
    return state;
}
