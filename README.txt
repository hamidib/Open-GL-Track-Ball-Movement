bhamidi@csu.fullerton.edu
Bijan Hamidi
Teapot-Trackball
teapot_trackball.cpp

For this assignment we were given the current mouse position and the previous mouse position in two seperate tuples.  However these positions were related to the pixels on the screen rather than the orthognal base in 3D space.  The first task was the convert these mouse postions as if they had been on the near plane rather than pixels.  Using the mouse positions windowsize height/width and the nearplane halfheight/halfwidth the coordinates were changed on line 231, 232, 241 & 242 into cooresponding floating point numbers for the x and y values.  I then converted these x and y values into a vec4 by adding mainCamera.near as the z, and 0 for w.  The purpose of this was to project the new near plane coordinates into camera space by multiplying them by the lookAtMatrix. 

These new camera space coordinates where then passed to the shoemake function which calculated if the point selected on the near plane (p) landed on the sphere (P), and also new values for the selected point to relate to the surface of the sphere.  

With these two points the I then calculated the cross product and the dot product to get an axis of rotation and the angle of rotation.  With these values I could then create the rotation matrix to multiply by the modelViewMatrix.  

I had some issues with getting formula 32(line255,255) on page 211 of the handout to create the rotation matrix as the teapot does not spin.