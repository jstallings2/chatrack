import React, { useState, useRef, useEffect, useMemo } from 'react';
import { Send } from 'lucide-react';
import ActionCard from './ActionCard';
import ThinkingIndicator from './ThinkingIndicator';
import RackBar from './RackBar';

function ChatPanel({
  messages,
  isThinking,
  onSendMessage,
  masterBypassed,
  onToggleMasterBypass,
}) {
  const [inputValue, setInputValue] = useState('');
  const messagesEndRef = useRef(null);
  const inputRef = useRef(null);
  const cardRefs = useRef({});

  const rackSlots = useMemo(() => {
    const byIntent = new Map();
    for (const msg of messages) {
      if (msg.type !== 'assistant' || !msg.content?.intentId)
        continue;
      byIntent.set(msg.content.intentId, {
        intentId: msg.content.intentId,
        title: msg.content.title || msg.content.intentId,
      });
    }
    return [...byIntent.values()];
  }, [messages]);

  const jumpToCard = (intentId) => {
    cardRefs.current[intentId]?.scrollIntoView({ behavior: 'smooth', block: 'center' });
  };
  
  const scrollToBottom = () => {
    messagesEndRef.current?.scrollIntoView({ behavior: 'smooth' });
  };
  
  useEffect(() => {
    scrollToBottom();
  }, [messages, isThinking]);
  
  const handleSubmit = (e) => {
    e.preventDefault();
    if (inputValue.trim() && !isThinking) {
      onSendMessage(inputValue);
      setInputValue('');
    }
  };
  
  const handleKeyDown = (e) => {
    if (e.key === 'Enter' && !e.shiftKey) {
      e.preventDefault();
      handleSubmit(e);
    }
  };
  
  return (
    <div className="h-full flex flex-col">
      <RackBar
        slots={rackSlots}
        masterBypassed={masterBypassed}
        onToggleMaster={onToggleMasterBypass}
        onSelect={jumpToCard}
      />
      {/* Messages area */}
      <div className="flex-1 overflow-y-auto px-4 py-4 space-y-4">
        {messages.length === 0 && (
          <div className="flex flex-col items-center justify-center h-full text-center">
            <h2 className="text-xl font-medium text-chatrack-text mb-2">
              What would you like to adjust?
            </h2>
            <p className="text-chatrack-text-muted text-sm max-w-md">
              Describe what you want to change about the sound. For example:
            </p>
            <div className="mt-4 space-y-2">
              {[
                "This vocal sounds muddy",
                "Make it brighter",
                "Add some warmth",
                "It's too harsh"
              ].map((suggestion, i) => (
                <button
                  key={i}
                  onClick={() => setInputValue(suggestion)}
                  className="block w-full px-4 py-2 text-sm text-chatrack-text-muted hover:text-chatrack-text bg-chatrack-surface hover:bg-chatrack-surface-light rounded-lg transition-colors text-left"
                >
                  "{suggestion}"
                </button>
              ))}
            </div>
          </div>
        )}
        
        {messages.map((msg, index) => (
          <div key={index}>
            {msg.type === 'user' && (
              <div className="flex justify-end">
                <div className="max-w-[80%] px-4 py-2 bg-chatrack-accent rounded-2xl rounded-br-md text-white">
                  {msg.content}
                </div>
              </div>
            )}
            
            {msg.type === 'assistant' && (
              <div
                className="flex justify-start"
                ref={(el) => {
                  if (msg.content?.intentId)
                    cardRefs.current[msg.content.intentId] = el;
                }}
              >
                <ActionCard data={msg.content} />
              </div>
            )}
            
            {msg.type === 'error' && (
              <div className="flex justify-start">
                <div className="max-w-[80%] px-4 py-2 bg-red-900/30 border border-red-700/50 rounded-lg text-red-300 text-sm">
                  {msg.content}
                </div>
              </div>
            )}
          </div>
        ))}
        
        {isThinking && (
          <div className="flex justify-start">
            <ThinkingIndicator />
          </div>
        )}
        
        <div ref={messagesEndRef} />
      </div>
      
      {/* Input area */}
      <form onSubmit={handleSubmit} className="px-4 py-3 border-t border-chatrack-border bg-chatrack-surface">
        <div className="flex items-center gap-2">
          <input
            ref={inputRef}
            type="text"
            value={inputValue}
            onChange={(e) => setInputValue(e.target.value)}
            onKeyDown={handleKeyDown}
            placeholder="Describe what you want to change..."
            disabled={isThinking}
            className="flex-1 px-4 py-2.5 bg-chatrack-bg border border-chatrack-border rounded-lg text-chatrack-text placeholder-chatrack-text-muted focus:border-chatrack-accent disabled:opacity-50"
          />
          <button
            type="submit"
            disabled={!inputValue.trim() || isThinking}
            className="p-2.5 bg-chatrack-accent hover:bg-chatrack-accent-light disabled:bg-chatrack-surface-light disabled:text-chatrack-text-muted rounded-lg text-white transition-colors"
          >
            <Send size={20} />
          </button>
        </div>
      </form>
    </div>
  );
}

export default ChatPanel;
