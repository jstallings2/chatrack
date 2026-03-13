#pragma once

#include "LLMService.h"

class IntentParser
{
public:
    IntentParser() = default;
    ~IntentParser() = default;
    
    IntentResult parse(const juce::String& llmResponse) const;
    IntentResult parseContent(const juce::String& jsonContent) const;

private:
    juce::String extractContentFromResponse(const juce::String& response) const;
};
