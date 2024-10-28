#include "encoder.h"

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment (lib, "wmcodecdspuuid.lib")

static const GUID *AudioFormats[] = { &MFAudioFormat_PCM , &MFAudioFormat_MP3, &MFAudioFormat_AAC };
const char *AudioFormatNames[] = { "wav", "mp3", "aac", NULL};
const int AudioFormatBitrates[] = { 0, 128, 96 };

static void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    Encoder * encoder = (Encoder *)pDevice->pUserData;
    IMFSample *pSample = NULL;
    IMFMediaBuffer *pBuffer = NULL;
    BYTE *pData = NULL;
    DWORD size = frameCount * encoder->sampleSize;

    HR(MFCreateMemoryBuffer(size, &pBuffer));
    HR(IMFMediaBuffer_Lock(pBuffer, &pData, NULL, NULL));
    memcpy(pData, pInput, size);
    HR(IMFMediaBuffer_Unlock(pBuffer));
    HR(IMFMediaBuffer_SetCurrentLength(pBuffer, size));
    HR(MFCreateSample(&pSample));
    HR(IMFSample_AddBuffer(pSample, pBuffer))
    if (encoder->kind != MP3) {
        HR(IMFSample_SetSampleTime(pSample, encoder->frameStart));
        HR(IMFSample_SetSampleDuration(pSample, encoder->frameDuration * frameCount));
        encoder->frameStart += encoder->frameDuration * frameCount;
    }
    HR(IMFSinkWriter_WriteSample(encoder->writer, encoder->stream, pSample));

failed:
    IMFSample_Release(pSample);
    IMFMediaBuffer_Release(pBuffer);
}

Encoder *Encoder_Create(wchar_t *fname, EncoderType kind, int channels, int sampleRate, int bitrate, ma_device_id **id) {
    ma_device_config deviceConfig;
    Encoder *encoder = calloc(1, sizeof(Encoder));
    ma_result result;
    IMFMediaType *pOutputType = NULL, *pInputType = NULL;
   
    if (!bitrate)
        bitrate = AudioFormatBitrates[kind];
    HR(MFCreateSinkWriterFromURL(fname, NULL, NULL, &encoder->writer));
    HR(MFCreateMediaType(&pOutputType));
    IMFMediaType_SetGUID(pOutputType, &MF_MT_MAJOR_TYPE, &MFMediaType_Audio);
    IMFMediaType_SetGUID(pOutputType, &MF_MT_SUBTYPE, AudioFormats[kind]);
    if (kind == MP3) {
        MPEGLAYER3WAVEFORMAT Format;
        Format.wID = MPEGLAYER3_ID_MPEG;
        Format.fdwFlags = MPEGLAYER3_FLAG_PADDING_OFF;
        Format.nBlockSize = (WORD)(144 * bitrate * 1000 / sampleRate);
        Format.nFramesPerBlock = 1;
        Format.nCodecDelay = 0;
        IMFMediaType_SetUINT32(pOutputType, &MF_MT_AUDIO_BLOCK_ALIGNMENT, 1);
        IMFMediaType_SetUINT32(pOutputType, &MF_MT_AUDIO_AVG_BYTES_PER_SECOND, bitrate * 1000 / 8);
        IMFMediaType_SetBlob(pOutputType, &MF_MT_USER_DATA, (const UINT8*)(&Format.wfx + 1), MPEGLAYER3_WFX_EXTRA_BYTES);
    } else {
        encoder->frameDuration = 10000000 / sampleRate;
        HR(IMFMediaType_SetUINT32(pOutputType, &MF_MT_AUDIO_BITS_PER_SAMPLE, 16));
        if (kind == WAV) {
            HR(IMFMediaType_SetUINT32(pOutputType, &MF_MT_AUDIO_BLOCK_ALIGNMENT, 2*channels));
            HR(IMFMediaType_SetUINT32(pOutputType, &MF_MT_AUDIO_AVG_BYTES_PER_SECOND, sampleRate*channels*sizeof(short)));
        } else
            HR(IMFMediaType_SetUINT32(pOutputType, &MF_MT_AUDIO_AVG_BYTES_PER_SECOND, bitrate * 1000 / 8));
    }
    IMFMediaType_SetUINT32(pOutputType, &MF_MT_AUDIO_SAMPLES_PER_SECOND, sampleRate);
    IMFMediaType_SetUINT32(pOutputType, &MF_MT_AUDIO_NUM_CHANNELS, channels);   
    HR(IMFSinkWriter_AddStream(encoder->writer, pOutputType, &encoder->stream));
    encoder->kind = kind;

    HR(MFCreateMediaType(&pInputType));
    IMFMediaType_SetGUID(pInputType, &MF_MT_MAJOR_TYPE, &MFMediaType_Audio);
    IMFMediaType_SetGUID(pInputType, &MF_MT_SUBTYPE, &MFAudioFormat_PCM);
    IMFMediaType_SetUINT32(pInputType, &MF_MT_AUDIO_BITS_PER_SAMPLE, sizeof(short)*8);
    IMFMediaType_SetUINT32(pInputType, &MF_MT_AUDIO_SAMPLES_PER_SECOND, sampleRate);
    IMFMediaType_SetUINT32(pInputType, &MF_MT_AUDIO_NUM_CHANNELS, channels);
    IMFMediaType_SetUINT32(pInputType, &MF_MT_AUDIO_BLOCK_ALIGNMENT, 4);
    IMFMediaType_SetUINT32(pInputType, &MF_MT_AUDIO_AVG_BYTES_PER_SECOND, 176400);
    encoder->sampleSize = channels * sizeof(short); 

    deviceConfig = ma_device_config_init(ma_device_type_capture);
    deviceConfig.capture.format = ma_format_s16;
    deviceConfig.capture.channels = channels;
    deviceConfig.sampleRate = sampleRate;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.capture.pDeviceID = *id;
    deviceConfig.pUserData = encoder;

    encoder->out = pOutputType;
    encoder->in = pInputType;
    if (ma_device_init(NULL, &deviceConfig, &encoder->device) == MA_SUCCESS) {
        *id = &encoder->device.capture.id;
        HR(IMFSinkWriter_SetInputMediaType(encoder->writer, encoder->stream, pInputType, NULL));  
        HR(IMFSinkWriter_BeginWriting(encoder->writer));
        if (ma_device_start(&encoder->device) == MA_SUCCESS)
            return encoder;
    }
failed:
    Encoder_Release(encoder);
    return NULL;
}

void Encoder_Release(Encoder *encoder) {
    ma_device_stop(&encoder->device);
    ma_device_uninit(&encoder->device);
    if (encoder->writer) {
        IMFSinkWriter_Flush(encoder->writer, encoder->stream);
        if (encoder->kind == WAV)
            IMFSinkWriter_Finalize(encoder->writer);
        IMFSinkWriter_Release(encoder->writer);
        IMFMediaType_Release(encoder->in);
        IMFMediaType_Release(encoder->out);
    }
    free(encoder);
}