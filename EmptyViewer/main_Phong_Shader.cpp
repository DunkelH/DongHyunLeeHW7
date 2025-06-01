#include <Windows.h>
#include <glad/glad.h>
#include <iostream>
#include <GL/GL.h>
#include <GL/freeglut.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

#define GLFW_INCLUDE_GLU
#define GLFW_DLL
#include <GLFW/glfw3.h>x`
#include <vector>

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION

std::string readFile(const char* path) {
    std::ifstream file(path);
    std::stringstream buf;
    buf << file.rdbuf();
    return buf.str();
}

GLuint compileShader(const char* path, GLenum type) {
    std::string source = readFile(path);
    const char* src = source.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetShaderInfoLog(shader, 512, nullptr, info);
        std::cerr << "Shader Compile Error: " << info << std::endl;
    }
    return shader;
}

GLuint createProgram(const char* vertPath, const char* fragPath) {
    GLuint v = compileShader(vertPath, GL_VERTEX_SHADER);
    GLuint f = compileShader(fragPath, GL_FRAGMENT_SHADER);
    GLuint program = glCreateProgram();
    glAttachShader(program, v);
    glAttachShader(program, f);
    glLinkProgram(program);
    glDeleteShader(v);
    glDeleteShader(f);
    return program;
}

void generateSphere(std::vector<float>& vertices, std::vector<unsigned int>& indices, unsigned int sectorCount = 36, unsigned int stackCount = 18) {
    const float PI = 3.1415926f;
    for (unsigned int i = 0; i <= stackCount; ++i) {
        float phi = PI * i / stackCount;
        for (unsigned int j = 0; j <= sectorCount; ++j) {
            float theta = 2 * PI * j / sectorCount;
            float x = sin(phi) * cos(theta);
            float y = sin(phi) * sin(theta);
            float z = cos(phi);
            vertices.insert(vertices.end(), { x, y, z, x, y, z });
        }
    }
    for (unsigned int i = 0; i < stackCount; ++i) {
        for (unsigned int j = 0; j < sectorCount; ++j) {
            unsigned int k1 = i * (sectorCount + 1) + j;
            unsigned int k2 = k1 + sectorCount + 1;
            indices.insert(indices.end(), { k1, k2, k1 + 1 });
            indices.insert(indices.end(), { k1 + 1, k2, k2 + 1 });
        }
    }
}

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(512, 512, "HW7 - Phong Shader Sphere", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    GLuint program = createProgram("Phong.vert", "Phong.frag");
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    generateSphere(vertices, indices);

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -7));
        model = glm::scale(model, glm::vec3(2.0f));
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::frustum(-0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 1000.0f);

        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glUniform3f(glGetUniformLocation(program, "ka"), 0.0f, 1.0f, 0.0f);
        glUniform3f(glGetUniformLocation(program, "kd"), 0.0f, 0.5f, 0.0f);
        glUniform3f(glGetUniformLocation(program, "ks"), 0.5f, 0.5f, 0.5f);
        glUniform1f(glGetUniformLocation(program, "shininess"), 32.0f);
        glUniform1f(glGetUniformLocation(program, "ambientIntensity"), 0.2f);
        glUniform3f(glGetUniformLocation(program, "lightPos"), -4.0f, 4.0f, -3.0f);
        glUniform3f(glGetUniformLocation(program, "viewPos"), 0.0f, 0.0f, 0.0f);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


