import React, { useState } from 'react';
import './LibraryPanel.css';

const LibraryPanel = ({ games, onScan, onLaunch, onTransfer }) => {
  const [selectedFolder, setSelectedFolder] = useState('');
  const [filter, setFilter] = useState('all');
  const [sortBy, setSortBy] = useState('title');
  const [searchTerm, setSearchTerm] = useState('');

  const handleFolderSelect = async (e) => {
    const folder = e.target.value;
    setSelectedFolder(folder);
    if (folder) {
      await onScan(folder);
    }
  };

  const filteredGames = games.filter((game) => {
    const matchesSearch = game.title.toLowerCase().includes(searchTerm.toLowerCase());
    return matchesSearch;
  });

  const sortedGames = [...filteredGames].sort((a, b) => {
    if (sortBy === 'title') return a.title.localeCompare(b.title);
    if (sortBy === 'size') return b.size - a.size;
    if (sortBy === 'modified') return b.modified - a.modified;
    return 0;
  });

  return (
    <div className="library-panel">
      <div className="library-header">
        <h2>ROM Library Manager</h2>
        <div className="library-controls">
          <input
            type="text"
            placeholder="Search games..."
            value={searchTerm}
            onChange={(e) => setSearchTerm(e.target.value)}
            className="search-input"
          />
          <select value={sortBy} onChange={(e) => setSortBy(e.target.value)}>
            <option value="title">Sort by Title</option>
            <option value="size">Sort by Size</option>
            <option value="modified">Sort by Modified</option>
          </select>
        </div>
      </div>

      <div className="library-content">
        <div className="games-grid">
          {sortedGames.length === 0 ? (
            <div className="empty-library">
              <p>No ROMs found. Add a folder to scan for games.</p>
              <button
                onClick={() => {
                  // File picker would go here
                  alert('File picker not implemented in demo');
                }}
              >
                Browse Folder
              </button>
            </div>
          ) : (
            sortedGames.map((game) => (
              <div key={game.id} className="game-card">
                <div className="game-cover">
                  <img src={game.cover || '/game-placeholder.png'} alt={game.title} />
                  <div className="game-overlay">
                    <button
                      className="play-btn"
                      onClick={() => onLaunch(game.id)}
                      title="Launch game"
                    >
                      ▶ Play
                    </button>
                  </div>
                </div>
                <div className="game-info">
                  <h4>{game.title}</h4>
                  <p className="game-meta">
                    {(game.size / (1024 * 1024)).toFixed(2)} MB
                  </p>
                </div>
              </div>
            ))
          )}
        </div>
      </div>

      <div className="library-footer">
        <div className="stats">
          <span>{sortedGames.length} games found</span>
          <span>
            {(sortedGames.reduce((sum, g) => sum + g.size, 0) / (1024 * 1024 * 1024)).toFixed(2)} GB total
          </span>
        </div>
        <button className="btn sync-btn" onClick={() => onTransfer([])}>Sync to Device</button>
      </div>
    </div>
  );
};

export default LibraryPanel;
