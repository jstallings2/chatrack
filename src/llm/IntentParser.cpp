#include "IntentParser.h"

IntentResult IntentParser::parse(const juce::String& llmResponse) const
{
    juce::String content = extractContentFromResponse(llmResponse);
    return parseContent(content);
}

juce::String IntentParser::extractContentFromResponse(const juce::String& response) const
{
    auto parsed = juce::JSON::parse(response);
    
    if (auto* obj = parsed.getDynamicObject())
    {
        if (auto* choices = obj->getProperty("choices").getArray())
        {
            if (!choices->isEmpty())
            {
                if (auto* choice = (*choices)[0].getDynamicObject())
                {
                    if (auto* message = choice->getProperty("message").getDynamicObject())
                    {
                        return message->getProperty("content").toString();
                    }
                }
            }
        }
        
        if (auto* content = obj->getProperty("content").getArray())
        {
            if (!content->isEmpty())
            {
                if (auto* block = (*content)[0].getDynamicObject())
                {
                    return block->getProperty("text").toString();
                }
            }
        }
    }
    
    return response;
}

IntentResult IntentParser::parseContent(const juce::String& jsonContent) const
{
    IntentResult result;
    
    auto parsed = juce::JSON::parse(jsonContent);
    
    if (auto* obj = parsed.getDynamicObject())
    {
        juce::String intent = obj->getProperty("intent").toString();
        
        if (intent == "unknown" || intent.isEmpty())
        {
            result.success = false;
            result.errorMessage = obj->getProperty("error").toString();
            if (result.errorMessage.isEmpty())
                result.errorMessage = "Could not understand the request";
            return result;
        }
        
        result.success = true;
        result.intentId = intent;
        result.magnitude = static_cast<float>(obj->getProperty("magnitude"));
        
        if (result.magnitude <= 0.0f || result.magnitude > 1.0f)
            result.magnitude = 0.6f;

        if (obj->hasProperty("confidence"))
        {
            result.confidence = static_cast<float>(obj->getProperty("confidence"));
            if (result.confidence <= 0.0f || result.confidence > 1.0f)
                result.confidence = 0.9f;
        }
        
        result.title = obj->getProperty("title").toString();
        if (result.title.isEmpty())
        {
            result.title = intent.replace("_", " ");
            result.title = result.title.substring(0, 1).toUpperCase() + 
                           result.title.substring(1);
        }
        
        if (auto* summaryArr = obj->getProperty("summary").getArray())
        {
            for (const auto& item : *summaryArr)
            {
                result.summary.add(item.toString());
            }
        }
        
        if (auto* overrides = obj->getProperty("parameter_overrides").getDynamicObject())
        {
            for (const auto& prop : overrides->getProperties())
            {
                result.parameterOverrides[prop.name.toString()] = 
                    static_cast<float>(prop.value);
            }
        }
    }
    else
    {
        result.success = false;
        result.errorMessage = "Failed to parse LLM response";
    }
    
    return result;
}
