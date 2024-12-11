#include <Windows.h>
#include <xaudio2.h>
#include <stdlib.h>
#include "sound.h"

// MS chunk types
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

// MS XAudio2 RIFF-parsing code
static HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD* dwChunkSize, DWORD* dwChunkDataPosition);
static HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset);
static HRESULT LoadChunkFile(const char* filename, WAVEFORMATEXTENSIBLE* wfx, XAUDIO2_BUFFER* buffer);
static HRESULT PlayAudio(IXAudio2* pXAudio2, WAVEFORMATEX* wfx, XAUDIO2_BUFFER* buffer, int soundId);
static HRESULT StopAudio(IXAudio2* pXAudio2, int soundId);

typedef struct sound_source_t {
    const char* filename;
    WAVEFORMATEXTENSIBLE wfx;
    XAUDIO2_BUFFER buffer;
} SoundSource;

static struct sound_manager_t {
    SoundSource*    sounds;
    int32_t         maxSounds;

    // dx audio system
    IXAudio2SourceVoice** audios;
    IXAudio2* pXAudio2;
    IXAudio2MasteringVoice* pMasterVoice;
} _soundMgr = { NULL, 0, NULL, NULL, NULL };

/**
 * @brief allocate sound system resources
 * @return 
*/
bool soundInit(int32_t maxSounds) {
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        return false;
    }

    hr = XAudio2Create(&_soundMgr.pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr)) {
        return false;
    }

    hr = IXAudio2_CreateMasteringVoice(_soundMgr.pXAudio2, &_soundMgr.pMasterVoice,
        XAUDIO2_DEFAULT_CHANNELS,
        XAUDIO2_DEFAULT_SAMPLERATE,
        0,
        NULL,
        NULL,
        AudioCategory_GameEffects
    );
    if (FAILED(hr)) {
        IXAudio2_Release(_soundMgr.pXAudio2);
        _soundMgr.pXAudio2 = NULL;
        return false;
    }

    _soundMgr.sounds = malloc(maxSounds * sizeof(SoundSource));
    if(_soundMgr.sounds != NULL)
        ZeroMemory(_soundMgr.sounds, maxSounds * sizeof(SoundSource));
    _soundMgr.audios = malloc(maxSounds * sizeof(IXAudio2SourceVoice*));
    if(_soundMgr.audios != NULL)
        ZeroMemory(_soundMgr.audios, maxSounds * sizeof(IXAudio2SourceVoice*));
    _soundMgr.maxSounds = maxSounds;

    return true;
}

/**
 * @brief release sound system resources
 * @return 
*/
bool soundShutdown() {

    for (int32_t i = 0; i < _soundMgr.maxSounds; ++i)
    {
        StopAudio(_soundMgr.pXAudio2, i);

        SoundSource* sound = &_soundMgr.sounds[i];
        if (sound->filename != NULL)
        {
            soundUnload(i);
        }
    }
    free(_soundMgr.sounds);
    free(_soundMgr.audios);

    IXAudio2_Release(_soundMgr.pXAudio2);
    _soundMgr.pXAudio2 = NULL;

    return true;
}

/**
 * @brief Loads a clip from file into memory for playback
 * @param filename 
 * @return id which is a handle to the clip data
*/
int32_t soundLoad(const char* filename) {
    for (int32_t i = 0; i < _soundMgr.maxSounds; ++i)
    {
        if (_soundMgr.sounds[i].filename == NULL)
        {
            SoundSource* sound = &_soundMgr.sounds[i];
            sound->filename = filename;

            HRESULT hr = LoadChunkFile(filename, &sound->wfx, &sound->buffer);
            if (FAILED(hr)) {
                sound->filename = NULL;
                return SOUND_NOSOUND;
            }
            return i;
        }
    }

    return SOUND_NOSOUND;
}

/**
 * @brief Releases resources associated w/ a loaded clip
 * @param soundId 
*/
void soundUnload(int32_t soundId) {
    if (soundId == SOUND_NOSOUND)
        return;

    SoundSource* sound = &_soundMgr.sounds[soundId];
    if (sound->buffer.pAudioData != NULL) {
        free((BYTE*)sound->buffer.pAudioData);
        sound->buffer.pAudioData = NULL;

        sound->filename = NULL;
    }
}

/**
 * @brief Plays a clip loaded w/ LoadSound
 * @param soundId 
*/
void soundPlay(int32_t soundId) {
    if (soundId == SOUND_NOSOUND)
        return;

    SoundSource* sound = &_soundMgr.sounds[soundId];
    PlayAudio(_soundMgr.pXAudio2, (WAVEFORMATEX*)&sound->wfx, &sound->buffer, soundId);
}

void soundStop(int32_t soundId) {
    if (soundId == SOUND_NOSOUND)
        return;

    SoundSource* sound = &_soundMgr.sounds[soundId];
    StopAudio(_soundMgr.pXAudio2, soundId);
}

/**
 * @brief FROM: https://learn.microsoft.com/en-us/windows/win32/xaudio2/how-to--load-audio-data-files-in-xaudio2
*/
static HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD* dwChunkSize, DWORD* dwChunkDataPosition)
{
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());

    DWORD dwChunkType;
    DWORD dwChunkDataSize;
    DWORD dwRIFFDataSize = 0;
    DWORD dwFileType;
    DWORD bytesRead = 0;
    DWORD dwOffset = 0;

    while (hr == S_OK)
    {
        DWORD dwRead;
        if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
            hr = HRESULT_FROM_WIN32(GetLastError());

        switch (dwChunkType)
        {
        case fourccRIFF:
            dwRIFFDataSize = dwChunkDataSize;
            dwChunkDataSize = 4;
            if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
                hr = HRESULT_FROM_WIN32(GetLastError());
            break;

        default:
            if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
                return HRESULT_FROM_WIN32(GetLastError());
        }

        dwOffset += sizeof(DWORD) * 2;

        if (dwChunkType == fourcc)
        {
            *dwChunkSize = dwChunkDataSize;
            *dwChunkDataPosition = dwOffset;
            return S_OK;
        }

        dwOffset += dwChunkDataSize;

        if (bytesRead >= dwRIFFDataSize) return S_FALSE;

    }

    return S_OK;

}

/**
 * @brief FROM: https://learn.microsoft.com/en-us/windows/win32/xaudio2/how-to--load-audio-data-files-in-xaudio2
*/
static HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
{
    HRESULT hr = S_OK;
    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
        return HRESULT_FROM_WIN32(GetLastError());
    DWORD dwRead;
    if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
        hr = HRESULT_FROM_WIN32(GetLastError());
    return hr;
}

/**
 * @brief FROM: https://learn.microsoft.com/en-us/windows/win32/xaudio2/how-to--load-audio-data-files-in-xaudio2
*/
static HRESULT LoadChunkFile(const char* filename, WAVEFORMATEXTENSIBLE* wfx, XAUDIO2_BUFFER* buffer) {
    // Open the file
    HANDLE hFile = CreateFile(
        filename,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (INVALID_HANDLE_VALUE == hFile) {
        return S_FALSE;
    }

    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN)) {
        CloseHandle(hFile);
        return S_FALSE;
    }

    DWORD dwChunkSize;
    DWORD dwChunkPosition;
    //check the file type, should be fourccWAVE or 'XWMA'
    FindChunk(hFile, fourccRIFF, &dwChunkSize, &dwChunkPosition);
    DWORD filetype;
    ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
    if (filetype != fourccWAVE) {
        CloseHandle(hFile);
        return S_FALSE;
    }

    FindChunk(hFile, fourccFMT, &dwChunkSize, &dwChunkPosition);
    ReadChunkData(hFile, wfx, dwChunkSize, dwChunkPosition);

    //fill out the audio data buffer with the contents of the fourccDATA chunk
    FindChunk(hFile, fourccDATA, &dwChunkSize, &dwChunkPosition);
    BYTE* pDataBuffer = malloc(dwChunkSize * sizeof(BYTE));
    ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

    buffer->AudioBytes = dwChunkSize;  //size of the audio buffer in bytes
    buffer->pAudioData = pDataBuffer;  //buffer containing audio data
    buffer->Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

    CloseHandle(hFile);

    return S_OK;
}

/**
 * @brief FROM: https://learn.microsoft.com/en-us/windows/win32/xaudio2/how-to--load-audio-data-files-in-xaudio2
*/
static HRESULT PlayAudio(IXAudio2* pXAudio2, WAVEFORMATEX* wfx, XAUDIO2_BUFFER* buffer, int soundId) {
    HRESULT hr = S_OK;

    //if (_soundMgr.audios[soundId] == NULL)
    {
        IXAudio2SourceVoice* pSourceVoice;

        hr = IXAudio2_CreateSourceVoice(_soundMgr.pXAudio2, &pSourceVoice, wfx,
            0,
            XAUDIO2_DEFAULT_FREQ_RATIO,
            NULL,
            NULL,
            NULL);
        if (FAILED(hr)) {
            return hr;
        }

        hr = IXAudio2SourceVoice_SubmitSourceBuffer(pSourceVoice, buffer, NULL);
        if (FAILED(hr)) {
            return hr;
        }

        _soundMgr.audios[soundId] = pSourceVoice;
    }

    hr = IXAudio2SourceVoice_Start(_soundMgr.audios[soundId], 0, XAUDIO2_COMMIT_NOW);

    return hr;
}

static HRESULT StopAudio(IXAudio2* pXAudio2, int soundId) {
    HRESULT hr = S_FALSE;

    if (_soundMgr.audios[soundId] != NULL)
    {
        hr = IXAudio2SourceVoice_Stop(_soundMgr.audios[soundId], 0, XAUDIO2_COMMIT_NOW);
    }

    return hr;
}


