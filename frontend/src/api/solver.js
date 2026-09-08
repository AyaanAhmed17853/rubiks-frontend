const API_URL = 'http://localhost:8080';

export async function solveScramble(scramble) {
  const response = await fetch(`${API_URL}/solve`, {
    method: 'POST',
    headers: {
      'Content-Type': 'text/plain',
    },
    body: scramble,
  });

  if (!response.ok) {
    throw new Error(`Server returned HTTP ${response.status}`);
  }

  const data = await response.json();

  if (!data.success) {
    throw new Error(data.error || 'Solver failed');
  }

  return data;
}
