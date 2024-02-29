
//
//  Camera.hpp
//
//  Created by CGIS on 28/10/2016.
//  Copyright © 2016 CGIS. All rights reserved.
//

#ifndef Camera_hpp
#define Camera_hpp

#include <stdio.h>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

namespace gps {

    enum MOVE_DIRECTION { MOVE_FORWARD, MOVE_BACKWARD, MOVE_RIGHT, MOVE_LEFT };

    class Camera
    {
    public:
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget);
        glm::mat4 getViewMatrix();
        void move(MOVE_DIRECTION direction, float speed);
        void rotate(float pitch, float yaw);
        void moveToPoint(glm::vec3 targetPosition, float animationDuration, float deltaTime);
        void setCameraPosition(const glm::vec3& position);
        void moveCameraToDestination();
        void rotateAroundCenter(float angle);
         
    private:
        glm::vec3 interpolate(const glm::vec3& start, const glm::vec3& end, float t);
        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        glm::vec3 cameraDirection;
        glm::vec3 cameraRightDirection;
        glm::vec3 cameraFrontDirection;
        glm::vec3 cameraUpDirection;

    };
}

#endif /* Camera_hpp */