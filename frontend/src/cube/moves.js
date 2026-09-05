// Face indices in the 54-sticker representation:
//
// U = 0..8
// R = 9..17
// F = 18..26
// D = 27..35
// L = 36..44
// B = 45..53

function cycle4(state, a, b, c, d) {
  const temp = state[a];

  state[a] = state[d];
  state[d] = state[c];
  state[c] = state[b];
  state[b] = temp;
}

function turnFace(state, face) {
  switch (face) {
    // U
    case 0:
      cycle4(state, 0, 2, 8, 6);
      cycle4(state, 1, 5, 7, 3);

      cycle4(state, 9, 18, 36, 45);
      cycle4(state, 10, 19, 37, 46);
      cycle4(state, 11, 20, 38, 47);
      break;

    // R
    case 1:
      cycle4(state, 9, 11, 17, 15);
      cycle4(state, 10, 14, 16, 12);

      cycle4(state, 2, 51, 29, 20);
      cycle4(state, 5, 48, 32, 23);
      cycle4(state, 8, 45, 35, 26);
      break;

    // F
    case 2:
      cycle4(state, 18, 20, 26, 24);
      cycle4(state, 19, 23, 25, 21);

      cycle4(state, 6, 9, 29, 44);
      cycle4(state, 7, 12, 28, 41);
      cycle4(state, 8, 15, 27, 38);
      break;

    // D
    case 3:
      cycle4(state, 27, 29, 35, 33);
      cycle4(state, 28, 32, 34, 30);

      cycle4(state, 24, 15, 51, 42);
      cycle4(state, 25, 16, 52, 43);
      cycle4(state, 26, 17, 53, 44);
      break;

    // L
    case 4:
      cycle4(state, 36, 38, 44, 42);
      cycle4(state, 37, 41, 43, 39);

      cycle4(state, 0, 18, 27, 53);
      cycle4(state, 3, 21, 30, 50);
      cycle4(state, 6, 24, 33, 47);
      break;

    // B
    case 5:
      cycle4(state, 45, 47, 53, 51);
      cycle4(state, 46, 50, 52, 48);

      cycle4(state, 0, 42, 35, 11);
      cycle4(state, 1, 39, 34, 14);
      cycle4(state, 2, 36, 33, 17);
      break;
  }
}

export function applyMove(state, move) {
  const result = [...state];

  const face = 'URFDLB'.indexOf(move[0]);

  if (face === -1) {
    throw new Error(`Invalid move: ${move}`);
  }

  let times = 1;

  if (move.length === 2) {
    if (move[1] === '2') {
      times = 2;
    } else if (move[1] === "'") {
      times = 3;
    } else {
      throw new Error(`Invalid move: ${move}`);
    }
  } else if (move.length > 2) {
    throw new Error(`Invalid move: ${move}`);
  }

  for (let i = 0; i < times; i++) {
    turnFace(result, face);
  }

  return result;
}

export function parseMoves(sequence) {
  if (!sequence || !sequence.trim()) {
    return [];
  }

  return sequence.trim().split(/\s+/);
}

export function applyMoveSequence(state, sequence) {
  let result = [...state];

  for (const move of parseMoves(sequence)) {
    result = applyMove(result, move);
  }

  return result;
}