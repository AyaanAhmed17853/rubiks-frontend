"""
Standalone validation of the cube/move/corner-encoding logic used in the
C++ implementation (src/cube.cpp, src/pieces.cpp). No C++ toolchain is
available in this environment, so this script mirrors the same tables and
checks the invariants the C++ code relies on:

  1. Each move applied 4 times returns to the solved state.
  2. A random scramble followed by the reversed sequence of inverse moves
     returns to the solved state (validates inverseMove + cycle tables).
  3. decodeCorners/decodeEdges always produce valid invariants
     (corner orientation sum % 3 == 0, edge permutation is a bijection,
     edge orientation sum % 2 == 0) after random scrambles.
  4. The cubie-level composition formula (cpMove/coMove + applyCornerMove)
     matches decodeCorners(actual scrambled cube) exactly, for many random
     scrambles -- this is the property the corner pattern database and the
     IDA* heuristic depend on.
"""
import random

SOLVED = "U" * 9 + "R" * 9 + "F" * 9 + "D" * 9 + "L" * 9 + "B" * 9

# (a,b,c,d): content moves a -> b -> c -> d -> a
FACE_CYCLES = {
    0: [(0, 2, 8, 6), (1, 5, 7, 3), (9, 18, 36, 45), (10, 19, 37, 46), (11, 20, 38, 47)],   # U
    1: [(9, 11, 17, 15), (10, 14, 16, 12), (2, 51, 29, 20), (5, 48, 32, 23), (8, 45, 35, 26)], # R
    2: [(18, 20, 26, 24), (19, 23, 25, 21), (6, 9, 29, 44), (7, 12, 28, 41), (8, 15, 27, 38)], # F
    3: [(27, 29, 35, 33), (28, 32, 34, 30), (24, 15, 51, 42), (25, 16, 52, 43), (26, 17, 53, 44)], # D
    4: [(36, 38, 44, 42), (37, 41, 43, 39), (0, 18, 27, 53), (3, 21, 30, 50), (6, 24, 33, 47)], # L
    5: [(45, 47, 53, 51), (46, 50, 52, 48), (0, 42, 35, 11), (1, 39, 34, 14), (2, 36, 33, 17)], # B
}


def cycle4(f, a, b, c, d):
    t = f[a]
    f[a] = f[d]
    f[d] = f[c]
    f[c] = f[b]
    f[b] = t


def turn_face(f, face):
    for (a, b, c, d) in FACE_CYCLES[face]:
        cycle4(f, a, b, c, d)


def apply_move(f, idx):
    face, var = divmod(idx, 3)
    times = 1 if var == 0 else 2 if var == 1 else 3
    for _ in range(times):
        turn_face(f, face)


def inverse_move(idx):
    face, var = divmod(idx, 3)
    inv_var = 1 if var == 1 else 2 - var
    return face * 3 + inv_var


def move_str(idx):
    faces = "URFDLB"
    face, var = divmod(idx, 3)
    return faces[face] + ("2" if var == 1 else "'" if var == 2 else "")


# --- corner / edge tables (mirrors pieces.cpp) ---

CORNER_FACELET = [
    (8, 9, 20),
    (6, 18, 38),
    (0, 36, 47),
    (2, 45, 11),
    (29, 26, 15),
    (27, 44, 24),
    (33, 53, 42),
    (35, 17, 51),
]
CORNER_COLOR = [
    "URF", "UFL", "ULB", "UBR", "DFR", "DLF", "DBL", "DRB",
]

EDGE_FACELET = [
    (5, 10), (7, 19), (3, 37), (1, 46),
    (32, 16), (28, 25), (30, 43), (34, 52),
    (23, 12), (21, 41), (50, 39), (48, 14),
]
EDGE_COLOR = [
    "UR", "UF", "UL", "UB", "DR", "DF", "DL", "DB", "FR", "FL", "BL", "BR",
]


def decode_corners(f):
    cp = [0] * 8
    co = [0] * 8
    for s in range(8):
        a, b, d = (f[i] for i in CORNER_FACELET[s])
        for k in range(8):
            if sorted((a, b, d)) == sorted(CORNER_COLOR[k]):
                cp[s] = k
                break
        if a in "UD":
            co[s] = 0
        elif b in "UD":
            co[s] = 1
        else:
            co[s] = 2
    return cp, co


def decode_edges(f):
    ep = [0] * 12
    eo = [0] * 12
    for s in range(12):
        a, b = (f[i] for i in EDGE_FACELET[s])
        for k in range(12):
            if sorted((a, b)) == sorted(EDGE_COLOR[k]):
                ep[s] = k
                eo[s] = 0 if a == EDGE_COLOR[k][0] else 1
                break
    return ep, eo


def encode_corners(cp, co):
    fact = [1, 1, 2, 6, 24, 120, 720, 5040]
    perm_index = 0
    for i in range(8):
        smaller = sum(1 for j in range(i + 1, 8) if cp[j] < cp[i])
        perm_index += smaller * fact[7 - i]
    ori_index = 0
    p3 = 1
    for i in range(7):
        ori_index += co[i] * p3
        p3 *= 3
    return perm_index * 2187 + ori_index


def apply_corner_move(cp, co, cp_move, co_move):
    new_cp = [cp[cp_move[i]] for i in range(8)]
    new_co = [(co[cp_move[i]] + co_move[i]) % 3 for i in range(8)]
    return new_cp, new_co


def main():
    random.seed(12345)

    # 1. Each move^4 == identity
    for m in range(18):
        f = list(SOLVED)
        for _ in range(4):
            apply_move(f, m)
        assert f == list(SOLVED), f"move {move_str(m)} ^4 != identity"
    print("OK: every move has order dividing 4")

    # Build cpMove/coMove tables
    cp_move = [None] * 18
    co_move = [None] * 18
    for m in range(18):
        f = list(SOLVED)
        apply_move(f, m)
        cp_move[m], co_move[m] = decode_corners(f)
    print("OK: built cpMove/coMove tables")
    for m in range(18):
        print(f"  {move_str(m):>3}: cp={cp_move[m]} co={co_move[m]}")

    # 2 & 3 & 4. Random scrambles
    for trial in range(200):
        length = random.randint(1, 30)
        moves = [random.randint(0, 17) for _ in range(length)]

        f = list(SOLVED)
        for m in moves:
            apply_move(f, m)

        # invariants
        cp, co = decode_corners(f)
        assert sorted(cp) == list(range(8)), (trial, moves, cp)
        assert sum(co) % 3 == 0, (trial, moves, co)

        ep, eo = decode_edges(f)
        assert sorted(ep) == list(range(12)), (trial, moves, ep)
        assert sum(eo) % 2 == 0, (trial, moves, eo)

        # cubie-level composition matches direct decode
        cp2, co2 = [0, 1, 2, 3, 4, 5, 6, 7], [0] * 8
        for m in moves:
            cp2, co2 = apply_corner_move(cp2, co2, cp_move[m], co_move[m])
        assert cp2 == cp and co2 == co, (trial, moves, cp, co, cp2, co2)

        # inverse sequence undoes the scramble
        f2 = list(f)
        for m in reversed(moves):
            apply_move(f2, inverse_move(m))
        assert f2 == list(SOLVED), (trial, moves)

    print("OK: 200 random scrambles -- invariants hold, composition formula "
          "matches direct decode, inverse sequence restores solved state")

    # encode range check
    seen = set()
    for trial in range(2000):
        length = random.randint(0, 15)
        moves = [random.randint(0, 17) for _ in range(length)]
        f = list(SOLVED)
        for m in moves:
            apply_move(f, m)
        cp, co = decode_corners(f)
        idx = encode_corners(cp, co)
        assert 0 <= idx < 40320 * 2187
        seen.add(idx)
    print(f"OK: encode_corners stayed in range; {len(seen)} distinct corner "
          f"states seen across 2000 random scrambles (<= 88179840)")


if __name__ == "__main__":
    main()