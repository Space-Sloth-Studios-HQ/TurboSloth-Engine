#pragma once

#include <glm/glm.hpp>

namespace Engine {
class Camera {
public:
    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);

    glm::mat4 GetViewMatrix() const;

private:
    void UpdateCameraVectors();

    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Up;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;

    float m_Yaw;
    float m_Pitch;
};
} // namespace Engine