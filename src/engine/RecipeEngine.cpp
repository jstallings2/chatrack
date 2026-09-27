#include "RecipeEngine.h"

RecipeEngine::RecipeEngine()
{
    initializeRecipes();
}

void RecipeEngine::initializeRecipes()
{
    recipes["reduce_mud"] = {
        "reduce_mud",
        "Reduce Mud",
        "eq",
        {
            {"eq.band1.freq", 280.0f},
            {"eq.band1.gain", -2.5f},
            {"eq.band1.q", 1.2f},
            {"eq.band1.enabled", 1.0f}
        },
        {
            {"eq.band1.freq", {200.0f, 400.0f}},
            {"eq.band1.gain", {-6.0f, 0.0f}},
            {"eq.band1.q", {0.5f, 3.0f}}
        },
        {"eq.band1.gain"},
        {"Applied low-mid EQ cut", "Targeted buildup around 280 Hz"}
    };
    
    recipes["brighten"] = {
        "brighten",
        "Brighten",
        "eq",
        {
            {"eq.band3.freq", 7000.0f},
            {"eq.band3.gain", 5.0f},
            {"eq.band3.q", 0.7f},
            {"eq.band3.enabled", 1.0f}
        },
        {
            {"eq.band3.freq", {4000.0f, 16000.0f}},
            {"eq.band3.gain", {0.0f, 8.0f}},
            {"eq.band3.q", {0.3f, 1.5f}}
        },
        {"eq.band3.gain"},
        {"High shelf from ~7 kHz", "Should be obvious on bypass"}
    };
    
    recipes["add_warmth"] = {
        "add_warmth",
        "Add Warmth",
        "saturation",
        {
            {"sat.drive", 0.25f},
            {"sat.mix", 0.6f},
            {"sat.enabled", 1.0f}
        },
        {
            {"sat.drive", {0.1f, 0.5f}},
            {"sat.mix", {0.3f, 1.0f}}
        },
        {"sat.drive"},
        {"Added subtle saturation", "Gentle harmonic thickening"}
    };
    
    recipes["tame_harshness"] = {
        "tame_harshness",
        "Tame Harshness",
        "eq",
        {
            {"eq.band2.freq", 3500.0f},
            {"eq.band2.gain", -2.0f},
            {"eq.band2.q", 1.5f},
            {"eq.band2.enabled", 1.0f}
        },
        {
            {"eq.band2.freq", {2000.0f, 5000.0f}},
            {"eq.band2.gain", {-6.0f, 0.0f}},
            {"eq.band2.q", {0.8f, 3.0f}}
        },
        {"eq.band2.gain"},
        {"Reduced harsh frequencies", "Smoothed upper-mids around 3.5 kHz"}
    };
    
    recipes["make_forward"] = {
        "make_forward",
        "Make More Forward",
        "eq",
        {
            {"eq.band2.freq", 2500.0f},
            {"eq.band2.gain", 2.0f},
            {"eq.band2.q", 1.0f},
            {"eq.band2.enabled", 1.0f}
        },
        {
            {"eq.band2.freq", {1500.0f, 4000.0f}},
            {"eq.band2.gain", {0.0f, 5.0f}},
            {"eq.band2.q", {0.5f, 2.0f}}
        },
        {"eq.band2.gain"},
        {"Boosted presence frequencies", "Enhanced forward projection"}
    };
    
    recipes["widen"] = {
        "widen",
        "Widen Stereo",
        "stereo",
        {
            {"stereo.width", 1.4f},
            {"stereo.enabled", 1.0f}
        },
        {
            {"stereo.width", {1.0f, 2.0f}}
        },
        {"stereo.width"},
        {"Increased stereo width", "Enhanced spatial separation"}
    };
    
    recipes["add_saturation"] = {
        "add_saturation",
        "Add Saturation",
        "saturation",
        {
            {"sat.drive", 0.35f},
            {"sat.mix", 0.8f},
            {"sat.enabled", 1.0f}
        },
        {
            {"sat.drive", {0.2f, 0.7f}},
            {"sat.mix", {0.5f, 1.0f}}
        },
        {"sat.drive"},
        {"Applied saturation", "Added harmonic color and edge"}
    };
    
    recipes["glue"] = {
        "glue",
        "Glue / Compress",
        "compressor",
        {
            {"comp.threshold", -18.0f},
            {"comp.ratio", 3.0f},
            {"comp.attack", 15.0f},
            {"comp.release", 150.0f},
            {"comp.enabled", 1.0f}
        },
        {
            {"comp.threshold", {-30.0f, -6.0f}},
            {"comp.ratio", {1.5f, 8.0f}},
            {"comp.attack", {1.0f, 50.0f}},
            {"comp.release", {50.0f, 500.0f}}
        },
        {"comp.ratio"},
        {"Applied gentle compression", "Added cohesion and glue"}
    };
    
    recipes["make_punchy"] = {
        "make_punchy",
        "Make Punchier",
        "compressor",
        {
            {"comp.threshold", -15.0f},
            {"comp.ratio", 4.0f},
            {"comp.attack", 25.0f},
            {"comp.release", 80.0f},
            {"comp.enabled", 1.0f}
        },
        {
            {"comp.threshold", {-25.0f, -8.0f}},
            {"comp.ratio", {2.0f, 6.0f}},
            {"comp.attack", {10.0f, 50.0f}},
            {"comp.release", {40.0f, 200.0f}}
        },
        {"comp.ratio", "comp.attack"},
        {"Enhanced transient punch", "Slow attack lets transients through"}
    };
    
    recipes["narrow"] = {
        "narrow",
        "Narrow Stereo",
        "stereo",
        {
            {"stereo.width", 0.6f},
            {"stereo.enabled", 1.0f}
        },
        {
            {"stereo.width", {0.0f, 1.0f}}
        },
        {"stereo.width"},
        {"Reduced stereo width", "More focused center image"}
    };
}

ResolvedRecipe RecipeEngine::resolveIntent(const juce::String& intentId, float magnitude)
{
    return resolveIntentWithOverrides(intentId, magnitude, {});
}

ResolvedRecipe RecipeEngine::resolveIntentWithOverrides(
    const juce::String& intentId,
    float magnitude,
    const std::map<juce::String, float>& overrides)
{
    ResolvedRecipe result;
    
    auto it = recipes.find(intentId);
    if (it == recipes.end())
    {
        result.recipeId = "unknown";
        result.label = "Unknown Intent";
        return result;
    }
    
    const Recipe& recipe = it->second;
    result.recipeId = recipe.id;
    result.label = recipe.label;
    result.summaryBullets = recipe.summaryBullets;
    
    magnitude = juce::jlimit(0.0f, 1.0f, magnitude);
    
    for (const auto& [paramId, defaultValue] : recipe.defaults)
    {
        float value = defaultValue;
        
        auto overrideIt = overrides.find(paramId);
        if (overrideIt != overrides.end())
        {
            value = overrideIt->second;
            DBG("Recipe override " << paramId << " = " << value);
        }
        else
        {
            bool shouldScale = std::find(recipe.magnitudeScales.begin(),
                                          recipe.magnitudeScales.end(),
                                          paramId) != recipe.magnitudeScales.end();
            
            if (shouldScale && magnitude != 1.0f)
            {
                value = defaultValue * magnitude;
            }
        }
        
        float minVal = -1000.0f;
        float maxVal = 1000.0f;
        
        auto boundsIt = recipe.bounds.find(paramId);
        if (boundsIt != recipe.bounds.end())
        {
            minVal = boundsIt->second.first;
            maxVal = boundsIt->second.second;
            value = juce::jlimit(minVal, maxVal, value);
        }
        
        ParameterAction action;
        action.parameterId = paramId;
        action.value = value;
        action.min = minVal;
        action.max = maxVal;
        result.actions.push_back(action);
        
        if (paramId.endsWith(".enabled"))
            continue;
        
        ControlSpec control;
        control.id = paramId;
        
        if (paramId.contains("freq"))
            control.label = "Frequency";
        else if (paramId.contains("gain"))
            control.label = "Gain";
        else if (paramId.contains("q"))
            control.label = "Q";
        else if (paramId.contains("drive"))
            control.label = "Drive";
        else if (paramId.contains("mix"))
            control.label = "Mix";
        else if (paramId.contains("threshold"))
            control.label = "Threshold";
        else if (paramId.contains("ratio"))
            control.label = "Ratio";
        else if (paramId.contains("attack"))
            control.label = "Attack";
        else if (paramId.contains("release"))
            control.label = "Release";
        else if (paramId.contains("width"))
            control.label = "Width";
        else
            control.label = paramId;
        
        control.value = value;
        control.min = minVal;
        control.max = maxVal;
        result.controls.push_back(control);
    }
    
    return result;
}

std::vector<juce::String> RecipeEngine::getAvailableIntents() const
{
    std::vector<juce::String> intents;
    for (const auto& [id, _] : recipes)
    {
        intents.push_back(id);
    }
    return intents;
}

const Recipe* RecipeEngine::getRecipe(const juce::String& intentId) const
{
    auto it = recipes.find(intentId);
    if (it != recipes.end())
        return &it->second;
    return nullptr;
}

void RecipeEngine::loadRecipesFromJson(const juce::String& json)
{
    auto parsed = juce::JSON::parse(json);
    if (auto* obj = parsed.getDynamicObject())
    {
        for (const auto& prop : obj->getProperties())
        {
            if (auto* recipeObj = prop.value.getDynamicObject())
            {
                Recipe recipe;
                recipe.id = prop.name.toString();
                recipe.label = recipeObj->getProperty("label").toString();
                recipe.processor = recipeObj->getProperty("processor").toString();
                
                if (auto* defaults = recipeObj->getProperty("defaults").getDynamicObject())
                {
                    for (const auto& defProp : defaults->getProperties())
                    {
                        recipe.defaults[defProp.name.toString()] = 
                            static_cast<float>(defProp.value);
                    }
                }
                
                if (auto* bounds = recipeObj->getProperty("bounds").getDynamicObject())
                {
                    for (const auto& boundProp : bounds->getProperties())
                    {
                        if (auto* arr = boundProp.value.getArray())
                        {
                            if (arr->size() >= 2)
                            {
                                recipe.bounds[boundProp.name.toString()] = {
                                    static_cast<float>((*arr)[0]),
                                    static_cast<float>((*arr)[1])
                                };
                            }
                        }
                    }
                }
                
                if (auto* scales = recipeObj->getProperty("magnitude_scales").getArray())
                {
                    for (const auto& scale : *scales)
                    {
                        recipe.magnitudeScales.push_back(scale.toString());
                    }
                }
                
                recipes[recipe.id] = recipe;
            }
        }
    }
}
