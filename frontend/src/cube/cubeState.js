export const SOLVED_CUBE =
  'UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB';

export function createSolvedCube() {
  return SOLVED_CUBE.split('');
}

export function isSolved(state) {
  return state.join('') === SOLVED_CUBE;
}