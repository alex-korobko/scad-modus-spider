#include "global.h"

string riff_Id("RIFF");
string wave_Id("WAVE");

sound::sound()
{
/*
	card = 0;
	dev = 0;
	pcmHandle = NULL;
*/
}

sound::~sound()
{
/*
	if (pcmHandle)
	    	snd_pcm_plugin_flush(pcmHandle, SND_PCM_CHANNEL_PLAYBACK);

	if (mixHandle)
		snd_mixer_close(mixHandle);
  
	if (pcmHandle)
		snd_pcm_close(pcmHandle);

	card = 0;
	dev = 0;
	pcmHandle = NULL;
*/
}

bool sound::init()
{
/*
 	if (snd_pcm_open_preferred(&pcmHandle, &card, &dev, SND_PCM_OPEN_PLAYBACK) < 0)
 		return 0;

    	if (snd_pcm_plugin_set_disable(pcmHandle, PLUGIN_DISABLE_MMAP) < 0)
		return 0;

	memset(&pcmInfo, 0, sizeof(pcmInfo));
    	pcmInfo.channel = SND_PCM_CHANNEL_PLAYBACK;
    	if (snd_pcm_plugin_info(pcmHandle, &pcmInfo) < 0)
		return 0;
	
	memset (&pcmParams, 0, sizeof (pcmParams));

	pcmParams.mode = SND_PCM_MODE_BLOCK;
    	pcmParams.channel = SND_PCM_CHANNEL_PLAYBACK;
    	pcmParams.start_mode = SND_PCM_START_FULL;
    	pcmParams.stop_mode = SND_PCM_STOP_STOP;

    	pcmParams.buf.block.frag_size = pcmInfo.max_fragment_size;
    	pcmParams.buf.block.frags_max = 1;
    	pcmParams.buf.block.frags_min = 1;

    	pcmParams.format.interleave = 1;
    	pcmParams.format.rate = 22050;
    	pcmParams.format.voices = 2;
	pcmParams.format.format = SND_PCM_SFMT_S16_LE;

    	if (snd_pcm_plugin_params(pcmHandle, &pcmParams) < 0)
		return 0;

	if (snd_pcm_plugin_prepare(pcmHandle, SND_PCM_CHANNEL_PLAYBACK) < 0)
       	return 0;

    	memset (&pcmSetup, 0, sizeof(pcmSetup));
    	memset (&mixGroup, 0, sizeof (mixGroup));
    	pcmSetup.channel = SND_PCM_CHANNEL_PLAYBACK;
    	pcmSetup.mixer_gid = &mixGroup.gid;
	if (snd_pcm_plugin_setup (pcmHandle, &pcmSetup) < 0)
		return 0;

	if (mixGroup.gid.name[0] == 0)
		return 0;

	if (snd_mixer_open(&mixHandle, card, pcmSetup.mixer_device) < 0)
		return 0;
*/
	return true;	
}

bool sound::play(string name)
{
/*
	pthread_attr_t      attr;
	

	filename = name;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	if (pthread_create(NULL, &attr, &Player, this) != EOK)
	{
		printf("Error: start thread\n");
		return 0;
	}
*/
    	return true;
}

static bool find_tag (FILE * fp, string tag)
{
/*
    	int     	retVal = 0;
    	RiffTag tagBfr = {"", 0};

    	// Keep reading until we find the tag or hit the EOF.
    	while(fread((unsigned char*)&tagBfr, sizeof(tagBfr), 1, fp))
    	{
        	// If this is our tag, set the length and break.    
        	if (strncmp(tag, tagBfr.tag, sizeof(tagBfr.tag)) == 0)
        	{
            	retVal = tagBfr.length;
            	break;
        	}

        	// Skip ahead the specified number of bytes in the stream           
        	fseek(fp, tagBfr.length, SEEK_CUR);
    	}

    	// Return the result of our operation
    	return retVal;
  */
  return true;
}

static bool check_hdr(FILE * fp)
{
/*
    	RiffHdr riffHdr = {"", 0};

    	// Read the header and, if successful, play the file
    	// file or WAVE file.
    	if (fread((unsigned char*)&riffHdr, sizeof(RiffHdr), 1, fp) == 0)
        	return 0;

    	if (strncmp (riffHdr.riff, RiffId, strlen(RiffId)) || strncmp(riffHdr.wave, WaveId, strlen(WaveId)))
        	return -1;
*/

   	return true;
}

void* Player(void* arg)
{
/*
	FILE*		fd;
	WaveHdr	wavHdr;
	int		samples;
    	char   *sampleBfr;
    	int     	n, N = 0;
    	fd_set  rfds, wfds;
	int		maxDesc, bufSize;
	Sound* 	device = NULL;

	if (!arg)
		return 0;
	
	device = (Sound*)arg;
	
    	if ((fd = fopen(device->filename, "r")) == 0)
		return 0;

    	if (CheckHdr(fd) == -1)
        	return 0;

	samples = FindTag(fd, "fmt ");
    	fread(&wavHdr, sizeof(wavHdr), 1, fd);
    	fseek(fd, (samples - sizeof(WaveHdr)), SEEK_CUR);
 	samples = FindTag(fd, "data");

	bufSize = device->pcmSetup.buf.block.frag_size;
    	sampleBfr = (char*)malloc(bufSize);
	FD_ZERO (&rfds);
    	FD_ZERO (&wfds);
    	n = 1;
    	while (N < samples && n > 0)
    	{
        	FD_SET(snd_mixer_file_descriptor(device->mixHandle), &rfds);
        	FD_SET(snd_pcm_file_descriptor(device->pcmHandle, SND_PCM_CHANNEL_PLAYBACK), &wfds);

	    	maxDesc = __max(snd_mixer_file_descriptor(device->mixHandle),
            snd_pcm_file_descriptor(device->pcmHandle, SND_PCM_CHANNEL_PLAYBACK));

        	if (select(maxDesc + 1, &rfds, &wfds, NULL, NULL) == -1)
            	return 0;

        	if (FD_ISSET(snd_mixer_file_descriptor(device->mixHandle), &rfds))
        	{
            	snd_mixer_callbacks_t callbacks = {0, 0, 0, 0};
			snd_mixer_read(device->mixHandle, &callbacks);
        	}

        	if (FD_ISSET(snd_pcm_file_descriptor(device->pcmHandle, SND_PCM_CHANNEL_PLAYBACK), &wfds))
        	{
            	snd_pcm_channel_status_t status;
            	int written = 0;

            	if ((n = fread(sampleBfr, 1, __min(samples - N, bufSize), fd)) <= 0)
                	continue;
            	written = snd_pcm_plugin_write(device->pcmHandle, sampleBfr, n);
            	if (written < n)
            	{
                	memset(&status, 0, sizeof(status));
                	if (snd_pcm_plugin_status(device->pcmHandle, &status) < 0)
                		return 0;

                	if (status.status == SND_PCM_STATUS_READY ||
                    		status.status == SND_PCM_STATUS_UNDERRUN)
                	{
                   	 	if (snd_pcm_plugin_prepare(device->pcmHandle, SND_PCM_CHANNEL_PLAYBACK) < 0)
                    			return 0;
                	}
                	if (written < 0)
                    		written = 0;
                	written += snd_pcm_plugin_write(device->pcmHandle, sampleBfr + written, n - written);
            	}
            	N += written;
        	}
    }

    fclose(fd);
*/    
    return 0;
}
