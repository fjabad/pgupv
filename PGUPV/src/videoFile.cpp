
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

#include "log.h"
#include "videoFile.h"

using media::VideoFile;

VideoFile::VideoFile(const std::filesystem::path &filepath) : filepath(filepath) {

	// Open video file
	if (avformat_open_input(&pFormatCtx, filepath.u8string().c_str(), NULL, NULL) != 0)
		ERRT("No se ha podido abrir el fichero " + filepath.string());

	if (!searchAudioVideoStreams())
		ERRT(filepath.string() + ": No se reconoce el formato del fichero o no contiene una pista de vídeo");

	prepareForReading();
    
    INFO("VideoFile creado (" + filepath.string() + ") " + std::to_string(reinterpret_cast<uint64_t>(this)));
}

VideoFile::~VideoFile() {
    INFO("VideoFile destruido (" + filepath.string() + ") " + std::to_string(reinterpret_cast<uint64_t>(this)));
}

void VideoFile::rewind() {

	int err = av_seek_frame(pFormatCtx, firstVideoStream, pFormatCtx->start_time, AVSEEK_FLAG_BACKWARD); 
	if (err < 0) {
		ERR("No se ha podido saltar al principio del vídeo " + filepath.string() + " (" + ffmpegError(err) + ")");
	}
	else {
		avcodec_flush_buffers(pCodecCtx);
	}
}
