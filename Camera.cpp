// Camera.cpp
#include "Camera.hpp"
#include <math.h>

namespace gps {

    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget)
    {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
    }

    glm::mat4 Camera::getViewMatrix()
    {
        return glm::lookAt(cameraPosition, cameraPosition + cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    void Camera::move(MOVE_DIRECTION direction, float speed)
    {
        switch (direction) {
        case MOVE_FORWARD:
            cameraPosition += cameraDirection * speed;
            break;

        case MOVE_BACKWARD:
            cameraPosition -= cameraDirection * speed;
            break;

        case MOVE_RIGHT:
            cameraPosition += cameraRightDirection * speed;
            break;

        case MOVE_LEFT:
            cameraPosition -= cameraRightDirection * speed;
            break;
        }
    }

    void Camera::rotate(float pitch, float yaw)
    {
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        cameraDirection = glm::normalize(front);
        cameraRightDirection = glm::normalize(glm::cross(cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
    }
   
    void Camera::setCameraPosition(const glm::vec3& position) {
        cameraPosition = position;
    }
    void Camera::moveCameraToDestination()
    {
        // Define the start and end positions
        glm::vec3 startPosition(140.834, 15.2127, -15.2127);
        glm::vec3 endPosition(-40.834, 15.2127, -15.2127);

        // Calculate the direction towards the end position
        glm::vec3 moveDirection = glm::normalize(endPosition - startPosition);

        // Define the speed at which the camera should move
        float speed = 0.1f; // You can adjust the speed as needed

        // Move the camera towards the end position
        cameraPosition += moveDirection * speed;

        // Update the camera's direction and right direction based on the new position
        cameraDirection = glm::normalize(cameraTarget - cameraPosition);
        cameraRightDirection = glm::normalize(glm::cross(cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
    }
    void Camera::rotateAroundCenter(float angle)
    {
        glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        this->cameraPosition = glm::vec4(rot * glm::vec4(this->cameraPosition, 1.0f));
        this->cameraDirection = glm::normalize(cameraTarget - cameraPosition);
        cameraRightDirection = glm::normalize(glm::cross(cameraDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
    }
} // namespace gps
