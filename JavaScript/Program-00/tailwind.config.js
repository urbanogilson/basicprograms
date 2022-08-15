/** @type {import('tailwindcss').Config} */
module.exports = {
  content: ["./src/**/*.{js,jsx,ts,tsx}",],
  theme: {
    extend: {
      typography: {
        DEFAULT: {
          css: {
            color: '#333',
            a: {
              color: '#3182ce',
              '&:hover': {
                color: '#2c5282',
              },
            },
            img: {
              display: 'inline-block',
            },
            hr: {
              marginTop: '1em',
              marginBottom: '1em',
            }
          },
        },
      },
    },
  },
  plugins: [require('@tailwindcss/typography'),],
}
