/**
 * API Integration Module
 * Handles communication with joke APIs
 */

const API = {
    // JokeAPI configuration
    jokeApi: {
        baseUrl: 'https://v2.jokeapi.dev/joke',
        name: 'JokeAPI'
    },

    // Official Joke API configuration
    officialApi: {
        baseUrl: 'https://official-joke-api.appspot.com/jokes',
        name: 'Official Joke API'
    },

    /**
     * Fetch joke from JokeAPI
     * @param {string} type - Joke type (any, general, programming, knock-knock)
     * @returns {Promise<Object>} Joke object
     */
    async fetchFromJokeAPI(type = 'any') {
        try {
            // Build URL based on type
            let url = this.jokeApi.baseUrl;
            
            if (type !== 'any') {
                url += `/${type}`;
            } else {
                // Get random type
                const types = ['general', 'programming', 'knock-knock'];
                const randomType = types[Math.floor(Math.random() * types.length)];
                url += `/${randomType}`;
            }

            url += '?format=json';

            const response = await fetch(url, {
                headers: {
                    'Accept': 'application/json'
                }
            });

            if (!response.ok) {
                throw new Error(`API Error: ${response.status}`);
            }

            const data = await response.json();

            // Format response
            return {
                setup: data.setup || '',
                delivery: data.delivery || data.joke,
                type: data.type,
                apiName: this.jokeApi.name,
                success: true
            };
        } catch (error) {
            console.error('JokeAPI Error:', error);
            throw new Error('Failed to fetch from JokeAPI: ' + error.message);
        }
    },

    /**
     * Fetch joke from Official Joke API
     * @returns {Promise<Object>} Joke object
     */
    async fetchFromOfficialAPI() {
        try {
            const url = this.officialApi.baseUrl + '/random';

            const response = await fetch(url, {
                headers: {
                    'Accept': 'application/json'
                }
            });

            if (!response.ok) {
                throw new Error(`API Error: ${response.status}`);
            }

            const data = await response.json();

            // Format response
            return {
                setup: data.setup || '',
                delivery: data.punchline,
                type: data.type,
                apiName: this.officialApi.name,
                success: true
            };
        } catch (error) {
            console.error('Official API Error:', error);
            throw new Error('Failed to fetch from Official API: ' + error.message);
        }
    },

    /**
     * Get joke from selected API
     * @param {string} apiType - API to use (jokeapi or official)
     * @param {string} jokeType - Joke type (for JokeAPI)
     * @returns {Promise<Object>} Formatted joke
     */
    async getJoke(apiType = 'jokeapi', jokeType = 'any') {
        try {
            let joke;

            if (apiType === 'jokeapi') {
                joke = await this.fetchFromJokeAPI(jokeType);
            } else if (apiType === 'official') {
                joke = await this.fetchFromOfficialAPI();
            } else {
                throw new Error('Unknown API type: ' + apiType);
            }

            return joke;
        } catch (error) {
            console.error('Error getting joke:', error);
            throw error;
        }
    },

    /**
     * Get multiple jokes
     * @param {number} count - Number of jokes to fetch
     * @param {string} apiType - API to use
     * @returns {Promise<Array>} Array of joke objects
     */
    async getMultipleJokes(count = 5, apiType = 'jokeapi') {
        try {
            const jokes = [];
            for (let i = 0; i < count; i++) {
                const joke = await this.getJoke(apiType);
                jokes.push(joke);
                // Add delay to avoid rate limiting
                await new Promise(resolve => setTimeout(resolve, 500));
            }
            return jokes;
        } catch (error) {
            console.error('Error getting multiple jokes:', error);
            throw error;
        }
    }
};

// Export for use in other modules
if (typeof module !== 'undefined' && module.exports) {
    module.exports = API;
}
