#pragma once

#include <juce_core/juce_core.h>
#include <map>
#include <vector>
#include <functional>

struct UndoSnapshot
{
    juce::String actionId;
    juce::String intentId;
    float magnitude = 0.6f;
    juce::String title;
    std::map<juce::String, float> previousValues;
    std::map<juce::String, float> newValues;
};

class ActionHistory
{
public:
    ActionHistory() = default;
    ~ActionHistory() = default;
    
    void pushSnapshot(const juce::String& actionId,
                      const juce::String& intentId,
                      const std::map<juce::String, float>& previousValues,
                      const std::map<juce::String, float>& newValues,
                      float magnitude = 0.6f,
                      const juce::String& title = {});
    
    bool undo(std::function<void(const juce::String&, float)> applyValue);
    bool redo(std::function<void(const juce::String&, float)> applyValue);
    
    bool canUndo() const;
    bool canRedo() const;
    
    void clear();
    
    size_t getUndoCount() const { return undoStack.size(); }
    size_t getRedoCount() const { return redoStack.size(); }
    
    const UndoSnapshot* getLastSnapshot() const;

private:
    std::vector<UndoSnapshot> undoStack;
    std::vector<UndoSnapshot> redoStack;
    
    static constexpr size_t MAX_HISTORY = 50;
};
