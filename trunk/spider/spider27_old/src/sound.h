#ifndef __SOUND_H__
#define __SOUND_H__

typedef struct
{
    char    tag[4];
    long    length;
} riff_tag;

typedef struct
{
    char    riff[4];
    long    size;
    char    wave[4];
} riff_hdr;

typedef struct
{
    short   formatTag;
    short   channels;
    long    samplesPerSec;
    long    avgBytesPerSec;
    short   blockAlign;
    short   bitsPerSample;
} wave_hdr;

struct sound
{
	bool play(string filename);
};

void* Player(void* arg);

#endif
