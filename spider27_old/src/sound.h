#ifndef __SOUND_H__
#define __SOUND_H__

typedef struct
{
    char    tag[4];
    long    length;
} RiffTag;

typedef struct
{
    char    riff[4];
    long    size;
    char    wave[4];
} RiffHdr;

typedef struct
{
    short   formatTag;
    short   channels;
    long    samplesPerSec;
    long    avgBytesPerSec;
    short   blockAlign;
    short   bitsPerSample;
} WaveHdr;

class sound
{
protected:
/*
	snd_pcm_t 	*pcmHandle;
    	int     			card;
	int     			dev;
	snd_pcm_channel_info_t 	pcmInfo;
	snd_mixer_t 					*mixHandle;
    	snd_mixer_group_t 			mixGroup;
    	snd_pcm_channel_params_t 	pcmParams;
    	snd_pcm_channel_setup_t 	pcmSetup;
	char*			filename;
*/
public:
	sound();
	~sound();
	bool init();
	bool play(string filename);
	friend void* Player(void* arg);
};

#endif
