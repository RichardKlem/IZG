/*!
 * @file
 * @brief This file contains phong rendering method
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <student/method.hpp>

/// \addtogroup cpu_side Úkoly v cpu části
/// @{

/**
 * @brief This class holds all variables of phong method.
 */
class PhongMethod: public Method{
  public:
    BufferID bufferVertices;
    BufferID bufferIndices;
    ObjectID vertexPuller;
    ProgramID program;

    PhongMethod();
    ~PhongMethod() override;
    void onDraw(glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera) override;
    /// \todo Zde si vytvořte proměnné, které budete potřebovat (id bufferů, programu, ...)

};
float dotVec3(glm::vec3 a, glm::vec3 b);
/// @}
