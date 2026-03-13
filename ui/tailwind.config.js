/** @type {import('tailwindcss').Config} */
export default {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}",
  ],
  theme: {
    extend: {
      colors: {
        'chatrack': {
          'bg': '#0f0f1a',
          'surface': '#1a1a2e',
          'surface-light': '#252542',
          'border': '#2d2d4a',
          'accent': '#7c3aed',
          'accent-light': '#8b5cf6',
          'text': '#e2e8f0',
          'text-muted': '#94a3b8',
          'success': '#10b981',
          'error': '#ef4444',
        }
      },
      fontFamily: {
        'sans': ['-apple-system', 'BlinkMacSystemFont', 'Segoe UI', 'Roboto', 'sans-serif'],
        'mono': ['SF Mono', 'Menlo', 'Monaco', 'monospace'],
      }
    },
  },
  plugins: [],
}
