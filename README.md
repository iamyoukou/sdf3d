# Signed distance field (SDF) in 3D
Calculate an SDF for a 3D object.

# The barycentric coordinates system
The full article of calculating the barycentric coordinate of a point can be found [here](https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/barycentric-coordinates).

For short, if a triangle is defined by vertices `ABC` in counter-clockwise,
and `P` is a point on the same plane with `ABC`,
then `P` can be described as `P = uA + vB + wC`.
Here, `(u, v, w)` is called the barycentric coordinate of `P` with respect to `ABC`.

`(u, v, w)` can be calculated using the ratio of triangle areas.

Let `Sabc`, `Sabp`, `Sbcp` and `Sacp` be the area of triangle `ABC`, `ABP`, `BCP` and `ACP`, respectively.
Then `(u, v, w)` can be calculated as
```
u = Sbcp/Sabc
v = Sacp/Sabc
w = Sabp/Sabc
```

## Triangle area
If a triangle is defined by vertices `ABC`,
then its area can be calculated as
`Sabc = 0.5 * length(cross(AB, AC))`.

Note that the magnitude of the cross product of `AB` and `AC` defines the area of a parallelogram,
and the area of a triangle is equal to half of that area, i.e.
```
Sparallelogram = |AB|*|AC|*sin(∠A) = length(cross(AB, AC))
Striangle = 0.5 * Sparallelogram
```

# Reference
[Fuhrmann,2003] Fuhrmann, Arnulph, Gerrit Sobotka, and Clemens Groß. "Distance fields for rapid collision detection in physically based modeling." Proceedings of GraphiCon 2003. 2003.
