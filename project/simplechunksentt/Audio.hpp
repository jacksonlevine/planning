#include <AL/al.h>
#include <AL/alc.h>
#include "glm/glm.hpp"
class AudioWrapper {
public:
	ALCdevice* device;
	ALCcontext* context;
	ALfloat listenerPosition[3];
	ALfloat listenerOrientation[4];
	ALfloat listenerVelocity[3];
	AudioWrapper();
	void makeContextCurrent();
	void setAudioPosition(glm::vec3 pos, glm::vec4 orient, glm::vec3 vel);
};

AudioWrapper::AudioWrapper() : 
	device(alcOpenDevice(nullptr)),
	listenerPosition{ 0.0f,0.0f,0.0f },
	listenerOrientation{ 0.0f,0.0f,0.0f,0.0f },
	listenerVelocity{ 0.0f,0.0f,0.0f }
{
	this->context = alcCreateContext(device, nullptr);
}

void AudioWrapper::makeContextCurrent()
{
	alcMakeContextCurrent(this->context);
}

void AudioWrapper::setAudioPosition(glm::vec3 pos, glm::vec4 orient, glm::vec3 vel)
{
	this->listenerPosition[0] = pos.x;
	this->listenerPosition[1] = pos.y;
	this->listenerPosition[2] = pos.z;

	this->listenerOrientation[0] = orient.r;
	this->listenerOrientation[1] = orient.g;
	this->listenerOrientation[2] = orient.b;
	this->listenerOrientation[3] = orient.a;

	this->listenerVelocity[0] = vel.x;
	this->listenerVelocity[1] = vel.y;
	this->listenerVelocity[2] = vel.z;
	alListenerfv(AL_POSITION, this->listenerPosition);
	alListenerfv(AL_ORIENTATION, this->listenerOrientation);
	alListenerfv(AL_VELOCITY, this->listenerVelocity);
}

