import React, { useState } from 'react';
import { X, Key, Save } from 'lucide-react';
import * as Juce from '../juce/javascript/index.js';

const saveApiKey = Juce.getNativeFunction("saveApiKey");

function SettingsPanel({ onClose, onApiKeyUpdated }) {
  const [apiKey, setApiKey] = useState('');
  const [saving, setSaving] = useState(false);
  const [saved, setSaved] = useState(false);
  
  const handleSave = async () => {
    if (!apiKey.trim()) return;
    
    console.log("Attempting to save API key...");
    setSaving(true);
    try {
      console.log("Calling saveApiKey native function...");
      const result = await saveApiKey(apiKey);
      console.log("saveApiKey result:", result);
      const maskedKey = apiKey.substring(0, 8) + '...' + apiKey.substring(apiKey.length - 4);
      onApiKeyUpdated(maskedKey);
      setSaved(true);
      setTimeout(() => {
        onClose();
      }, 500);
    } catch (error) {
      console.error('Failed to save API key:', error);
    } finally {
      setSaving(false);
    }
  };
  
  return (
    <div className="fixed inset-0 bg-black/50 flex items-center justify-center z-50">
      <div className="bg-chatrack-surface border border-chatrack-border rounded-xl w-full max-w-md mx-4 overflow-hidden">
        {/* Header */}
        <div className="flex items-center justify-between px-4 py-3 border-b border-chatrack-border">
          <h2 className="font-medium text-chatrack-text">Settings</h2>
          <button
            onClick={onClose}
            className="p-1 hover:bg-chatrack-surface-light rounded text-chatrack-text-muted hover:text-chatrack-text transition-colors"
          >
            <X size={18} />
          </button>
        </div>
        
        {/* Content */}
        <div className="p-4 space-y-4">
          <div>
            <label className="flex items-center gap-2 text-sm font-medium text-chatrack-text mb-2">
              <Key size={14} />
              OpenAI API Key
            </label>
            <input
              type="password"
              value={apiKey}
              onChange={(e) => setApiKey(e.target.value)}
              placeholder="sk-..."
              className="w-full px-3 py-2 bg-chatrack-bg border border-chatrack-border rounded-lg text-chatrack-text placeholder-chatrack-text-muted"
            />
            <p className="mt-1.5 text-xs text-chatrack-text-muted">
              Your API key is stored locally and never sent anywhere except OpenAI.
            </p>
          </div>
          
          <button
            onClick={handleSave}
            disabled={!apiKey.trim() || saving}
            className="w-full flex items-center justify-center gap-2 px-4 py-2.5 bg-chatrack-accent hover:bg-chatrack-accent-light disabled:bg-chatrack-surface-light disabled:text-chatrack-text-muted rounded-lg text-white font-medium transition-colors"
          >
            {saving ? (
              'Saving...'
            ) : saved ? (
              <>
                <Save size={16} />
                Saved!
              </>
            ) : (
              <>
                <Save size={16} />
                Save API Key
              </>
            )}
          </button>
        </div>
      </div>
    </div>
  );
}

export default SettingsPanel;
