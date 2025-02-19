All of our implementation of Super Sampling is handled in the Aliasing.h file. 
In Main.cpp, we are calling the supersampling methods and saving them to our directory.

Here are the five filters we are calling in Main:
1. Box filter anti-aliased image: Applies anti-aliasing to the image after ray tracing (box filter)
2. Per-pixel box filter anti-aliased image: Applies anti-aliasing to the image w/uniform sampling and box filter
3. Per-pixel Gaussian filter anti-aliased image: Applies anti-aliasing to the image w/uniform sampling and Gaussian filter
4. Jittered box filter anti-aliased image: Applies anti-aliasing to the image w/jittering sampling and box filter
5. Jittered Gaussian filter anti-aliased image: Applies anti-aliasing to the image w/jittering sampling and Gaussian filter
6. Downsampled image: Downsampled image.

More details on how we implemented them is in our documentation and comments within Aliasing.h.

If you do not want certain alaising methods to run, then comment out the:
1. unsinged char* 
2. alias. method
3. char outputPath
4. snprintf
5. simplePNG_write
6.free
of the corresponding alaising methods.

How you run the program should be the same as before, the antiAliasing outputs an image that is twice of what was in the command line.
Ex: 'cmake --build . && ./build/tracer -l -r 400 400  ./tests/blue_sphere.obj test1.png' will output a 800x800 image for the anti-aliasing
It will be half the size of the original output for under sampling (200x200 in this case)
