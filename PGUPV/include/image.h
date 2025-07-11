#ifndef _IMAGE_H
#define _IMAGE_H

#include <string>
#include <GL/glew.h>
#include <memory>
#include <filesystem>
#include "common.h"
#include "geoTiff.h"


namespace PGUPV {
	/**

	\class Image
	Esta clase representa una imagen, que normalmente se carga desde un fichero.
	\warning No confundir con las Images de OpenGL que se usan para escribir en
	ellas desde un shader (se usa glBindImageTexture desde CPU para definirlas)

	*/

	class Image {
	public:
		Image(const Image&) = delete;
		Image& operator=(const Image&) = delete;

		// Carga la imagen desde el fichero indicado
		explicit Image(const std::filesystem::path & filename);
		// Crea una imagen con el tamaño indicado. Opcionalmente, copia la información
		// apuntada por data
		Image(uint width, uint height, uint bpp, void* data = NULL);
		Image(Image&& other) noexcept;
		~Image();
		// Carga la imagen en el fichero indicado. La imagen que contenía este objeto
		// se destruye (incluyendo el puntero que devolvería Image::getPixels)
		bool load(std::string filename);
		/**
	  Guarda la imagen en el fichero indicado. Se puede guardar un frame de una animación
	  o una cara de un cubo
	  \param filename Nombre del fichero resultante
	  \param frame cara o frame a guardar
	  */
		bool save(const std::string& filename, uint frame = 0);

		/**
		Guarda en el fichero indicado la imagen definida por el resto de parámetros
		*/
		static bool save(const std::string& filename, uint width, uint height, uint bpp, uint8_t* bytes);
		static bool saveHDR(const std::string& filename, uint32_t width, uint32_t height, uint32_t bpp, const float* bytes);

		// Ancho
		uint getWidth() const;
		// Alto
		uint getHeight() const;
		// Bits por píxel
		uint getBPP() const;
		// Número de caras (6 si la imagen es un mapa de entorno cúbico, 1 si es una
		// imagen normal)
		uint getNumFaces() const;
		/**
		\return Número de frames de la animación
		*/
		uint getAnimationFrames() const;
		// Devuelve el tamaño en bytes de una fila de píxeles de la image
		uint getStride() const;

		// Invierte la imagen verticalmente
		void flipV();
		/**
		 Devuelve un nuevo objeto Image, con el frame indicado.
		 \param frame Número de frame a extraer (de 0 a getAnimationFrames() - 1)
		 */
		Image* extractFrame(uint frame) const;

		/**
		Compara la similitud de otra imagen con la actual. Se considera que dos
		imágenes son iguales si la máxima diferencia entre cualquier canal de color de
		cualquier píxel visible es menor o igual a la especificada. Por defecto deben
		ser exactamente iguales.
		\param other La otra imagen
		\param maxDifference Máxima diferencia en cualquier canal de color (0-255)
		\return true, si la diferencia entre cualquier par de píxeles es menor
		o igual que la indicada
		*/
		bool equals(Image& other, uint maxDifference = 0) const;

		/**
		Devuelve una nueva imagen con la diferencia entre this y la imagen
		proporcionada. Las imágenes deben ser del mismo tamaño.
		Se devuelve una imagen RGBA, donde cada pixel contiene la diferencia entre los
		dos píxeles originales por canal de color.
		\param other La otra imagen
		\param ignoreAlpha si true, ignora las diferencias en el canal alfa, y la imagen resultante
		  no tendrá canal alfa
		\return Una nueva imagen con la diferencia entre ambas.
		*/
		Image* difference(Image& other, bool ignoreAlpha = true) const;

		/**
		Devuelve un puntero al píxel indicado. El origen de la imagen está en la
		esquina inferior izquierda.

		\param x coordenada x del píxel
		\param y coordenada y del píxel
		\param layer capa a devolver (en caso de que la imagen sea un mapa cúbico, o
		una animación)
		\return un puntero a los datos del píxel
		*/
		void* getPixels(uint x = 0, uint y = 0, uint layer = 0) const;

		/**
		\return la constante de GL que describe el contenido de la imagen (GL_RED, GL_RG,
		  GL_RGB o GL_RGBA)
		*/
		GLenum getGLFormatType() const;

		/**
		\return la constante de GL que describe el tipo de datos básico de los componentes de la
		imagen (GL_UNSIGNED_BYTE, GL_FLOAT, etc)
		*/
		GLenum getGLPixelBaseType() const;
		/**
		\return la constante de GL que mejor describe el contenido de la imagen (puede ser de componentes
		tipo float para HDR)
		*/
		GLenum getSuggestedGLInternalFormatType() const;

		/**
		Convierte una imagen de entrada que tiene 8 bpp en una imagen en escala de grises con 24 bits por pixel.
		Para cada pixel replica el valor original para RGB.
		*/
		static Image convert8BPPGrayTo24BPPGray(const Image& src);

		/**
		\return Información sobre la versión de la biblioteca de carga de imágenes utilizada
		*/
		static const std::string getLibraryInfo();

		/**
		 * @brief If the loaded image is a GeoTIFF, returns its metadata
		 * @return 
		*/
		std::optional<GeoTiffMetadata> getGeoTiffMetadata() const;
	private:
		class ImageImpl;
		std::unique_ptr<ImageImpl> impl;

#ifdef PG_TESTING
		void swapRB(uint frame) const;
		friend class DummyImageTestClass_TestSwapRB24BPP_Test;
		friend class DummyImageTestClass_TestSwapRB32BPP_Test;
#endif
	};
};

#endif
