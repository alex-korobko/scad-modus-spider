#ifndef __SOUND_H__
#define __SOUND_H__

class sound {
friend class only_for_compiler_happening;
 private:
  typedef struct {
    char    tag[4];
    long    length;
  } riff_tag;

typedef struct {
    char    riff[4];
    long    size;
    char    wave[4];
   } riff_hdr;

typedef struct{
    short   formatTag;
    short   channels;
    long    samplesPerSec;
    long    avgBytesPerSec;
    short   blockAlign;
    short   bitsPerSample;
 } wave_hdr;

//not used externally metods for singleton
sound() {};
sound(const sound&) {};
sound& operator=(const sound& s) { return const_cast<sound&>(s);};
~sound() {};

 class sound_exception {
       private:
          string test_exc_descr;

        public:
           explicit sound_exception (string description) : 
                   test_exc_descr (description) {};

            string get_description () {return test_exc_descr;};
       };

class sound_mutex_locker {
	private:
 		static pthread_mutex_t  playing_process_mutex;
	public:
		sound_mutex_locker() {
			pthread_mutex_lock(&playing_process_mutex);
		}

		~sound_mutex_locker() {
			pthread_mutex_unlock(&playing_process_mutex);
		}
};

//private static metods
 static long find_tag (FILE * fp, string tag);
 static bool  check_hdr(FILE * fp);
 static void* player(void* arg); 

 public:
	bool play(vector<string> *filename);

//public static metods
    static sound& get_instance() {
            static sound snd;
            return snd;};
};

#endif
