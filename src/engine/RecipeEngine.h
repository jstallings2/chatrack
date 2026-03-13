#pragma once

#include <juce_core/juce_core.h>
#include <map>
#include <vector>

#include "MixOntology.h"

struct ParameterAction
{
    juce::String parameterId;
    float value;
    float min;
    float max;
};

struct ControlSpec
{
    juce::String id;
    juce::String label;
    float value;
    float min;
    float max;
};

struct Recipe
{
    juce::String id;
    juce::String label;
    juce::String processor;
    std::map<juce::String, float> defaults;
    std::map<juce::String, std::pair<float, float>> bounds;
    std::vector<juce::String> magnitudeScales;
    juce::StringArray summaryBullets;
};

struct ResolvedRecipe
{
    juce::String recipeId;
    juce::String label;
    std::vector<ParameterAction> actions;
    std::vector<ControlSpec> controls;
    juce::StringArray summaryBullets;
};

class RecipeEngine
{
public:
    RecipeEngine();
    ~RecipeEngine() = default;
    
    ResolvedRecipe resolveIntent(const juce::String& intentId, float magnitude);
    ResolvedRecipe resolveIntentWithOverrides(const juce::String& intentId,
                                               float magnitude,
                                               const std::map<juce::String, float>& overrides);
    
    std::vector<juce::String> getAvailableIntents() const;
    const Recipe* getRecipe(const juce::String& intentId) const;
    const MixOntology& getOntology() const { return ontology; }

private:
    void initializeRecipes();
    void loadRecipesFromJson(const juce::String& json);
    
    std::map<juce::String, Recipe> recipes;
    MixOntology ontology;
};
