#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <iomanip>
#include <vector>
#include "Header.h"

ShaderType fill;
Color u_color = { 1, 0, 0, 1 };

Vertex vertices[20] = {
    { -1.0f, 0.2f }, { -0.6f, 1.0f }, { -0.2f, 0.2f }, //TRIANGLE
    { 0.2f, 0.2f }, { 1.0f, 0.2f },  { 0.2f,  0.8f }, { 1.0f,  0.8f }, //QUAD
    {0.6f, -1.0f}, {0.2f, -0.8f}, {0.3f,-0.5f}, {0.6f,-0.3f}, {0.89f, -0.5f}, {0.99f, -0.8f}, //FAN
    {-1.0f, -0.5f}, {-0.57f,-0.2f}, {-0.14f,-0.5f}, {-0.3f, -1.0f}, {-0.83f, -1.0f}, {-1.0f, -0.5f} //PENTAGON
};

Color colors[24] = {
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {1.0f, 1.0f, 0.0f},
    {1.0f, 0.0f, 1.0f},
    {0.0f, 1.0f, 1.0f},
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {1.0f, 1.0f, 0.0f},
    {1.0f, 0.0f, 1.0f},
    {0.0f, 1.0f, 1.0f},
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {1.0f, 1.0f, 0.0f},
    {1.0f, 0.0f, 1.0f},
    {0.0f, 1.0f, 1.0f},
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {1.0f, 1.0f, 0.0f},
    {1.0f, 0.0f, 1.0f},
    {0.0f, 1.0f, 1.0f}
};

int main()
{
    fill = Static;
    u_color = { 0, 1, 1, 1 };


    sf::Window window(sf::VideoMode(600, 600), "My OpenGL window", sf::Style::Default, sf::ContextSettings(24));
    window.setVerticalSyncEnabled(true);
    window.setActive(true);
    glewInit();

    Init();
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) { window.close(); break; }
            else if (event.type == sf::Event::Resized) { glViewport(0, 0, event.size.width, event.size.height); }
            // Обработка ввода с клавиатуры
            else if (event.type == sf::Event::KeyPressed) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                    fill = static_cast<ShaderType>((fill + 1) % 3);
                    Init();
                }
                else if (event.key.code == sf::Keyboard::Escape) {
                    std::cout << "Выход из программы\n";
                    window.close();
                }
            }
        }
        if (!window.isOpen()) continue;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Draw();
        window.display();
    }

    Release();
    return 0;
}


void InitVBO() {
    //создаем буфер
    glGenBuffers(1, &VBO);
    // Передаем вершины в буфер
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &CBO);
    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    checkOpenGLerror(); //Пример функции есть в лабораторной
    // Проверка ошибок OpenGL, если есть, то вывод в консоль тип ошибки
}

void InitShader() {
    // Создаем вершинный шейдер
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    // Передаем исходный код

    if (fill == Gradient)
        glShaderSource(vShader, 1, &GradientVertexShaderSource, NULL);
    else
        glShaderSource(vShader, 1, &VertexShaderSource, NULL);

    // Компилируем шейдер
    glCompileShader(vShader);
    std::cout << "vertex shader \n";
    // Функция печати лога шейдера
    ShaderLog(vShader); //Пример функции есть в лабораторной

    // Создаем фрагментный шейдер
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    // Передаем исходный код

    switch (fill)
    {
    case Static: glShaderSource(fShader, 1, &FragShaderSource, NULL); break;
    case Uniform: glShaderSource(fShader, 1, &UniformFragShaderSource, NULL); break;
    case Gradient: glShaderSource(fShader, 1, &GradientFragShaderSource, NULL); break;
    default:break;
    }

    // Компилируем шейдер
    glCompileShader(fShader);
    std::cout << "fragment shader \n";
    // Функция печати лога шейдера
    ShaderLog(fShader);

    // Создаем программу и прикрепляем шейдеры к ней
    Program = glCreateProgram();
    glAttachShader(Program, vShader);
    glAttachShader(Program, fShader);
    // Линкуем шейдерную программу
    glLinkProgram(Program);
    // Проверяем статус сборки
    int link_ok;
    glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
        std::cout << "error attach shaders \n";
        return;
    }

    // Вытягиваем ID атрибута из собранной программы
    const char* attr_name = "coord"; //имя в шейдере
    Attrib_vertex = glGetAttribLocation(Program, attr_name);
    if (Attrib_vertex == -1) {
        std::cout << "could not bind attrib " << attr_name << std::endl;
        return;
    }

    checkOpenGLerror();
}

void Init() {
    // Шейдеры
    InitShader();
    // Вершинный буфер
    InitVBO();
}

void Draw() {
    glUseProgram(Program); // Устанавливаем шейдерную программу текущей

    if (fill == Uniform) {
        GLuint u_color_id = glGetUniformLocation(Program, "u_color");
        glUniform4f(u_color_id, u_color.r, u_color.g, u_color.b, u_color.a);
    }

    glEnableVertexAttribArray(Attrib_vertex); // Включаем массив атрибутов.
    //активируем буфер 
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // сообщаем OpenGL как он должен интерпретировать вершинные данные.
    glVertexAttribPointer(Attrib_vertex, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Отключаем VBO

    if (fill == Gradient) {
        v_color_id = glGetAttribLocation(Program, "vertexColor");
        glEnableVertexAttribArray(v_color_id);
        glBindBuffer(GL_ARRAY_BUFFER, CBO);
        glVertexAttribPointer(v_color_id, 4, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // Передаем данные на видеокарту(рисуем)
    glDrawArrays(GL_TRIANGLES, 0, 3);              // Треугольник
    glDrawArrays(GL_TRIANGLE_STRIP, 3, 4);         // Квадрат
    glDrawArrays(GL_TRIANGLE_FAN, 7, 6);           // Веер
    glDrawArrays(GL_POLYGON, 13, 6);               // Пятиугольник


    glDisableVertexAttribArray(Attrib_vertex); // Отключаем массив атрибутов
    if (fill == Gradient)
        glDisableVertexAttribArray(v_color_id);
    glUseProgram(0); // Отключаем шейдерную программу

    checkOpenGLerror();
}

// Освобождение буфера
void ReleaseVBO() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &VBO);
}

// Освобождение шейдеров
void ReleaseShader() {
    // Передавая ноль, мы отключаем шейдерную программу
    glUseProgram(0);
    // Удаляем шейдерную программу
    glDeleteProgram(Program);
}

void Release() {
    // Шейдеры
    ReleaseShader();
    // Вершинный буфер
    ReleaseVBO();
}



void checkOpenGLerror()
{
    GLenum err;
    if ((err = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "OpenGL Error Code: " << std::hex << err << std::dec << std::endl;
    }
}

void ShaderLog(unsigned int shader)
{
    int infologLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
    if (infologLen > 1)
    {
        int charsWritten = 0;
        std::vector<char> infoLog(infologLen);
        glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog.data());
        std::cout << "InfoLog: " << infoLog.data() << std::endl;
    }
}