// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <functional>

// Marching Cubes masks
#include "TriTable.hpp"
#define BOTTOM_LEFT_FRONT   1
#define BOTTOM_RIGHT_FRONT  2
#define BOTTOM_RIGHT_BACK   4
#define BOTTOM_LEFT_BACK    8
#define TOP_LEFT_FRONT      16
#define TOP_RIGHT_FRONT     32
#define TOP_RIGHT_BACK      64
#define TOP_LEFT_BACK       128

#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;
GLuint ProgramID;	// Link the program
GLuint MatrixID;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <vector>
#include <iostream>
#include <ostream>
#include <cmath>
#include <fstream>
using namespace std;

/****** CAMERA ******/
glm::vec3 cameraPos = {5.0f, 5.0f, 5.0f};     // world position for camera
glm::vec3 cameraFront = {0.0f, 0.0f, 0.0f};
glm::vec3 cameraUp = {0.0f, 1.0f, 0.0f};
bool screenFlip = false;

// caluclate offsets to move camera
float lastX;
float lastY;
float xoffset;
float yoffset;

float speed = 2.5f;     // forward/backward camera speed
float r = 5.0f;
float camYaw = -90.0f;  // theta
float camPitch = 90.0f; // phi

// mouse input
bool leftClickPressed = false;
double mouseX, mouseY;

// time
float deltaTime = 0.0f;
float lastTime = 0.0f;

/****** Coordinate axes ******/
class Axes {

	glm::vec3 origin;
	glm::vec3 extents;

	glm::vec3 xcol = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 ycol = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 zcol = glm::vec3(0.0f, 0.0f, 1.0f);

public:

	Axes(glm::vec3 orig, glm::vec3 ex) : origin(orig), extents(ex) {}

	void draw() {

		glLineWidth(2.0f);
		glBegin(GL_LINES);
            glColor3f(xcol.x, xcol.y, xcol.z);
            glVertex3f(origin.x, origin.y, origin.z);
            glVertex3f(origin.x + extents.x + 0.1, origin.y, origin.z);

            glVertex3f(origin.x + extents.x + 0.1, origin.y, origin.z);
            glVertex3f(origin.x + extents.x + 0.1, origin.y, origin.z+0.1);
            glVertex3f(origin.x + extents.x + 0.1, origin.y, origin.z);
            glVertex3f(origin.x + extents.x + 0.1, origin.y, origin.z-0.1);

            glColor3f(ycol.x, ycol.y, ycol.z);
            glVertex3f(origin.x, origin.y, origin.z);
            glVertex3f(origin.x, origin.y + extents.y + 0.1, origin.z);

            glVertex3f(origin.x, origin.y + extents.y + 0.1, origin.z);
            glVertex3f(origin.x, origin.y + extents.y + 0.1, origin.z+0.1);
            glVertex3f(origin.x, origin.y + extents.y + 0.1, origin.z);
            glVertex3f(origin.x, origin.y + extents.y + 0.1, origin.z-0.1);
            
            glColor3f(zcol.x, zcol.y, zcol.z);
            glVertex3f(origin.x, origin.y, origin.z);
            glVertex3f(origin.x, origin.y, origin.z + extents.z + 0.1);
            
            glVertex3f(origin.x, origin.y, origin.z + extents.z + 0.1);
            glVertex3f(origin.x+0.1, origin.y, origin.z + extents.z + 0.1);
            glVertex3f(origin.x, origin.y, origin.z + extents.z + 0.1);
            glVertex3f(origin.x-0.1, origin.y, origin.z + extents.z + 0.1);
		glEnd();

	}

};

/****** Box around marching volume ******/
class Box {

	glm::vec3 origin;
	glm::vec3 extents;

public:

	Box(glm::vec3 orig, glm::vec3 ex) : origin(orig), extents(ex) {}

	void draw() {

		glLineWidth(2.0f);
        glColor3f(0.5f, 0.5f, 0.5f);
		glBegin(GL_LINES);
        
            glVertex3f(origin.x, origin.y, origin.z);
            glVertex3f(origin.x + extents.x, origin.y, origin.z);
            glVertex3f(origin.x + extents.x, origin.y, origin.z);
            glVertex3f(origin.x + extents.x, origin.y + extents.y, origin.z);
            glVertex3f(origin.x + extents.x, origin.y + extents.y, origin.z);
            glVertex3f(origin.x, origin.y + extents.y, origin.z);

            glVertex3f(origin.x, origin.y + extents.y, origin.z);
            glVertex3f(origin.x, origin.y + extents.y, origin.z + extents.z);
            glVertex3f(origin.x, origin.y + extents.y, origin.z + extents.z);
            glVertex3f(origin.x + extents.x, origin.y + extents.y, origin.z + extents.z);
            glVertex3f(origin.x + extents.x, origin.y + extents.y, origin.z + extents.z);
            glVertex3f(origin.x + extents.x, origin.y + extents.y, origin.z);


            glVertex3f(origin.x, origin.y, origin.z);
            glVertex3f(origin.x, origin.y + extents.y, origin.z);
            glVertex3f(origin.x, origin.y, origin.z + extents.z);
            glVertex3f(origin.x, origin.y + extents.y, origin.z + extents.z);

            glVertex3f(origin.x, origin.y, origin.z + extents.z);
            glVertex3f(origin.x + extents.x, origin.y, origin.z + extents.z);
            
            
            glVertex3f(origin.x + extents.x, origin.y, origin.z);
            glVertex3f(origin.x + extents.x, origin.y, origin.z + extents.z);
            glVertex3f(origin.x + extents.x, origin.y, origin.z + extents.z);
            glVertex3f(origin.x + extents.x, origin.y + extents.y, origin.z + extents.z);
            glVertex3f(origin.x, origin.y, origin.z);
            glVertex3f(origin.x, origin.y, origin.z + extents.z);

		glEnd();

	}

};

/****** Marching cubes ******/
typedef float (*scalar_field)(float, float, float);

float fWave(float x, float y, float z) {
    return y - sin(x) * cos(z);
}

float fCone(float x, float y, float z) {
	return pow(x, 2) - pow(y, 2) - pow(z, 2) - z;
}

vector<float> marching_cubes(scalar_field f, float isovalue, float min, float max, float stepsize) {
    
    std::vector<float> vertices;

	float tlb, trb, brb, blb;   // 'back' corners
	float tlf, trf, brf, blf;   // 'front' corners
	int cubeIndex = 0;
	int* verts;

    cout << "Marching..." << endl;

	for (float z = min; z < max; z += stepsize) {
        for (float y = min; y < max; y += stepsize) {
            for (float x = min ; x < max; x += stepsize) {

                // test the cube
                blf = f(x, y, z);
                brf = f(x+stepsize, y, z);
                brb = f(x+stepsize, y, z+stepsize);
                blb = f(x, y, z+stepsize);

                tlf = f(x, y+stepsize, z);
                trf = f(x+stepsize, y+stepsize, z);
                trb = f(x+stepsize, y+stepsize, z+stepsize);
                tlb = f(x, y+stepsize, z+stepsize);

                cubeIndex = 0;
                if (blf < isovalue) {
                    cubeIndex |= BOTTOM_LEFT_FRONT;
                }
                if (brf < isovalue) {
                    cubeIndex |= BOTTOM_RIGHT_FRONT;
                }
                if (brb < isovalue) {
                    cubeIndex |= BOTTOM_RIGHT_BACK;
                }
                if (blb < isovalue) {
                    cubeIndex |= BOTTOM_LEFT_BACK;
                }
                if (tlf < isovalue) {
                    cubeIndex |= TOP_LEFT_FRONT;
                }
                if (trf < isovalue) {
                    cubeIndex |= TOP_RIGHT_FRONT;
                }
                if (trb < isovalue) {
                    cubeIndex |= TOP_RIGHT_BACK;
                }
                if (tlb < isovalue) {
                    cubeIndex |= TOP_LEFT_BACK;
                }

                verts = marching_cubes_lut[cubeIndex];
                for (int i = 0; i < 10; i += 3) {      // encode line segments
                    if (verts[i] >= 0) {
                        vertices.emplace_back(x+stepsize*vertTable[verts[i]][0]);
                        vertices.emplace_back(y+stepsize*vertTable[verts[i]][1]);
                        vertices.emplace_back(z+stepsize*vertTable[verts[i]][2]);
                        
                        vertices.emplace_back(x+stepsize*vertTable[verts[i+1]][0]);
                        vertices.emplace_back(y+stepsize*vertTable[verts[i+1]][1]);
                        vertices.emplace_back(z+stepsize*vertTable[verts[i+1]][2]);
                        
                        vertices.emplace_back(x+stepsize*vertTable[verts[i+2]][0]);
                        vertices.emplace_back(y+stepsize*vertTable[verts[i+2]][1]);
                        vertices.emplace_back(z+stepsize*vertTable[verts[i+2]][2]);
                    }
                }
            }
        }
	}

    cout << "DONE" << endl;
	return vertices;
}

/****** Encode normal vectors for each vertex in input ******/
vector<float> compute_normals(vector<float> vertices) {
    vector<float> normals;
    glm::vec3 triNormal;
    cout << "Computing normals..." << endl;

    for (int i = 0; i < vertices.size(); i = i + 9) {

        // get triangle vertices
        glm::vec3 a = glm::vec3(vertices[i], vertices[i+1], vertices[i+2]);
        glm::vec3 b = glm::vec3(vertices[i+3], vertices[i+4], vertices[i+5]);
        glm::vec3 c = glm::vec3(vertices[i+6], vertices[i+7], vertices[i+8]);

        // normalise normal vectors
        triNormal = glm::normalize(glm::cross(b - a, c - a));

        // all points of a triangle have the same normal
        for (int j = 0; j < 3; j++) {
            normals.push_back(triNormal.x);
            normals.push_back(triNormal.y);
            normals.push_back(triNormal.z);
        }
    }

    cout << "DONE" << endl;
    return normals;
}

/****** Write to a PLY file and return faces ******/
void writePLY(vector<float> vertices, vector<float> normals, string fileName) {
    ofstream PLYfile(fileName);
    vector<int> faces;
    cout << "Writing to file..." << endl;

    // headers
    PLYfile << "ply" << endl;
    PLYfile << "format ascii 1.0" << endl;
    PLYfile << "element vertex " << vertices.size() / 3 << endl;
    PLYfile << "property float x" << endl;
    PLYfile << "property float y" << endl;
    PLYfile << "property float z" << endl;
    PLYfile << "property float nx" << endl;
    PLYfile << "property float ny" << endl;
    PLYfile << "property float nz" << endl;
    PLYfile << "element face " << vertices.size() / 9 << endl;
    PLYfile << "property list uchar uint vertex_indices" << endl;
    PLYfile << "end_header" << endl;

    // vertices and normals
    for (int i = 0; i < vertices.size(); i += 3) {
        PLYfile << vertices[i] << " " << vertices[i+1] << " " << vertices[i+2] <<
            " " << normals[i] << " " << normals[i+1] << " " << normals[i+2] << endl;
    }

    // faces
    for (int i = 0; i < vertices.size(); i += 9) {
        PLYfile << "3 " << i / 3 << " " << (i / 3) + 1 << " " << (i / 3) + 2 << endl;
        faces.push_back(i/3);
        faces.push_back(i/3 + 1);
        faces.push_back(i/3 + 2);
    }

    cout << "DONE" << endl;
    PLYfile.close();
}

/****** Handles mouse click + drag input ******/
void mouseCallback(GLFWwindow* window, float xpos, float ypos) {

    xoffset = xpos - lastX;
    yoffset = ypos - lastY;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;   // mouse sensitivity
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // if screen flips, flip x input
    if (!screenFlip) {
        camYaw += xoffset;
    } else {
        camYaw -= xoffset;
    }

    // keep pitch between 360 and -360 for easier handling
    camPitch += yoffset;
    if (camPitch >= 360) {
        camPitch -= 360;
    }
    if (camPitch <= -360) {
        camPitch += 360;
    }

}

//////////////////////////////////////////////////////////////////////////////
// Main
//////////////////////////////////////////////////////////////////////////////

int main( int argc, char* argv[]) {

	///////////////////////////////////////////////////////
	int currentStep = 1;
	int substep = 1;
	if (argc > 1 ) {
		currentStep = atoi(argv[1]);
	}
	if (argc > 2) {
		substep = atoi(argv[2]);
	}
	///////////////////////////////////////////////////////

	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);

	// Open a window and create its OpenGL context
	float screenW = 1400;
	float screenH = 900;
	window = glfwCreateWindow( screenW, screenH, "What the Phong", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}


	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.2f, 0.2f, 0.3f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    std::string VertexShaderCode = "\
    	#version 330 core\n\
		// Input vertex data, different for all executions of this shader.\n\
		layout(location = 0) in vec3 vertexPosition;\n\
		layout(location = 1) in vec3 vertexNormal;\n\
		// Output data ; will be interpolated for each fragment.\n\
		out vec3 cameraNormal;\n\
		out vec3 cameraEye;\n\
		out vec3 cameraLightDir;\n\
		// Values that stay constant for the whole mesh.\n\
		uniform mat4 MVP;\n\
		uniform mat4 V;\n\
		uniform mat4 M;\n\
		uniform vec3 LightDir;\n\
		void main(){ \n\
			// Output position of the vertex, in clip space : MVP * position\n\
			gl_Position =  MVP * vec4(vertexPosition,1);\n\
            vec3 vertexPosCamera = ( V * M * vec4(vertexPosition,1)).xyz;\n\
            cameraEye = vec3(0,0,0) - vertexPosCamera;\n\
            vec3 cameraLightPos = ( V * vec4(LightDir,1)).xyz;\n\
            cameraLightDir = cameraLightPos + cameraEye;\n\
            cameraNormal = (V * M * vec4(vertexNormal, 0)).xyz;\n\
		}\n";

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode = "\
		#version 330 core\n\
		in vec3 cameraNormal; \n\
		in vec3 cameraEye; \n\
		in vec3 cameraLightDir; \n\
		uniform vec3 modelColor;\n\
		void main() {\n\
            vec4 diffuse = vec4(modelColor.x, modelColor.y, modelColor.z, 1.0);\n\
			vec4 ambient = vec4(0.2, 0.2, 0.2, 1.0);\n\
			vec4 specular = vec4(1.0, 1.0, 1.0, 1.0);\n\
            vec3 N = normalize(cameraNormal);\n\
            vec3 E = normalize(cameraEye);\n\
            vec3 L = normalize(cameraLightDir);\n\
            vec3 R = reflect(-L, N);\n\
            float cosTheta = clamp(dot(N, L), 0, 1);\n\
            float cosAlpha = clamp(dot(E, R), 0, 1);\n\
            float shininess = 64.0f;\n\
			gl_FragColor = ambient + diffuse * cosTheta + specular * (pow(cosAlpha, shininess));\n\
		}\n";

	// Compile Vertex Shader
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);

    // Compile Fragment Shader
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);

    // Link the program
    ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

	// Get a handle for all uniforms
	MatrixID = glGetUniformLocation(ProgramID, "MVP");
	glm::mat4 MVP;

    GLuint ViewID = glGetUniformLocation(ProgramID, "V");
	glm::mat4 view;

    GLuint ModelID = glGetUniformLocation(ProgramID, "M");
	glm::mat4 M;

    GLuint LightDirID = glGetUniformLocation(ProgramID, "LightDir");
	glm::vec3 LightDir = vec3(5.0, 5.0, 5.0);

    GLuint ModelColorID = glGetUniformLocation(ProgramID, "modelColor");
	glm::vec3 modelColor = vec3(0.0, 1.0, 1.0);

    // scale transformation matrix to flip projection
    float flipArr[16] = {
        -1, 0, 0, 0,
        0, -1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    glm::mat4 flip = glm::transpose(glm::make_mat4(flipArr));

    // coordinate axes and box
    float min = -5.0;
    float max = 5.0f;
    glm::vec3 minVec = glm::vec3(min, min, min);
	glm::vec3 maxVec = glm::vec3(max, max, max);
    Axes ax(minVec, maxVec - minVec);
    Box box(minVec, maxVec - minVec);

    /****** MARCHING CUBES ******/
    float isovalue = 0.0f;
    float stepsize = 0.05f;
    std::vector <float> marchingVerts = marching_cubes(fWave, isovalue, min, max, stepsize);
    std::vector <float> marchingNorms = compute_normals(marchingVerts);
    // writePLY(marchingVerts, marchingNorms, "test.ply");

    // VAO and VBO IDs
    GLuint vertexID;
    GLuint normalID;
    GLuint vaoID;
    GLuint shaderID;
    GLuint texObjID;

    // bind and generate VAO before binding buffer objects + specifying vertex attributes!
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    // vertex data
    glGenBuffers(1, &vertexID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexID);
    glBufferData(GL_ARRAY_BUFFER, marchingVerts.size() * sizeof(GL_FLOAT), marchingVerts.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,                  // attribute, match shader's layout
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // data
    );

    // normals
    glGenBuffers(1, &normalID);
    glBindBuffer(GL_ARRAY_BUFFER, normalID);
    glBufferData(GL_ARRAY_BUFFER, marchingNorms.size() * sizeof(GL_FLOAT), marchingNorms.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,                  // attribute, match shader's layout
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,			// normalized?
        0,                  // stride
        (void*)0            // data
    );

    // unbind VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // texture?
    glGenTextures(1, &texObjID);
    glBindTexture(GL_TEXTURE_2D, texObjID);
    glBindTexture(GL_TEXTURE_2D, 0);

    /***** RUNNING PROGRAM ON WINDOW ******/
	do {

        // get time
        float currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;     // UPDATE TIME

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        /****** PROCESS INPUT FIRST so flipping scren doesn't glitch ******/

        // front and back camera movement
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            r -= speed * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            r += speed * deltaTime;
        }

        // don't let r get to 0
        if (r < 0.1f) {
            r = 0.1f;
        }

        // left/right and up/down camera movement
        int leftClickState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        
        // first frame of left click: don't move camera
        if (leftClickState == GLFW_PRESS && !leftClickPressed) {
            leftClickPressed = true;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            // set this click as frame of reference for offset
            lastX = static_cast<float>(mouseX);
            lastY = screenH - static_cast<float>(mouseY);

        // left click press and hold
        } else if (leftClickState == GLFW_PRESS && leftClickPressed) {
            glfwGetCursorPos(window, &mouseX, &mouseY);
            float floatMouseX = static_cast<float>(mouseX);
            float floatMouseY = screenH - static_cast<float>(mouseY);
            mouseCallback(window, floatMouseX, floatMouseY);

        // release left click
        } else if (leftClickState == GLFW_RELEASE && leftClickPressed) {
            leftClickPressed = false;
        }

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glm::mat4 Projection = glm::perspective(glm::radians(45.0f), screenW/screenH, 0.001f, 1000.0f);

        // make sure projection doesn't flip
		if ((camPitch <= 0.0 && camPitch >= -180.0)|| camPitch >= 180.0) {
            Projection *= flip;
            screenFlip = true;
        } else {
            screenFlip = false;
        }

		glLoadMatrixf(glm::value_ptr(Projection));

		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();

        // turn input into spherical coordinates
        cameraPos.x = r * cos(glm::radians(camYaw)) * sin(glm::radians(camPitch));
        cameraPos.y = r * cos(glm::radians(camPitch));
        cameraPos.z = r * sin(glm::radians(camYaw)) * sin(glm::radians(camPitch));

        view = glm::lookAt(cameraPos, cameraFront, cameraUp);

        M = glm::mat4(1.0f);

		glm::mat4 MV = view * M;
        
		glLoadMatrixf(glm::value_ptr(view));

		MVP = Projection * view * M;

		// DRAW STUFF HERE
        ax.draw();
        box.draw();

        // set up shader
        glUseProgram(ProgramID);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ViewID, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(ModelID, 1, GL_FALSE, &M[0][0]);
		glUniform3f(LightDirID, LightDir.x, LightDir.y, LightDir.z);
		glUniform3f(ModelColorID, modelColor.x, modelColor.y, modelColor.z);

        // DRAW ACTUAL MARCHING CUBES

        // re-bind VAO: recover all buffer bindings + vertex attrib specifications (aka state) before drawing
		glBindVertexArray(vaoID);

		// draw marching cubes surface
		glDrawArrays(GL_TRIANGLES, 0, marchingVerts.size());

        // unbind/disable things
		glBindVertexArray(0);
		glUseProgram(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

    }

	// Check if the ESC key was pressed or the window was closed
	while ( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && \
            glfwWindowShouldClose(window) == 0 );

	// Cleanup shader
	glDeleteProgram(ProgramID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
