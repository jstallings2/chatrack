import React from 'react';

function ThinkingIndicator() {
  return (
    <div className="flex items-center gap-1 px-4 py-3 bg-chatrack-surface border border-chatrack-border rounded-lg">
      <div className="thinking-dot w-2 h-2 bg-chatrack-accent rounded-full" />
      <div className="thinking-dot w-2 h-2 bg-chatrack-accent rounded-full" />
      <div className="thinking-dot w-2 h-2 bg-chatrack-accent rounded-full" />
    </div>
  );
}

export default ThinkingIndicator;
