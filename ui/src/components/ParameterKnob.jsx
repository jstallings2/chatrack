import React, { useRef, useCallback, useState } from 'react';
import clsx from 'clsx';

function ParameterKnob({ label, value, min, max, onChange, step = 0.01 }) {
  const knobRef = useRef(null);
  const [isDragging, setIsDragging] = useState(false);
  const dragStartRef = useRef({ y: 0, value: 0 });
  
  const normalizedValue = (value - min) / (max - min);
  const rotation = -135 + normalizedValue * 270;
  
  const formatValue = (val) => {
    if (Math.abs(val) >= 1000) {
      return (val / 1000).toFixed(1) + 'k';
    }
    if (Math.abs(val) >= 100) {
      return Math.round(val).toString();
    }
    if (Math.abs(val) >= 10) {
      return val.toFixed(1);
    }
    return val.toFixed(2);
  };
  
  const handleMouseDown = useCallback((e) => {
    e.preventDefault();
    setIsDragging(true);
    dragStartRef.current = { y: e.clientY, value };
    
    const handleMouseMove = (moveEvent) => {
      const deltaY = dragStartRef.current.y - moveEvent.clientY;
      const sensitivity = (max - min) / 200;
      let newValue = dragStartRef.current.value + deltaY * sensitivity;
      newValue = Math.max(min, Math.min(max, newValue));
      onChange(newValue);
    };
    
    const handleMouseUp = () => {
      setIsDragging(false);
      document.removeEventListener('mousemove', handleMouseMove);
      document.removeEventListener('mouseup', handleMouseUp);
    };
    
    document.addEventListener('mousemove', handleMouseMove);
    document.addEventListener('mouseup', handleMouseUp);
  }, [value, min, max, onChange]);
  
  const handleWheel = useCallback((e) => {
    e.preventDefault();
    const delta = e.deltaY > 0 ? -1 : 1;
    const sensitivity = (max - min) / 100;
    let newValue = value + delta * sensitivity;
    newValue = Math.max(min, Math.min(max, newValue));
    onChange(newValue);
  }, [value, min, max, onChange]);
  
  return (
    <div className="flex flex-col items-center">
      <div
        ref={knobRef}
        className={clsx(
          "relative w-12 h-12 rounded-full cursor-pointer knob-container",
          "bg-chatrack-surface-light border-2 border-chatrack-border",
          isDragging && "border-chatrack-accent"
        )}
        onMouseDown={handleMouseDown}
        onWheel={handleWheel}
      >
        {/* Knob indicator line */}
        <div
          className="absolute inset-2 rounded-full bg-chatrack-surface"
          style={{ transform: `rotate(${rotation}deg)` }}
        >
          <div className="absolute top-0 left-1/2 -translate-x-1/2 w-0.5 h-2 bg-chatrack-accent rounded-full" />
        </div>
        
        {/* Arc indicator (optional visual) */}
        <svg className="absolute inset-0 w-full h-full -rotate-90" viewBox="0 0 48 48">
          <circle
            cx="24"
            cy="24"
            r="20"
            fill="none"
            stroke="currentColor"
            strokeWidth="2"
            className="text-chatrack-border"
            strokeDasharray="94.25"
            strokeDashoffset="31.42"
            strokeLinecap="round"
          />
          <circle
            cx="24"
            cy="24"
            r="20"
            fill="none"
            stroke="currentColor"
            strokeWidth="2"
            className="text-chatrack-accent"
            strokeDasharray="94.25"
            strokeDashoffset={94.25 - normalizedValue * 62.83}
            strokeLinecap="round"
            style={{ transform: 'rotate(-45deg)', transformOrigin: 'center' }}
          />
        </svg>
      </div>
      
      {/* Value display */}
      <div className="mt-1 text-xs text-chatrack-text font-mono">
        {formatValue(value)}
      </div>
      
      {/* Label */}
      <div className="mt-0.5 text-xs text-chatrack-text-muted truncate max-w-full">
        {label}
      </div>
    </div>
  );
}

export default ParameterKnob;
