import { useState, useRef, useEffect } from 'react';
import Cube2D from './cube/Cube2D';
import { createSolvedCube } from './cube/cubeState';
import {
  applyMove,
  applyMoveSequence,
} from './cube/moves';
import './App.css';
import { solveScramble } from './api/solver';

const PLAYBACK_DELAY_MS = 500;

function App() {
  const [cubeState, setCubeState] = useState(createSolvedCube());
  const [scramble, setScramble] = useState('');
  const [solution, setSolution] = useState('');

  // --- New state for the playback cube ---
  const [scrambledState, setScrambledState] = useState(null); // frozen scrambled facelets, set on Solve
  const [playbackState, setPlaybackState] = useState(createSolvedCube());
  const [isPlaying, setIsPlaying] = useState(false);
  const [currentMoveIndex, setCurrentMoveIndex] = useState(0);
  const playbackTimerRef = useRef(null);

  // Clean up any running timer if the component unmounts mid-playback
  useEffect(() => {
    return () => {
      if (playbackTimerRef.current) {
        clearInterval(playbackTimerRef.current);
      }
    };
  }, []);

  function handleMove(move) {
    setCubeState((currentState) =>
      applyMove(currentState, move)
    );
  }

  async function handleSolve() {
    try {
      const data = await solveScramble(scramble);
      const facelets = data.facelets.split('');

      // Stop any in-progress playback from a previous solve
      if (playbackTimerRef.current) {
        clearInterval(playbackTimerRef.current);
        playbackTimerRef.current = null;
      }
      setIsPlaying(false);
      setCurrentMoveIndex(0);

      // Scramble is applied to BOTH cubes at once, from the same trusted source
      setCubeState(facelets);
      setScrambledState(facelets);
      setPlaybackState(facelets);
      setSolution(data.solution);

      console.log('Solution:', data.solution);
      console.log('Moves:', data.moveCount);
      console.log('Time:', data.timeSeconds);
      console.log('Verified:', data.verified);
    } catch (error) {
      alert(error.message);
    }
  }

  function handlePlay() {
    if (!scrambledState || !solution) return;

    const moves = solution.trim().split(/\s+/).filter(Boolean);
    if (moves.length === 0) return;

    // Guard against multiple simultaneous timers (also covers "restart")
    if (playbackTimerRef.current) {
      clearInterval(playbackTimerRef.current);
      playbackTimerRef.current = null;
    }

    setPlaybackState(scrambledState);
    setCurrentMoveIndex(0);
    setIsPlaying(true);

    let idx = 0;
    playbackTimerRef.current = setInterval(() => {
      idx += 1;
      const moveToApply = moves[idx - 1];

      setPlaybackState((prev) => applyMove(prev, moveToApply));
      setCurrentMoveIndex(idx);

      if (idx >= moves.length) {
        clearInterval(playbackTimerRef.current);
        playbackTimerRef.current = null;
        setIsPlaying(false);
      }
    }, PLAYBACK_DELAY_MS);
  }

  const totalMoves = solution
    ? solution.trim().split(/\s+/).filter(Boolean).length
    : 0;

  return (
    <div className="app">
      <h1>Rubik's Cube Solver</h1>

      <div className="scramble-section">
        <label htmlFor="scramble">Scramble</label>

        <div className="scramble-input-row">
          <input
            id="scramble"
            type="text"
            placeholder="R U2 F' L"
            value={scramble}
            onChange={(e) => setScramble(e.target.value)}
          />

          <button onClick={handleSolve}>
            Solve
          </button>
        </div>
      </div>

      <div className="cube-container">
        <div className="cubes-row">
          <div className="cube-panel">
            <h3>Main Cube</h3>
            <Cube2D state={cubeState} interactive={true} />
          </div>

          <div className="cube-panel">
            <h3>Playback Cube</h3>
            <Cube2D state={playbackState} interactive={false} />
            <div className="playback-controls">
              <button onClick={handlePlay} disabled={!solution}>
                {isPlaying ? 'Restart' : 'Play'}
              </button>
              <p className="move-progress">
                Move {currentMoveIndex} / {totalMoves}
              </p>
            </div>
          </div>
        </div>

        <div className="move-controls">
          {[
            'U', 'U2', "U'",
            'R', 'R2', "R'",
            'F', 'F2', "F'",
            'D', 'D2', "D'",
            'L', 'L2', "L'",
            'B', 'B2', "B'",
          ].map((move) => (
            <button
              key={move}
              onClick={() => handleMove(move)}
            >
              {move}
            </button>
          ))}
        </div>
      </div>

      <div className="solution-section">
        <h2>Solution</h2>
        <p>
          {solution || 'Enter a scramble and click Solve.'}
        </p>
      </div>
    </div>
  );
}

export default App;