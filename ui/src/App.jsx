import React, { useState, useEffect, useRef, useCallback } from 'react';
import * as Juce from './juce/javascript/index.js';
import ChatPanel from './components/ChatPanel';
import SettingsPanel from './components/SettingsPanel';
import { Settings } from 'lucide-react';

// Debug: Check JUCE integration
console.log("JUCE integration check:", {
  hasJUCE: typeof window.__JUCE__ !== 'undefined',
  hasFunctions: window.__JUCE__?.initialisationData?.__juce__functions,
  hasBackend: typeof window.__JUCE__?.backend !== 'undefined'
});

const sendChatMessage = Juce.getNativeFunction("sendChatMessage");
const loadApiKey = Juce.getNativeFunction("loadApiKey");

function App() {
  const [messages, setMessages] = useState([]);
  const [isThinking, setIsThinking] = useState(false);
  const [showSettings, setShowSettings] = useState(false);
  const [apiKeyStatus, setApiKeyStatus] = useState(null);
  
  useEffect(() => {
    const thinkingHandle = Juce.addEventListener("thinkingStatus", (status) => {
      setIsThinking(status);
    });
    
    const actionCardHandle = Juce.addEventListener("actionCardReady", (card) => {
      setMessages(prev => [...prev, { type: 'assistant', content: card }]);
    });
    
    loadApiKey().then((maskedKey) => {
      setApiKeyStatus(maskedKey || null);
    });
    
    return () => {
      Juce.removeEventListener(thinkingHandle);
      Juce.removeEventListener(actionCardHandle);
    };
  }, []);
  
  const handleSendMessage = useCallback(async (text) => {
    if (!text.trim()) return;
    
    setMessages(prev => [...prev, { type: 'user', content: text }]);
    setIsThinking(true);
    
    try {
      const result = await sendChatMessage(text);
      
      if (typeof result === 'string' && result.startsWith('Error:')) {
        setMessages(prev => [...prev, { 
          type: 'error', 
          content: result.replace('Error: ', '') 
        }]);
      }
    } catch (error) {
      setMessages(prev => [...prev, { 
        type: 'error', 
        content: error.message || 'Failed to process message' 
      }]);
    } finally {
      setIsThinking(false);
    }
  }, []);
  
  const handleApiKeyUpdated = useCallback((maskedKey) => {
    setApiKeyStatus(maskedKey);
  }, []);
  
  return (
    <div className="h-full flex flex-col bg-chatrack-bg">
      {/* Header */}
      <header className="flex items-center justify-between px-4 py-3 border-b border-chatrack-border bg-chatrack-surface">
        <div className="flex items-center gap-2">
          <h1 className="text-lg font-semibold text-chatrack-text">Chatrack</h1>
          <span className="text-xs text-chatrack-text-muted px-2 py-0.5 bg-chatrack-surface-light rounded">
            Mix Assistant
          </span>
        </div>
        <div className="flex items-center gap-3">
          {apiKeyStatus && (
            <span className="text-xs text-chatrack-text-muted">
              API: {apiKeyStatus}
            </span>
          )}
          <button
            onClick={() => setShowSettings(true)}
            className="p-2 rounded-lg hover:bg-chatrack-surface-light text-chatrack-text-muted hover:text-chatrack-text transition-colors"
            title="Settings"
          >
            <Settings size={18} />
          </button>
        </div>
      </header>
      
      {/* Main content */}
      <main className="flex-1 overflow-hidden">
        <ChatPanel
          messages={messages}
          isThinking={isThinking}
          onSendMessage={handleSendMessage}
        />
      </main>
      
      {/* Settings modal */}
      {showSettings && (
        <SettingsPanel
          onClose={() => setShowSettings(false)}
          onApiKeyUpdated={handleApiKeyUpdated}
        />
      )}
    </div>
  );
}

export default App;
