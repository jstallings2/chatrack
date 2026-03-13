#pragma once

#include "LLMService.h"
#include "IntentParser.h"

class CloudLLMProvider : public LLMService
{
public:
    CloudLLMProvider();
    ~CloudLLMProvider() override = default;
    
    void processMessage(
        const juce::String& userMessage,
        const ConversationContext& context,
        std::function<void(IntentResult)> onComplete
    ) override;
    
    void setApiKey(const juce::String& apiKey) override;
    bool hasApiKey() const override;
    
    void setModelProvider(const juce::String& provider);
    void setModelName(const juce::String& model);

private:
    juce::String buildSystemPrompt() const;
    juce::String buildRequestBody(const juce::String& userMessage,
                                   const ConversationContext& context) const;
    IntentResult parseResponse(const juce::String& responseJson) const;
    
    juce::String apiKey;
    juce::String modelProvider = "openai";
    juce::String modelName = "gpt-4o-mini";
    
    IntentParser intentParser;
    
    static constexpr const char* OPENAI_API_URL = "https://api.openai.com/v1/chat/completions";
    static constexpr const char* ANTHROPIC_API_URL = "https://api.anthropic.com/v1/messages";
};
