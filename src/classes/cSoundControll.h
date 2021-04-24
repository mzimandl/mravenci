#pragma once
#include <deque>
#include <SDL2/SDL.h>



//Maximum recording time
const int MAX_RECORDING_SECONDS = 5;

//Maximum recording time plus padding
const int RECORDING_BUFFER_SECONDS = MAX_RECORDING_SECONDS + 1;

const int MAX_AVERAGE_QUEUE_SIZE = 10;


class SoundControll {
    private:
        SDL_AudioDeviceID recordingDeviceId;
        SDL_AudioSpec gReceivedRecordingSpec;

        Uint8* gRecordingBuffer = NULL;
        Uint32 gBufferByteSize = 0;
        Uint32 gBufferByteMaxPosition = 0;

        std::deque<Sint8 > lastLevels;
        Uint32 lastLevel = 0;
        Uint32 averageLevel = 0;

    public:
        SoundControll();
        ~SoundControll();

        void checkAudio();
        int isLoudSound();

};
