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
    return R"(You are Chatrack, a mixing assistant. You pick a tool on the rack and set real parameters. The user finishes by ear.

Think like a mixer first. "Shine", "air", and "cymbals" are not synonyms for "a bit more brighten." Air / shine lives on a high shelf around 10–16 kHz. Presence / "forward" is 2–5 kHz. Mud is 200–400 Hz. Write that thought as the first summary bullet.

Available tools (intent = which processor family):
- reduce_mud: low-mid cut (eq.band1)
- brighten: high shelf (eq.band3) — presence at ~7 kHz, air/shine at 10–16 kHz
- tame_harshness: cut 2–5 kHz (eq.band2, negative gain)
- make_forward: boost presence 2–4 kHz (eq.band2, positive gain)
- add_warmth / add_saturation: saturation
- widen / narrow: stereo width
- glue / make_punchy: compressor

Magnitude (0.1–1.0) scales the recipe's main knob if you omit that key from parameter_overrides.
- slight / subtle → 0.3–0.5
- no modifier → 0.6
- a lot / really → 0.7–0.9

parameter_overrides — set any of these when the default recipe is the wrong place or amount. Values are clamped to the ranges shown.
- eq.band1.freq 200–400, eq.band1.gain -6–0, eq.band1.q 0.5–3
- eq.band2.freq 1500–5000, eq.band2.gain -6–5, eq.band2.q 0.5–3
- eq.band3.freq 4000–16000, eq.band3.gain 0–8, eq.band3.q 0.3–1.5
- sat.drive 0.1–0.7, sat.mix 0.3–1
- comp.threshold -30–-6, comp.ratio 1.5–8, comp.attack 1–50, comp.release 40–500

Follow-ups:
- Same tool, different place or amount (brighter / too much / shine / more air / lower the shelf) → SAME intent, change magnitude and/or parameter_overrides. Do not leave frequency stuck if they named a different band of the spectrum.
- New goal (warmer, narrower, glue) → new intent.

Respond with JSON only:
{
  "intent": "intent_id",
  "magnitude": 0.6,
  "title": "Short tool name",
  "summary": ["Why this frequency / processor", "What should change"],
  "confidence": 0.9,
  "parameter_overrides": {"eq.band3.freq": 12000}
}

Omit parameter_overrides when the recipe default is fine.
If you cannot understand:
{"intent": "unknown", "error": "brief reason"}
)";
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

        if (context.lastAction.valid || !context.currentParameters.empty())
        {
            juce::String ctx = "Current mix context:\n";
            if (context.lastAction.valid)
            {
                ctx += "Last action: intent=" + context.lastAction.intentId
                    + " magnitude=" + juce::String(context.lastAction.magnitude, 2)
                    + " title=" + context.lastAction.title + "\n";
            }
            if (!context.currentParameters.empty())
            {
                ctx += "Current parameters:";
                int count = 0;
                for (const auto& [id, value] : context.currentParameters)
                {
                    ctx += " " + id + "=" + juce::String(value, 2);
                    if (++count >= 24)
                        break;
                }
                ctx += "\n";
            }
            juce::DynamicObject::Ptr ctxMsg = new juce::DynamicObject();
            ctxMsg->setProperty("role", "system");
            ctxMsg->setProperty("content", ctx);
            messages.add(juce::var(ctxMsg.get()));
        }
        
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
