const themeToggle = document.getElementById('theme-toggle');
const body = document.body;

// Check for saved theme preference or default to light mode
const currentTheme = localStorage.getItem('theme') || 'light';
body.setAttribute('data-theme', currentTheme);
updateToggleButton();

themeToggle.addEventListener('click', () => {
    const currentTheme = body.getAttribute('data-theme');
    const newTheme = currentTheme === 'light' ? 'dark' : 'light';

    body.setAttribute('data-theme', newTheme);
    localStorage.setItem('theme', newTheme);
    updateToggleButton();
});

function updateToggleButton() {
    const currentTheme = body.getAttribute('data-theme');
    themeToggle.textContent = currentTheme === 'light' ? '🌙 Dark Mode' : '☀️ Light Mode';
}