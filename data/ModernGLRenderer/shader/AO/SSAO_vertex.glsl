
//-------------------------------------------- Inputs ----------------------------------------------
//View matrix to transform from world space to view space.
uniform mat4 view_matrix;

uniform mat4 projection_matrix_inv;

//Corner position of the screen space quad.
attribute vec3 position;

//Texture coordinates of the screen space quad.
attribute vec2 texture_coordinates;

//--------------------------------------------- Outputs --------------------------------------------

//Direction from the camera.
varying vec3 camera_dir;

//Texture coordinates of the screen space quad.
varying vec2 texcoords;

//------------------------------------------ Main Program ------------------------------------------
void main()
{
	texcoords = texture_coordinates;

	camera_dir = (view_matrix * vec4(position,1)).xyz;

	gl_Position = vec4(position, 1.0);
}