# **SDF Rendering**

---

## **Nathan Yang, Anthony Mui, Joshua Lowe, Tony Martin, Jason Heflinger**

### **Shapes Implemented:**
- Spheres
- Donut
- Julia Set
- Mandelbulb
- Helix/Spring
- Cool S (2D)
- Cross (2D)
- Box (2D)

**Other Features:**
- Union
- Intersection
- Difference
- Displace
- Smooth Minimum
- Twist
- 2D -> 3D: Extrusion and Revolution

---

**Rendering Solids Using SDFs**

To render solids with SDFs, we utilize raymarching. This involves finding the closest object in the scene, then moving along the ray that distance. So if we find that the nearest object is 5ft away, we march 5 ft *in the ray's direction* (not towards the object). We then check if the march distance is extremely small (< 0.00001). If it is, then we say that we have hit the object. If this never occurs, we continue marching until the max number of steps is reached.

Our marching function looks very similar to this:
```
bool marchLoop(Ray ray, Scene scene, Hitpoint hit){
	for (int i = 0; i < MAX_RAYMARCH_STEPS; i++)
	{
		// Find distance to closest object
		float safeStepSize = rootPrim.getSignedDistance(ray.origin(), hit);
		if (safeStepSize < MIN_RAYMARCH_STEP_SIZE)
		{
			 return true;
		}
		
		// Continue marching from new spot
		ray = Ray(ray.direction(), ray.pointAtParameter(safeStepSize));
    }
	return false;
}
```

To get the normal vector of a function, consider that the distance from an object increases fastest if we move along its normal vector. Also consider that we can find the direction at which distance increases fastest by using a gradient. By approximating a gradient, we can identify the SDF's normal vector, which can then be used for shading.

---

**Combining SDFs**

Since we determine hits and misses just by an object's distance, any operation we can perform on floats can be used to modify how our objects look. We can implement a few different operations using simple min's and max's

- Union = min(a, b)
- Intersection = max(a, b)
- Difference = min(a, -b)

Smooth minimums are a bit more complicated, requiring the use of a smoothing function. These are detailed [here](https://iquilezles.org/articles/smin/). We used a quadratic smoothing function.

---

**2D to 3D**

We used Inigo Quilez's extrusion and revolution algorithms [here](https://iquilezles.org/articles/distfunctions/) for converting 2-dimensional SDFs to 3-dimensional ones. Both of these work such that if the original SDF is "exact" (does not at all distort the object), then the resulting 3D SDF will also be exact. 

Extrusion works by computing the 2D SDF and using the Pythagorean theorem to find the diagonal to the target point. Because the object is uniform along one axis, the correct coordinate is given.

Revolution follows a similar principle, in which we compute the distance to the shape with an offset *as if* we are in its plane, transforming each point into (xz - offset, y). This has the effect of rotating the shape around the y axis and could be replicated on either of the other two by modifying the axes. 

---

**Fractals**

We implemented two fractals as well, the Julia set and the Mandelbulb. These fractals are extremely difficult to render with traditional raytracing since it is very hard to solve for the ray-fractal intersection. Solving for the SDF is easier, and then by just getting "close enough" with raymarching, we can pretty accurately render fractal shapes.

---

**How to Demo**

The command line arguments have been modified to allow the specification of either a raytraced scene, or a raymarched scene. To do this, inject the `-sdf` argument into the command like so:

`program.exe -r <resolution x> <resolution y> -sdf <obj_path.obj/sdf_scene_name> <output_image_path.png>`

If the `-sdf` argument exists, then the program execution will be switched to use raymarching and to process SDFs.
Note that the `.obj` file format does not support our custom SDF primitives and operations by default, so the SDF
demo scenes have been hardcoded in the `src/sdf_scenes/` folder and included using the `src/SDFSceneLoader.h` class. While the second-to-last argument for raytracing is the `.obj` scene path, when using raymarching, it simply becomes
the name of the demo scene to raymarch on. The currently supported demo options are the following:

- `julia`
- `mandel`
- `doohickey`
- `helix`
- `morph`
- `sharkegg`
- `donut`
- `cools`
- `rbox`
- `rcross`
- `peanut`
- `intersect`
- `union`

As a final example, the usage of the program to launch the `helix` scene may look like the following:

`program.exe -r 300 300 -sdf helix outimage.png`