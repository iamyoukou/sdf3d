#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

glm::vec2 lineUv(glm::vec3, glm::vec3, glm::vec3);
float signedArea(glm::vec3, glm::vec3, glm::vec3);
glm::vec3 baryCoord(glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3);
glm::vec3 point2plane(glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3);
float distPoint2Triangle(glm::vec3, glm::vec3, glm::vec3, glm::vec3, glm::vec3);
