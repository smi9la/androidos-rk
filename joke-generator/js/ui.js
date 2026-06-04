/**
 * UI Interaction Module
 * Handles all user interface updates and interactions
 */

const UI = {
    // DOM Elements
    elements: {
        jokeContent: document.getElementById('joke-content'),
        loading: document.getElementById('loading'),
        error: document.getElementById('error'),
        errorMessage: document.getElementById('error-message'),
        getJokeBtn: document.getElementById('get-joke-btn'),
        copyBtn: document.getElementById('copy-btn'),
        jokeCount: document.getElementById('joke-count'),
        apiName: document.getElementById('api-name'),
        apiSelect: document.getElementById('api-select'),
        jokeTypeSelect: document.getElementById('joke-type'),
        jokeTypeSelector: document.getElementById('joke-type-selector')
    },

    // State
    state: {
        jokeCount: 0,
        currentJoke: null,
        isLoading: false
    },

    /**
     * Initialize UI
     */
    init() {
        this.attachEventListeners();
        this.updateStats();
    },

    /**
     * Attach event listeners
     */
    attachEventListeners() {
        this.elements.getJokeBtn.addEventListener('click', () => {
            this.handleGetJoke();
        });

        this.elements.copyBtn.addEventListener('click', () => {
            this.handleCopyJoke();
        });

        this.elements.apiSelect.addEventListener('change', (e) => {
            this.handleApiChange(e.target.value);
        });

        // Allow Enter key to get joke
        document.addEventListener('keypress', (e) => {
            if (e.key === 'Enter' && !this.state.isLoading) {
                this.handleGetJoke();
            }
        });
    },

    /**
     * Handle Get Joke button click
     */
    async handleGetJoke() {
        if (this.state.isLoading) return;

        this.state.isLoading = true;
        this.showLoading(true);
        this.hideError();

        try {
            const apiType = this.elements.apiSelect.value;
            const jokeType = this.elements.jokeTypeSelect.value;

            const joke = await API.getJoke(apiType, jokeType);
            this.state.currentJoke = joke;
            this.state.jokeCount++;

            this.displayJoke(joke);
            this.updateStats();
            this.elements.copyBtn.disabled = false;
        } catch (error) {
            console.error('Error:', error);
            this.showError(error.message);
            this.elements.copyBtn.disabled = true;
        } finally {
            this.showLoading(false);
            this.state.isLoading = false;
        }
    },

    /**
     * Display joke in UI
     * @param {Object} joke - Joke object
     */
    displayJoke(joke) {
        const html = `
            <div class="joke-text">${this.escapeHtml(joke.setup)}</div>
            ${joke.delivery ? `<div class="joke-punchline">${this.escapeHtml(joke.delivery)}</div>` : ''}
        `;

        this.elements.jokeContent.innerHTML = html;
        this.elements.jokeContent.classList.remove('new-joke');
        // Trigger reflow to restart animation
        void this.elements.jokeContent.offsetWidth;
        this.elements.jokeContent.classList.add('new-joke');
    },

    /**
     * Show loading spinner
     * @param {boolean} show - Whether to show or hide
     */
    showLoading(show) {
        if (show) {
            this.elements.loading.classList.remove('hidden');
            this.elements.jokeContent.classList.add('hidden');
            this.elements.getJokeBtn.disabled = true;
        } else {
            this.elements.loading.classList.add('hidden');
            this.elements.jokeContent.classList.remove('hidden');
            this.elements.getJokeBtn.disabled = false;
        }
    },

    /**
     * Show error message
     * @param {string} message - Error message
     */
    showError(message) {
        this.elements.errorMessage.textContent = message || 'An error occurred. Please try again.';
        this.elements.error.classList.remove('hidden');
        this.elements.jokeContent.classList.add('hidden');
    },

    /**
     * Hide error message
     */
    hideError() {
        this.elements.error.classList.add('hidden');
        this.elements.jokeContent.classList.remove('hidden');
    },

    /**
     * Handle Copy Joke button click
     */
    handleCopyJoke() {
        if (!this.state.currentJoke) return;

        const joke = `${this.state.currentJoke.setup}\n${this.state.currentJoke.delivery}`;
        
        navigator.clipboard.writeText(joke).then(() => {
            const originalText = this.elements.copyBtn.textContent;
            this.elements.copyBtn.textContent = '✓ Copied!';
            setTimeout(() => {
                this.elements.copyBtn.textContent = originalText;
            }, 2000);
        }).catch(err => {
            console.error('Failed to copy:', err);
        });
    },

    /**
     * Handle API selection change
     * @param {string} apiType - Selected API
     */
    handleApiChange(apiType) {
        // Show/hide joke type selector based on API
        if (apiType === 'jokeapi') {
            this.elements.jokeTypeSelector.style.display = 'block';
        } else {
            this.elements.jokeTypeSelector.style.display = 'none';
        }
    },

    /**
     * Update statistics
     */
    updateStats() {
        this.elements.jokeCount.textContent = this.state.jokeCount;
        const apiType = this.elements.apiSelect.value;
        const apiName = apiType === 'jokeapi' ? 'JokeAPI' : 'Official Joke API';
        this.elements.apiName.textContent = apiName;
    },

    /**
     * Escape HTML special characters
     * @param {string} text - Text to escape
     * @returns {string} Escaped text
     */
    escapeHtml(text) {
        const map = {
            '&': '&amp;',
            '<': '&lt;',
            '>': '&gt;',
            '"': '&quot;',
            "'": '&#039;'
        };
        return text.replace(/[&<>"']/g, m => map[m]);
    }
};

// Export for use in other modules
if (typeof module !== 'undefined' && module.exports) {
    module.exports = UI;
}
