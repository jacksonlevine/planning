#include "GLSetup.h";




GLWrapper::GLWrapper()
{
    // Camera position and rotation
    this->cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    this->cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    this->cameraDirection = glm::normalize(cameraPos - cameraTarget);
    this->up = glm::vec3(0.0f, 1.0f, 0.0f);
    this->cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    this->cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    this->cameraUp = glm::cross(cameraDirection, cameraRight);
    this->cameraSpeed = 0.1f;
    this->cameraYaw = -90.0f;
    this->cameraPitch = 0.0f;
    this->lastX = 400;
    this->lastY = 300;
    this->firstMouse = true;
    this->view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

GLFWcursorposfun GLWrapper::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    cameraYaw += xoffset;
    cameraPitch += yoffset;

    if (cameraPitch > 89.0f)
        cameraPitch = 89.0f;
    if (cameraPitch < -89.0f)
        cameraPitch = -89.0f;


    glm::vec3 front;
    front.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    front.y = sin(glm::radians(cameraPitch));
    front.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    cameraFront = glm::normalize(front);

}

GLFWmousebuttonfun GLWrapper::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        firstMouse = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstMouse = true;
    }
}

int GLWrapper::initializeGL() {
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set up GLFW window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFW window
    window = glfwCreateWindow(800, 600, "My C++ Client", NULL, NULL);
    if (!this->window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(this->window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Set up the viewport
    glViewport(0, 0, 800, 600);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Set up GLM for 3D math
    model = glm::mat4(1.0f);


    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    // Enable pointer-locking first-person controls
    glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // Set up event callbacks
    glfwSetCursorPosCallback(this->window, mouse_callback);
    glfwSetMouseButtonCallback(this->window, mouse_button_callback);

    // Create vertex shader object
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Set vertex shader source code
    const GLchar* vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 position;\n"
        "layout (location = 1) in vec3 color;\n"
        "out vec3 vertexColor;\n"
        "uniform mat4 mvp;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = mvp * vec4(position, 1.0);\n"
        "    vertexColor = color;\n"
        "}\n";
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

    // Compile vertex shader
    glCompileShader(vertexShader);

    // Check vertex shader compilation errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Vertex shader compilation error: " << infoLog << std::endl;
    }

    // Create fragment shader object
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Set fragment shader source code
    const GLchar* fragmentShaderSource =
        "#version 330 core\n"
        "in vec3 vertexColor;\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(vertexColor, 1.0);\n"
        "}\n";
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

    // Compile fragment shader
    glCompileShader(fragmentShader);

    // Check fragment shader compilation errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Fragment shader compilation error: " << infoLog << std::endl;
    }

    // Create shader program object store it on this.shaderProgram
    this->shaderProgram = glCreateProgram();

    // Attach vertex and fragment shaders to shader program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // Link shader program
    glLinkProgram(shaderProgram);

    // Check shader program linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Shader program linking error: " << infoLog << std::endl;
    }

    // Delete shader objects
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

}

void GLWrapper::setupVAO() {
    // Generate a vertex array object (VAO)
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Use our shader program
    glUseProgram(this->shaderProgram);

}

void GLWrapper::bindGeometry(const GLfloat* vertices, const GLfloat* colors, int vsize, int csize) {

    // Generate a vertex buffer object (VBO) for the position data
    GLuint vbo_vertices;
    glGenBuffers(1, &vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, vsize, vertices, GL_STATIC_DRAW);
    // Set up the vertex attribute pointers for the position buffer object
    GLint pos_attrib = glGetAttribLocation(this->shaderProgram, "position");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    // Generate a vertex buffer object (VBO) for the color data
    GLuint vbo_colors;
    glGenBuffers(1, &vbo_colors);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_colors);
    glBufferData(GL_ARRAY_BUFFER, csize, colors, GL_STATIC_DRAW);

    // Set up the vertex attribute pointers for the color buffer object
    GLint col_attrib = glGetAttribLocation(this->shaderProgram, "color");
    glEnableVertexAttribArray(col_attrib);
    glVertexAttribPointer(col_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

}

void GLWrapper::orientCamera() {

    // Calculate the new direction vector based on the yaw and pitch angles

    direction.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    direction.y = sin(glm::radians(cameraPitch));
    direction.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));

    // Normalize the direction vector
    direction = glm::normalize(direction);

    // Set up the view matrix 
    view = glm::lookAt(cameraPos, cameraPos + direction, cameraUp);

    mvp = projection * view * model;


    // Set the mvp matrix uniform in the shader
    GLuint mvpLoc = glGetUniformLocation(shaderProgram, "mvp");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
}

void GLWrapper::runGLLoop() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->orientCamera();

    // Draw the triangle
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(this->window);

    glfwPollEvents();
}