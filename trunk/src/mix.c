#include <allegro.h>
#include <math.h>	//For sqrt()
#include "main.h"
#include "utility.h"
#include "beat.h"
#include "mix.h"

//AUDIOSTREAM * eof_mix_stream = NULL;
SAMPLE *    eof_sound_clap = NULL;
SAMPLE *    eof_sound_metronome = NULL;
SAMPLE *    eof_sound_note[EOF_MAX_VOCAL_TONES] = {NULL};
SAMPLE *    eof_sound_chosen_percussion = NULL;	//The user-selected percussion sound
SAMPLE *    eof_sound_cowbell = NULL;
SAMPLE *    eof_sound_tambourine1 = NULL;
SAMPLE *    eof_sound_tambourine2 = NULL;
SAMPLE *    eof_sound_tambourine3 = NULL;
SAMPLE *    eof_sound_triangle1 = NULL;
SAMPLE *    eof_sound_triangle2 = NULL;
SAMPLE *    eof_sound_woodblock1 = NULL;
SAMPLE *    eof_sound_woodblock2 = NULL;
SAMPLE *    eof_sound_woodblock3 = NULL;
SAMPLE *    eof_sound_woodblock4 = NULL;
SAMPLE *    eof_sound_woodblock5 = NULL;
SAMPLE *    eof_sound_woodblock6 = NULL;
SAMPLE *    eof_sound_woodblock7 = NULL;
SAMPLE *    eof_sound_woodblock8 = NULL;
SAMPLE *    eof_sound_woodblock9 = NULL;
SAMPLE *    eof_sound_woodblock10 = NULL;
SAMPLE *    eof_sound_clap1 = NULL;
SAMPLE *    eof_sound_clap2 = NULL;
SAMPLE *    eof_sound_clap3 = NULL;
SAMPLE *    eof_sound_clap4 = NULL;
EOF_MIX_VOICE eof_voice[EOF_MIX_MAX_CHANNELS];	//eof_voice[0] is "clap", eof_voice[1] is "metronome", eof_voice[2] is "vocal tone", eof_voice[3] is "vocal percussion"
//int eof_mix_freq = 44100;
//int eof_mix_buffer_size = 4096;
char eof_mix_claps_enabled = 0;
char eof_mix_metronome_enabled = 0;
char eof_mix_claps_note = 31; /* enable all by default */
char eof_mix_vocal_tones_enabled = 0;
char eof_mix_midi_tones_enabled = 0;
char eof_mix_percussion_enabled = 0;
int eof_selected_percussion_cue = 10;	//The user selected percussion sound (cowbell by default), corresponds to the radio button in the eof_audio_cues_dialog[] array

int eof_clap_volume = 100;	//Stores the volume level for the clap cue, specified as a percentage
int eof_tick_volume = 100;	//Stores the volume level for the tick cue, specified as a percentage
int eof_tone_volume = 100;	//Stores the volume level for the tone cue, specified as a percentage
int eof_percussion_volume = 100;	//Stores the volume level for the vocal percussion cue, specified as a percentage

int           eof_mix_speed = 1000;
char          eof_mix_speed_ticker;
double        eof_mix_sample_count = 0.0;
double        eof_mix_sample_increment = 1.0;
unsigned long eof_mix_next_clap;
unsigned long eof_mix_next_metronome;
unsigned long eof_mix_next_note;
unsigned long eof_mix_next_percussion;

unsigned long eof_mix_clap_pos[EOF_MAX_NOTES] = {0};
int eof_mix_claps = 0;
int eof_mix_current_clap = 0;

unsigned long eof_mix_note_pos[EOF_MAX_NOTES] = {0};
unsigned long eof_mix_note_note[EOF_MAX_NOTES] = {0};
int eof_mix_notes = 0;
int eof_mix_current_note = 0;

unsigned long eof_mix_metronome_pos[EOF_MAX_BEATS] = {0};
int eof_mix_metronomes = 0;
int eof_mix_current_metronome = 0;

unsigned long eof_mix_percussion_pos[EOF_MAX_NOTES] = {0};
int eof_mix_percussions = 0;
int eof_mix_current_percussion = 0;

void eof_mix_callback(void * buf, int length)
{
	unsigned long bytes_left = length / 2;
	unsigned short * buffer = (unsigned short *)buf;
	long sum;			//Use a signed long integer to allow the clipping logic to be more efficient
	long cuesample;	//Used to apply a volume to cues, where the appropriate amplitude multiplier for changing the cue's loudness to X% is to multiply its amplitudes by sqrt(X/100)
	int i, j;
	int increment = alogg_get_wave_is_stereo_ogg(eof_music_track) ? 2 : 1;

	/* add audio data to the buffer */
	for(i = 0; i < bytes_left; i += increment)
	{

		/* mix voices */
		for(j = 0; j < EOF_MIX_MAX_CHANNELS; j++)
		{
			if(eof_voice[j].playing)
			{
				cuesample = ((unsigned short *)(eof_voice[j].sp->data))[(unsigned long)eof_voice[j].pos] - 32768;
				if(eof_voice[j].volume != 100)
					cuesample *= sqrt(eof_voice[j].volume/100.0);	//Change the cue to be the specified loudness

				sum = buffer[i] + cuesample;
				if(sum < 0)
					sum = 0;
				else if(sum > 65535)
					sum = 65535;

				buffer[i] = sum;
				if(increment > 1)
				{
					sum = buffer[i + 1] + cuesample;

					if(sum < 0)
						sum = 0;
					else if(sum > 65535)
						sum = 65535;

					buffer[i + 1] = sum;
				}
				eof_voice[j].pos += eof_mix_sample_increment;
				if(eof_voice[j].pos >= eof_voice[j].sp->len)
				{
					eof_voice[j].playing = 0;
				}
			}
		}

		/* increment the sample and check sound triggers */
		eof_mix_sample_count += 1.0;
		if((eof_mix_sample_count >= eof_mix_next_clap) && (eof_mix_current_clap < eof_mix_claps))
		{
			if(eof_mix_claps_enabled)
			{
				eof_voice[0].sp = eof_sound_clap;
//				eof_voice[0].sp = eof_sound_cowbell;
				eof_voice[0].pos = 0.0;
				eof_voice[0].playing = 1;
			}
			eof_mix_current_clap++;
			eof_mix_next_clap = eof_mix_clap_pos[eof_mix_current_clap];
		}
		if((eof_mix_sample_count >= eof_mix_next_metronome) && (eof_mix_current_metronome < eof_mix_metronomes))
		{
			if(eof_mix_metronome_enabled)
			{
				eof_voice[1].sp = eof_sound_metronome;
				eof_voice[1].pos = 0.0;
				eof_voice[1].playing = 1;
			}
			eof_mix_current_metronome++;
			eof_mix_next_metronome = eof_mix_metronome_pos[eof_mix_current_metronome];
		}
		if((eof_mix_sample_count >= eof_mix_next_note) && (eof_mix_current_note < eof_mix_notes))
		{
			if(eof_mix_midi_tones_enabled)
			{
				eof_midi_play_note(eof_mix_note_note[eof_mix_current_note]);	//Play the MIDI note
			}
			else if(eof_mix_vocal_tones_enabled && eof_sound_note[eof_mix_note_note[eof_mix_current_note]])
			{
				eof_voice[2].sp = eof_sound_note[eof_mix_note_note[eof_mix_current_note]];
				eof_voice[2].pos = 0.0;
				eof_voice[2].playing = 1;
			}
			eof_mix_current_note++;
			eof_mix_next_note = eof_mix_note_pos[eof_mix_current_note];
		}
		if((eof_mix_sample_count >= eof_mix_next_percussion) && (eof_mix_current_percussion < eof_mix_notes))
		{
			if(eof_mix_percussion_enabled)
			{
				eof_voice[3].sp = eof_sound_chosen_percussion;
				eof_voice[3].pos = 0.0;
				eof_voice[3].playing = 1;
			}
			eof_mix_current_percussion++;
			eof_mix_next_percussion = eof_mix_percussion_pos[eof_mix_current_percussion];
		}
	}
	eof_just_played = 1;
}

unsigned long eof_mix_msec_to_sample(unsigned long msec, int freq)
{
	unsigned long sample;
	double second = (double)msec / (double)1000.0;

	sample = (unsigned long)(second * (double)freq);
	return sample;
}

/*Unused function, it may have malfunctioned anyway, because it divided seconds by 1000 and casted
 to integer from floating point, which would not have returned the correct millisecond time value?
unsigned long eof_mix_sample_to_msec(unsigned long sample, int freq)
{
	unsigned long msec;
	double second = (double)sample / (double)freq;

	msec = (unsigned long)(second / (double)1000.0);
	return msec;
}
*/

void eof_mix_find_claps(void)
{
	int i;

	eof_mix_claps = 0;
	eof_mix_current_clap = 0;
	if(eof_vocals_selected)
	{
		for(i = 0; i < eof_song->vocal_track->lyrics; i++)
		{
			eof_mix_clap_pos[eof_mix_claps] = eof_mix_msec_to_sample(eof_song->vocal_track->lyric[i]->pos, alogg_get_wave_freq_ogg(eof_music_track));
			eof_mix_claps++;
		}
	}
	else
	{
		for(i = 0; i < eof_song->track[eof_selected_track]->notes; i++)
		{
			if((eof_song->track[eof_selected_track]->note[i]->type == eof_note_type) && (eof_song->track[eof_selected_track]->note[i]->note & eof_mix_claps_note))
			{
				eof_mix_clap_pos[eof_mix_claps] = eof_mix_msec_to_sample(eof_song->track[eof_selected_track]->note[i]->pos, alogg_get_wave_freq_ogg(eof_music_track));
				eof_mix_claps++;
			}
		}
	}

	eof_mix_metronomes = 0;
	eof_mix_current_metronome = 0;
	for(i = 0; i < eof_song->beats; i++)
	{
		eof_mix_metronome_pos[eof_mix_metronomes] = eof_mix_msec_to_sample(eof_song->beat[i]->pos, alogg_get_wave_freq_ogg(eof_music_track));
		eof_mix_metronomes++;
	}

	eof_mix_notes = 0;
	eof_mix_current_note = 0;
	eof_mix_percussions = 0;
	for(i = 0; i < eof_song->vocal_track->lyrics; i++)
	{
		if((eof_song->vocal_track->lyric[i]->note >= 36) && (eof_song->vocal_track->lyric[i]->note <= 84))
		{	//This is a vocal pitch
			eof_mix_note_pos[eof_mix_notes] = eof_mix_msec_to_sample(eof_song->vocal_track->lyric[i]->pos, alogg_get_wave_freq_ogg(eof_music_track));
			eof_mix_note_note[eof_mix_notes] = eof_song->vocal_track->lyric[i]->note;
			eof_mix_notes++;
		}
		else if(eof_song->vocal_track->lyric[i]->note == EOF_LYRIC_PERCUSSION)
		{	//This is vocal percussion
			eof_mix_percussion_pos[eof_mix_percussions] = eof_mix_msec_to_sample(eof_song->vocal_track->lyric[i]->pos, alogg_get_wave_freq_ogg(eof_music_track));
			eof_mix_percussions++;
		}
	}
}

void eof_mix_init(void)
{
	int i;
	char fbuffer[1024] = {0};

	eof_sound_clap = load_wav("eof.dat#clap.wav");
	if(!eof_sound_clap)
	{
		allegro_message("Couldn't load clap sound!");
	}
	eof_sound_metronome = load_wav("eof.dat#metronome.wav");
	if(!eof_sound_metronome)
	{
		allegro_message("Couldn't load metronome sound!");
	}
	for(i = 0; i < EOF_MAX_VOCAL_TONES; i++)
	{	//Load piano tones
		sprintf(fbuffer, "eof.dat#piano.esp/NOTE_%02d_OGG", i);
		eof_sound_note[i] = eof_mix_load_ogg_sample(fbuffer);
	}
	eof_sound_cowbell = eof_mix_load_ogg_sample("percussion.dat#cowbell.ogg");
	eof_sound_chosen_percussion = eof_sound_cowbell;	//Until the user specifies otherwise, make cowbell the default percussion
	eof_selected_percussion_cue = 10;
	eof_sound_tambourine1 = eof_mix_load_ogg_sample("percussion.dat#tambourine1.ogg");
	eof_sound_tambourine2 = eof_mix_load_ogg_sample("percussion.dat#tambourine2.ogg");
	eof_sound_tambourine3 = eof_mix_load_ogg_sample("percussion.dat#tambourine3.ogg");
	eof_sound_triangle1 = eof_mix_load_ogg_sample("percussion.dat#triangle1.ogg");
	eof_sound_triangle2 = eof_mix_load_ogg_sample("percussion.dat#triangle2.ogg");
	eof_sound_woodblock1 = eof_mix_load_ogg_sample("percussion.dat#woodblock1.ogg");
	eof_sound_woodblock2 = eof_mix_load_ogg_sample("percussion.dat#woodblock2.ogg");
	eof_sound_woodblock3 = eof_mix_load_ogg_sample("percussion.dat#woodblock3.ogg");
	eof_sound_woodblock4 = eof_mix_load_ogg_sample("percussion.dat#woodblock4.ogg");
	eof_sound_woodblock5 = eof_mix_load_ogg_sample("percussion.dat#woodblock5.ogg");
	eof_sound_woodblock6 = eof_mix_load_ogg_sample("percussion.dat#woodblock6.ogg");
	eof_sound_woodblock7 = eof_mix_load_ogg_sample("percussion.dat#woodblock7.ogg");
	eof_sound_woodblock8 = eof_mix_load_ogg_sample("percussion.dat#woodblock8.ogg");
	eof_sound_woodblock9 = eof_mix_load_ogg_sample("percussion.dat#woodblock9.ogg");
	eof_sound_woodblock10 = eof_mix_load_ogg_sample("percussion.dat#woodblock10.ogg");
	eof_sound_clap1 = eof_mix_load_ogg_sample("percussion.dat#clap1.ogg");
	eof_sound_clap2 = eof_mix_load_ogg_sample("percussion.dat#clap2.ogg");
	eof_sound_clap3 = eof_mix_load_ogg_sample("percussion.dat#clap3.ogg");
	eof_sound_clap4 = eof_mix_load_ogg_sample("percussion.dat#clap4.ogg");

	alogg_set_buffer_callback(eof_mix_callback);
}

SAMPLE *eof_mix_load_ogg_sample(char *fn)
{
	ALOGG_OGG * temp_ogg = NULL;
	char * buffer = NULL;
	SAMPLE *loadedsample = NULL;

	if(fn == NULL)
		return NULL;

	buffer = eof_buffer_file(fn);
	if(buffer)
	{
		temp_ogg = alogg_create_ogg_from_buffer(buffer, file_size_ex(fn));
		if(temp_ogg)
		{
			loadedsample = alogg_create_sample_from_ogg(temp_ogg);
			alogg_destroy_ogg(temp_ogg);
		}
		if(loadedsample == NULL)
			allegro_message("Couldn't load sample %s!",fn);
		free(buffer);
	}

	return loadedsample;
}

void eof_mix_exit(void)
{
	int i;

	destroy_sample(eof_sound_clap);
	eof_sound_clap=NULL;
	destroy_sample(eof_sound_metronome);
	eof_sound_metronome=NULL;
	destroy_sample(eof_sound_cowbell);
	eof_sound_cowbell=NULL;
	destroy_sample(eof_sound_tambourine1);
	eof_sound_tambourine1=NULL;
	destroy_sample(eof_sound_tambourine2);
	eof_sound_tambourine2=NULL;
	destroy_sample(eof_sound_tambourine3);
	eof_sound_tambourine3=NULL;
	destroy_sample(eof_sound_triangle1);
	eof_sound_triangle1=NULL;
	destroy_sample(eof_sound_triangle2);
	eof_sound_triangle2=NULL;
	destroy_sample(eof_sound_woodblock1);
	eof_sound_woodblock1=NULL;
	destroy_sample(eof_sound_woodblock2);
	eof_sound_woodblock2=NULL;
	destroy_sample(eof_sound_woodblock3);
	eof_sound_woodblock3=NULL;
	destroy_sample(eof_sound_woodblock4);
	eof_sound_woodblock4=NULL;
	destroy_sample(eof_sound_woodblock5);
	eof_sound_woodblock5=NULL;
	destroy_sample(eof_sound_woodblock6);
	eof_sound_woodblock6=NULL;
	destroy_sample(eof_sound_woodblock7);
	eof_sound_woodblock7=NULL;
	destroy_sample(eof_sound_woodblock8);
	eof_sound_woodblock8=NULL;
	destroy_sample(eof_sound_woodblock9);
	eof_sound_woodblock9=NULL;
	destroy_sample(eof_sound_woodblock10);
	eof_sound_woodblock10=NULL;
	destroy_sample(eof_sound_clap1);
	eof_sound_clap1=NULL;
	destroy_sample(eof_sound_clap2);
	eof_sound_clap2=NULL;
	destroy_sample(eof_sound_clap3);
	eof_sound_clap3=NULL;
	destroy_sample(eof_sound_clap4);
	eof_sound_clap4=NULL;

	for(i = 0; i < EOF_MAX_VOCAL_TONES; i++)
	{
		if(eof_sound_note[i] != NULL)
		{
			destroy_sample(eof_sound_note[i]);
			eof_sound_note[i]=NULL;
		}
	}
}

void eof_mix_start_helper(void)
{
	int i;

	eof_mix_current_clap = -1;
	eof_mix_next_clap = -1;
	for(i = 0; i < eof_mix_claps; i++)
	{
		if(eof_mix_clap_pos[i] >= eof_mix_sample_count)
		{
			eof_mix_current_clap = i;
			eof_mix_next_clap = eof_mix_clap_pos[i];
			break;
		}
	}
	eof_mix_current_metronome = -1;
	eof_mix_next_metronome = -1;
	for(i = 0; i < eof_mix_metronomes; i++)
	{
		if(eof_mix_metronome_pos[i] >= eof_mix_sample_count)
		{
			eof_mix_current_metronome = i;
			eof_mix_next_metronome = eof_mix_metronome_pos[i];
			break;
		}
	}
	eof_mix_current_note = -1;
	eof_mix_next_note = -1;
	for(i = 0; i < eof_mix_notes; i++)
	{
		if(eof_mix_note_pos[i] >= eof_mix_sample_count)
		{
			eof_mix_current_note = i;
			eof_mix_next_note = eof_mix_note_pos[i];
			break;
		}
	}
	eof_mix_current_percussion = -1;
	eof_mix_next_percussion = -1;
	for(i = 0; i < eof_mix_percussions; i++)
	{
		if(eof_mix_percussion_pos[i] >= eof_mix_sample_count)
		{
			eof_mix_current_percussion = i;
			eof_mix_next_percussion = eof_mix_percussion_pos[i];
			break;
		}
	}
}

void eof_mix_start(unsigned long start, int speed)
{
	int i;

	eof_mix_next_clap = -1;
	eof_mix_next_metronome = -1;
	for(i = 0; i < EOF_MIX_MAX_CHANNELS; i++)
	{
		eof_voice[i].sp = NULL;
		eof_voice[i].pos = 0.0;
		eof_voice[i].playing = 0;
		eof_voice[i].volume = 100;	//Default to 100% volume
	}
	eof_voice[0].volume = eof_clap_volume;	//Put the clap volume into effect
	eof_voice[1].volume = eof_tick_volume;	//Put the tick volume into effect
	eof_voice[2].volume = eof_tone_volume;	//Put the tone volume into effect
	eof_voice[3].volume = eof_percussion_volume;	//Put the percussion volume into effect
	eof_mix_speed = speed;
	eof_mix_speed_ticker = 0;
	eof_mix_sample_count = start;
	eof_mix_sample_increment = (1000.0 / (float)eof_mix_speed) * (44100.0 / (float)alogg_get_wave_freq_ogg(eof_music_track));
	eof_mix_start_helper();
}

void eof_mix_seek(int pos)
{
	int i;

	eof_mix_next_clap = -1;
	eof_mix_next_metronome = -1;

	eof_mix_sample_count = eof_mix_msec_to_sample(pos, alogg_get_wave_freq_ogg(eof_music_track));
	for(i = 0; i < eof_mix_claps; i++)
	{
		if(eof_mix_clap_pos[i] >= eof_mix_sample_count)
		{
			eof_mix_current_clap = i;
			eof_mix_next_clap = eof_mix_clap_pos[i];
			break;
		}
	}
	for(i = 0; i < eof_mix_metronomes; i++)
	{
		if(eof_mix_metronome_pos[i] >= eof_mix_sample_count)
		{
			eof_mix_current_metronome = i;
			eof_mix_next_metronome = eof_mix_metronome_pos[i];
			break;
		}
	}
	for(i = 0; i < eof_mix_notes; i++)
	{
		if(eof_mix_note_pos[i] >= eof_mix_sample_count)
		{
			eof_mix_current_note = i;
			eof_mix_next_note = eof_mix_note_pos[i];
			break;
		}
	}
	for(i = 0; i < eof_mix_percussions; i++)
	{
		if(eof_mix_percussion_pos[i] >= eof_mix_sample_count)
		{
			eof_mix_current_percussion = i;
			eof_mix_next_percussion = eof_mix_percussion_pos[i];
			break;
		}
	}
}

void eof_mix_play_note(int note)
{
	if((note < EOF_MAX_VOCAL_TONES) && eof_sound_note[note])
	{
		if(eof_mix_midi_tones_enabled)
			eof_midi_play_note(note);
		else
			play_sample(eof_sound_note[note], 255.0 * (eof_tone_volume / 100.0), 127, 1000 + eof_audio_fine_tune, 0);	//Play the tone at the user specified cue volume
	}
}

void eof_midi_play_note(int note)
{
	unsigned char NOTE_ON_DATA[3]={0x91,0x0,127};		//Data sequence for a Note On, channel 1, Note 0
	unsigned char NOTE_OFF_DATA[3]={0x81,0x0,127};		//Data sequence for a Note Off, channel 1, Note 0
	static unsigned char lastnote=0;					//Remembers the last note that was played, so it can be turned off
	static unsigned char lastnotedefined=0;

	if(note < EOF_MAX_VOCAL_TONES)
	{
		NOTE_ON_DATA[1]=note;	//Alter the data sequence to be the appropriate note number
		if(lastnotedefined)
		{
			NOTE_OFF_DATA[1]=lastnote;
			midi_out(NOTE_OFF_DATA,3);	//Turn off the last note that was played
		}
		midi_out(NOTE_ON_DATA,3);	//Turn on this note
		lastnote=note;
		lastnotedefined=1;
	}
}