#ifndef HEADER
#define HEADER

// ID шейдерной программы
GLuint Program;
// ID атрибута
GLint Attrib_vertex;
GLint v_color_id;
// ID Vertex Buffer Object
GLuint VBO;
GLuint CBO;
GLuint VBO_Button1;
GLuint VBO_Button2;
GLuint VBO_Button3;

struct Vertex {
    GLfloat x;
    GLfloat y;
};

struct Color {
    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat a;
};

enum Shape {
    Triangle,
    Quadrangle,
    Fan,
    Pentagon
};

enum Fill {
    Hardcode,
    Uniform,
    Gradient
};

// Исходный код вершинного шейдера
const char* VertexShaderSource = R"(
 #version 330 core
 in vec2 coord;
 void main() {
    gl_Position = vec4(coord, 0.0, 1.0);
 }
)";

const char* GradientVertexShaderSource = R"(
 #version 330 core
 in vec2 coord;
 in vec4 vertexColor;
 out vec4 v_color;

 void main() {
    gl_Position = vec4(coord, 0.0, 1.0);
    v_color = vertexColor;
 }
)";

//_________________________________________

// Исходный код фрагментного шейдера
const char* FragShaderSource = R"(
 #version 330 core
 out vec4 color;
 void main() {
    color = vec4(1, 1, 0, 1);
 }
)";

const char* UniformFragShaderSource = R"(
 #version 330 core
 uniform vec4 u_color;
 out vec4 color;
 void main() {
    color = u_color;
 }
)";

const char* GradientFragShaderSource = R"(
 #version 330 core
 in vec4 v_color;
 out vec4 color;
 void main() {
    color = v_color;
 }
)";



void checkOpenGLerror();
void ShaderLog(unsigned int);
void InitVBO();
void InitShader();
void Init();
void Draw();
void ReleaseVBO();
void ReleaseShader();
void Release();
std::vector<Color> GenerateColors(int);

#endif