import React from 'react';
import { Power } from 'lucide-react';

function RackBar({ slots, masterBypassed, onToggleMaster, onSelect }) {
  if (!slots.length)
    return null;

  return (
    <div className="flex items-center gap-2 px-4 py-2 border-b border-chatrack-border bg-chatrack-surface">
      <span className="text-[10px] uppercase tracking-wide text-chatrack-text-muted flex-shrink-0">
        Rack
      </span>
      <div className="flex-1 flex items-center gap-1.5 overflow-x-auto min-w-0">
        {slots.map((slot) => (
          <button
            key={slot.intentId}
            type="button"
            onClick={() => onSelect(slot.intentId)}
            className="flex-shrink-0 px-2.5 py-1 text-xs rounded bg-chatrack-surface-light text-chatrack-text hover:bg-chatrack-border transition-colors"
            title="Jump to this card"
          >
            {slot.title}
          </button>
        ))}
      </div>
      <button
        type="button"
        onClick={onToggleMaster}
        className={`flex-shrink-0 flex items-center gap-1.5 px-2.5 py-1 text-xs rounded transition-colors ${
          masterBypassed
            ? 'bg-chatrack-accent/20 text-chatrack-accent border border-chatrack-accent/50'
            : 'text-chatrack-text-muted hover:text-chatrack-text bg-chatrack-surface-light'
        }`}
        title="Bypass the whole rack"
      >
        <Power size={12} />
        {masterBypassed ? 'Bypassed' : 'A/B'}
      </button>
    </div>
  );
}

export default RackBar;
