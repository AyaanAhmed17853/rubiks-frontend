import React, { useMemo } from 'react';
import * as THREE from 'three';
import { Canvas } from '@react-three/fiber';
import { OrbitControls } from '@react-three/drei';

// ============================================================
// Cube / sticker colors
// ============================================================
//
// Your existing cube state uses:
//
// U = White
// R = Red
// F = Green
// D = Yellow
// L = Orange
// B = Blue
//

const STICKER_COLORS = {
  U: '#ffffff',
  R: '#ff2020',
  F: '#00c853',
  D: '#ffeb00',
  L: '#ff8c00',
  B: '#1555ff',
};

const PLASTIC_COLOR = '#111111';

function getColor(value) {
  return STICKER_COLORS[value] || PLASTIC_COLOR;
}

// ============================================================
// 54-facelet indexing
// ============================================================
//
// The existing cube state is:
//
// 0  - 8   = U
// 9  - 17  = R
// 18 - 26  = F
// 27 - 35  = D
// 36 - 44  = L
// 45 - 53  = B
//
// Each face is stored row-major:
//
// 0 1 2
// 3 4 5
// 6 7 8
//
// These mappings convert a 3D cubie's x/y/z position into
// the corresponding sticker index on each visible face.
// ============================================================

// U: row 0 = back, row 2 = front
const uIndex = (x, y, z) =>
  0 + (z + 1) * 3 + (x + 1);

// R: row 0 = top, col 0 = front
const rIndex = (x, y, z) =>
  9 + (1 - y) * 3 + (1 - z);

// F: row 0 = top, col 0 = left
const fIndex = (x, y, z) =>
  18 + (1 - y) * 3 + (x + 1);

// D: row 0 = front
const dIndex = (x, y, z) =>
  27 + (1 - z) * 3 + (x + 1);

// L: row 0 = top, col 0 = back
const lIndex = (x, y, z) =>
  36 + (1 - y) * 3 + (z + 1);

// B: row 0 = top, col 0 = right
const bIndex = (x, y, z) =>
  45 + (1 - y) * 3 + (1 - x);

// ============================================================
// Geometry
// ============================================================

const CUBIE_SIZE = 0.99;
const SPACING = 1.0;

const boxGeometry = new THREE.BoxGeometry(
  CUBIE_SIZE,
  CUBIE_SIZE,
  CUBIE_SIZE
);

// ============================================================
// Individual cubie
// ============================================================

function Cubie({ x, y, z, state }) {
  const materials = useMemo(() => {
    //
    // BoxGeometry material order:
    //
    // 0 = +X = Right
    // 1 = -X = Left
    // 2 = +Y = Up
    // 3 = -Y = Down
    // 4 = +Z = Front
    // 5 = -Z = Back
    //

    const rightColor =
      x === 1
        ? getColor(state[rIndex(x, y, z)])
        : PLASTIC_COLOR;

    const leftColor =
      x === -1
        ? getColor(state[lIndex(x, y, z)])
        : PLASTIC_COLOR;

    const upColor =
      y === 1
        ? getColor(state[uIndex(x, y, z)])
        : PLASTIC_COLOR;

    const downColor =
      y === -1
        ? getColor(state[dIndex(x, y, z)])
        : PLASTIC_COLOR;

    const frontColor =
      z === 1
        ? getColor(state[fIndex(x, y, z)])
        : PLASTIC_COLOR;

    const backColor =
      z === -1
        ? getColor(state[bIndex(x, y, z)])
        : PLASTIC_COLOR;

    return [
      rightColor,
      leftColor,
      upColor,
      downColor,
      frontColor,
      backColor,
    ].map(
      (color) =>
        new THREE.MeshStandardMaterial({
          color,
          roughness: 0.8,
          metalness: 0,
        })
    );
  }, [x, y, z, state]);

  return (
    <mesh
      geometry={boxGeometry}
      material={materials}
      position={[
        x * SPACING,
        y * SPACING,
        z * SPACING,
      ]}
    />
  );
}

// ============================================================
// Complete 3x3x3 cube
// ============================================================

function CubeGroup({ state }) {
  const cubies = [];

  for (let x = -1; x <= 1; x++) {
    for (let y = -1; y <= 1; y++) {
      for (let z = -1; z <= 1; z++) {
        cubies.push(
          <Cubie
            key={`${x}-${y}-${z}`}
            x={x}
            y={y}
            z={z}
            state={state}
          />
        );
      }
    }
  }

  return <group>{cubies}</group>;
}

// ============================================================
// Main component
// ============================================================
//
// NEW: `interactive` prop (default true) controls whether
// OrbitControls is mounted. This is the ONLY behavioral
// change to this file. Existing usage <Cube2D state={x} />
// is unaffected — it still defaults to interactive.
//
// For the playback cube, render:
//   <Cube2D state={playbackState} interactive={false} />
// This mounts a fixed camera (same default position) with
// no OrbitControls, so there is no mouse rotation/zoom and
// no drift between renders.
// ============================================================

export default function Cube2D({ state, interactive = true }) {
  return (
    <div className="cube3d-container">
      <Canvas
        camera={{
          position: [4.5, 4, 5],
          fov: 45,
        }}
      >
        {/* Basic lighting */}
        <ambientLight intensity={0.7} />

        <directionalLight
          position={[5, 8, 5]}
          intensity={0.8}
        />

        <directionalLight
          position={[-5, -3, -5]}
          intensity={0.3}
        />

        {/* The actual 3x3x3 Rubik's Cube */}
        <CubeGroup state={state} />

        {/* Mouse-controlled camera — only for the interactive cube */}
        {interactive && (
          <OrbitControls
            enableDamping
            dampingFactor={0.1}
            enablePan={false}
            minDistance={3.5}
            maxDistance={9}
            minPolarAngle={0.2}
            maxPolarAngle={Math.PI - 0.2}
            target={[0, 0, 0]}
          />
        )}
      </Canvas>
    </div>
  );
}