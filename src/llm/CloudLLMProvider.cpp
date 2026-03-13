#include "CloudLLMProvider.h"

CloudLLMProvider::CloudLLMProvider()
{
}

void CloudLLMProvider::setApiKey(const juce::String& key)
{
    apiKey = key;
}

bool CloudLLMProvider::hasApiKey() const
{
    return apiKey.isNotEmpty();
}

void CloudLLMProvider::setModelProvider(const juce::String& provider)
{
    modelProvider = provider;
}

void CloudLLMProvider::setModelName(const juce::String& model)
{
    modelName = model;
}

juce::String CloudLLMProvider::buildSystemPrompt() const
{
    return R"(You are Chatrack, an AI mixing assistant that translates natural language descriptions of sound into specific mixing actions.

Your job is to:
1. Understand the user's intent about how they want the sound to change
2. Choose the appropriate mixing action (intent)
3. Determine the magnitude (0.0-1.0) based on words like "slightly", "a lot", "subtle", etc.

Available intents:
- reduce_mud: Cut low-mid frequencies (200-400Hz) to reduce muddiness, boxiness
- brighten: Add high frequency presence and air
- add_warmth: Add subtle saturation for analog warmth
- tame_harshness: Cut harsh frequencies (2-5kHz)
- make_forward: Boost presence for forward projection
- widen: Increase stereo width
- narrow: Decrease stereo width
- add_saturation: Add harmonic distortion
- glue: Apply compression for cohesion
- make_punchy: Enhance transients and punch

Magnitude guidelines:
- "slightly", "subtle", "a bit" → 0.3-0.5
- "moderately", "some" → 0.5-0.7
- "a lot", "significantly", "really" → 0.7-0.9
- no modifier → 0.6 (default)

You MUST respond with valid JSON in this exact format:
{
  "intent": "intent_id",
  "magnitude": 0.6,
  "title": "Action Title",
  "summary": ["First bullet point", "Second bullet point"],
  "confidence": 0.9
}

If you cannot understand the request, respond with:
{
  "intent": "unknown",
  "error": "Brief explanation of why you couldn't understand"
})";
}

juce::String CloudLLMProvider::buildRequestBody(
    const juce::String& userMessage,
    const ConversationContext& context) const
{
    juce::DynamicObject::Ptr requestObj = new juce::DynamicObject();
    
    if (modelProvider == "openai")
    {
        requestObj->setProperty("model", modelName);
        
        juce::Array<juce::var> messages;
        
        juce::DynamicObject::Ptr systemMsg = new juce::DynamicObject();
        systemMsg->setProperty("role", "system");
        systemMsg->setProperty("content", buildSystemPrompt());
        messages.add(juce::var(systemMsg.get()));
        
        for (const auto& msg : context.messages)
        {
            juce::DynamicObject::Ptr msgObj = new juce::DynamicObject();
            msgObj->setProperty("role", msg.role);
            msgObj->setProperty("content", msg.content);
            messages.add(juce::var(msgObj.get()));
        }
        
        juce::DynamicObject::Ptr userMsg = new juce::DynamicObject();
        userMsg->setProperty("role", "user");
        userMsg->setProperty("content", userMessage);
        messages.add(juce::var(userMsg.get()));
        
        requestObj->setProperty("messages", messages);
        requestObj->setProperty("temperature", 0.3);
        requestObj->setProperty("max_tokens", 500);
        
        juce::DynamicObject::Ptr responseFormat = new juce::DynamicObject();
        responseFormat->setProperty("type", "json_object");
        requestObj->setProperty("response_format", juce::var(responseFormat.get()));
    }
    
    return juce::JSON::toString(juce::var(requestObj.get()));
}

void CloudLLMProvider::processMessage(
    const juce::String& userMessage,
    const ConversationContext& context,
    std::function<void(IntentResult)> onComplete)
{
    if (!hasApiKey())
    {
        IntentResult result;
        result.success = false;
        result.errorMessage = "No API key configured. Please add your API key in settings.";
        onComplete(result);
        return;
    }
    
    juce::String requestBody = buildRequestBody(userMessage, context);
    DBG("LLM Request body: " << requestBody.substring(0, 200) << "...");
    
    juce::URL url(OPENAI_API_URL);
    url = url.withPOSTData(requestBody);
    
    // Build headers string in HTTP format
    juce::String headersString = "Content-Type: application/json\r\n";
    headersString += "Authorization: Bearer " + apiKey + "\r\n";
    
    DBG("Making API request to: " << OPENAI_API_URL);
    
    int statusCode = 0;
    auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inPostData)
        .withExtraHeaders(headersString)
        .withConnectionTimeoutMs(30000)
        .withStatusCode(&statusCode);
    
    std::unique_ptr<juce::InputStream> stream = url.createInputStream(options);
    
    DBG("API response status code: " << statusCode);
    
    if (stream == nullptr)
    {
        DBG("Failed to create input stream for API request");
        IntentResult result;
        result.success = false;
        result.errorMessage = "Failed to connect to LLM API. Check your internet connection.";
        onComplete(result);
        return;
    }
    
    juce::String responseText = stream->readEntireStreamAsString();
    DBG("API response: " << responseText.substring(0, 500));
    
    if (statusCode != 200)
    {
        IntentResult result;
        result.success = false;
        result.errorMessage = "API error (status " + juce::String(statusCode) + "): " + responseText.substring(0, 200);
        onComplete(result);
        return;
    }
    
    IntentResult result = parseResponse(responseText);
    onComplete(result);
}

IntentResult CloudLLMProvider::parseResponse(const juce::String& responseJson) const
{
    return intentParser.parse(responseJson);
}
