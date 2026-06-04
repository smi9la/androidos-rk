# Development Guide

## Local Development Setup

### Prerequisites
- Modern web browser (Chrome, Firefox, Safari, Edge)
- Text editor (VS Code, Sublime, etc.)
- Basic understanding of JavaScript, HTML, CSS

### Getting Started

1. **Clone the repository**
   ```bash
   git clone https://github.com/smi9la/joke-generator.git
   cd joke-generator
   ```

2. **Start a local server** (required for CORS)
   
   **Using Python 3:**
   ```bash
   python3 -m http.server 8000
   ```

   **Using Python 2:**
   ```bash
   python -m SimpleHTTPServer 8000
   ```

   **Using Node.js:**
   ```bash
   npx http-server
   ```

3. **Open in browser**
   ```
   http://localhost:8000
   ```

---

## Project Structure

```
joke-generator/
├── index.html              # Main HTML file
├── css/
│   └── style.css           # All styling
├── js/
│   ├── main.js            # Application initialization
│   ├── api.js             # API integration
│   └── ui.js              # UI interactions
├── docs/
│   ├── API.md             # API documentation
│   └── DEVELOPMENT.md     # This file
└── README.md              # Project README
```

---

## Code Structure

### api.js - API Integration

**Purpose:** Handles all communication with external APIs

**Main Functions:**
- `API.fetchFromJokeAPI(type)` - Fetch from JokeAPI
- `API.fetchFromOfficialAPI()` - Fetch from Official API
- `API.getJoke(apiType, jokeType)` - Get joke from selected API
- `API.getMultipleJokes(count, apiType)` - Get multiple jokes

**Example Usage:**
```javascript
const joke = await API.getJoke('jokeapi', 'programming');
console.log(joke.setup);
console.log(joke.delivery);
```

### ui.js - User Interface

**Purpose:** Handles all UI interactions and updates

**Main Functions:**
- `UI.init()` - Initialize UI and attach listeners
- `UI.displayJoke(joke)` - Display joke in UI
- `UI.showLoading(show)` - Show/hide loading spinner
- `UI.showError(message)` - Display error message
- `UI.handleGetJoke()` - Handle Get Joke button click
- `UI.handleCopyJoke()` - Copy joke to clipboard

**State Management:**
```javascript
UI.state = {
  jokeCount: 0,        // Number of jokes fetched
  currentJoke: null,   // Current displayed joke
  isLoading: false     // Loading state
}
```

### main.js - Application Initialization

**Purpose:** Entry point for the application

**Responsibilities:**
- Wait for DOM to load
- Initialize UI
- Global error handling

---

## Adding a New API

### Step 1: Add API Configuration

```javascript
// In api.js
API.newApi = {
    baseUrl: 'https://api.example.com/jokes',
    name: 'Example API'
};
```

### Step 2: Add Fetch Function

```javascript
// In api.js
async fetchFromNewAPI() {
    try {
        const response = await fetch(this.newApi.baseUrl + '/random');
        if (!response.ok) throw new Error(`API Error: ${response.status}`);
        
        const data = await response.json();
        return {
            setup: data.joke || '',
            delivery: data.punchline || '',
            type: 'general',
            apiName: this.newApi.name,
            success: true
        };
    } catch (error) {
        throw new Error('Failed to fetch from New API: ' + error.message);
    }
}
```

### Step 3: Add to getJoke Function

```javascript
// In api.js
async getJoke(apiType = 'jokeapi', jokeType = 'any') {
    if (apiType === 'newapi') {
        joke = await this.fetchFromNewAPI();
    }
    // ...
}
```

### Step 4: Add Option to HTML

```html
<!-- In index.html -->
<select id="api-select">
    <option value="jokeapi">JokeAPI</option>
    <option value="official">Official Joke API</option>
    <option value="newapi">Example API</option>
</select>
```

---

## Styling Customization

### Color Scheme

```css
/* Primary colors */
--primary: #667eea;
--secondary: #764ba2;

/* Text colors */
--text-dark: #2c3e50;
--text-light: #95a5a6;

/* Background colors */
--bg-light: #f8f9fa;
--bg-white: #ffffff;
```

### Modifying the Gradient

```css
body {
    background: linear-gradient(135deg, #your-color1 0%, #your-color2 100%);
}
```

### Changing Font

```css
body {
    font-family: 'Your Font', sans-serif;
}
```

---

## Debugging

### Enable Debug Logging

Add to `main.js` or console:

```javascript
// Log all API calls
const originalFetch = window.fetch;
window.fetch = function(...args) {
    console.log('Fetch:', args[0]);
    return originalFetch.apply(this, args);
};
```

### Debug UI State

```javascript
// In browser console
console.log(UI.state);
console.log(UI.elements);
```

### Test API Directly

```javascript
// In browser console
API.getJoke('jokeapi', 'programming')
    .then(joke => console.log(joke))
    .catch(error => console.error(error));
```

---

## Performance Optimization

### 1. Lazy Loading

```javascript
// Load jokes only when needed
let jokeCache = {};

async function getCachedJoke(type) {
    if (jokeCache[type]) {
        return jokeCache[type];
    }
    const joke = await API.getJoke('jokeapi', type);
    jokeCache[type] = joke;
    return joke;
}
```

### 2. Request Throttling

```javascript
let lastRequestTime = 0;
const MIN_REQUEST_INTERVAL = 500; // ms

async function throttledGetJoke() {
    const now = Date.now();
    const timeSinceLastRequest = now - lastRequestTime;
    
    if (timeSinceLastRequest < MIN_REQUEST_INTERVAL) {
        await new Promise(resolve => 
            setTimeout(resolve, MIN_REQUEST_INTERVAL - timeSinceLastRequest)
        );
    }
    
    lastRequestTime = Date.now();
    return API.getJoke();
}
```

### 3. Image Optimization

Currently uses emojis. To use custom images:

```html
<img src="emoji.svg" alt="Joke" />
```

---

## Testing

### Manual Testing Checklist

- [ ] Get Joke button works
- [ ] Copy button works and shows feedback
- [ ] API selector changes between APIs
- [ ] Joke type selector only shows for JokeAPI
- [ ] Loading spinner appears during fetch
- [ ] Error message displays on failure
- [ ] Joke count increments
- [ ] Enter key triggers Get Joke
- [ ] Responsive on mobile
- [ ] Works offline (if cached)

### Automated Testing (Jest)

```javascript
// test/api.test.js
describe('API Module', () => {
    test('fetches joke from JokeAPI', async () => {
        const joke = await API.getJoke('jokeapi', 'general');
        expect(joke).toHaveProperty('setup');
        expect(joke).toHaveProperty('delivery');
    });
});
```

---

## Deployment

### Option 1: GitHub Pages

```bash
# Push to gh-pages branch
git subtree push --prefix joke-generator origin gh-pages
```

Access at: `https://smi9la.github.io/joke-generator`

### Option 2: Netlify

1. Connect GitHub repository
2. Set build command: (none needed)
3. Deploy!

### Option 3: Self-Hosted

```bash
# Copy files to web server
cp -r joke-generator/* /var/www/html/jokes/
```

---

## Common Issues

### CORS Error

**Problem:** "Access to XMLHttpRequest blocked by CORS policy"

**Solution:** Use a local server or CORS proxy

```bash
python3 -m http.server 8000
```

### Blank Page

**Problem:** Nothing displays

**Solution:** 
1. Check browser console for errors
2. Verify all files are in correct directories
3. Clear cache and refresh

### API Not Responding

**Problem:** "Failed to fetch from API"

**Solution:**
1. Check internet connection
2. Verify API is still active (check https://jokeapi.dev)
3. Try switching to different API

---

## Contributing

1. Fork repository
2. Create feature branch: `git checkout -b feature/name`
3. Commit changes: `git commit -am 'Add feature'`
4. Push to branch: `git push origin feature/name`
5. Submit pull request

---

## Resources

- [MDN Web Docs](https://developer.mozilla.org)
- [JavaScript.info](https://javascript.info)
- [CSS-Tricks](https://css-tricks.com)
- [Fetch API Guide](https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API)

---

## License

MIT License - Feel free to use and modify
