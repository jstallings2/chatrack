#include "PluginProcessor.h"
#include "PluginEditor.h"

ChatrackAudioProcessor::ChatrackAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout()),
      rackProcessor(apvts),
      recipeEngine(),
      actionHistory(),
      rackController(rackProcessor, actionHistory, apvts),
      llmService(std::make_unique<CloudLLMProvider>())
{
    // Load saved API key on startup
    loadSavedApiKey();
}

void ChatrackAudioProcessor::recordConversationTurn(const juce::String& role,
                                                    const juce::String& content)
{
    conversationMessages.push_back({ role, content });

    while (conversationMessages.size() > MAX_CONVERSATION_TURNS)
        conversationMessages.erase(conversationMessages.begin());
}

void ChatrackAudioProcessor::loadSavedApiKey()
{
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
            if (apiKey.isNotEmpty())
            {
                if (auto* cloudProvider = dynamic_cast<CloudLLMProvider*>(llmService.get()))
                {
                    cloudProvider->setApiKey(apiKey);
                    DBG("Loaded saved API key");
                }
            }
        }
    }
}

ChatrackAudioProcessor::~ChatrackAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout ChatrackAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // EQ Parameters (4 bands)
    for (int band = 0; band < 4; ++band)
    {
        juce::String prefix = "eq.band" + juce::String(band) + ".";
        
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{prefix + "freq", 1},
            "EQ Band " + juce::String(band) + " Frequency",
            juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f),
            band == 0 ? 100.0f : (band == 1 ? 500.0f : (band == 2 ? 2000.0f : 8000.0f))
        ));
        
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{prefix + "gain", 1},
            "EQ Band " + juce::String(band) + " Gain",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
            0.0f
        ));
        
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{prefix + "q", 1},
            "EQ Band " + juce::String(band) + " Q",
            juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.5f),
            1.0f
        ));
        
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID{prefix + "enabled", 1},
            "EQ Band " + juce::String(band) + " Enabled",
            false
        ));
    }

    // Compressor Parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"comp.threshold", 1},
        "Compressor Threshold",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f),
        -20.0f
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"comp.ratio", 1},
        "Compressor Ratio",
        juce::NormalisableRange<float>(1.0f, 20.0f, 0.1f, 0.5f),
        4.0f
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"comp.attack", 1},
        "Compressor Attack",
        juce::NormalisableRange<float>(0.1f, 100.0f, 0.1f, 0.5f),
        10.0f
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"comp.release", 1},
        "Compressor Release",
        juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.5f),
        100.0f
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{"comp.enabled", 1},
        "Compressor Enabled",
        false
    ));

    // Saturation Parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"sat.drive", 1},
        "Saturation Drive",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"sat.mix", 1},
        "Saturation Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        1.0f
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{"sat.enabled", 1},
        "Saturation Enabled",
        false
    ));

    // Stereo Width Parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"stereo.width", 1},
        "Stereo Width",
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f),
        1.0f
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{"stereo.enabled", 1},
        "Stereo Width Enabled",
        false
    ));

    return { params.begin(), params.end() };
}

const juce::String ChatrackAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ChatrackAudioProcessor::acceptsMidi() const
{
    return false;
}

bool ChatrackAudioProcessor::producesMidi() const
{
    return false;
}

bool ChatrackAudioProcessor::isMidiEffect() const
{
    return false;
}

double ChatrackAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ChatrackAudioProcessor::getNumPrograms()
{
    return 1;
}

int ChatrackAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ChatrackAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String ChatrackAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void ChatrackAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void ChatrackAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());
    
    rackProcessor.prepare(spec);
}

void ChatrackAudioProcessor::releaseResources()
{
    rackProcessor.reset();
}

bool ChatrackAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void ChatrackAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                           juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block(buffer);
    rackProcessor.process(block);
}

bool ChatrackAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* ChatrackAudioProcessor::createEditor()
{
    return new ChatrackAudioProcessorEditor(*this);
}

void ChatrackAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ChatrackAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChatrackAudioProcessor();
}
