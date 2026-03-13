#include "PluginEditor.h"

ChatrackAudioProcessorEditor::ChatrackAudioProcessorEditor(ChatrackAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setupWebView();
    setSize(800, 600);
    
    // Start a timer to load the URL after the component is fully visible
    startTimer(100);
}

ChatrackAudioProcessorEditor::~ChatrackAudioProcessorEditor()
{
    stopTimer();
}

void ChatrackAudioProcessorEditor::timerCallback()
{
    stopTimer();
    
    if (webComponent == nullptr)
        return;
    
    // Only load when the component is actually showing
    if (!webComponent->isShowing())
    {
        // Retry in a bit
        startTimer(50);
        return;
    }
    
#if JUCE_DEBUG
    DBG("Timer: Loading WebView from dev server: " << LOCAL_DEV_SERVER_URL);
    webComponent->goToURL(LOCAL_DEV_SERVER_URL);
#else
    DBG("Timer: Loading WebView from resource provider");
    webComponent->goToURL(juce::WebBrowserComponent::getResourceProviderRoot());
#endif
}

void ChatrackAudioProcessorEditor::setupWebView()
{
    auto options = juce::WebBrowserComponent::Options{}
        .withNativeIntegrationEnabled()
        .withNativeFunction("sendChatMessage", [this](const auto& args, auto complete) {
            handleChatMessage(args, std::move(complete));
        })
        .withNativeFunction("setParameter", [this](const auto& args, auto complete) {
            handleSetParameter(args, std::move(complete));
        })
        .withNativeFunction("undoLastAction", [this](const auto& args, auto complete) {
            handleUndo(args, std::move(complete));
        })
        .withNativeFunction("getParameterState", [this](const auto& args, auto complete) {
            handleGetState(args, std::move(complete));
        })
        .withNativeFunction("saveApiKey", [this](const auto& args, auto complete) {
            handleSaveApiKey(args, std::move(complete));
        })
        .withNativeFunction("loadApiKey", [this](const auto& args, auto complete) {
            handleLoadApiKey(args, std::move(complete));
        })
        .withNativeFunction("bypassAction", [this](const auto& args, auto complete) {
            handleBypassAction(args, std::move(complete));
        })
        .withResourceProvider([this](const auto& url) {
            return getResource(url);
        }, juce::URL{LOCAL_DEV_SERVER_URL}.getOrigin());

#if JUCE_WINDOWS
    options = options
        .withBackend(juce::WebBrowserComponent::Options::Backend::webview2)
        .withWinWebView2Options(juce::WebBrowserComponent::Options::WinWebView2{}
            .withUserDataFolder(juce::File::getSpecialLocation(
                juce::File::SpecialLocationType::tempDirectory)));
#endif

    webComponent = std::make_unique<ChatrackWebView>(options);
    webComponent->setOpaque(true);
    addAndMakeVisible(*webComponent);
    
    DBG("WebBrowserComponent created");
}

std::optional<juce::WebBrowserComponent::Resource> ChatrackAudioProcessorEditor::getResource(const juce::String& url)
{
    DBG("getResource called for: " << url);
    
    // In release builds, serve from the embedded ZIP (TODO: implement ZIP loading)
    // For now, serve a fallback page
    static const char* fallbackHtml = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Chatrack</title>
    <style>
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background: #1a1a2e;
            color: #eee;
            height: 100vh;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
        }
        h1 { color: #7c3aed; margin-bottom: 1rem; }
        p { color: #888; }
    </style>
</head>
<body>
    <h1>Chatrack</h1>
    <p>WebView UI - Run dev server: cd ui && npm run dev</p>
</body>
</html>
)";

    if (url.contains("index.html") || url.endsWith("/"))
    {
        juce::WebBrowserComponent::Resource resource;
        resource.data = std::vector<std::byte>(
            reinterpret_cast<const std::byte*>(fallbackHtml),
            reinterpret_cast<const std::byte*>(fallbackHtml) + std::strlen(fallbackHtml)
        );
        resource.mimeType = "text/html";
        return resource;
    }

    return std::nullopt;
}

void ChatrackAudioProcessorEditor::handleChatMessage(
    const juce::Array<juce::var>& args,
    juce::WebBrowserComponent::NativeFunctionCompletion complete)
{
    if (args.isEmpty())
    {
        complete(juce::var("Error: No message provided"));
        return;
    }

    juce::String userMessage = args[0].toString();
    
    // Show thinking indicator
    webComponent->emitEventIfBrowserIsVisible("thinkingStatus", juce::var(true));
    
    // Process through LLM service on background thread
    juce::Thread::launch([this, userMessage, complete = std::move(complete)]() mutable {
        ConversationContext context;
        
        processorRef.getLLMService().processMessage(
            userMessage,
            context,
            [this, complete = std::move(complete)](IntentResult result) mutable {
                juce::MessageManager::callAsync([this, result, complete = std::move(complete)]() mutable {
                    // Hide thinking indicator
                    webComponent->emitEventIfBrowserIsVisible("thinkingStatus", juce::var(false));
                    
                    if (result.success)
                    {
                        // Apply recipe and get action card data
                        auto actions = processorRef.getRecipeEngine().resolveIntent(
                            result.intentId,
                            result.magnitude
                        );
                        
                        // Apply through rack controller (creates undo snapshot)
                        processorRef.getRackController().applyActions(actions, result.intentId);
                        
                        // Build action card response
                        juce::DynamicObject::Ptr actionCard = new juce::DynamicObject();
                        actionCard->setProperty("title", result.title);
                        actionCard->setProperty("intentId", result.intentId);
                        
                        // Convert StringArray to var array for summary
                        juce::Array<juce::var> summaryArr;
                        for (const auto& bullet : result.summary)
                            summaryArr.add(bullet);
                        actionCard->setProperty("summary", summaryArr);
                        
                        DBG("Action card intentId: " << result.intentId);
                        
                        // Use controls from the resolved recipe, not the LLM result
                        juce::Array<juce::var> controls;
                        for (const auto& control : actions.controls)
                        {
                            juce::DynamicObject::Ptr ctrl = new juce::DynamicObject();
                            ctrl->setProperty("id", control.id);
                            ctrl->setProperty("label", control.label);
                            ctrl->setProperty("value", control.value);
                            ctrl->setProperty("min", control.min);
                            ctrl->setProperty("max", control.max);
                            controls.add(juce::var(ctrl.get()));
                        }
                        actionCard->setProperty("controls", controls);
                        
                        DBG("Built action card with " << actions.controls.size() << " controls");
                        
                        // Convert to JSON string for proper serialization
                        juce::String jsonResponse = juce::JSON::toString(juce::var(actionCard.get()));
                        DBG("Action card response: " << jsonResponse);
                        
                        // Emit action card event
                        webComponent->emitEventIfBrowserIsVisible("actionCardReady", juce::var(actionCard.get()));
                        
                        // Return the parsed var (juce will serialize it properly)
                        complete(juce::JSON::parse(jsonResponse));
                    }
                    else
                    {
                        complete(juce::var("Error: " + result.errorMessage));
                    }
                });
            }
        );
    });
}

void ChatrackAudioProcessorEditor::handleSetParameter(
    const juce::Array<juce::var>& args,
    juce::WebBrowserComponent::NativeFunctionCompletion complete)
{
    if (args.size() < 2)
    {
        complete(juce::var(false));
        return;
    }

    juce::String paramId = args[0].toString();
    float value = static_cast<float>(args[1]);
    
    if (auto* param = processorRef.getAPVTS().getParameter(paramId))
    {
        param->setValueNotifyingHost(param->convertTo0to1(value));
        
        // Emit parameter changed event for UI sync
        juce::DynamicObject::Ptr update = new juce::DynamicObject();
        update->setProperty("id", paramId);
        update->setProperty("value", value);
        webComponent->emitEventIfBrowserIsVisible("parameterChanged", juce::var(update.get()));
        
        complete(juce::var(true));
    }
    else
    {
        complete(juce::var(false));
    }
}

void ChatrackAudioProcessorEditor::handleUndo(
    const juce::Array<juce::var>& args,
    juce::WebBrowserComponent::NativeFunctionCompletion complete)
{
    juce::ignoreUnused(args);
    
    DBG("handleUndo called, canUndo: " << (processorRef.getActionHistory().canUndo() ? "yes" : "no"));
    
    bool success = processorRef.getActionHistory().undo(
        [this](const juce::String& paramId, float value) {
            DBG("Undo: reverting " << paramId << " to " << value);
            processorRef.getRackController().setParameterDirectly(paramId, value);
        }
    );
    
    DBG("handleUndo result: " << (success ? "success" : "failed"));
    
    complete(juce::var(success));
}

void ChatrackAudioProcessorEditor::handleGetState(
    const juce::Array<juce::var>& args,
    juce::WebBrowserComponent::NativeFunctionCompletion complete)
{
    juce::ignoreUnused(args);
    
    juce::DynamicObject::Ptr state = new juce::DynamicObject();
    
    // Get all parameter values
    for (auto* param : processorRef.getAPVTS().processor.getParameters())
    {
        if (auto* rangedParam = dynamic_cast<juce::RangedAudioParameter*>(param))
        {
            state->setProperty(rangedParam->getParameterID(),
                               rangedParam->convertFrom0to1(rangedParam->getValue()));
        }
    }
    
    complete(juce::var(state.get()));
}

void ChatrackAudioProcessorEditor::handleSaveApiKey(
    const juce::Array<juce::var>& args,
    juce::WebBrowserComponent::NativeFunctionCompletion complete)
{
    if (args.isEmpty())
    {
        complete(juce::var(false));
        return;
    }

    juce::String apiKey = args[0].toString();
    
    auto configDir = juce::File::getSpecialLocation(
        juce::File::userApplicationDataDirectory).getChildFile("Chatrack");
    configDir.createDirectory();
    
    auto configFile = configDir.getChildFile("config.json");
    
    juce::DynamicObject::Ptr config = new juce::DynamicObject();
    config->setProperty("apiKey", apiKey);
    
    configFile.replaceWithText(juce::JSON::toString(juce::var(config.get())));
    
    // Update the LLM service with the new key
    if (auto* cloudProvider = dynamic_cast<CloudLLMProvider*>(&processorRef.getLLMService()))
    {
        cloudProvider->setApiKey(apiKey);
    }
    
    complete(juce::var(true));
}

void ChatrackAudioProcessorEditor::handleLoadApiKey(
    const juce::Array<juce::var>& args,
    juce::WebBrowserComponent::NativeFunctionCompletion complete)
{
    juce::ignoreUnused(args);
    
    auto configFile = juce::File::getSpecialLocation(
        juce::File::userApplicationDataDirectory)
        .getChildFile("Chatrack")
        .getChildFile("config.json");
    
    if (configFile.existsAsFile())
    {
        auto json = juce::JSON::parse(configFile.loadFileAsString());
        if (auto* obj = json.getDynamicObject())
        {
            juce::String apiKey = obj->getProperty("apiKey").toString();
            
            // Update the LLM service
            if (auto* cloudProvider = dynamic_cast<CloudLLMProvider*>(&processorRef.getLLMService()))
            {
                cloudProvider->setApiKey(apiKey);
            }
            
            // Return masked key for display
            juce::String maskedKey = apiKey.substring(0, 8) + "..." + apiKey.substring(apiKey.length() - 4);
            complete(juce::var(maskedKey));
            return;
        }
    }
    
    complete(juce::var());
}

void ChatrackAudioProcessorEditor::handleBypassAction(
    const juce::Array<juce::var>& args,
    juce::WebBrowserComponent::NativeFunctionCompletion complete)
{
    if (args.size() < 2)
    {
        DBG("Bypass: not enough args");
        complete(juce::var(false));
        return;
    }
    
    juce::String intentId = args[0].toString();
    bool bypass = static_cast<bool>(args[1]);
    
    DBG("Bypass action: intentId='" << intentId << "' bypass=" << (bypass ? "true" : "false"));
    
    // Get the recipe to find which processor to bypass
    const auto* recipe = processorRef.getRecipeEngine().getRecipe(intentId);
    if (recipe == nullptr)
    {
        DBG("Bypass: recipe not found for intentId: " << intentId);
        complete(juce::var(false));
        return;
    }
    
    DBG("Bypass: found recipe, processor=" << recipe->processor);
    
    // Find ALL enabled parameters in the recipe and toggle them
    juce::StringArray enabledParams;
    for (const auto& [paramId, value] : recipe->defaults)
    {
        DBG("Bypass: checking param " << paramId);
        if (paramId.endsWith(".enabled"))
        {
            enabledParams.add(paramId);
        }
    }
    
    if (enabledParams.isEmpty())
    {
        DBG("Bypass: no enabled params found in recipe defaults");
        // Fall back to processor-level enable
        if (recipe->processor == "compressor")
            enabledParams.add("comp.enabled");
        else if (recipe->processor == "saturation")
            enabledParams.add("sat.enabled");
        else if (recipe->processor == "stereo")
            enabledParams.add("stereo.enabled");
    }
    
    float enabledValue = bypass ? 0.0f : 1.0f;
    for (const auto& param : enabledParams)
    {
        DBG("Bypass: setting " << param << " to " << enabledValue);
        processorRef.getRackController().setParameterDirectly(param, enabledValue);
    }
    
    complete(juce::var(true));
}

void ChatrackAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a2e));
}

void ChatrackAudioProcessorEditor::resized()
{
    if (webComponent)
        webComponent->setBounds(getLocalBounds());
}
