#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"

// Custom WebBrowserComponent that allows navigation to our URLs
class ChatrackWebView : public juce::WebBrowserComponent
{
public:
    using juce::WebBrowserComponent::WebBrowserComponent;
    
    bool pageAboutToLoad(const juce::String& newURL) override
    {
        DBG("pageAboutToLoad: " << newURL);
        // Allow navigation to: juce resource provider, localhost dev server, about:blank
        return newURL.startsWith("juce://")
            || newURL.startsWith("http://localhost:")
            || newURL.startsWith("about:")
            || newURL.startsWith(getResourceProviderRoot());
    }
};

class ChatrackAudioProcessorEditor : public juce::AudioProcessorEditor,
                                     private juce::Timer
{
public:
    explicit ChatrackAudioProcessorEditor(ChatrackAudioProcessor&);
    ~ChatrackAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    static constexpr const char* LOCAL_DEV_SERVER_URL = "http://localhost:5173";
    
    void setupWebView();
    std::optional<juce::WebBrowserComponent::Resource> getResource(const juce::String& url);
    
    void handleChatMessage(const juce::Array<juce::var>& args,
                           juce::WebBrowserComponent::NativeFunctionCompletion complete);
    void handleSetParameter(const juce::Array<juce::var>& args,
                            juce::WebBrowserComponent::NativeFunctionCompletion complete);
    void handleUndo(const juce::Array<juce::var>& args,
                    juce::WebBrowserComponent::NativeFunctionCompletion complete);
    void handleGetState(const juce::Array<juce::var>& args,
                        juce::WebBrowserComponent::NativeFunctionCompletion complete);
    void handleSaveApiKey(const juce::Array<juce::var>& args,
                          juce::WebBrowserComponent::NativeFunctionCompletion complete);
    void handleLoadApiKey(const juce::Array<juce::var>& args,
                          juce::WebBrowserComponent::NativeFunctionCompletion complete);

    ChatrackAudioProcessor& processorRef;
    std::unique_ptr<ChatrackWebView> webComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChatrackAudioProcessorEditor)
};
