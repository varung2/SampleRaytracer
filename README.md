# SampleRaytracer
Sample Raytracer using C++ and minimal libraries

CS 296 Final Project by Varun Govind

Commands *--------------------------------------------------

1. run "make";
2. run "./main  -[PROJECTION]"
	a. put flag "-orthographic" "-orth" "-o" for an
	orthographic projection.

	b. default is a perspective projection.


Introduction *----------------------------------------------
	This is a barebones raytracer that currently uses
	several shapes to calculate the color of each pixel
	based on what object is hit. There are several types of
	objects/shapes used; triangles, spheres, planes, are 
	some of them.
	I generally followed through the book "Raytracing in One
	Weekend," and I tried to emulate how Shirley wrote his
	raytracer. I created the shapes triangle, and plane for
	the project.
