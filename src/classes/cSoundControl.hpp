#pragma once
#include <queue>
#include <SDL2/SDL.h>



//Maximum recording time
const int MAX_RECORDING_SECONDS = 5;

//Maximum recording time plus padding
const int RECORDING_BUFFER_SECONDS = MAX_RECORDING_SECONDS + 1;

const int MAX_AVERAGE_QUEUE_SIZE = 10;


class SoundControl {
    private:
        SDL_AudioDeviceID recordingDeviceId;
        SDL_AudioSpec gReceivedRecordingSpec;

        Uint8* gRecordingBuffer;
        Uint32 gBufferByteSize;
        Uint32 gBufferByteMaxPosition;

        std::queue<Sint8 > lastLevels;
        Uint32 sumLevels;
        Uint32 averageLevel;
        Uint32 lastLevel;

    public:
        SoundControl();
        ~SoundControl();

        void checkAudio();
        int avgVolumeChange();
        int lastVolume();

};
