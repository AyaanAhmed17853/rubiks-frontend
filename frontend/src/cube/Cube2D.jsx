function Face({ facelets }) {
  return (
    <div className="cube-face">
      {facelets.map((color, index) => (
        <div
          key={index}
          className={`sticker sticker-${color}`}
        />
      ))}
    </div>
  );
}

function Cube2D({ state }) {
  const U = state.slice(0, 9);
  const R = state.slice(9, 18);
  const F = state.slice(18, 27);
  const D = state.slice(27, 36);
  const L = state.slice(36, 45);
  const B = state.slice(45, 54);

  return (
    <div className="cube-net">
      <div className="cube-row cube-row-top">
        <Face facelets={U} />
      </div>

      <div className="cube-row">
        <Face facelets={L} />
        <Face facelets={F} />
        <Face facelets={R} />
        <Face facelets={B} />
      </div>

      <div className="cube-row cube-row-bottom">
        <Face facelets={D} />
      </div>
    </div>
  );
}

export default Cube2D;
