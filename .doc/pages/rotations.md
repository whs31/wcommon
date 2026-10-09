# Vectors, matrices and rotations {#rotations}

`Vector3`, `Matrix3x3` and `Quaternion` are small `constexpr` value types of doubles for
kinematics and rigid-body math. They are not a general linear-algebra library; use Eigen when
more is needed.

## Frames and units

A vector does not know its frame or unit: the variable name does. Name vectors
`<quantity>_<frame>` (`velocity_ned`, `omega_body`), attitudes `attitude_<from>_to_<to>`
(`attitude_body_to_ned`) and rotation matrices `<from>_to_<to>`.

## Quaternion conventions

- Quaternions are **Hamilton**, scalar first `(w, x, y, z)`: `i * j == k`.
- Rotations are **active** and right-handed about their axis: `q.rotate(v) == q ⊗ (0, v) ⊗ q*`.
- An attitude is a **body-to-reference** rotation:
  `v_ned = attitude_body_to_ned.rotate(v_body)` and
  `v_body = attitude_body_to_ned.inverse_rotate(v_ned)`.
- Composition chains the frame names: `attitude_a_to_c = attitude_b_to_c * attitude_a_to_b`, i.e.
  `a * b` is the rotation `b` followed by `a`.
- `q.to_rotation_matrix()` is the same rotation: `body_to_ned * v_body == v_ned`. Its inverse is
  its transpose.
- `q` and `−q` are the same rotation; no sign of `w` is enforced.
- Kinematics: `d/dt attitude_body_to_ned == attitude_body_to_ned.derivative(omega_body)`, i.e.
  `½ q ⊗ (0, ω)` with `ω` the angular velocity of the body relative to the reference frame, in
  body axes. Integration drifts off the unit sphere; renormalize after each step.

## Euler angles

`Quaternion::from_euler_angles` and `to_euler_angles`, and their mirrors
`EulerAngles::to_quaternion` and `EulerAngles::from_quaternion`, use the aerospace ZYX sequence:
`q = q_z(yaw) ⊗ q_y(pitch) ⊗ q_x(roll)`, i.e. yaw about the reference z axis, then pitch about
the new y axis, then roll about the new x axis. With NED reference axes and FRD body axes, yaw
90° points the nose east, positive pitch raises the nose and positive roll lowers the right side.

At ±90° pitch only the sum or difference of roll and yaw is defined; `to_euler_angles` then
returns zero roll. Keep quaternions in state and convert to Euler angles only at boundaries
(configuration, logging, displays).

## Matrices

`Matrix3x3` is row-major: `(m * v).x() == m.row(0).dot(v)`. `Matrix3x3::cross_product(v) * u`
equals `v.cross(u)`. `inverse()` throws `NumericError` for a singular matrix and `try_inverse()`
returns `std::nullopt` instead; the singularity test is relative to the scale of the rows, so a
tiny but well-conditioned matrix inverts.

## Layout

`Vector2`, `Vector3`, `EulerAngles`, `Matrix3x3` and `Quaternion` guarantee their storage layout
(contiguous doubles in the documented order, standard layout, trivially copyable), so reflection
systems may register them by offset or bit-cast them to `std::array<double, N>`. Each header
checks its layout with `static_assert`s.
