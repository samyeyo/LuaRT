#pragma once

#define COBJMACROS
#include <mfapi.h>
#include <mfidl.h>
#include <mfobjects.h>
#include <mftransform.h>
#include <mferror.h>
#include <mfreadwrite.h>
#include "miniaudio.h"

#define HR(hr) if (FAILED(hr)) goto failed; 

typedef enum {WAV = 0, MP3, AAC, FLAC} EncoderType;
extern const char *AudioFormatNames[];

typedef struct {
    EncoderType         kind;
    ma_device           device;
    size_t              sampleSize;
    IMFSinkWriter       *writer;
    DWORD               stream;
    LONGLONG            frameStart;
    LONGLONG            frameDuration;
    IMFMediaType        *out;
    IMFMediaType        *in;
} Encoder;

Encoder *Encoder_Create(wchar_t *fname, EncoderType kind, int channels, int sampleRate, int bitrate, ma_device_id **id);
void    Encoder_Release(Encoder *encoder);