#pragma once
#include <PGRenderCore/shader.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace PGRenderCore {

    class ShaderGL : public Shader {
    public:
        explicit ShaderGL(const ShaderDesc& desc);
        ~ShaderGL() override;

        bool compile() override;
        void release() override;

        std::string getLastError() const;
        unsigned long nativeHandle() const override;
        const ShaderDesc& getDesc() const override;

    private:
        ShaderDesc m_desc;
        unsigned long m_programId = 0;
        std::unordered_map<unsigned int, unsigned int> m_shaderObjects; // GLuint equiv.
        std::string m_lastError;

        unsigned int shaderTypeToGL(ShaderStage stage) const;
        bool compileShaderStage(unsigned int shaderType, const std::string& source, unsigned int& outShader);
        void detachAndDeleteShaders();
    };

} // namespace PGRenderCore

