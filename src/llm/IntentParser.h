#pragma once

#include "LLMService.h"

#include <optional>

class IntentParser
{
public:
    IntentParser() = default;
    ~IntentParser() = default;

    IntentResult parse(const juce::String& llmResponse) const;
    IntentResult parseContent(const juce::String& jsonContent) const;

    static juce::String unwrapJsonPayload(const juce::String& raw);
    static juce::String sanitizeDashes(const juce::String& text);
    static juce::String normalizeOverrideKey(const juce::String& key);
    static float parseNumericVar(const juce::var& value, bool& ok);
    static void readParameterOverrides(const juce::var& overridesVar,
                                       std::map<juce::String, float>& out);
    static std::optional<float> extractFrequencyHz(const juce::String& text);
    static bool mentionsHighShelfTarget(const juce::String& text);
    static bool mentionsMudRequest(const juce::String& text);
    static void applyOverrideFallbacks(IntentResult& result, const juce::String& userMessage);
    static juce::DynamicObject::Ptr toLogObject(const IntentResult& result);

private:
    juce::String extractContentFromResponse(const juce::String& response) const;
};
