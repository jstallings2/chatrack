#pragma once

#include <juce_core/juce_core.h>
#include <map>
#include <vector>

struct PerceptualTerm
{
    juce::String id;
    juce::String label;
    juce::String description;
    std::vector<juce::String> synonyms;
};

class MixOntology
{
public:
    MixOntology();
    ~MixOntology() = default;
    
    const PerceptualTerm* findTerm(const juce::String& termOrSynonym) const;
    std::vector<juce::String> getAllTermIds() const;
    juce::String getTermsDescription() const;

private:
    void initializeTerms();
    
    std::map<juce::String, PerceptualTerm> terms;
    std::map<juce::String, juce::String> synonymLookup;
};
