#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class GameSector {       
public:             
    
    glm::vec3 WorldPosition;
     
    GameSector(glm::vec3 wp) {
        WorldPosition = wp;
    }


    
};