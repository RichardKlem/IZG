/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>
#include <list>
#include <iostream>
#include <algorithm>
#include <new>
#include <cstring>
#include <array>
#include <vector>
#include <typeinfo>

Vertex_puller_settings::Vertex_puller_settings(){}
Vertex_puller_settings::~Vertex_puller_settings(){}

/// \addtogroup gpu_init
/// @{

/**
 * @brief Constructor of GPU
 */
GPU::GPU(){
  /// \todo Zde můžete alokovat/inicializovat potřebné proměnné grafické karty
  activeVertexPuller = nullptr;
  frameBuffer = nullptr;
}

/**
 * @brief Destructor of GPU
 */
GPU::~GPU(){
  /// \todo Zde můžete dealokovat/deinicializovat grafickou kartu
}

/// @}

/** \addtogroup buffer_tasks 01. Implementace obslužných funkcí pro buffery
 * @{
 */

/**
 * @brief This function allocates buffer on GPU.
 *
 * @param size size in bytes of new buffer on GPU.
 *
 * @return unique identificator of the buffer
 */
BufferID GPU::createBuffer(uint64_t size) { 
  /// \todo Tato funkce by měla na grafické kartě vytvořit buffer dat.<br>
  /// Velikost bufferu je v parameteru size (v bajtech).<br>
  /// Funkce by měla vrátit unikátní identifikátor identifikátor bufferu.<br>
  /// Na grafické kartě by mělo být možné alkovat libovolné množství bufferů o libovolné velikosti.<br>
    BufferID * buff;
    try {
        buff = new BufferID[size];
        bufferList.push_back((BufferID)buff); //ukladam si ukazatele na buffery
        return (BufferID)buff; //vracim pretypovany ukazatel
    }
    catch (std::bad_alloc&) {
        return emptyID;
    }
}

/**
 * @brief This function frees allocated buffer on GPU.
 *
 * @param buffer buffer identificator
 */
void GPU::deleteBuffer(BufferID buffer) {
  /// \todo Tato funkce uvolní buffer na grafické kartě.
  /// Buffer pro smazání je vybrán identifikátorem v parameteru "buffer".
  /// Po uvolnění bufferu je identifikátor volný a může být znovu použit při vytvoření nového bufferu.
    BufferID * tmp; //pomocny ukazatel na buffer v listu bufferu
    std::list<BufferID>::iterator it; //iterator na hledani bufferu v listu
    it = std::find(bufferList.begin(), bufferList.end(), buffer); //nalezeni bufferu v listu
    //kdyz buffer naleznu
    if (it != bufferList.end()){
        tmp = (BufferID *) *it; //ulozim si hodnotu iteratoru, tedy ukazatel na buffer
        bufferList.erase(it); //smazu zaznam o bufferu z listu
        delete tmp; //dealokuji buffer
    }
}

/**
 * @brief This function uploads data to selected buffer on the GPU
 *
 * @param buffer buffer identificator
 * @param offset specifies the offset into the buffer's data
 * @param size specifies the size of buffer that will be uploaded
 * @param data specifies a pointer to new data
 */
void GPU::setBufferData(BufferID buffer, uint64_t offset, uint64_t size, void const* data) {
  /// \todo Tato funkce nakopíruje data z cpu na "gpu".<br>
  /// Data by měla být nakopírována do bufferu vybraného parametrem "buffer".<br>
  /// Parametr size určuje, kolik dat (v bajtech) se překopíruje.<br>
  /// Parametr offset určuje místo v bufferu (posun v bajtech) kam se data nakopírují.<br>
  /// Parametr data obsahuje ukazatel na data na cpu pro kopírování.<br>
    std::list<BufferID>::iterator it; //iterator na hledani bufferu v listu
    it = std::find(bufferList.begin(), bufferList.end(), buffer); //nalezeni bufferu v listu
    memcpy(reinterpret_cast<void *>(*it + (BufferID) offset), data, size);
}

/**
 * @brief This function downloads data from GPU.
 *
 * @param buffer specfies buffer
 * @param offset specifies the offset into the buffer from which data will be returned, measured in bytes. 
 * @param size specifies data size that will be copied
 * @param data specifies a pointer to the location where buffer data is returned. 
 */
void GPU::getBufferData(BufferID buffer,
                        uint64_t offset,
                        uint64_t size,
                        void*    data)
{
  /// \todo Tato funkce vykopíruje data z "gpu" na cpu.
  /// Data by měla být vykopírována z bufferu vybraného parametrem "buffer".<br>
  /// Parametr size určuje kolik dat (v bajtech) se překopíruje.<br>
  /// Parametr offset určuje místo v bufferu (posun v bajtech) odkud se začne kopírovat.<br>
  /// Parametr data obsahuje ukazatel, kam se data nakopírují.<br>
    std::list<BufferID>::iterator it; //iterator na hledani bufferu v listu
    it = std::find(bufferList.begin(), bufferList.end(), buffer); //nalezeni bufferu v listu
    memcpy(data, reinterpret_cast<const void *>(*it + (BufferID)offset), size);
}

/**
 * @brief This function tests if buffer exists
 *
 * @param buffer selected buffer id
 *
 * @return true if buffer points to existing buffer on the GPU.
 */
bool GPU::isBuffer(BufferID buffer) { 
  /// \todo Tato funkce by měla vrátit true pokud buffer je identifikátor existující bufferu.<br>
  /// Tato funkce by měla vrátit false, pokud buffer není identifikátor existujícího bufferu. (nebo bufferu, který byl smazán).<br>
  /// Pro emptyId vrací false.<br>
    std::list<BufferID>::iterator it; //iterator na hledani bufferu v listu
    it = std::find(bufferList.begin(), bufferList.end(), buffer); //nalezeni bufferu v listu
    //kdyz buffer naleznu
    if (buffer == emptyID)
        return false;
    return it != bufferList.end();
}

/// @}

/**
 * \addtogroup vertexpuller_tasks 02. Implementace obslužných funkcí pro vertex puller
 * @{
 */

/**
 * @brief This function creates new vertex puller settings on the GPU,
 *
 * @return unique vertex puller identificator
 */
ObjectID GPU::createVertexPuller     (){
  /// \todo Tato funkce vytvoří novou práznou tabulku s nastavením pro vertex puller.<br>
  /// Funkce by měla vrátit identifikátor nové tabulky.
  /// Prázdná tabulka s nastavením neobsahuje indexování a všechny čtecí hlavy jsou vypnuté.
    auto * vertex_puller = new Vertex_puller_settings;
    vertex_puller->indexing.enabled = false;
    vertexPullerList.push_back((ObjectID) vertex_puller); //ukladam si ukazatele na buffery
        return (ObjectID) vertex_puller; //vracim pretypovany ukazatel
}

/**
 * @brief This function deletes vertex puller settings
 *
 * @param vao vertex puller identificator
 */
void     GPU::deleteVertexPuller     (VertexPullerID vao){
  /// \todo Tato funkce by měla odstranit tabulku s nastavení pro vertex puller.<br>
  /// Parameter "vao" obsahuje identifikátor tabulky s nastavením.<br>
  /// Po uvolnění nastavení je identifiktátor volný a může být znovu použit.<br>
    ObjectID * tmp; //pomocny ukazatel na vertexPuller v listu VertexPulleru
    std::list<ObjectID>::iterator it; //iterator na hledani VertexPulleru v listu
    it = std::find(vertexPullerList.begin(), vertexPullerList.end(), vao); //nalezeni VertexPulleru v listu
    //kdyz VertexPulleru naleznu
    if (it != vertexPullerList.end()){
        tmp = (ObjectID *) *it; //ulozim si hodnotu iteratoru, tedy ukazatel na VertexPulleru
        vertexPullerList.erase(it); //smazu zaznam o bufferu z listu
        delete tmp; //dealokuji buffer
    }
}

/**
 * @brief This function sets one vertex puller reading head.
 *
 * @param vao identificator of vertex puller
 * @param head id of vertex puller head
 * @param type type of attribute
 * @param stride stride in bytes
 * @param offset offset in bytes
 * @param buffer id of buffer
 */
void     GPU::setVertexPullerHead    (VertexPullerID vao,uint32_t head,AttributeType type,uint64_t stride,uint64_t offset,BufferID buffer){
  /// \todo Tato funkce nastaví jednu čtecí hlavu vertex pulleru.<br>
  /// Parametr "vao" vybírá tabulku s nastavením.<br>
  /// Parametr "head" vybírá čtecí hlavu vybraného vertex pulleru.<br>
  /// Parametr "type" nastaví typ atributu, který čtecí hlava čte. Tímto se vybere kolik dat v bajtech se přečte.<br>
  /// Parametr "stride" nastaví krok čtecí hlavy.<br>
  /// Parametr "offset" nastaví počáteční pozici čtecí hlavy.<br>
  /// Parametr "buffer" vybere buffer, ze kterého bude čtecí hlava číst.<br>
    ObjectID * tmp; //pomocny ukazatel na vertexPuller v listu VertexPulleru
    Vertex_puller_settings * vao_tmp;
    std::list<ObjectID>::iterator it; //iterator na hledani VertexPulleru v listu
    it = std::find(vertexPullerList.begin(), vertexPullerList.end(), vao); //nalezeni VertexPulleru v listu
    //kdyz VertexPulleru naleznu
    if (it != vertexPullerList.end()){
        vao_tmp = (Vertex_puller_settings *) *it;
        vao_tmp->heads[head].attrib_type = type;
        vao_tmp->heads[head].stride = stride;
        vao_tmp->heads[head].offset = offset;
        vao_tmp->heads[head].buffer_id = buffer;
    }
}

/**
 * @brief This function sets vertex puller indexing.
 *
 * @param vao vertex puller id
 * @param type type of index
 * @param buffer buffer with indices
 */
void     GPU::setVertexPullerIndexing(VertexPullerID vao,IndexType type,BufferID buffer){
  /// \todo Tato funkce nastaví indexování vertex pulleru.
  /// Parametr "vao" vybírá tabulku s nastavením.<br>
  /// Parametr "type" volí typ indexu, který je uložený v bufferu.<br>
  /// Parametr "buffer" volí buffer, ve kterém jsou uloženy indexy.<br>
    ObjectID * tmp; //pomocny ukazatel na vertexPuller v listu VertexPulleru
    Vertex_puller_settings * vao_tmp;
    std::list<ObjectID>::iterator it; //iterator na hledani VertexPulleru v listu
    it = std::find(vertexPullerList.begin(), vertexPullerList.end(), vao); //nalezeni VertexPulleru v listu
    if (it != vertexPullerList.end()){
        vao_tmp = (Vertex_puller_settings *) *it;
        vao_tmp->indexing.enabled = true;
        vao_tmp->indexing.buffer_id = buffer;
        vao_tmp->indexing.index_type = type;
    }
}

/**
 * @brief This function enables vertex puller's head.
 *
 * @param vao vertex puller 
 * @param head head id
 */
void     GPU::enableVertexPullerHead (VertexPullerID vao,uint32_t head){
  /// \todo Tato funkce povolí čtecí hlavu daného vertex pulleru.<br>
  /// Pokud je čtecí hlava povolena, hodnoty z bufferu se budou kopírovat do atributu vrcholů vertex shaderu.<br>
  /// Parametr "vao" volí tabulku s nastavením vertex pulleru (vybírá vertex puller).<br>
  /// Parametr "head" volí čtecí hlavu.<br>
    ObjectID * tmp; //pomocny ukazatel na vertexPuller v listu VertexPulleru
    Vertex_puller_settings * vao_tmp;
    std::list<ObjectID>::iterator it; //iterator na hledani VertexPulleru v listu
    it = std::find(vertexPullerList.begin(), vertexPullerList.end(), vao); //nalezeni VertexPulleru v listu
    //kdyz VertexPulleru naleznu
    if (it != vertexPullerList.end()){
        vao_tmp = (Vertex_puller_settings *) *it;
        vao_tmp->heads[head].enabled = true;
    }
}

/**
 * @brief This function disables vertex puller's head
 *
 * @param vao vertex puller id
 * @param head head id
 */
void     GPU::disableVertexPullerHead(VertexPullerID vao,uint32_t head){
  /// \todo Tato funkce zakáže čtecí hlavu daného vertex pulleru.<br>
  /// Pokud je čtecí hlava zakázána, hodnoty z bufferu se nebudou kopírovat do atributu vrcholu.<br>
  /// Parametry "vao" a "head" vybírají vertex puller a čtecí hlavu.<br>
    ObjectID * tmp; //pomocny ukazatel na vertexPuller v listu VertexPulleru
    Vertex_puller_settings * vao_tmp;
    std::list<ObjectID>::iterator it; //iterator na hledani VertexPulleru v listu
    it = std::find(vertexPullerList.begin(), vertexPullerList.end(), vao); //nalezeni VertexPulleru v listu
    //kdyz VertexPulleru naleznu
    if (it != vertexPullerList.end()){
        vao_tmp = (Vertex_puller_settings *) *it;
        vao_tmp->heads[head].enabled = false;
    }
}

/**
 * @brief This function selects active vertex puller.
 *
 * @param vao id of vertex puller
 */
void     GPU::bindVertexPuller       (VertexPullerID vao){
  /// \todo Tato funkce aktivuje nastavení vertex pulleru.<br>
  /// Pokud je daný vertex puller aktivován, atributy z bufferů jsou vybírány na základě jeho nastavení.<br>
    ObjectID * tmp; //pomocny ukazatel na vertexPuller v listu VertexPulleru
    Vertex_puller_settings * vao_tmp;
    std::list<ObjectID>::iterator it; //iterator na hledani VertexPulleru v listu
    it = std::find(vertexPullerList.begin(), vertexPullerList.end(), vao); //nalezeni VertexPulleru v listu
    //kdyz VertexPulleru naleznu
    if (it != vertexPullerList.end()){
        this->activeVertexPuller = (ObjectID *) *it;
    }
}

/**
 * @brief This function deactivates vertex puller.
 */
void     GPU::unbindVertexPuller     (){
  /// \todo Tato funkce deaktivuje vertex puller.
  /// To většinou znamená, že se vybere neexistující "emptyID" vertex puller.
    this->activeVertexPuller = nullptr;
}

/**
 * @brief This function tests if vertex puller exists.
 *
 * @param vao vertex puller
 *
 * @return true, if vertex puller "vao" exists
 */
bool     GPU::isVertexPuller         (VertexPullerID vao){
  /// \todo Tato funkce otestuje, zda daný vertex puller existuje.
  /// Pokud ano, funkce vrací true.
    ObjectID * tmp; //pomocny ukazatel na vertexPuller v listu VertexPulleru
    Vertex_puller_settings * vao_tmp;
    std::list<ObjectID>::iterator it; //iterator na hledani VertexPulleru v listu
    it = std::find(vertexPullerList.begin(), vertexPullerList.end(), vao); //nalezeni VertexPulleru v listu
    //kdyz VertexPulleru naleznu
    return it != vertexPullerList.end();
}

/// @}

/** \addtogroup program_tasks 03. Implementace obslužných funkcí pro shader programy
 * @{
 */

/**
 * @brief This function creates new shader program.
 *
 * @return shader program id
 */
ProgramID GPU::createProgram(){
  /// \todo Tato funkce by měla vytvořit nový shader program.<br>
  /// Funkce vrací unikátní identifikátor nového proramu.<br>
  /// Program je seznam nastavení, které obsahuje: ukazatel na vertex a fragment shader.<br>
  /// Dále obsahuje uniformní proměnné a typ výstupních vertex attributů z vertex shaderu, které jsou použity pro interpolaci do fragment atributů.<br>
  auto program = new Program;
  programList.push_back((ProgramID) program);
  return (ProgramID) program;
}

/**
 * @brief This function deletes shader program
 *
 * @param prg shader program id
 */
void             GPU::deleteProgram         (ProgramID prg){
  /// \todo Tato funkce by měla smazat vybraný shader program.<br>
  /// Funkce smaže nastavení shader programu.<br>
  /// Identifikátor programu se stane volným a může být znovu využit.<br>
    ProgramID * tmp;
    auto it = std::find(programList.begin(), programList.end(), prg);
    if (it != programList.end()){
        tmp = (ProgramID *) *it;
        programList.erase(it);
        delete tmp;
    }
}

/**
 * @brief This function attaches vertex and frament shader to shader program.
 *
 * @param prg shader program
 * @param vs vertex shader 
 * @param fs fragment shader
 */
void             GPU::attachShaders         (ProgramID prg,VertexShader vs,FragmentShader fs){
  /// \todo Tato funkce by měla připojít k vybranému shader programu vertex a fragment shader.
    auto it = std::find(programList.begin(), programList.end(), prg);
    if (it != programList.end()){
        ((Program *) *it)->vertexShader = vs;
        ((Program *) *it)->fragmentShader = fs;
    }
}

/**
 * @brief This function selects which vertex attributes should be interpolated during rasterization into fragment attributes.
 *
 * @param prg shader program
 * @param attrib id of attribute
 * @param type type of attribute
 */
void             GPU::setVS2FSType          (ProgramID prg, uint32_t attrib, AttributeType type){
  /// \todo tato funkce by měla zvolit typ vertex atributu, který je posílán z vertex shaderu do fragment shaderu.<br>
  /// V průběhu rasterizace vznikají fragment.<br>
  /// Fragment obsahují fragment atributy.<br>
  /// Tyto atributy obsahují interpolované hodnoty vertex atributů.<br>
  /// Tato funkce vybere jakého typu jsou tyto interpolované atributy.<br>
  /// Bez jakéhokoliv nastavení jsou atributy prázdne AttributeType::EMPTY<br>
}

/**
 * @brief This function actives selected shader program
 *
 * @param prg shader program id
 */
void             GPU::useProgram            (ProgramID prg){
  /// \todo tato funkce by měla vybrat aktivní shader program.
    this->activeProgram = (ProgramID *)prg;
}

/**
 * @brief This function tests if selected shader program exists.
 *
 * @param prg shader program
 *
 * @return true, if shader program "prg" exists.
 */
bool             GPU::isProgram             (ProgramID prg){
  /// \todo tato funkce by měla zjistit, zda daný program existuje.<br>
  /// Funkce vráti true, pokud program existuje.<br>
    auto it = std::find(programList.begin(), programList.end(), prg);
    return it != programList.end();
}

/**
 * @brief This function sets uniform value (1 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform1f      (ProgramID prg, uint32_t uniformId, float const &d){
  /// \todo tato funkce by měla nastavit uniformní proměnnou shader programu.<br>
  /// Parametr "prg" vybírá shader program.<br>
  /// Parametr "uniformId" vybírá uniformní proměnnou. Maximální počet uniformních proměnných je uložen v programné \link maxUniforms \endlink.<br>
  /// Parametr "d" obsahuje data (1 float).<br>
    auto it = std::find(programList.begin(), programList.end(), prg);
    if (it != programList.end())
        ((Program *) *it)->uniforms.uniform[uniformId].v1 = d;
}

/**
 * @brief This function sets uniform value (2 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform2f      (ProgramID prg, uint32_t uniformId, glm::vec2 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 2 floaty.
    auto it = std::find(programList.begin(), programList.end(), prg);
    if (it != programList.end())
        ((Program *) *it)->uniforms.uniform[uniformId].v2 = d;
}

/**
 * @brief This function sets uniform value (3 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform3f      (ProgramID prg,uint32_t uniformId,glm::vec3 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 3 floaty.
    auto it = std::find(programList.begin(), programList.end(), prg);
    if (it != programList.end())
        ((Program *) *it)->uniforms.uniform[uniformId].v3 = d;
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform4f      (ProgramID prg,uint32_t uniformId,glm::vec4 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 4 floaty.
    auto it = std::find(programList.begin(), programList.end(), prg);
    if (it != programList.end())
        ((Program *) *it)->uniforms.uniform[uniformId].v4 = d;
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniformMatrix4f(ProgramID prg,uint32_t uniformId,glm::mat4 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává matici 4x4 (16 floatů).
    auto it = std::find(programList.begin(), programList.end(), prg);
    if (it != programList.end())
        ((Program *) *it)->uniforms.uniform[uniformId].m4 = d;
}

/// @}





/** \addtogroup framebuffer_tasks 04. Implementace obslužných funkcí pro framebuffer
 * @{
 */

/**
 * @brief This function creates framebuffer on GPU.
 *
 * @param width width of framebuffer
 * @param height height of framebuffer
 */
void GPU::createFramebuffer(uint32_t width, uint32_t height){
  /// \todo Tato funkce by měla alokovat framebuffer od daném rozlišení.<br>
  /// Framebuffer se skládá z barevného a hloukového bufferu.<br>
  /// Buffery obsahují width x height pixelů.<br>
  /// Barevný pixel je složen z 4 x uint8_t hodnot - to reprezentuje RGBA barvu.<br>
  /// Hloubkový pixel obsahuje 1 x float - to reprezentuje hloubku.<br>
  /// Nultý pixel framebufferu je vlevo dole.<br>
    this->frameBuffer = new FrameBuffer(width, height);
}

/**
 * @brief This function deletes framebuffer.
 */
void GPU::deleteFramebuffer(){
  /// \todo tato funkce by měla dealokovat framebuffer.
  delete this->frameBuffer;
}

/**
 * @brief This function resizes framebuffer.
 *
 * @param width new width of framebuffer
 * @param height new heght of framebuffer
 */
void GPU::resizeFramebuffer(uint32_t width,uint32_t height){
  /// \todo Tato funkce by měla změnit velikost framebuffer.
    GPU::deleteFramebuffer();
    GPU::createFramebuffer(width, height);
}

/**
 * @brief This function returns pointer to color buffer.
 *
 * @return pointer to color buffer
 */
uint8_t* GPU::getFramebufferColor  (){
  /// \todo Tato funkce by měla vrátit ukazatel na začátek barevného bufferu.<br>
    return reinterpret_cast<uint8_t *>(this->frameBuffer->colorBuffer);
}

/**
 * @brief This function returns pointer to depth buffer.
 *
 * @return pointer to dept buffer.
 */
float* GPU::getFramebufferDepth    (){
  /// \todo tato funkce by mla vrátit ukazatel na začátek hloubkového bufferu.<br>
  return this->frameBuffer->depthBuffer;
}

/**
 * @brief This function returns width of framebuffer
 *
 * @return width of framebuffer
 */
uint32_t GPU::getFramebufferWidth (){
  /// \todo Tato funkce by měla vrátit šířku framebufferu.
  return this->frameBuffer->width;
}

/**
 * @brief This function returns height of framebuffer.
 *
 * @return height of framebuffer
 */
uint32_t GPU::getFramebufferHeight(){
  /// \todo Tato funkce by měla vrátit výšku framebufferu.
  return this->frameBuffer->height;
}

/// @}

/** \addtogroup draw_tasks 05. Implementace vykreslovacích funkcí
 * Bližší informace jsou uvedeny na hlavní stránce dokumentace.
 * @{
 */

/**
 * @brief This functino clears framebuffer.
 *
 * @param r red channel
 * @param g green channel
 * @param b blue channel
 * @param a alpha channel
 */
void GPU::clear(float r,float g,float b,float a){
  /// \todo Tato funkce by měla vyčistit framebuffer.<br>
  /// Barevný buffer vyčistí na barvu podle parametrů r g b a (0 - nulová intenzita, 1 a větší - maximální intenzita).<br>
  /// (0,0,0) - černá barva, (1,1,1) - bílá barva.<br>
  /// Hloubkový buffer nastaví na takovou hodnotu, která umožní rasterizaci trojúhelníka, který leží v rámci pohledového tělesa.<br>
  /// Hloubka by měla být tedy větší než maximální hloubka v NDC (normalized device coordinates).<br>
    r = (uint8_t )denormalize_color(r, (uint8_t) 256, true);
    g = (uint8_t )denormalize_color(g, (uint8_t) 256, true);
    b = (uint8_t )denormalize_color(b, (uint8_t) 256, true);
    a = (uint8_t )denormalize_color(a, (uint8_t) 256, true);

    uint8_t * colorBuffer = getFramebufferColor();
    float * depthBuffer = getFramebufferDepth();
    uint32_t frameBufferSize = getFramebufferWidth() * getFramebufferHeight();

    for (int i = 0; i < frameBufferSize; i++){
        colorBuffer[4 * i] = r;
        colorBuffer[4 * i + 1] = g;
        colorBuffer[4 * i + 2] = b;
        colorBuffer[4 * i + 3] = a;
        depthBuffer[i] = FLT_MAX;
    }
}


void GPU::drawTriangles(uint32_t  nofVertices){
  /// \todo Tato funkce vykreslí trojúhelníky podle daného nastavení.<br>
  /// Vrcholy se budou vybírat podle nastavení z aktivního vertex pulleru (pomocí bindVertexPuller).<br>
  /// Vertex shader a fragment shader se zvolí podle aktivního shader programu (pomocí useProgram).<br>
  /// Parametr "nofVertices" obsahuje počet vrcholů, který by se měl vykreslit (3 pro jeden trojúhelník).<br>
    if (activeVertexPuller == nullptr or activeProgram == nullptr)
        throw std::range_error("Vertex puller or Program is NULL, which cannot be.");
    if (nofVertices < 3 or nofVertices % 3 != 0)
        throw std::range_error("Parameter nofVertices has invalid value.");

    auto * outVertices = new OutVertex[nofVertices];
    std::vector<PrimitiveTriangle> primitiveTriangles;
    primitiveTriangles.reserve(nofVertices/3);
    Program * program = (Program *) * std::find(programList.begin(), programList.end(), (ProgramID) activeProgram);

    /*---VERTEX PROCESSOR---*/
    vertexProcessor(nofVertices, outVertices, program);
    /*---PRIMITIVE ASSEMBLY---*/
    for (int i = 0; i < nofVertices/3; i++)
        primitiveTriangles.push_back(PrimitiveTriangle{outVertices[i * 3], outVertices[i * 3 + 1], outVertices[i * 3 + 2]});
    /*---CLIPPING---*/

    OutVertex a, b, c, n;
    bool aIsOut, bIsOut, cIsOut;
    int x = 0, y = 1, z = 2, w = 3;
    std::vector<PrimitiveTriangle> newTriangles ;

    for (auto primitiveTriangle : primitiveTriangles){
        a = primitiveTriangle.ov1;
        b = primitiveTriangle.ov2;
        c = primitiveTriangle.ov3;
        aIsOut = -a.gl_Position[w] > a.gl_Position[z];
        bIsOut = -b.gl_Position[w] > b.gl_Position[z];
        cIsOut = -c.gl_Position[w] > c.gl_Position[z];

        if (aIsOut && bIsOut && cIsOut)
            ;
        else if (!aIsOut && !bIsOut && !cIsOut){
            newTriangles.push_back(primitiveTriangle);
            continue;
        }
        // 1 new triangle
        /*else if (aIsOut && bIsOut){
            n = getClippedPoint(c, a); //override point A
            primitiveTriangle.ov1 = n;
            n = getClippedPoint(c, b); //override point B
            primitiveTriangle.ov2 = n;
        }*/

    }
}

OutVertex GPU::getClippedPoint(OutVertex a, OutVertex b){
    float numerator = (-a.gl_Position[3] - a.gl_Position[2]);
    float denominator = (b.gl_Position[3] - a.gl_Position[3] + b.gl_Position[2] - a.gl_Position[2]);
    float t =  numerator/denominator;
    OutVertex x;


    x.gl_Position = a.gl_Position + t * (b.gl_Position - a.gl_Position);
    float *f;
    glm::vec2 *vec2; ///< vector of two floats
    glm::vec3 *vec3; ///< vector of three floats
    glm::vec4 * vec4; ///< vector of four floats
    glm::vec4 * vectt; ///< vector of four floats
    Attribute * tt;
    float ttt[5];
    //tt = ttt;
    if (typeid(vec4) == typeid(vectt))
        printf("AHoj\n");

    for (int i = 0; i < maxAttributes; i++){
        if (typeid(std::remove_reference<decltype(a.attributes)>::type) == typeid(std::remove_reference<decltype(f)>::type))
            printf("FLOAT\n");
        else if (typeid(std::remove_reference<decltype(a.attributes)>::type) == typeid(std::remove_reference<decltype(vec2)>::type))
            printf("VEC2\n");
        else if (typeid(std::remove_reference<decltype(a.attributes)>::type) == typeid(std::remove_reference<decltype(vec3)>::type))
            printf("VEC3\n");
        else if (typeid(std::remove_reference<decltype(a.attributes)>::type) == typeid(std::remove_reference<decltype(vec4)>::type))
            printf("VEC4\n");
        exit(55);
    }
    /*    switch (){
            case :
                x.attributes[i].v1 = a.attributes[i].v1 + t * (b.attributes[i].v1 - a.attributes[i].v1);
            case AttributeType::VEC2:
                x.attributes[i].v2 = a.attributes[i].v2 + t * (b.attributes[i].v2 - a.attributes[i].v2);
            case AttributeType::VEC3:
                x.attributes[i].v3 = a.attributes[i].v3 + t * (b.attributes[i].v3 - a.attributes[i].v3);
            case AttributeType::VEC4:
                x.attributes[i].v4 = a.attributes[i].v4 + t * (b.attributes[i].v4 - a.attributes[i].v4);
            default: break;
        }*/
    return x;
}

/**
 * @brief This method represents Vertex Processor
 * @param nofVertices number of vertices to process
 * @param outVertices array of OutVertices
 * @param program active program with shaders etc.
 */
void GPU::vertexProcessor(uint32_t nofVertices, OutVertex * outVertices, Program * program) {
    Vertex_puller_settings * vertexPullerSettings = (Vertex_puller_settings *) * std::find(vertexPullerList.begin(), vertexPullerList.end(), (VertexPullerID) activeVertexPuller);
    BufferID * vertexPullerBuffer = (BufferID *) *std::find(bufferList.begin(), bufferList.end(), vertexPullerSettings->indexing.buffer_id);
    InVertex inVertex;
    OutVertex outVertex;
    int index = 0;
    for (int i = 0; i < nofVertices; i++) {
        if (vertexPullerSettings->indexing.enabled) {
            if (vertexPullerSettings->indexing.index_type == IndexType::UINT8)
                getBufferData((BufferID) vertexPullerBuffer, i * sizeof(uint8_t), sizeof(uint8_t), &index);
            else if (vertexPullerSettings->indexing.index_type == IndexType::UINT16)
                getBufferData((BufferID) vertexPullerBuffer, i * sizeof(uint16_t), sizeof(uint16_t), &index);
            else if (vertexPullerSettings->indexing.index_type == IndexType::UINT32)
                getBufferData((BufferID) vertexPullerBuffer, i * sizeof(uint32_t), sizeof(uint32_t), &index);
        }
        else
            index = i;

        int k = 0;
        for (auto & attribute : inVertex.attributes) {
            auto head = vertexPullerSettings->heads[k++];
            if (head.enabled) {
                BufferID *headBuffer = (BufferID *) *std::find(bufferList.begin(), bufferList.end(), head.buffer_id);

                switch (head.attrib_type) {
                    case AttributeType::FLOAT:
                        attribute.v1 = *((float *) ((size_t) headBuffer + head.offset +
                                                                 head.stride * index));
                        break;
                    case AttributeType::VEC2:
                        attribute.v2 = *((glm::vec2 *) ((size_t) headBuffer + head.offset +
                                                                     head.stride * index));
                        break;
                    case AttributeType::VEC3:
                        attribute.v3 = *((glm::vec3 *) ((size_t) headBuffer + head.offset +
                                                                     head.stride * index));
                        break;
                    case AttributeType::VEC4:
                        attribute.v4 = *((glm::vec4 *) ((size_t) headBuffer + head.offset +
                                                                     head.stride * index));
                        break;
                    default:
                        break;
                }
            }
        }
        inVertex.gl_VertexID = index;
        program->vertexShader(outVertex, inVertex, program->uniforms);
        outVertices[i] = outVertex;
    }
}

/// @}
FrameBuffer::FrameBuffer(uint32_t width, uint32_t height) {
    this->height = height;
    this->width = width;
    this->colorBuffer = new  uint8_t[width * height * 4];
    this->depthBuffer = new float[width * height];
}
/**
 * @brief Normalize number by normalizator
 * @param num number to be normalized
 * @param normalizator
 * @param trunc if true, apply truncation to float number to cut it to <0.0, 1.0> interval, default false
 * @return float number from <0.0, 1.0> interval
 */
float normalize_color(uint8_t num, uint8_t normalizator, bool trunc = false) {
    if (normalizator == 0)
        throw std::overflow_error("Division by zero leads to infinity.");
    if (trunc)
        return fit_color((float) num / (float) normalizator);
    return (float)num / (float)normalizator;
}
/**
 * @brief Denormalize float to uint8_t by normalizer, e.g. num:0.5, normalize:256 => return:128
 * @param num number to denormalization
 * @param normalizer
 * @param trunc if true, apply truncation to float number to cut it to <0.0, 1.0> interval, default false
 * @return denormalized uint8_t
 */
uint8_t denormalize_color(float num, uint8_t normalizer, bool trunc = false) {
    if (trunc)
        return (uint8_t)(fit_color(num) * (float)normalizer);
    return (uint8_t)(num * (float)normalizer);
}
/**
 * @brief Round float number to fit in <0.0, 1.0> interval
 * @param num number to round
 * @return rounded float number
 */
float fit_color(float num) {
    if (num < 0.0)
        return 0.0;
    else if (num > 1.0)
        return 1.0;
    return num;
}

