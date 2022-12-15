# Procedurally Generated Planetary Systems [[Demo Video]](https://drive.google.com/file/d/17O0q1xAGQV3OTreEXil6ZQOanpqKTdn6/view)

## 1. Orbits and movements

A planet orbits around another with respect to an orbital axis and radius. Planets are stored in a tree such that updating each planet’s CTM is equivalent to performing a pre-order traversal. This hierarchical structure enables nested rotational relationships (moons orbiting around planets, systems orbiting around systems, etc.)

## 2. Planetary system generation

The user could generate a procedural planetary system that is similar to our solar system with some control over the parameters. These planets use procedurally generated textures for further variations.

## 3. Camera

There are two camera modes:

- Default Camera: Use **W, A, S, D** to move and the **mouse** to look around.
- Orbit Camera: Use **W, S** to zoom in and out; use **A, D** to switch between planets; use the **mouse** to rotate about the planet.

## 4. Texture Generation

The planet colors are random variations on some pre-defined color palettes. We divide planets into two types - one with “terrain” and the other with “rings”.

For terrain-like texture generation, we implemented Perlin noise, and used the noise values to get the colors. For planets with rings, we used a simple Beizer Curve.

## 5. Normal Mapping

Normals are sampled from image textures, which stores values in tangent space. To make lighting works correctly, we transform the lighting variables from world space to tangent space, and calculate lighting with tangent-space normals. This transformation is done in the vertex shader, since lighting variables remain the same across all fragments.
