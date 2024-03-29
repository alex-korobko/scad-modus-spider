#include "global.h"

string riff_Id("RIFF");
string wave_Id("WAVE");


bool sound::play(string name)
{
	pthread_attr_t      attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	char* file_name =  new char[name.size()+1];
	strcpy(file_name, name.c_str());
	if (pthread_create(NULL, &attr, &Player, file_name) != EOK)
	{
		g_system_settings.sys_message(system_settings::ERROR_MSG,  string("sound::play can`t start thread"));			
		return false;
	}
    	return true;
}

static long find_tag (FILE * fp, string tag)
{
    	riff_tag tag_bfr;

    	// Keep reading until we find the tag or hit the EOF.
    	while(fread(&tag_bfr, sizeof(tag_bfr), 1, fp))
    	{
        	// If this is our tag, set the length and return.    
        	if (tag.compare(0, tag.size(), &tag_bfr.tag[0], __min(strlen(&tag_bfr.tag[0]), tag.size())) == 0)
        	{
            	return tag_bfr.length;
        	}

        	// Skip ahead the specified number of bytes in the stream           
        	fseek(fp, tag_bfr.length, SEEK_CUR);
    	}
	return 0;
}

static bool check_hdr(FILE * fp)
{
    	riff_hdr riff_header = {"", 0};

    	// Read the header and, if successful, play the file
    	// file or WAVE file.
    	if (fread(&riff_header, sizeof(riff_header), 1, fp) == 0)
    		{
		g_system_settings.sys_message(system_settings::ERROR_MSG,  string("sound.cpp@check_hdr(FILE * fp)  fread((unsigned char*)&riff_header, sizeof(riff_hdr), 1, fp) == 0"));	    		
        	return false;
        	}
        	
    	if (riff_Id.compare(0,  riff_Id.size(), &riff_header.riff[0], __min(strlen(&riff_header.riff[0]), riff_Id.size()) )  != 0 ||  wave_Id.compare(0, wave_Id.size(), &riff_header.wave[0], __min(strlen(&riff_header.wave[0]), wave_Id.size()) ) != 0)
    	 {
        	return false;
      }

   	return true;
}

void* Player(void* arg)
{
	snd_pcm_t 	*pcmHandle= NULL;
    	int     			card;
	int     			dev;
	snd_pcm_channel_info_t 	pcmInfo;
	snd_mixer_t 					*mixHandle;
    	snd_mixer_group_t 			mixGroup;
    	snd_pcm_channel_params_t 	pcmParams;
    	snd_pcm_channel_setup_t 	pcmSetup;
    	
	string filename;
	FILE*		fd;
	wave_hdr	wavHdr;
	long		samples;
    	vector<byte> sampleBfr;
    	int     	n, N = 0;
    	fd_set  rfds, wfds;
	int		maxDesc, bufSize;

	if (arg==NULL)
		return 0;
		
	filename=static_cast<char*>(arg);
	delete(static_cast<char*>(arg));

 	if (snd_pcm_open_preferred(&pcmHandle, &card, &dev, SND_PCM_OPEN_PLAYBACK) < 0)
 		{
		g_system_settings.sys_message(system_settings::ERROR_MSG,  string("sound.cpp@Player snd_pcm_open_preferred"));
 		return NULL;
 		}

    	if (snd_pcm_plugin_set_disable(pcmHandle, PLUGIN_DISABLE_MMAP) < 0)
    		{
		g_system_settings.sys_message(system_settings::ERROR_MSG,  string("sound.cpp@Player snd_pcm_plugin_set_disable"));		    		
		return NULL;
		}

	memset(&pcmInfo, 0, sizeof(pcmInfo));
    	pcmInfo.channel = SND_PCM_CHANNEL_PLAYBACK;
    	if (snd_pcm_plugin_info(pcmHandle, &pcmInfo) < 0)
    	 {
		g_system_settings.sys_message(system_settings::ERROR_MSG,  string("sound.cpp@Player snd_pcm_plugin_info"));
		return NULL;
	 }
	
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
    		{
		g_system_settings.sys_message(system_settings::ERROR_MSG,  string("sound.cpp@Player snd_pcm_plugin_params"));	
		return NULL;
		}

	if (snd_pcm_plugin_prepare(pcmHandle, SND_PCM_CHANNEL_PLAYBACK) < 0)
	   {
		g_system_settings.sys_message(system_settings::ERROR_MSG,  string("sound.cpp@Player snd_pcm_plugin_prepare"));	
       	return NULL;
       	}

    	memset (&pcmSetup, 0, sizeof(pcmSetup));
    	memset (&mixGroup, 0, sizeof (mixGroup));
    	pcmSetup.channel = SND_PCM_CHANNEL_PLAYBACK;
    	pcmSetup.mixer_gid = &mixGroup.gid;
	if (snd_pcm_plugin_setup (pcmHandle, &pcmSetup) < 0)
	   {
		g_system_settings.sys_message(system_settings::ERROR_MSG,  string("sound.cpp@Player snd_pcm_plugin_setup"));	
		return NULL;
		}

	if (mixGroup.gid.name[0] == 0)
		{
		g_system_settings.sys_message(system_settings::ERROR_MSG,  string("sound.cpp@Player mixGroup.gid.name[0] == 0"));	
		return NULL;
		}

	if (snd_mixer_open(&mixHandle, card, pcmSetup.mixer_device) < 0)
		{
		g_system_settings.sys_message(system_settings::ERROR_MSG,  string("sound.cpp@Player snd_mixer_open"));	
		return NULL;
		}
	
    	if ((fd = fopen(filename.c_str(), "r")) == 0)
    		{
    		string mess("sound.cpp@Player Not found ");
    		mess+=filename;
	 	g_system_settings.sys_message(system_settings::ERROR_MSG,  mess);	   		    		

		return NULL;
		}

    	if (check_hdr(fd) <=0)
    		{
    		string mess("sound.cpp@Player Bad file ");
    		mess+=filename;
    		mess+=" header";
	 	g_system_settings.sys_message(system_settings::ERROR_MSG,  mess);	   		    		
        	return NULL;
        	}

	samples = find_tag(fd, "fmt");
    	fread(&wavHdr, sizeof(wavHdr), 1, fd);
    	fseek(fd, (samples - sizeof(wave_hdr)), SEEK_CUR);
 	samples = find_tag(fd, "data");

	bufSize = __min (pcmSetup.buf.block.frag_size, samples);
    	sampleBfr.resize(bufSize);
	FD_ZERO (&rfds);
    	FD_ZERO (&wfds);
    	n = 1;
/*    	
    	printf ("Samples %d \n", samples);
    printf ("Mixer gid %d \n",	pcmSetup.mixer_gid);
    printf ("Format %s  ==%d== \n", snd_pcm_get_format_name (pcmSetup.format.format), pcmSetup.format.format);
    printf ("Frag Size %d \n", pcmSetup.buf.block.frag_size);
    printf ("Rate %d \n", pcmSetup.format.rate);
 */ 	
    	
    	while (N < samples && n > 0)
    	{
        	FD_SET(snd_mixer_file_descriptor(mixHandle), &rfds);
        	FD_SET(snd_pcm_file_descriptor(pcmHandle, SND_PCM_CHANNEL_PLAYBACK), &wfds);

	    	maxDesc = __max(snd_mixer_file_descriptor(mixHandle),
            snd_pcm_file_descriptor(pcmHandle, SND_PCM_CHANNEL_PLAYBACK));

        	if (select(maxDesc + 1, &rfds, &wfds, NULL, NULL) == -1)
            	return 0;

        	if (FD_ISSET(snd_mixer_file_descriptor(mixHandle), &rfds))
        	{
            	snd_mixer_callbacks_t callbacks = {0, 0, 0, 0};
			snd_mixer_read(mixHandle, &callbacks);
        	}

        	if (FD_ISSET(snd_pcm_file_descriptor(pcmHandle, SND_PCM_CHANNEL_PLAYBACK), &wfds))
        	{
            	snd_pcm_channel_status_t status;
            	int written = 0;

            	if ((n = fread(&sampleBfr[0], sizeof(byte), __min(samples - N, bufSize), fd)) <= 0)
                	continue;
            	written = snd_pcm_plugin_write(pcmHandle, &sampleBfr[0], n);
            	if (written < n)
            	{
                	memset(&status, 0, sizeof(status));
                	if (snd_pcm_plugin_status(pcmHandle, &status) < 0)
                		return 0;

                	if (status.status == SND_PCM_STATUS_READY ||
                    		status.status == SND_PCM_STATUS_UNDERRUN)
                	{
                   	 	if (snd_pcm_plugin_prepare(pcmHandle, SND_PCM_CHANNEL_PLAYBACK) < 0)
                    			return 0;
                	}
                	if (written < 0)
                    		written = 0;
                	written += snd_pcm_plugin_write(pcmHandle, &sampleBfr[written], n - written);
            	}
            	N += written;
        	}
    }

    fclose(fd);

	if (pcmHandle)
	    	snd_pcm_plugin_flush(pcmHandle, SND_PCM_CHANNEL_PLAYBACK);

	if (mixHandle)
		snd_mixer_close(mixHandle);
  
	if (pcmHandle)
		snd_pcm_close(pcmHandle);
		
 return NULL;
}
