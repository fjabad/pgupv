#pragma once

#pragma once
#include <cstddef>
#include <cstdint>

namespace PGRenderCore {

    /**
     * @brief Interface para buffers de GPU usados en gráfico y cómputo.
     *
     * Soporta definición de buffers mutables o inmutables, mapeo de memoria,
     * lectura/escritura, y consulta de estado.
     */
    class BufferObject {
    public:
        /**
         * @brief Tipos de uso del buffer.
         */
        enum class Usage {
            Immutable,      ///< Buffer solo escrito una vez (durante creación).
            Dynamic,        ///< Buffer actualizado frecuentemente desde CPU.
            Staging,        ///< Buffer intermedio para copias GPU-CPU.
            Stream          ///< Buffer para uso temporal rápido.
        };

        /**
         * @brief Tipos de buffer según su uso específico en pipeline.
         */
        enum class Type {
            Vertex,         ///< Buffer de vértices.
            Index,          ///< Buffer de índices.
            Uniform,        ///< Buffer uniforme (UBO).
            Storage         ///< Buffer de almacenamiento (SSBO).
        };

        /**
         * @brief Acceso al memoria para mapeo CPU.
         */
        enum class MapAccess {
            Read,
            Write,
            ReadWrite
        };

        /**
         * @brief Descriptor para crear y configurar un BufferObject.
         */
        struct Desc {
            size_t sizeBytes = 0;          ///< Tamaño en bytes del buffer.
            Usage usage = Usage::Immutable; ///< Uso del buffer.
            Type type = Type::Vertex;      ///< Tipo del buffer.
            bool cpuReadable = false;      ///< Permite lectura desde CPU.
            bool cpuWritable = true;       ///< Permite escritura desde CPU.

            // Se pueden añadir flags adicionales (coherencia, cached, etc.)
        };

        virtual ~BufferObject() = default;

        /**
         * @brief Actualiza una región del buffer (para buffers mutables).
         * @param data Puntero a los datos a copiar.
         * @param sizeBytes Tamaño en bytes a actualizar.
         * @param offset Offset dentro del buffer para actualizar.
         */
        virtual void update(const void* data, size_t sizeBytes, size_t offset = 0) = 0;

        /**
         * @brief Mapea el buffer para acceso directo CPU.
         * @param access Tipo de acceso deseado.
         * @param offset Offset inicial del mapeo.
         * @param sizeBytes Tamaño del área a mapear (0 para todo).
         * @return Puntero al área mapeada en memoria CPU.
         */
        virtual void* map(MapAccess access, size_t offset = 0, size_t sizeBytes = 0) = 0;

        /**
         * @brief Desmapea el buffer tras acceso CPU, sincronizando si es necesario.
         */
        virtual void unmap() = 0;

        /**
         * @brief Lee datos del buffer (solo si cpuReadable es true).
         * @param dst Puntero destino para copiar datos.
         * @param sizeBytes Tamaño a leer en bytes.
         * @param offset Offset inicial para lectura.
         */
        virtual void read(void* dst, size_t sizeBytes, size_t offset = 0) = 0;

        /**
         * @brief Tamaño total en bytes del buffer.
         */
        virtual size_t size() const = 0;

        /**
         * @brief Usa definido del buffer (Immutable, Dynamic, etc.).
         */
        virtual Usage usage() const = 0;

        /**
         * @brief Tipo del buffer (Vertex, Index, Uniform, Storage).
         */
        virtual Type bufferType() const = 0;

        /**
         * @brief Indica si el buffer es legible desde CPU.
         */
        virtual bool isCpuReadable() const = 0;

        /**
         * @brief Indica si el buffer es escribible desde CPU.
         */
        virtual bool isCpuWritable() const = 0;

        /**
         * @brief Indica si el buffer está actualmente mapeado.
         */
        virtual bool isMapped() const = 0;

        /**
         * @brief Retorna el puntero actual mapeado o nullptr si no está mapeado.
         */
        virtual void* mappedPointer() const = 0;
    };

} // namespace PGRenderCore
