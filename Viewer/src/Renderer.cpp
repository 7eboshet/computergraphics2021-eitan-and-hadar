#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>

#include "Renderer.h"
#include "InitShader.h"

#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)
#define Z_INDEX(width,x,y) ((x)+(y)*(width))

Renderer::Renderer(int viewport_width, int viewport_height) :
	viewport_width_(viewport_width),
	viewport_height_(viewport_height)
{
	InitOpenGLRendering();
	CreateBuffers(viewport_width, viewport_height);
}

Renderer::~Renderer()
{
	delete[] color_buffer_;
}

void Renderer::PutPixel(int i, int j, const glm::vec3& color)
{
	if (i < 0) return; if (i >= viewport_width_) return;
	if (j < 0) return; if (j >= viewport_height_) return;
	
	color_buffer_[INDEX(viewport_width_, i, j, 0)] = color.x;
	color_buffer_[INDEX(viewport_width_, i, j, 1)] = color.y;
	color_buffer_[INDEX(viewport_width_, i, j, 2)] = color.z;
}

void Renderer::DrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color)
{
<<<<<<< HEAD
=======
	int half_width = viewport_width_ / 2;
	int half_height = viewport_height_ / 2;
	int thickness = 15;


>>>>>>> ad9eb4426372cd8eea7a05acf670940196962153
	// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
	int delta_x, delta_y,x_0,y_0,x_1,y_1,p_0,two_delta_y,two_delta_x_y;
	//(x_0,y_0) is the left endpoint
	if (p1.x < p2.x) {
		x_0 = p1.x;
		y_0 = p1.y;
		x_1 = p2.x;
		y_1 = p2.y;
	}
	else {
		x_0 = p2.x;
		y_0 = p2.y;
		x_1 = p1.x;
		y_1 = p1.y;
	}
	//compute delta_x and delta_y
	delta_x = x_1 - x_0;
	if (y_0 - y_1 > 0) {
		delta_y = y_0 - y_1;
	}
	else {
		delta_y = y_1 - y_0;
	}
	//compute the step of the while
	int st = 0;
	if (delta_x < delta_y) {
		st = delta_y;
	}
	else {
		st = delta_x;
	}
	//compute some const
	p_0 = 2 * delta_y - delta_x;
	two_delta_y = 2 * delta_y;
	two_delta_x_y = two_delta_y - 2 * delta_x;
	//load (x_0,y_0) pixel
	PutPixel(x_0, y_0, color);
	while (st > 0) {
		if (p_0 < 0) {
			x_0++;
			p_0 = p_0 + two_delta_y;
		}
		else {
			x_0++;
			y_0++;
			p_0 = p_0 + two_delta_x_y;
		}
<<<<<<< HEAD
		PutPixel(x_0, y_0, color);
=======

		for (int j = x_0 - thickness; j < x_0 + thickness; j++)
		{
			for (int i = y_0 - thickness; i < y_0 + thickness; i++)
			{
				PutPixel(j, i, color);
			}
		}
		
		
>>>>>>> ad9eb4426372cd8eea7a05acf670940196962153
		st--;
	}
}
void Renderer::CreateBuffers(int w, int h)
{
	CreateOpenGLBuffer(); //Do not remove this line.
	color_buffer_ = new float[3 * w * h];
	ClearColorBuffer(glm::vec3(0.0f, 0.0f, 0.0f));
}

//##############################
//##OpenGL stuff. Don't touch.##
//##############################

// Basic tutorial on how opengl works:
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
// don't linger here for now, we will have a few tutorials about opengl later.
void Renderer::InitOpenGLRendering()
{
	// Creates a unique identifier for an opengl texture.
	glGenTextures(1, &gl_screen_tex_);

	// Same for vertex array object (VAO). VAO is a set of buffers that describe a renderable object.
	glGenVertexArrays(1, &gl_screen_vtc_);

	GLuint buffer;

	// Makes this VAO the current one.
	glBindVertexArray(gl_screen_vtc_);

	// Creates a unique identifier for a buffer.
	glGenBuffers(1, &buffer);

	// (-1, 1)____(1, 1)
	//	     |\  |
	//	     | \ | <--- The exture is drawn over two triangles that stretch over the screen.
	//	     |__\|
	// (-1,-1)    (1,-1)
	const GLfloat vtc[]={
		-1, -1,
		 1, -1,
		-1,  1,
		-1,  1,
		 1, -1,
		 1,  1
	};

	const GLfloat tex[]={
		0,0,
		1,0,
		0,1,
		0,1,
		1,0,
		1,1};

	// Makes this buffer the current one.
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// This is the opengl way for doing malloc on the gpu. 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc)+sizeof(tex), NULL, GL_STATIC_DRAW);

	// memcopy vtc to buffer[0,sizeof(vtc)-1]
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);

	// memcopy tex to buffer[sizeof(vtc),sizeof(vtc)+sizeof(tex)]
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	// Loads and compiles a sheder.
	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );

	// Make this program the current one.
	glUseProgram(program);

	// Tells the shader where to look for the vertex position data, and the data dimensions.
	GLint  vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition,2,GL_FLOAT,GL_FALSE,0,0 );

	// Same for texture coordinates data.
	GLint  vTexCoord = glGetAttribLocation( program, "vTexCoord" );
	glEnableVertexAttribArray( vTexCoord );
	glVertexAttribPointer( vTexCoord,2,GL_FLOAT,GL_FALSE,0,(GLvoid *)sizeof(vtc) );

	//glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );

	// Tells the shader to use GL_TEXTURE0 as the texture id.
	glUniform1i(glGetUniformLocation(program, "texture"),0);

}

void Renderer::CreateOpenGLBuffer()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, gl_screen_tex_);

	// malloc for a texture on the gpu.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, viewport_width_, viewport_height_, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, viewport_width_, viewport_height_);
}

void Renderer::SwapBuffers()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, gl_screen_tex_);

	// memcopy's colorBuffer into the gpu.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, viewport_width_, viewport_height_, GL_RGB, GL_FLOAT, color_buffer_);

	// Tells opengl to use mipmapping
	glGenerateMipmap(GL_TEXTURE_2D);

	// Make glScreenVtc current VAO
	glBindVertexArray(gl_screen_vtc_);

	// Finally renders the data.
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::ClearColorBuffer(const glm::vec3& color)
{
	for (int i = 0; i < viewport_width_; i++)
	{
		for (int j = 0; j < viewport_height_; j++)
		{
			PutPixel(i, j, color);
		}
	}
}

void Renderer::Render(const Scene& scene)
{
	// TODO: Replace this code with real scene rendering code
	int half_width = viewport_width_ / 2;
	int half_height = viewport_height_ / 2;
	int thickness = 15;
	
	for(int i = 0; i < viewport_width_; i++)
	{
		for (int j = half_height - thickness; j < half_height + thickness; j++)
		{
			PutPixel(i, j, glm::vec3(1, 1, 0));
		}
	}

	for (int i = 0; i < viewport_height_; i++)
	{
		for (int j = half_width - thickness; j < half_width + thickness; j++)
		{
			PutPixel(j, i, glm::vec3(1, 0, 1));
		}	
	}
<<<<<<< HEAD
	DrawLine(glm::ivec2(1000, 1000), glm::ivec2(1000, 15000), glm::ivec3(1,0,1));
=======
	DrawLine(glm::ivec2(300, 300), glm::ivec2(700, 600), glm::ivec3(1,0,1));
>>>>>>> ad9eb4426372cd8eea7a05acf670940196962153
}

int Renderer::GetViewportWidth() const
{
	return viewport_width_;
}

int Renderer::GetViewportHeight() const
{
	return viewport_height_;
}

void Renderer::UseDrawLine(const glm::ivec2& p1, const glm::ivec2& p2, const glm::vec3& color)
{
	DrawLine(p1, p2, color);
}
