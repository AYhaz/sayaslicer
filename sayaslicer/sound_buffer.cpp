#include <filesystem>
#include <sndfile.h>
#include <stdexcept>
#include <cstring>

#include "sound_buffer.hpp"

struct callbackData {
	unsigned long long samplePos;
	unsigned long long length;
	unsigned long long currentPos;
	SoundBuffer *sound;
};

int callback(const void* input, void* output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
	float* out = (float*)output;
	struct callbackData* callbackData = (struct callbackData*)userData;
	unsigned int channelCount = callbackData->sound->getChannelCount();

	memset(out, 0, sizeof(float) * frameCount * channelCount);
	auto& buffer = callbackData->sound->getSamples();

	if (callbackData->currentPos >= callbackData->length) {
		return paComplete;
	}

	unsigned long long numSamplesToCopy = frameCount * channelCount;
	bool complete = false;
	if (callbackData->currentPos + numSamplesToCopy >= callbackData->length) {
		numSamplesToCopy = callbackData->length - callbackData->currentPos;
		complete = true;
	}

	memcpy(out, &buffer[callbackData->samplePos + callbackData->currentPos], numSamplesToCopy * sizeof(float));

	if (complete) {
		return paComplete;
	}
	callbackData->currentPos += numSamplesToCopy;
	return paContinue;
}

void streamFinishedCallback(void* userData) {
	free(userData);
}

SoundBuffer::~SoundBuffer() {
	if (stream != NULL)
		Pa_CloseStream(stream);
}

unsigned int SoundBuffer::getChannelCount() {
	return this->channelCount;
}

unsigned int SoundBuffer::getSampleRate() {
	return this->sampleRate;
}

float SoundBuffer::getDuration() {
	return this->duration;
}

std::vector<float>& SoundBuffer::getSamples() {
	return this->buffer;
}

unsigned long long SoundBuffer::getSampleCount() {
	return this->buffer.size();
}

int SoundBuffer::loadFromFile(std::string path) {
	auto fp = std::filesystem::u8path(path);
	if (!std::filesystem::exists(fp))
		return -1;

	SF_INFO info = { 0 };

#if _WIN32
	SNDFILE *file = sf_wchar_open(fp.wstring().c_str(), SFM_READ, &info);
#else
	SNDFILE *file = sf_open(fp.u8string().c_str(), SFM_READ, &info);
#endif
	if (sf_error(file) != SF_ERR_NO_ERROR)
		return -2;

	if (this->stream != NULL)
		Pa_CloseStream(stream);

	this->channelCount = info.channels;
	this->sampleRate = info.samplerate;
	this->duration = (float)info.frames / (float)info.samplerate;

	this->buffer.clear();
	this->buffer.shrink_to_fit();
	this->buffer.reserve(info.frames * info.channels);

	size_t totalFramesRead = 0;
	float* tmp = (float*)malloc(sizeof(float) * 4096 * info.channels);

	while (totalFramesRead < info.frames) {
		size_t framesRead = sf_readf_float(file, tmp, 4096);
		if (framesRead == 0)
			break;
		totalFramesRead += framesRead;
		for (size_t i = 0; i < framesRead * info.channels; i++)
			this->buffer.push_back(tmp[i]);
	}

	free(tmp);
	sf_close(file);

	return 0;
}

bool SoundBuffer::writeFile(std::filesystem::path path, unsigned int sampleRate, unsigned int channelCount, float *buffer, size_t bufSize) {
	SF_INFO info = { 0 };
	info.samplerate = sampleRate;
	info.channels = channelCount;
	info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

#if _WIN32
	SNDFILE* file = sf_wchar_open(path.wstring().c_str(), SFM_WRITE, &info);
#else
	SNDFILE* file = sf_open(path.u8string().c_str(), SFM_WRITE, &info);
#endif
	if (sf_error(file) != SF_ERR_NO_ERROR)
		return false;

	if (sf_write_float(file, buffer, bufSize) != bufSize) {
		// Buffer wasn't entirely written and I can't bother to implement the loop yet (it shouldn't happen)
		return false;
	}
	sf_close(file);

	return true;
}

void SoundBuffer::play(unsigned long long samplePos, unsigned long long length) {
	if (stream != NULL)
		Pa_CloseStream(stream);

	struct callbackData* data = (struct callbackData*)calloc(1, sizeof(struct callbackData));
	if (!data)
		throw std::runtime_error("Cannot allocate PortAudio user data (this should not happen)");
	data->samplePos = samplePos;
	data->length = length;
	data->currentPos = 0;
	data->sound = this;

	if (Pa_OpenDefaultStream(&stream, 0, channelCount, paFloat32, sampleRate, 512, callback, data) != paNoError) {
		throw std::runtime_error("Error opening PortAudio stream");
	}

	if (Pa_SetStreamFinishedCallback(stream, streamFinishedCallback) != paNoError) {
		throw std::runtime_error("Error setting PortAudio stream finished callback");
	}

	if (Pa_StartStream(stream) != paNoError) {
		throw std::runtime_error("Error starting PortAudio stream");
	}
}

bool SoundBuffer::isPlaying() {
	return Pa_IsStreamActive(stream);
}

void SoundBuffer::stop() {
	Pa_AbortStream(stream);
}