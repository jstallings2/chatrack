#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include "dsp/RackProcessor.h"
#include "engine/RecipeEngine.h"
#include "engine/ActionHistory.h"
#include "engine/RackController.h"
#include "llm/LLMService.h"
#include "llm/CloudLLMProvider.h"

class ChatrackAudioProcessor : public juce::AudioProcessor
{
public:
    ChatrackAudioProcessor();
    ~ChatrackAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    RackProcessor& getRackProcessor() { return rackProcessor; }
    RecipeEngine& getRecipeEngine() { return recipeEngine; }
    ActionHistory& getActionHistory() { return actionHistory; }
    RackController& getRackController() { return rackController; }
    LLMService& getLLMService() { return *llmService; }

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void loadSavedApiKey();

    juce::AudioProcessorValueTreeState apvts;
    RackProcessor rackProcessor;
    RecipeEngine recipeEngine;
    ActionHistory actionHistory;
    RackController rackController;
    std::unique_ptr<LLMService> llmService;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChatrackAudioProcessor)
};
