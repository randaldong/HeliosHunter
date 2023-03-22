# Helios Hunter: Ray-Tracer

## 1. [Structure](https://cseweb.ucsd.edu/~alchern/teaching/cse167_fa21/7-1RayTracing.pdf)

```c++
Image Raytrace(Camera cam, Scene scene, int width, int height){
    Image image = new Image(width,height);
 	for (int j=0; j<height; j++){
 		for (int i=0; i<width; i++){
 			Ray ray = RayThruPixel( cam, i, j );
			Intersection hit = Intersect( ray, scene );
 			image[i][j] = FindColor( hit );
 		}
	}
    return image;
}
```

- `RayThruPixel(cam, x, y)`  generates a ray originated from the camera position, through the center of the `(x, y)` pixel into the world
- `Intersect(ray, scene)` searches over all all geometries in the scene and returns the closest hit 
- `FindColor(hit)` shade the light color seen by the in-coming ray 
  - Ambient + Lambertian-diffuse + Blinn–Phong formula
  - Add the contribution of light only when the ray connecting the hit and the light source does not have any intersection with the scene (Numerical inaccuracy may cause intersection to be below surface, causing surface to incorrectly shadow itself. To avoid self-shadowing, the secondary ray is shot off slightly above the hitting point.)
  - Do recursive ray tracing

### 1.1 RayThruPixel

```c++
Ray RayThruPixel(int x, int y) 
	{	// w = glm::normalize(eye - center);
		// u = glm::normalize(glm::cross(up, w));
		// v = glm::cross(w, u);
    	// radian fovy
		float alpha = aspect * tan(fovy / 2) * (2 * (x + 0.5)) - width) / width;
		float beta = tan(fovy / 2) * (height - 2 * (y + 0.5)) / height;
		vec3 dir = glm::normalize(alpha * u + beta * v - w);
		return Ray(eye, dir);
	}
```



### 1.2 Ray-Scene Intersection

```c++
Intersection Intersect(Ray ray, Scene scene){
 	Distance mindist = INFINITY;
 	Intersection hit;
 	foreach (object in scene){ // Find closest intersection; test all objects
 		Intersection hit_temp = Intersect(ray, object);
 		if (hit_temp.dist< mindist){ // closer than previous hit
 			mindist = hit_temp.dist;
 			hit = hit_temp;
 		}
 	}
 	return hit;
}
```

## 2. Classes

```c++
Transform
	|__Utils
    
Film
    |__Camera
    |__Scene
    |__Intersection
    
    Camera
    	|__Ray
    		|__Utils
    
    Scene
    	|__Light
    	|__BVH
    		|__Object
    			|__Bbox
    				|__Ray
    	
    Intersection
    	|__Object
    
```

## 3. Results

The rendering results and timings are shown in the following images.

### 3.1 Scene 4 Table with Spheres

#### 3.1.1 Ambient

![scene4-ambient](https://i.imgur.com/4xSUAru.png)

![](https://i.imgur.com/7pBtrq5.png)

#### 3.1.2 Diffuse

![scene4-diffuse](https://i.imgur.com/4s1ET71.png)

![](https://i.imgur.com/P3TtYtd.png)

#### 3.1.3 Emission

![scene4-emission](https://i.imgur.com/zz8aHen.png)

![](https://i.imgur.com/2fyZfSD.png)

#### 3.1.4 Specular

![scene4-specular](https://i.imgur.com/7W778vE.png)

![](https://i.imgur.com/5l0gshb.png)

### 3.2 Scene 5 Thousand Spheres

![scene5](https://i.imgur.com/KaFHdDP.png)

![](https://i.imgur.com/wZkzJMn.png)

### 3.3 Scene 6 Cornell Box

![scene6](https://i.imgur.com/ePKgaVc.png)

![](https://i.imgur.com/2kTVNP1.png)

### 3.4 Scene 7 Stanford Dragon

![scene7](https://i.imgur.com/YrXt7zN.png)

![](https://i.imgur.com/Qj0X2n9.png)

