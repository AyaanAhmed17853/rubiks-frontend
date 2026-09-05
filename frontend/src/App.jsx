import { useState } from 'react';
import Cube2D from './cube/Cube2D';
import { createSolvedCube } from './cube/cubeState';
import {
  applyMove,
  applyMoveSequence,
} from './cube/moves';
import './App.css';

function App() {
  const [cubeState, setCubeState] = useState(createSolvedCube());
  const [scramble, setScramble] = useState('');

  function handleMove(move) {
    setCubeState((currentState) =>
      applyMove(currentState, move)
    );
  }

  function handleScramble() {
    try {
      const newState = applyMoveSequence(
        createSolvedCube(),
        scramble
      );

      setCubeState(newState);
    } catch (error) {
      alert(error.message);
    }
  }

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

          <button onClick={handleScramble}>
            Scramble
          </button>
        </div>
      </div>

      <div className="cube-container">
        <Cube2D state={cubeState} />

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
        <p>Enter a scramble and click Solve.</p>
      </div>
    </div>
  );
}

export default App;

