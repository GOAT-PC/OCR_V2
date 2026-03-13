/** @type {import('tailwindcss').Config} */
export default {
  content: ['./index.html', './src/**/*.{js,jsx,ts,tsx}'],
  theme: {
    extend: {
      colors: {
        chassis: {
          outer: '#111827',
          inner: '#020617',
        },
      },
      boxShadow: {
        'chassis-outer': '0 30px 80px rgba(15,23,42,0.9)',
      },
      fontFamily: {
        sans: [
          'system-ui',
          '-apple-system',
          'BlinkMacSystemFont',
          '"Segoe UI"',
          'sans-serif',
        ],
        mono: ['"JetBrains Mono"', '"SF Mono"', 'ui-monospace', 'monospace'],
      },
    },
  },
  plugins: [],
};




