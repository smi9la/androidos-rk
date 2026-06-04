/**
 * Main Application Module
 * Initializes and runs the joke generator
 */

(function() {
    'use strict';

    // Initialize application when DOM is ready
    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', init);
    } else {
        init();
    }

    function init() {
        console.log('🎭 Random Joke Generator Initialized');
        UI.init();
        // Load first joke on startup
        UI.handleGetJoke();
    }

    // Global error handler
    window.addEventListener('error', (event) => {
        console.error('Uncaught error:', event.error);
    });

    window.addEventListener('unhandledrejection', (event) => {
        console.error('Unhandled promise rejection:', event.reason);
    });
})();
