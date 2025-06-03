#pragma once

#include <filesystem>
#include "media.h"

namespace media {
	class VideoFile : public Media {
	public:
		VideoFile(const std::filesystem::path &filepath);
        ~VideoFile();
		/**
		Salta al principio del vídeo
		*/
		void rewind();
	private:
		std::filesystem::path filepath;
	};
};