# Chatrack Code Tour

A guided walkthrough of the most architecturally interesting code for interview discussion.

---

## 1. Intent Classification — LLM System Prompt

**File:** [`src/llm/CloudLLMProvider.cpp`](../src/llm/CloudLLMProvider.cpp) (lines 27-67)

The system prompt defines the contract between natural language and structured actions. This is the core of the chatbot architecture.

```cpp
juce::String CloudLLMProvider::buildSystemPrompt() const
{
    return R"(You are Chatrack, an AI mixing assistant that translates natural language descriptions of sound into specific mixing actions.

Your job is to:
1. Understand the user's intent about how they want the sound to change
2. Choose the appropriate mixing action (intent)
3. Determine the magnitude (0.0-1.0) based on words like "slightly", "a lot", "subtle", etc.

Available intents:
- reduce_mud: Cut low-mid frequencies (200-400Hz) to reduce muddiness, boxiness
- brighten: Add high frequency presence and air
- add_warmth: Add subtle saturation for analog warmth
- tame_harshness: Cut harsh frequencies (2-5kHz)
- make_forward: Boost presence for forward projection
- widen: Increase stereo width
- narrow: Decrease stereo width
- add_saturation: Add harmonic distortion
- glue: Apply compression for cohesion
- make_punchy: Enhance transients and punch

Magnitude guidelines:
- "slightly", "subtle", "a bit" → 0.3-0.5
- "moderately", "some" → 0.5-0.7
- "a lot", "significantly", "really" → 0.7-0.9
- no modifier → 0.6 (default)

You MUST respond with valid JSON in this exact format:
{
  "intent": "intent_id",
  "magnitude": 0.6,
  "title": "Action Title",
  "summary": ["First bullet point", "Second bullet point"],
  "confidence": 0.9
}
```

**Why this matters:**
- Structured JSON output eliminates parsing ambiguity
- Magnitude extraction lets users express intensity in natural language
- Intent enumeration bounds the action space (no hallucinated actions)
- This pattern applies directly to any chatbot → function calling system

---

## 2. Recipe Engine — Decoupling Intent from Implementation

**File:** [`src/engine/RecipeEngine.cpp`](../src/engine/RecipeEngine.cpp) (lines 8-50)

The Recipe Engine maps intent IDs to concrete DSP parameters. This separation is key: the LLM doesn't need to know DSP details, and new recipes can be added without changing the LLM prompt.

```cpp
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
        {"eq.band1.gain"},  // Parameters that scale with magnitude
        {"Applied low-mid EQ cut", "Targeted buildup around 280 Hz"}
    };
    
    recipes["brighten"] = {
        "brighten",
        "Brighten",
        "eq",
        {
            {"eq.band3.freq", 10000.0f},
            {"eq.band3.gain", 2.0f},
            {"eq.band3.q", 0.7f},
            {"eq.band3.enabled", 1.0f}
        },
        // ... bounds and magnitude scales
    };
```

**Recipe structure:**
- `defaults` — Starting parameter values
- `bounds` — Min/max ranges for user adjustment
- `magnitudeScales` — Which parameters respond to "slightly" vs "a lot"
- `summaryBullets` — Human-readable explanation

**Why this matters:**
- Adding a new capability = adding a recipe + updating the prompt's intent list
- No model retraining, no code changes to the LLM layer
- Bounds ensure the AI can't produce dangerous values
- This is the "function calling" pattern before OpenAI formalized it

---

## 3. Magnitude Scaling — Natural Language to Parameter Values

**File:** [`src/engine/RecipeEngine.cpp`](../src/engine/RecipeEngine.cpp) (lines 193-257)

When the user says "make it slightly brighter" vs "make it much brighter", the magnitude (0.0-1.0) scales the relevant parameters.

```cpp
ResolvedRecipe RecipeEngine::resolveIntentWithOverrides(
    const juce::String& intentId,
    float magnitude,
    const std::map<juce::String, float>& overrides)
{
    // ...
    magnitude = juce::jlimit(0.0f, 1.0f, magnitude);
    
    for (const auto& [paramId, defaultValue] : recipe.defaults)
    {
        float value = defaultValue;
        
        // Check if this parameter should scale with magnitude
        bool shouldScale = std::find(recipe.magnitudeScales.begin(),
                                      recipe.magnitudeScales.end(),
                                      paramId) != recipe.magnitudeScales.end();
        
        if (shouldScale && magnitude != 1.0f)
        {
            value = defaultValue * magnitude;
        }
        
        // Clamp to bounds
        auto boundsIt = recipe.bounds.find(paramId);
        if (boundsIt != recipe.bounds.end())
        {
            value = juce::jlimit(boundsIt->second.first, 
                                 boundsIt->second.second, value);
        }
        
        // ... build parameter action
    }
```

**Why this matters:**
- Converts qualitative language ("slightly", "a lot") to quantitative values
- Bounds clamping prevents out-of-range values regardless of LLM output
- The same pattern works for any domain: "zoom in a bit" → magnitude 0.4

---

## 4. WebView Bridge — Native Function Registration

**File:** [`src/PluginEditor.cpp`](../src/PluginEditor.cpp) (lines 43-69)

The WebView bridge enables bidirectional communication between the React UI and C++ backend. Native functions are registered at setup time.

```cpp
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
```

**Why this matters:**
- Clean async API: JS calls → C++ handler → completion callback
- Each function has a single responsibility
- Resource provider enables embedded UI in release builds
- Same pattern applies to Electron, React Native, or any hybrid architecture

---

## 5. Chat Message Handler — The Full Pipeline

**File:** [`src/PluginEditor.cpp`](../src/PluginEditor.cpp) (lines 136-221)

This is the main orchestration point: user message → LLM → recipe → DSP → UI response.

```cpp
void ChatrackAudioProcessorEditor::handleChatMessage(
    const juce::Array<juce::var>& args,
    juce::WebBrowserComponent::NativeFunctionCompletion complete)
{
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
                        
                        // Build action card response for UI
                        // ... JSON serialization ...
                        
                        // Emit action card event
                        webComponent->emitEventIfBrowserIsVisible("actionCardReady", juce::var(actionCard.get()));
                    }
                });
            }
        );
    });
}
```

**The pipeline:**
1. User sends message from React UI
2. Background thread calls LLM API
3. LLM returns structured JSON (intent + magnitude)
4. Recipe Engine resolves intent to DSP parameters
5. Rack Controller applies parameters (with undo snapshot)
6. Action Card data sent back to UI

**Why this matters:**
- Async architecture keeps UI responsive during API calls
- Thread safety via `MessageManager::callAsync` for UI updates
- Clean separation: LLM layer doesn't know about DSP, UI doesn't know about audio

---

## 6. Action Card UI — Explainable, Reversible AI

**File:** [`ui/src/components/ActionCard.jsx`](../ui/src/components/ActionCard.jsx)

Every AI action renders as an Action Card with bypass toggle and adjustable parameters.

```jsx
function ActionCard({ data }) {
  const [expanded, setExpanded] = useState(false);
  const [bypassed, setBypassed] = useState(false);
  const [paramValues, setParamValues] = useState(() => {
    const values = {};
    if (data.controls) {
      data.controls.forEach(ctrl => {
        values[ctrl.id] = ctrl.value;
      });
    }
    return values;
  });
  
  const handleBypass = async () => {
    const newBypassed = !bypassed;
    setBypassed(newBypassed);
    await bypassAction(data.intentId, newBypassed);
  };
  
  const handleParamChange = async (paramId, value) => {
    setParamValues(prev => ({ ...prev, [paramId]: value }));
    await setParameter(paramId, value);
  };
  
  return (
    <div className="...">
      {/* Header with title */}
      <h3>{title || 'Action'}</h3>
      
      {/* Summary bullets explaining what was done */}
      {summary.map((bullet, i) => (
        <div key={i}>
          <Check size={14} />
          <span>{bullet}</span>
        </div>
      ))}
      
      {/* Bypass toggle for A/B comparison */}
      <button onClick={handleBypass}>
        <Power size={14} />
        {bypassed ? 'Bypassed' : 'Bypass'}
      </button>
      
      {/* Expandable parameter knobs */}
      {expanded && controls.map((ctrl) => (
        <ParameterKnob
          key={ctrl.id}
          label={ctrl.label}
          value={paramValues[ctrl.id]}
          min={ctrl.min}
          max={ctrl.max}
          onChange={(value) => handleParamChange(ctrl.id, value)}
        />
      ))}
    </div>
  );
}
```

**Why this matters:**
- Every AI action is transparent (summary bullets)
- Every AI action is reversible (bypass toggle)
- Every AI action is adjustable (parameter knobs)
- This UX pattern should be standard for AI assistants

---

## 7. Undo System — Snapshot-Based History

**File:** [`src/engine/ActionHistory.cpp`](../src/engine/ActionHistory.cpp)

Each action creates a snapshot of previous values, enabling full undo/redo.

```cpp
void ActionHistory::pushSnapshot(
    const juce::String& actionId,
    const juce::String& intentId,
    const std::map<juce::String, float>& previousValues,
    const std::map<juce::String, float>& newValues)
{
    UndoSnapshot snapshot;
    snapshot.actionId = actionId;
    snapshot.intentId = intentId;
    snapshot.previousValues = previousValues;
    snapshot.newValues = newValues;
    
    undoStack.push_back(snapshot);
    
    if (undoStack.size() > MAX_HISTORY)
        undoStack.erase(undoStack.begin());
    
    redoStack.clear();  // New action clears redo stack
}

bool ActionHistory::undo(std::function<void(const juce::String&, float)> applyValue)
{
    if (undoStack.empty())
        return false;
    
    UndoSnapshot snapshot = undoStack.back();
    undoStack.pop_back();
    
    // Restore all previous values
    for (const auto& [paramId, value] : snapshot.previousValues)
    {
        applyValue(paramId, value);
    }
    
    redoStack.push_back(snapshot);
    return true;
}
```

**Why this matters:**
- Users need control over AI actions
- Storing both previous and new values enables redo
- Bounded history prevents memory bloat
- Callback pattern decouples history from parameter application

---

## 8. DSP Chain — Real-Time Audio Processing

**File:** [`src/dsp/RackProcessor.cpp`](../src/dsp/RackProcessor.cpp)

The DSP chain processes audio in real-time, reading parameters set by the AI.

```cpp
RackProcessor::RackProcessor(juce::AudioProcessorValueTreeState& apvts)
    : eq(apvts),
      compressor(apvts),
      saturation(apvts),
      stereoWidth(apvts),
      apvts(apvts)
{
}

void RackProcessor::process(juce::dsp::AudioBlock<float>& block)
{
    eq.process(block);
    compressor.process(block);
    saturation.process(block);
    stereoWidth.process(block);
}
```

**File:** [`src/dsp/EQProcessor.cpp`](../src/dsp/EQProcessor.cpp) (if you want to show DSP detail)

**Why this matters:**
- Lock-free parameter updates via `AudioProcessorValueTreeState`
- Audio thread never blocks on UI or LLM operations
- Same architecture applies to any real-time system (video, sensors, etc.)

---

## 9. Data-Driven Recipes (JSON)

**File:** [`resources/recipes.json`](../resources/recipes.json)

Recipes can also be loaded from JSON, enabling runtime configuration.

```json
{
  "reduce_mud": {
    "label": "Reduce Mud",
    "processor": "eq",
    "defaults": {
      "eq.band1.freq": 280,
      "eq.band1.gain": -2.5,
      "eq.band1.q": 1.2,
      "eq.band1.enabled": 1
    },
    "bounds": {
      "eq.band1.freq": [200, 400],
      "eq.band1.gain": [-6, 0],
      "eq.band1.q": [0.5, 3.0]
    },
    "magnitude_scales": ["eq.band1.gain"],
    "summary": ["Applied low-mid EQ cut", "Targeted buildup around 280 Hz"]
  }
}
```

**Why this matters:**
- Recipes can be updated without recompiling
- Easy A/B testing of different parameter presets
- Path to user-created recipes or cloud-synced configurations

---

## Architecture Summary

```
User: "This sounds muddy"
         │
         ▼
┌─────────────────────────────────────────┐
│  WebView Bridge (JS → C++)              │
│  sendChatMessage(text)                  │
└─────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────┐
│  LLM Service (OpenAI API)               │
│  → Structured JSON: {intent, magnitude} │
└─────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────┐
│  Recipe Engine                          │
│  → Resolve intent to DSP parameters     │
│  → Apply magnitude scaling              │
│  → Clamp to bounds                      │
└─────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────┐
│  Rack Controller                        │
│  → Snapshot previous values (undo)      │
│  → Apply new values (lock-free)         │
└─────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────┐
│  DSP Chain (audio thread)               │
│  EQ → Compressor → Saturation → Width   │
└─────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────┐
│  Action Card (React UI)                 │
│  → Show what was done                   │
│  → Bypass toggle                        │
│  → Adjustable knobs                     │
└─────────────────────────────────────────┘
```

---

## Quick Reference: Key Files

| Component | File | Lines of Interest |
|-----------|------|-------------------|
| LLM Prompt | `src/llm/CloudLLMProvider.cpp` | 27-67 |
| API Call | `src/llm/CloudLLMProvider.cpp` | 112-177 |
| Recipe Definitions | `src/engine/RecipeEngine.cpp` | 8-191 |
| Magnitude Scaling | `src/engine/RecipeEngine.cpp` | 193-295 |
| WebView Bridge | `src/PluginEditor.cpp` | 43-69 |
| Chat Handler | `src/PluginEditor.cpp` | 136-221 |
| Undo System | `src/engine/ActionHistory.cpp` | full file |
| Action Card UI | `ui/src/components/ActionCard.jsx` | full file |
| DSP Chain | `src/dsp/RackProcessor.cpp` | full file |
| Recipe JSON | `resources/recipes.json` | full file |
