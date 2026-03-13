import React, { useState } from 'react';
import { Check, Power, ChevronDown, ChevronUp } from 'lucide-react';
import * as Juce from '../juce/javascript/index.js';
import ParameterKnob from './ParameterKnob';

const setParameter = Juce.getNativeFunction("setParameter");
const bypassAction = Juce.getNativeFunction("bypassAction");

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
    console.log('Bypass clicked, data:', data);
    console.log('intentId:', data.intentId);
    console.log('newBypassed:', newBypassed);
    setBypassed(newBypassed);
    const result = await bypassAction(data.intentId, newBypassed);
    console.log('Bypass result:', result);
  };
  
  const handleParamChange = async (paramId, value) => {
    setParamValues(prev => ({ ...prev, [paramId]: value }));
    await setParameter(paramId, value);
  };
  
  if (!data || typeof data === 'string') {
    return (
      <div className="max-w-[85%] px-4 py-3 bg-chatrack-surface border border-chatrack-border rounded-lg">
        <p className="text-chatrack-text-muted text-sm">{data || 'Unknown response'}</p>
      </div>
    );
  }
  
  const { title, summary, controls } = data;
  
  return (
    <div className="max-w-[85%] bg-chatrack-surface border border-chatrack-border rounded-lg overflow-hidden">
      {/* Header */}
      <div className="px-4 py-3 border-b border-chatrack-border">
        <h3 className="font-medium text-chatrack-text">{title || 'Action'}</h3>
      </div>
      
      {/* Summary */}
      {summary && summary.length > 0 && (
        <div className="px-4 py-3 space-y-1">
          {summary.map((bullet, i) => (
            <div key={i} className="flex items-start gap-2 text-sm">
              <Check size={14} className="mt-0.5 text-chatrack-success flex-shrink-0" />
              <span className="text-chatrack-text-muted">{bullet}</span>
            </div>
          ))}
        </div>
      )}
      
      {/* Action buttons */}
      <div className="px-4 py-2 flex items-center gap-2 border-t border-chatrack-border">
        <button
          onClick={handleBypass}
          className={`flex items-center gap-1.5 px-3 py-1.5 text-sm rounded transition-colors ${
            bypassed 
              ? 'bg-chatrack-accent/20 text-chatrack-accent border border-chatrack-accent/50' 
              : 'text-chatrack-text-muted hover:text-chatrack-text bg-chatrack-surface-light hover:bg-chatrack-border'
          }`}
        >
          <Power size={14} />
          {bypassed ? 'Bypassed' : 'Bypass'}
        </button>
        
        {controls && controls.length > 0 && (
          <button
            onClick={() => setExpanded(!expanded)}
            className="flex items-center gap-1.5 px-3 py-1.5 text-sm text-chatrack-text-muted hover:text-chatrack-text bg-chatrack-surface-light hover:bg-chatrack-border rounded transition-colors ml-auto"
          >
            {expanded ? <ChevronUp size={14} /> : <ChevronDown size={14} />}
            {expanded ? 'Hide' : 'Adjust'} parameters
          </button>
        )}
      </div>
      
      {/* Parameters accordion */}
      {expanded && controls && controls.length > 0 && (
        <div className="px-4 py-4 border-t border-chatrack-border bg-chatrack-bg/50">
          <div className="grid grid-cols-2 sm:grid-cols-3 gap-4">
            {controls.map((ctrl) => (
              <ParameterKnob
                key={ctrl.id}
                label={ctrl.label}
                value={paramValues[ctrl.id] ?? ctrl.value}
                min={ctrl.min}
                max={ctrl.max}
                onChange={(value) => handleParamChange(ctrl.id, value)}
              />
            ))}
          </div>
        </div>
      )}
    </div>
  );
}

export default ActionCard;
