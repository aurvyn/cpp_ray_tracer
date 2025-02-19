**SDF Rendering**

---

**Nathan Yang, Anthony Mui, Joshua Lowe, Tony Martin, Jason Heflinger**

**Shapes Implemented:**
- Spheres
- Donut
- Julia Set
- Mandelbulb
- Extruded 2D shapes (Not fully operational)

**Other Features:**
- Union
- Intersection
- Difference
- Smooth Minimum
- Twist

---

**Rendering Solids Using SDFs**

To render solids with SDFs, we utilize raymarching. This involves finding the closest object in the scene, then moving along the ray that distance. So if we find that the nearest object is 5ft away, we march 5ft *along the ray* (Not towards the object). We then check if the march distance is extremely small (< 0.00001). If it is, then we say that we have hit the object. If this never occurs, we continue marching until the max number of steps is reached.

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
- Intersction = max(a, b)
- Difference = min(a, -b)

Smooth minimums are a bit more complicated, requiring the use of a smoothing function. These are detailed [here](https://iquilezles.org/articles/smin/). We used a quadratic smoothing function.

---

**2D to 3D**

---

**Fractals**