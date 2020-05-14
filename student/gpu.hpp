/*!
 * @file
 * @brief This file contains class that represents graphic card.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#pragma once

#include <student/fwd.hpp>
#include "list"

class FrameBuffer{
    public:
        uint8_t * colorBuffer;
        float * depthBuffer;
        uint32_t width;
        uint32_t height;
        FrameBuffer(uint32_t width, uint32_t height);
};
class Program{
public:
    VertexShader vertexShader{};
    FragmentShader fragmentShader{};
    Uniforms uniforms;
    AttributeType attributeType = AttributeType::EMPTY;
};
/**
 * This struct is necessary because the program needs to know AttributeType
 * of OutVertex attributes and union does not contain information about what
 * type of data is stored at the moment.
 */
struct OutAbstractVertex {
    OutVertex ov;
    AttributeType attributeType[maxAttributes]{};
};

/**
 * @brief This class represent software GPU
 */
class GPU{
  public:
    GPU();
    virtual ~GPU();

    //buffer object commands
    BufferID  createBuffer           (uint64_t size);
    void      deleteBuffer           (BufferID buffer);
    void      setBufferData          (BufferID buffer,uint64_t offset,uint64_t size,void const* data);
    void      getBufferData          (BufferID buffer,uint64_t offset,uint64_t size,void      * data);
    bool      isBuffer               (BufferID buffer);

    //vertex array object commands (vertex puller)
    ObjectID  createVertexPuller     ();
    void      deleteVertexPuller     (VertexPullerID vao);
    void      setVertexPullerHead    (VertexPullerID vao,uint32_t head,AttributeType type,uint64_t stride,uint64_t offset,BufferID buffer);
    void      setVertexPullerIndexing(VertexPullerID vao,IndexType type,BufferID buffer);
    void      enableVertexPullerHead (VertexPullerID vao,uint32_t head);
    void      disableVertexPullerHead(VertexPullerID vao,uint32_t head);
    void      bindVertexPuller       (VertexPullerID vao);
    void      unbindVertexPuller     ();
    bool      isVertexPuller         (VertexPullerID vao);

    //program object commands
    ProgramID createProgram          ();
    void      deleteProgram          (ProgramID prg);
    void      attachShaders          (ProgramID prg,VertexShader vs,FragmentShader fs);
    void      setVS2FSType           (ProgramID prg,uint32_t attrib,AttributeType type);
    void      useProgram             (ProgramID prg);
    bool      isProgram              (ProgramID prg);
    void      programUniform1f       (ProgramID prg,uint32_t uniformId,float     const&d);
    void      programUniform2f       (ProgramID prg,uint32_t uniformId,glm::vec2 const&d);
    void      programUniform3f       (ProgramID prg,uint32_t uniformId,glm::vec3 const&d);
    void      programUniform4f       (ProgramID prg,uint32_t uniformId,glm::vec4 const&d);
    void      programUniformMatrix4f (ProgramID prg,uint32_t uniformId,glm::mat4 const&d);

    //framebuffer functions
    void      createFramebuffer      (uint32_t width,uint32_t height);
    void      deleteFramebuffer      ();
    void      resizeFramebuffer      (uint32_t width,uint32_t height);
    uint8_t*  getFramebufferColor    ();
    float*    getFramebufferDepth    ();
    uint32_t  getFramebufferWidth    ();
    uint32_t  getFramebufferHeight   ();

    //execution commands
    void      clear                  (float r,float g,float b,float a);
    void      drawTriangles          (uint32_t  nofVertices);

    /// \addtogroup gpu_init 00. proměnné, inicializace / deinicializace grafické karty
    /// @{
    /// \todo zde si můžete vytvořit proměnné grafické karty (buffery, programy, ...)
    /// @}
    std::list<BufferID> bufferList; //pole ukazatelu na buffery
    std::list<ObjectID> vertexPullerList; //pole ukazatelu na buffery
    std::list<ProgramID> programList;
    ObjectID * activeVertexPuller;
    ProgramID * activeProgram;
    FrameBuffer * frameBuffer;

    void vertexProcessor(uint32_t nofVertices, OutAbstractVertex *outAbstractVertices, Program * program);
    OutAbstractVertex getClippedPoint(OutAbstractVertex a, OutAbstractVertex b);
    float triangleSurface(OutAbstractVertex &a, OutAbstractVertex &b, OutAbstractVertex &c);
    bool edgeFunction(OutAbstractVertex &a, OutAbstractVertex &b, OutAbstractVertex &c);
};
struct Head {
    BufferID buffer_id;
    uint32_t  offset;
    uint32_t  stride;
    AttributeType attrib_type;
    bool enabled;
};

struct Indexing {
    bool enabled;
    BufferID buffer_id;
    IndexType index_type;
};

class Vertex_puller_settings {
    public:
        Vertex_puller_settings();
        virtual ~Vertex_puller_settings();
        Head heads[maxAttributes];
        Indexing indexing;
};
struct PrimitiveTriangle {
    OutAbstractVertex ov1;
    OutAbstractVertex ov2;
    OutAbstractVertex ov3;
};



float normalize_color(uint8_t num, uint8_t normalizator, bool trunc);
uint8_t denormalize_color(float num, uint8_t normalizer, bool trunc);
float fit_color(float num);

