#pragma once
#include <stdlib.h>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
float rando() {
    return (float)(rand() / (float)RAND_MAX);
}

glm::vec3 rotateVector(glm::vec3 direction, float angleDegrees) {
    // Normalize the input direction vector
    direction = glm::normalize(direction);

    // Create a rotation matrix
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angleDegrees), glm::vec3(0.0f, 1.0f, 0.0f));

    // Transform the direction vector by the rotation matrix
    glm::vec4 rotatedVec4 = rotationMatrix * glm::vec4(direction, 0.0f);

    // Convert the resulting vec4 back to vec3
    glm::vec3 rotatedDirection(rotatedVec4.x, rotatedVec4.y, rotatedVec4.z);

    return rotatedDirection;
}
