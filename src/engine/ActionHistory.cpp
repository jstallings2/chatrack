#include "ActionHistory.h"

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
    {
        undoStack.erase(undoStack.begin());
    }
    
    redoStack.clear();
}

bool ActionHistory::undo(std::function<void(const juce::String&, float)> applyValue)
{
    if (undoStack.empty())
        return false;
    
    UndoSnapshot snapshot = undoStack.back();
    undoStack.pop_back();
    
    for (const auto& [paramId, value] : snapshot.previousValues)
    {
        applyValue(paramId, value);
    }
    
    redoStack.push_back(snapshot);
    
    return true;
}

bool ActionHistory::redo(std::function<void(const juce::String&, float)> applyValue)
{
    if (redoStack.empty())
        return false;
    
    UndoSnapshot snapshot = redoStack.back();
    redoStack.pop_back();
    
    for (const auto& [paramId, value] : snapshot.newValues)
    {
        applyValue(paramId, value);
    }
    
    undoStack.push_back(snapshot);
    
    return true;
}

bool ActionHistory::canUndo() const
{
    return !undoStack.empty();
}

bool ActionHistory::canRedo() const
{
    return !redoStack.empty();
}

void ActionHistory::clear()
{
    undoStack.clear();
    redoStack.clear();
}

const UndoSnapshot* ActionHistory::getLastSnapshot() const
{
    if (undoStack.empty())
        return nullptr;
    return &undoStack.back();
}
