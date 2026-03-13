#pragma once

#include <juce_core/juce_core.h>
#include <functional>
#include <vector>

struct ConversationMessage
{
    juce::String role;
    juce::String content;
};

struct ConversationContext
{
    std::vector<ConversationMessage> messages;
    std::map<juce::String, float> currentParameters;
};

struct ControlInfo
{
    juce::String id;
    juce::String label;
    float value;
    float min;
    float max;
};

struct IntentResult
{
    bool success = false;
    juce::String intentId;
    float magnitude = 1.0f;
    juce::String title;
    juce::StringArray summary;
    std::vector<ControlInfo> controls;
    juce::String errorMessage;
    std::map<juce::String, float> parameterOverrides;
};

class LLMService
{
public:
    virtual ~LLMService() = default;
    
    virtual void processMessage(
        const juce::String& userMessage,
        const ConversationContext& context,
        std::function<void(IntentResult)> onComplete
    ) = 0;
    
    virtual void setApiKey(const juce::String& apiKey) = 0;
    virtual bool hasApiKey() const = 0;
};
