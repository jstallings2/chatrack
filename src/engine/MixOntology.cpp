#include "MixOntology.h"

MixOntology::MixOntology()
{
    initializeTerms();
}

void MixOntology::initializeTerms()
{
    terms["mud"] = {
        "mud",
        "Muddiness",
        "Low-mid frequency buildup that obscures clarity, typically 200-400Hz",
        {"muddy", "boxy", "boomy", "cloudy", "thick", "congested"}
    };
    
    terms["brightness"] = {
        "brightness",
        "Brightness",
        "High frequency presence and clarity, typically 8-16kHz",
        {"bright", "crisp", "air", "airy", "sparkle", "shimmer", "presence"}
    };
    
    terms["warmth"] = {
        "warmth",
        "Warmth",
        "Pleasant low-mid harmonics and fullness, often from saturation or gentle low-end boost",
        {"warm", "full", "rich", "thick", "fat", "analog"}
    };
    
    terms["harshness"] = {
        "harshness",
        "Harshness",
        "Unpleasant high-mid frequencies that cause listener fatigue, typically 2-5kHz",
        {"harsh", "brittle", "piercing", "shrill", "strident", "sibilant"}
    };
    
    terms["punch"] = {
        "punch",
        "Punch",
        "Transient impact and attack, often in the low-mid range",
        {"punchy", "impact", "attack", "snap", "bite", "forward"}
    };
    
    terms["width"] = {
        "width",
        "Stereo Width",
        "Spatial spread across the stereo field",
        {"wide", "spacious", "spread", "stereo", "narrow", "mono"}
    };
    
    terms["compression"] = {
        "compression",
        "Compression",
        "Dynamic range control and glue",
        {"compressed", "glued", "squashed", "pumping", "dynamic", "glue"}
    };
    
    terms["saturation"] = {
        "saturation",
        "Saturation",
        "Harmonic distortion and coloration",
        {"saturated", "driven", "distorted", "gritty", "edge", "color"}
    };
    
    for (const auto& [id, term] : terms)
    {
        synonymLookup[id] = id;
        for (const auto& synonym : term.synonyms)
        {
            synonymLookup[synonym.toLowerCase()] = id;
        }
    }
}

const PerceptualTerm* MixOntology::findTerm(const juce::String& termOrSynonym) const
{
    auto it = synonymLookup.find(termOrSynonym.toLowerCase());
    if (it != synonymLookup.end())
    {
        auto termIt = terms.find(it->second);
        if (termIt != terms.end())
            return &termIt->second;
    }
    return nullptr;
}

std::vector<juce::String> MixOntology::getAllTermIds() const
{
    std::vector<juce::String> ids;
    for (const auto& [id, _] : terms)
    {
        ids.push_back(id);
    }
    return ids;
}

juce::String MixOntology::getTermsDescription() const
{
    juce::String desc;
    for (const auto& [id, term] : terms)
    {
        desc += term.id + ": " + term.description + "\n";
    }
    return desc;
}
