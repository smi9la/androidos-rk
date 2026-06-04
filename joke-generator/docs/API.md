# API Documentation

## Supported APIs

### 1. JokeAPI (Recommended)

**Base URL:** `https://v2.jokeapi.dev/joke`

**Features:**
- Multiple joke types (general, programming, knock-knock, etc.)
- High reliability
- No authentication required
- JSON response format
- Fast response times

**Endpoints:**

#### Get Random Joke of Specific Type
```
GET https://v2.jokeapi.dev/joke/general?format=json
GET https://v2.jokeapi.dev/joke/programming?format=json
GET https://v2.jokeapi.dev/joke/knock-knock?format=json
```

**Response Example:**
```json
{
  "error": false,
  "category": "General",
  "type": "twopart",
  "setup": "Why don't scientists trust atoms?",
  "delivery": "Because they make up everything!",
  "flags": {
    "nsfw": false,
    "religious": false,
    "political": false,
    "racist": false,
    "sexist": false,
    "explicit": false
  },
  "id": 1,
  "safe": true,
  "lang": "en"
}
```

**Rate Limits:**
- No specific rate limit mentioned
- Typically allows 100+ requests per minute

---

### 2. Official Joke API

**Base URL:** `https://official-joke-api.appspot.com/jokes`

**Features:**
- Simple and easy to use
- No authentication required
- JSON response format
- Multiple joke types

**Endpoints:**

#### Get Random Joke
```
GET https://official-joke-api.appspot.com/jokes/random
```

#### Get Random Joke by Type
```
GET https://official-joke-api.appspot.com/jokes/{type}/random
```

**Response Example:**
```json
{
  "type": "general",
  "setup": "Why did the scarecrow win an award?",
  "punchline": "He was outstanding in his field.",
  "id": 1
}
```

**Available Types:**
- general
- knock-knock
- programming
- knock-knock

**Rate Limits:**
- Typically 100+ requests per minute

---

## Implementation Guide

### Fetching a Joke

```javascript
// Using fetch API
fetch('https://v2.jokeapi.dev/joke/general?format=json')
  .then(response => response.json())
  .then(data => {
    console.log(data.setup);
    console.log(data.delivery);
  })
  .catch(error => console.error('Error:', error));
```

### Error Handling

```javascript
async function getJoke() {
  try {
    const response = await fetch('https://v2.jokeapi.dev/joke/general?format=json');
    
    if (!response.ok) {
      throw new Error(`HTTP Error: ${response.status}`);
    }
    
    const data = await response.json();
    return data;
  } catch (error) {
    console.error('Failed to fetch joke:', error.message);
    return null;
  }
}
```

### CORS Handling

Both APIs support CORS headers, so requests from frontend JavaScript are allowed.

```javascript
// No special CORS configuration needed
const response = await fetch(apiUrl, {
  headers: {
    'Accept': 'application/json'
  }
});
```

---

## Rate Limiting

Both APIs are public and free to use with reasonable rate limits:

- **JokeAPI:** ~100+ requests/minute
- **Official Joke API:** ~100+ requests/minute

For production use with higher volume, consider:
1. Caching responses
2. Implementing request throttling
3. Using batch endpoints if available

---

## Response Codes

### Successful Responses
- **200 OK** - Joke retrieved successfully

### Error Responses
- **400 Bad Request** - Invalid parameters
- **404 Not Found** - Endpoint or joke not found
- **429 Too Many Requests** - Rate limit exceeded
- **500 Server Error** - API server error

---

## Best Practices

### 1. Error Handling
```javascript
try {
  const joke = await fetchJoke();
  displayJoke(joke);
} catch (error) {
  displayError('Failed to load joke. Please try again.');
}
```

### 2. Loading States
```javascript
// Show loading indicator
showLoading(true);

try {
  const joke = await fetchJoke();
  displayJoke(joke);
} finally {
  showLoading(false);
}
```

### 3. Caching
```javascript
const jokeCache = new Map();

async function getCachedJoke(type) {
  if (jokeCache.has(type)) {
    return jokeCache.get(type);
  }
  
  const joke = await fetchJoke(type);
  jokeCache.set(type, joke);
  return joke;
}
```

### 4. Timeout Handling
```javascript
function fetchWithTimeout(url, timeout = 5000) {
  return Promise.race([
    fetch(url),
    new Promise((_, reject) =>
      setTimeout(() => reject(new Error('Timeout')), timeout)
    )
  ]);
}
```

---

## References

- **JokeAPI:** https://jokeapi.dev
- **Official Joke API:** https://official-joke-api.appspot.com

---

## License

Both APIs are free to use. Check their respective documentation for usage terms.
