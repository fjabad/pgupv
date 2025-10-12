# PGRenderCore - Documentación de Usuario

## Descripción General

**PGRenderCore** es una librería de renderizado 3D multiplataforma y multi-API diseñada para proporcionar una abstracción moderna y eficiente sobre diferentes backends de renderizado (OpenGL, Vulkan, DirectX 12, Metal).

### Características principales

- **Multiplataforma**: Windows, Linux, macOS
- **Multi-API**: OpenGL 4.6, Vulkan, DirectX 12, Metal (en desarrollo)
- **Abstracción moderna**: Inspirada en Vulkan y DirectX 12
- **Sin dependencias de frameworks**: No depende de SDL, GLFW u otros (el usuario elige)
- **Type-safe**: Uso extensivo de enums y tipos específicos
- **Performance-oriented**: Minimiza cambios de estado y overhead
- **Ray Tracing**: Soporte para trazado de rayos por hardware (donde esté disponible)
- **Gestión de recursos**: Smart pointers y RAII para seguridad de memoria


### Filosofía de diseño

PGRenderCore sigue estos principios:

1. **Separación de responsabilidades**: Pipeline (estado inmutable) vs Context (estado dinámico)
2. **Explicit is better than implicit**: El usuario tiene control total
3. **Modern GPU architecture**: Diseño compatible con APIs modernas
4. **Zero overhead abstraction**: Mapeo directo a APIs nativas sin penalizaciones

***

## Esquema de Uso Típico

### Flujo de trabajo básico

```
1. Crear Device (seleccionar backend)
   ↓
2. Crear Context (asociado a ventana)
   ↓
3. Crear recursos (buffers, texturas, shaders)
   ↓
4. Crear VertexArray (geometría + layout)
   ↓
5. Crear Pipeline (shaders + estado de renderizado)
   ↓
6. Crear RenderPass (estructura de framebuffer)
   ↓
7. Loop de renderizado:
   - beginRenderPass()
   - bindPipeline()
   - bindVertexArray()
   - draw() / drawIndexed()
   - endRenderPass()
   - swapBuffers()
```


### Ejemplo mínimo completo

```cpp
#include <PGRenderCore/DeviceFactory.h>
#include <SDL3/SDL.h>

int main() {
    // 1. Inicializar SDL y crear ventana
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("App", 1280, 720, SDL_WINDOW_OPENGL);
    
    // 2. Obtener handles nativos
    SDL_PropertiesID props = SDL_GetWindowProperties(window);
    void* nativeWindow = SDL_GetPointerProperty(props, 
        SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr); // Win32
    
    // 3. Crear Device y Context
    PGRenderCore::DeviceDesc deviceDesc;
    deviceDesc.backend = PGRenderCore::RenderBackend::OpenGL4;
    auto device = PGRenderCore::createDevice(deviceDesc);
    
    PGRenderCore::ContextDesc contextDesc;
    contextDesc.nativeWindowHandle = nativeWindow;
    auto context = device->createContext(contextDesc);
    
    // 4. Crear recursos (buffers, shaders, etc.)
    // ... (ver ejemplos detallados más abajo)
    
    // 5. Loop de renderizado
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
        }
        
        context->clear(PGRenderCore::ClearFlags::All);
        // ... renderizar ...
        context->swapBuffers();
    }
    
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
```


***

## Clases Principales

### 1. Device

**Propósito**: Representa el dispositivo de renderizado (GPU) y factoría de contextos.

**Creación**:

```cpp
PGRenderCore::DeviceDesc desc;
desc.backend = PGRenderCore::RenderBackend::OpenGL4; // u otro backend
desc.enableDebug = true; // Validación y mensajes de debug
auto device = PGRenderCore::createDevice(desc);
```

**Métodos principales**:

- `createContext(const ContextDesc&)`: Crea un contexto de renderizado
- `getBackendType()`: Obtiene el backend actual
- `getCapabilities()`: Información sobre capacidades del dispositivo

**Uso típico**: Se crea una vez al inicio de la aplicación.

***

### 2. Context

**Propósito**: Contexto de renderizado asociado a una ventana. Gestiona recursos, estado y comandos.

**Creación**:

```cpp
PGRenderCore::ContextDesc desc;
desc.nativeWindowHandle = windowHandle; // Handle de ventana nativo
desc.nativeDisplayHandle = displayHandle; // Para X11/Wayland
desc.width = 1280;
desc.height = 720;
desc.enableVSync = true;
auto context = device->createContext(desc);
```

**Categorías de métodos**:

#### a) Fábricas de recursos

```cpp
auto buffer = context->createBuffer(bufferDesc);
auto texture = context->createTexture(textureDesc);
auto shader = context->createShader(shaderDesc);
auto pipeline = context->createPipeline(pipelineDesc);
auto vertexArray = context->createVertexArray(vaoDesc);
auto renderPass = context->createRenderPass(renderPassDesc);
```


#### b) Binding de recursos

```cpp
context->bindPipeline(pipeline);
context->bindVertexArray(vao);
context->bindTexture(texture, 0); // slot 0
context->bindUniformBuffer(ubo, 0); // binding 0
context->bindShaderStorageBuffer(ssbo, 1);
```


#### c) Estado dinámico

```cpp
context->setViewport(0, 0, 1280, 720);
context->setScissor(100, 100, 500, 500);
context->setClearColor(0.1f, 0.1f, 0.15f, 1.0f);
context->setClearDepth(1.0f);

// Si el pipeline permite dynamic state:
context->setCullMode(PGRenderCore::CullMode::Front);
context->setDepthBias(1.0f, 0.5f);
```


#### d) Comandos de dibujo

```cpp
context->draw(vertexCount);
context->drawIndexed(indexCount);
context->drawInstanced(vertexCount, instanceCount);
context->drawIndexedInstanced(indexCount, instanceCount);
```


#### e) Render Pass

```cpp
context->beginRenderPass(renderPass, beginInfo);
// ... comandos de dibujo ...
context->endRenderPass();
```


#### f) Presentación

```cpp
context->swapBuffers(); // Presenta el frame renderizado
```

**Uso típico**: Se mantiene durante toda la vida de la aplicación.

***

### 3. BufferObject

**Propósito**: Almacenamiento de datos en GPU (vértices, índices, uniforms, etc.).

**Creación**:

```cpp
std::vector<float> vertices = {-0.5f, -0.5f, 0.0f, /*...*/};

PGRenderCore::BufferDesc desc;
desc.type = PGRenderCore::BufferType::Vertex;
desc.usage = PGRenderCore::BufferUsage::Static; // o Dynamic, Stream
desc.size = vertices.size() * sizeof(float);
desc.data = vertices.data(); // Datos iniciales (opcional)
desc.debugName = "MyVertexBuffer";

auto buffer = context->createBuffer(desc);
```

**Tipos de buffer**:

- `Vertex`: Datos de vértices
- `Index`: Índices de geometría
- `Uniform`: Uniform Buffer Objects (UBO)
- `ShaderStorage`: Shader Storage Buffer Objects (SSBO)

**Operaciones**:

```cpp
// Actualizar datos
buffer->update(newData, size, offset);

// Mapear para escritura directa
float* mapped = static_cast<float*>(buffer->map());
// ... escribir datos ...
buffer->unmap();

// Copiar entre buffers
dstBuffer->copyFrom(srcBuffer, srcOffset, dstOffset, size);

// Redimensionar
buffer->resize(newSize, optionalData);
```


***

### 4. VertexArray

**Propósito**: Encapsula geometría (buffers) + layout (cómo interpretar los datos).

**Creación**:

```cpp
// 1. Definir layout de vértices
auto layout = PGRenderCore::VertexLayoutBuilder()
    .addBufferBinding(0, sizeof(Vertex), false) // binding, stride, per-instance
    .addAttribute(0, PGRenderCore::VertexAttributeType::Float3, 0, 0)  // location, type, binding, offset
    .addAttribute(1, PGRenderCore::VertexAttributeType::Float3, 0, 12) // normal
    .addAttribute(2, PGRenderCore::VertexAttributeType::Float2, 0, 24) // texCoord
    .build();

// 2. Crear VAO
PGRenderCore::VertexArrayDesc vaoDesc;
vaoDesc.layout = layout;
vaoDesc.vertexBuffers.push_back(vertexBuffer);
vaoDesc.indexBuffer = indexBuffer; // opcional

auto vao = context->createVertexArray(vaoDesc);
```

**Ventajas**:

- Un solo `bindVertexArray()` configura toda la geometría
- Reutilizable con diferentes buffers
- Compatible con instancing

***

### 5. Shader

**Propósito**: Código ejecutado en GPU (vertex, fragment, compute, etc.).

**Creación**:

```cpp
PGRenderCore::ShaderDesc desc;

// Vertex shader
desc.stages.push_back({
    PGRenderCore::ShaderStage::Vertex,
    R"(
        #version 460 core
        layout(location = 0) in vec3 aPos;
        void main() {
            gl_Position = vec4(aPos, 1.0);
        }
    )"
});

// Fragment shader
desc.stages.push_back({
    PGRenderCore::ShaderStage::Fragment,
    R"(
        #version 460 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0, 0.5, 0.2, 1.0);
        }
    )"
});

auto shader = context->createShader(desc);

// Compilar
if (!shader->compile()) {
    std::cerr << "Error: " << shader->getCompilationLog() << std::endl;
}
```

**Stages soportados**:

- `Vertex`: Procesamiento de vértices
- `Fragment`: Procesamiento de píxeles
- `Geometry`: Generación de geometría (opcional)
- `Compute`: Compute shaders
- `TessControl`, `TessEvaluation`: Teselación (opcional)

***

### 6. Pipeline

**Propósito**: Define el estado de renderizado completo (shaders + configuración).

**Creación**:

```cpp
PGRenderCore::PipelineDesc desc;

// Shaders
desc.vertexShader = vertexShader;
desc.fragmentShader = fragmentShader;

// Rasterización
desc.cullMode = PGRenderCore::CullMode::Back;
desc.frontFace = PGRenderCore::FrontFace::CounterClockwise;
desc.polygonMode = PGRenderCore::PolygonMode::Fill;

// Depth/Stencil
desc.depthTestEnable = true;
desc.depthWriteEnable = true;
desc.depthFunc = PGRenderCore::DepthFunc::Less;

// Blending
desc.blendMode = PGRenderCore::BlendMode::None; // o AlphaBlend, Additive...

// Dynamic state (opcional)
desc.dynamicState.cullMode = true; // Permite setCullMode() sin cambiar pipeline

desc.debugName = "MainPipeline";

auto pipeline = context->createPipeline(desc);
```

**Estado que contiene**:

- Shaders (vertex, fragment, etc.)
- Culling mode (Front, Back, None)
- Depth test function (Less, LessEqual, etc.)
- Blend mode (None, AlphaBlend, Additive, etc.)
- Polygon mode (Fill, Line, Point)
- Topology (Triangles, Lines, Points)

**Uso**:

```cpp
context->bindPipeline(pipeline);
// Ahora todo el estado del pipeline está activo
```


***

### 7. RenderPass

**Propósito**: Define la estructura de renderizado (attachments, load/store ops, subpases).

**Creación básica**:

```cpp
PGRenderCore::RenderPassDesc desc;

// Attachment 0: Color buffer
PGRenderCore::AttachmentDesc colorAtt;
colorAtt.format = PGRenderCore::TextureFormat::RGBA8;
colorAtt.loadOp = PGRenderCore::LoadOp::Clear; // Clear al inicio
colorAtt.storeOp = PGRenderCore::StoreOp::Store; // Guardar al final
colorAtt.clearColor[0] = 0.1f;
colorAtt.clearColor[1] = 0.1f;
colorAtt.clearColor[2] = 0.15f;
colorAtt.clearColor[3] = 1.0f;
desc.attachments.push_back(colorAtt);

// Attachment 1: Depth buffer
PGRenderCore::AttachmentDesc depthAtt;
depthAtt.format = PGRenderCore::TextureFormat::Depth32F;
depthAtt.loadOp = PGRenderCore::LoadOp::Clear;
depthAtt.storeOp = PGRenderCore::StoreOp::DontCare; // No necesitamos guardarlo
depthAtt.clearDepth = 1.0f;
desc.attachments.push_back(depthAtt);

desc.debugName = "MainRenderPass";

auto renderPass = context->createRenderPass(desc);
```

**Uso**:

```cpp
PGRenderCore::RenderPassBeginInfo beginInfo;
beginInfo.renderTarget = nullptr; // nullptr = backbuffer
beginInfo.renderAreaWidth = 1280;
beginInfo.renderAreaHeight = 720;

context->beginRenderPass(renderPass, beginInfo);
// Clear y configuración automática

context->bindPipeline(pipeline);
context->bindVertexArray(vao);
context->drawIndexed(indexCount);

context->endRenderPass();
// Store automático
```

**Load/Store Ops**:

- `LoadOp::Clear`: Limpiar al inicio
- `LoadOp::Load`: Cargar contenido existente
- `LoadOp::DontCare`: No importa (optimización)
- `StoreOp::Store`: Guardar resultado
- `StoreOp::DontCare`: No guardar (optimización en móviles)

***

### 8. Texture

**Propósito**: Imágenes y datos 2D/3D en GPU.

**Creación**:

```cpp
PGRenderCore::TextureDesc desc;
desc.type = PGRenderCore::TextureType::Texture2D;
desc.format = PGRenderCore::TextureFormat::RGBA8;
desc.width = 1024;
desc.height = 1024;
desc.mipLevels = 1;
desc.mipmapped = false;

auto texture = context->createTexture(desc);

// Cargar datos
std::vector<uint8_t> pixels = loadImageData();
texture->update(pixels.data(), pixels.size());
```

**Tipos**:

- `Texture1D`, `Texture2D`, `Texture3D`
- `TextureCube`: Cubemap
- `TextureBuffer`: Buffer texture

**Formatos comunes**:

- `R8`, `RG8`, `RGB8`, `RGBA8`
- `R16F`, `RGBA16F`, `RGBA32F`
- `Depth24Stencil8`, `Depth32F`

**Uso**:

```cpp
context->bindTexture(texture, 0); // Slot 0
// En el shader: layout(binding = 0) uniform sampler2D texSampler;
```


***

### 9. RenderTarget

**Propósito**: Framebuffer offscreen (render to texture).

**Creación**:

```cpp
// Crear texturas de destino
auto colorTexture = context->createTexture(colorTexDesc);
auto depthTexture = context->createTexture(depthTexDesc);

// Crear render target
PGRenderCore::RenderTargetDesc desc;
desc.width = 1024;
desc.height = 1024;
desc.colorAttachments.push_back(colorTexture);
desc.depthStencilAttachment = depthTexture;

auto renderTarget = context->createRenderTarget(desc);
```

**Uso**:

```cpp
// En beginRenderPass, especificar el render target
beginInfo.renderTarget = renderTarget; // en lugar de nullptr
context->beginRenderPass(renderPass, beginInfo);
// ... renderizar ...
context->endRenderPass();

// Ahora colorTexture contiene el resultado
```


***

### 10. Sampler

**Propósito**: Define cómo se samplea una textura (filtrado, wrapping, etc.).

**Creación**:

```cpp
PGRenderCore::SamplerDesc desc;
desc.minFilter = PGRenderCore::TextureFilter::Linear;
desc.magFilter = PGRenderCore::TextureFilter::Linear;
desc.wrapS = PGRenderCore::TextureWrap::Repeat;
desc.wrapT = PGRenderCore::TextureWrap::Repeat;
desc.anisotropyEnable = true;
desc.maxAnisotropy = 16.0f;

auto sampler = context->createSampler(desc);
```

**Uso**:

```cpp
context->bindSampler(sampler, 0); // Slot 0
context->bindTexture(texture, 0);
```


***

## Ray Tracing (Opcional)

### Verificar soporte

```cpp
if (context->isRayTracingSupported()) {
    // Ray tracing disponible
}
```


### Acceleration Structures

```cpp
// BLAS (geometría)
PGRenderCore::RayTracingGeometryDesc geometry;
geometry.type = PGRenderCore::RayTracingGeometryType::Triangles;
geometry.triangles.vertexBuffer = vertexBuffer;
geometry.triangles.indexBuffer = indexBuffer;
// ... configurar ...

PGRenderCore::BLASDesc blasDesc;
blasDesc.geometries.push_back(geometry);
auto blas = context->createBLAS(blasDesc);

// TLAS (instancias)
PGRenderCore::RayTracingInstance instance;
instance.blas = blas;
// ... matriz de transformación ...

PGRenderCore::TLASDesc tlasDesc;
tlasDesc.instances.push_back(instance);
auto tlas = context->createTLAS(tlasDesc);
```


### Ray Tracing Pipeline

```cpp
PGRenderCore::RayTracingPipelineDesc desc;
desc.shaders.push_back({PGRenderCore::RayTracingShaderStage::RayGeneration, rayGenShader});
desc.shaders.push_back({PGRenderCore::RayTracingShaderStage::Miss, missShader});
desc.shaders.push_back({PGRenderCore::RayTracingShaderStage::ClosestHit, hitShader});
// ... configurar shader groups ...

auto rtPipeline = context->createRayTracingPipeline(desc);
```


### Trazar rayos

```cpp
context->bindRayTracingPipeline(rtPipeline);
context->bindAccelerationStructure(tlas, 0);

PGRenderCore::TraceRaysDesc traceDesc;
traceDesc.width = 1280;
traceDesc.height = 720;
context->traceRays(traceDesc);
```


***

## Patrones de Uso Comunes

### 1. Renderizado básico

```cpp
// Setup (una vez)
auto renderPass = context->createRenderPass(rpDesc);
auto pipeline = context->createPipeline(pipelineDesc);
auto vao = context->createVertexArray(vaoDesc);

// Loop
while (running) {
    context->beginRenderPass(renderPass, beginInfo);
    context->bindPipeline(pipeline);
    context->bindVertexArray(vao);
    context->drawIndexed(indexCount);
    context->endRenderPass();
    context->swapBuffers();
}
```


### 2. Múltiples objetos

```cpp
context->beginRenderPass(renderPass, beginInfo);
context->bindPipeline(pipeline);

for (const auto& obj : sceneObjects) {
    context->bindVertexArray(obj->vao);
    // Actualizar uniforms si es necesario
    context->drawIndexed(obj->indexCount);
}

context->endRenderPass();
```


### 3. Múltiples pipelines

```cpp
context->beginRenderPass(renderPass, beginInfo);

// Objetos opacos
context->bindPipeline(solidPipeline);
for (const auto& obj : opaqueObjects) {
    context->bindVertexArray(obj->vao);
    context->drawIndexed(obj->indexCount);
}

// Objetos transparentes
context->bindPipeline(transparentPipeline);
for (const auto& obj : transparentObjects) {
    context->bindVertexArray(obj->vao);
    context->drawIndexed(obj->indexCount);
}

context->endRenderPass();
```


### 4. Render to texture + post-processing

```cpp
// Primera pasada: render to texture
context->beginRenderPass(offscreenRenderPass, offscreenBeginInfo);
context->bindPipeline(scenePipeline);
// ... renderizar escena ...
context->endRenderPass();

// Segunda pasada: post-processing
context->beginRenderPass(screenRenderPass, screenBeginInfo);
context->bindPipeline(postProcessPipeline);
context->bindTexture(offscreenTexture, 0);
context->bindVertexArray(fullscreenQuadVAO);
context->draw(6); // 2 triángulos = quad fullscreen
context->endRenderPass();
```


***

## Mejores Prácticas

### Performance

1. **Minimizar cambios de pipeline**: Agrupa objetos por pipeline
2. **Reutilizar recursos**: Crea pipelines/VAOs una vez, úsalos muchas veces
3. **Usar dynamic state**: Para cambios pequeños sin recrear pipeline
4. **Batch rendering**: Dibuja múltiples objetos con un solo draw call (instancing)
5. **Usar StoreOp::DontCare**: Para depth buffers temporales (móviles)

### Gestión de memoria

1. **Smart pointers**: Todos los recursos usan `std::shared_ptr`
2. **RAII**: Los recursos se liberan automáticamente
3. **No guardar raw pointers**: Usa `shared_ptr` para evitar dangling pointers

### Debug

1. **Usar debug names**: Todos los recursos aceptan `debugName`
2. **Habilitar validación**: `deviceDesc.enableDebug = true`
3. **Verificar compilación de shaders**: Siempre check `shader->compile()`

### Portabilidad

1. **No asumir backend específico**: Escribe código agnóstico a la API
2. **Verificar capacidades**: Usa `context->isRayTracingSupported()`, etc.
3. **Probar en múltiples backends**: OpenGL, Vulkan, etc.

***

## Recursos Adicionales

- **Ejemplos**: Ver carpeta `examples/` en el repositorio
- **API Reference**: Documentación generada con Doxygen
- **Backends soportados**: OpenGL 4.6 (completo), Vulkan (en desarrollo)
- **Plataformas**: Windows, Linux, macOS

**Licencia**: [Especificar licencia]
**Autor**: [Tu nombre/organización]
**Repositorio**: [URL del repositorio]

