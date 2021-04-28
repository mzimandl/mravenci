#include <iostream>
#include <SDL2/SDL.h>

#include "cSoundControl.h"



SoundControl::SoundControl() :
gBufferByteSize(0), gBufferByteMaxPosition(0), sumLevels(0), lastLevel(0), averageLevel(0)
{

    //Get capture device count
    if (SDL_GetNumAudioDevices( SDL_TRUE ) >= 1) {
        SDL_AudioSpec desiredRecordingSpec;
        SDL_zero(desiredRecordingSpec);
        desiredRecordingSpec.freq = 22050;
        desiredRecordingSpec.format = AUDIO_U8;
        desiredRecordingSpec.channels = 1;
        desiredRecordingSpec.samples = 512;
        desiredRecordingSpec.callback = NULL;

        recordingDeviceId = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(0, SDL_TRUE), SDL_TRUE, &desiredRecordingSpec, &gReceivedRecordingSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);

        int bytesPerSample = gReceivedRecordingSpec.channels * (SDL_AUDIO_BITSIZE( gReceivedRecordingSpec.format ) / 8);
        int bytesPerSecond = gReceivedRecordingSpec.freq * bytesPerSample;

        //Calculate buffer size
        gBufferByteSize = RECORDING_BUFFER_SECONDS * bytesPerSecond;

        //Calculate max buffer use
        gBufferByteMaxPosition = MAX_RECORDING_SECONDS * bytesPerSecond;

        //Allocate and initialize byte buffer
        gRecordingBuffer = new Uint8[gBufferByteSize];
        memset(gRecordingBuffer, 0, gBufferByteSize);

        SDL_PauseAudioDevice(recordingDeviceId, SDL_FALSE);
    }
}

SoundControl::~SoundControl() {
    if( gRecordingBuffer != NULL ) {
        SDL_PauseAudioDevice(recordingDeviceId, SDL_TRUE);

        delete[] gRecordingBuffer;
        gRecordingBuffer = NULL;
    }
}

void SoundControl::checkAudio() {
    Uint32 coppiedSize = SDL_DequeueAudio(recordingDeviceId, gRecordingBuffer, gBufferByteSize);
    if (coppiedSize) {
        lastLevel = 0;
        for (int i=0; i<coppiedSize; i++) {
            lastLevel += abs((int)(gRecordingBuffer[i]) - 128);
        }
        lastLevel /= coppiedSize;

        sumLevels += lastLevel;
        if (lastLevels.size() == MAX_AVERAGE_QUEUE_SIZE) {
            sumLevels -= lastLevels.front();
            lastLevels.pop();
        }
        lastLevels.push(lastLevel);
        averageLevel = sumLevels/lastLevels.size();
    }
}

int SoundControl::avgLevelVariation() { 
    return lastLevel-averageLevel;
}
