#include "PGRenderCoreGL/bufferObjectGL.h"
#include <stdexcept>
#include <cstring>
#include <GL/glew.h>

namespace PGRenderCore {


	static GLenum toGLUsage(BufferObject::Usage usage) {
		switch (usage) {
		case BufferObject::Usage::Immutable: return GL_STATIC_DRAW;
		case BufferObject::Usage::Dynamic: return GL_DYNAMIC_DRAW;
		case BufferObject::Usage::Staging: return GL_STREAM_READ; // Aproximado para staging
		case BufferObject::Usage::Stream: return GL_STREAM_DRAW;
		default: return GL_STATIC_DRAW;
		}
	}

	static GLenum toGLTarget(BufferObject::Type type)
	{
		switch (type) {
		case BufferObject::Type::Vertex: return GL_ARRAY_BUFFER;
		case BufferObject::Type::Index: return GL_ELEMENT_ARRAY_BUFFER;
		case BufferObject::Type::Uniform: return GL_UNIFORM_BUFFER;
		case BufferObject::Type::Storage: return GL_SHADER_STORAGE_BUFFER;
		default: return GL_ARRAY_BUFFER;
		}
	}

	BufferObjectGL::BufferObjectGL(const Desc& desc)
		: m_size(desc.sizeBytes),
		m_usage(desc.usage),
		m_type(desc.type),
		m_cpuReadable(desc.cpuReadable),
		m_cpuWritable(desc.cpuWritable),
		m_isMapped(false),
		m_mappedPtr(nullptr)
	{
		glGenBuffers(1, &m_bufferId);
		GLenum target = toGLTarget(m_type);
		glBindBuffer(target, m_bufferId);

		GLenum usage = toGLUsage(m_usage);

		// Reservar memoria en GPU (data nullptr)
		glBufferData(target, m_size, nullptr, usage);

		// Opcional: desbind
		glBindBuffer(target, 0);
	}

	BufferObjectGL::~BufferObjectGL()
	{
		if (m_bufferId != 0) {
			glDeleteBuffers(1, &m_bufferId);
			m_bufferId = 0;
		}
	}

	void BufferObjectGL::update(const void* data, size_t sizeBytes, size_t offset)
	{
		if (!m_cpuWritable) {
			throw std::runtime_error("Buffer is not CPU writable");
		}
		if (offset + sizeBytes > m_size) {
			throw std::out_of_range("Update exceeds buffer size");
		}
		GLenum target = toGLTarget(m_type);
		glBindBuffer(target, m_bufferId);
		glBufferSubData(target, offset, sizeBytes, data);
		glBindBuffer(target, 0);
	}

	void* BufferObjectGL::map(MapAccess access, size_t offset, size_t sizeBytes)
	{
		if (m_isMapped) {
			throw std::runtime_error("Buffer is already mapped");
		}
		GLenum target = toGLTarget(m_type);
		glBindBuffer(target, m_bufferId);

		GLbitfield accessFlags = 0;
		switch (access) {
		case MapAccess::Read: accessFlags = GL_MAP_READ_BIT; break;
		case MapAccess::Write: accessFlags = GL_MAP_WRITE_BIT; break;
		case MapAccess::ReadWrite: accessFlags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT; break;
		default: throw std::runtime_error("Invalid MapAccess value");
		}

		if (sizeBytes == 0) {
			sizeBytes = m_size - offset;
		}

		m_mappedPtr = glMapBufferRange(target, offset, sizeBytes, accessFlags);
		m_isMapped = (m_mappedPtr != nullptr);

		glBindBuffer(target, 0);
		return m_mappedPtr;
	}

	void BufferObjectGL::unmap()
	{
		if (!m_isMapped) {
			throw std::runtime_error("Buffer is not mapped");
		}
		GLenum target = toGLTarget(m_type);
		glBindBuffer(target, m_bufferId);
		if (!glUnmapBuffer(target)) {
			glBindBuffer(target, 0);
			throw std::runtime_error("glUnmapBuffer failed - Data may be corrupted");
		}
		m_isMapped = false;
		m_mappedPtr = nullptr;
		glBindBuffer(target, 0);
	}

	void BufferObjectGL::read(void* dst, size_t sizeBytes, size_t offset)
	{
		if (!m_cpuReadable) {
			throw std::runtime_error("Buffer is not CPU readable");
		}
		if (offset + sizeBytes > m_size) {
			throw std::out_of_range("Read exceeds buffer size");
		}
		GLenum target = toGLTarget(m_type);
		glBindBuffer(target, m_bufferId);
		void* ptr = glMapBufferRange(target, offset, sizeBytes, GL_MAP_READ_BIT);
		if (!ptr) {
			glBindBuffer(target, 0);
			throw std::runtime_error("glMapBufferRange failed");
		}
		std::memcpy(dst, ptr, sizeBytes);
		if (!glUnmapBuffer(target)) {
			glBindBuffer(target, 0);
			throw std::runtime_error("glUnmapBuffer failed during read");
		}
		glBindBuffer(target, 0);
	}

} // namespace PGRenderCore
