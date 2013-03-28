#ifndef EOF_GP_IMPORT_H
#define EOF_GP_IMPORT_H

#ifndef EOF_BUILD
	//Compile standalone parse utility, use macros to make the file compile without Allegro
	#define PACKFILE FILE
	#define pack_fread(x, y, z) fread(x, 1, y, z)
	#define pack_fopen fopen
	#define pack_fclose fclose
	#define pack_getc getc
	#define pack_fseek(x, y) fseek(x, y, SEEK_CUR)
	#define EOF_SONG char
	#define pack_feof feof
	void eof_gp_debug_log(FILE *inf, char *text);		//Prints the current file position and the specified string to the console
	extern char **eof_note_names;
#else
	struct eof_gp_measure
	{
		unsigned char num, den;			//The 8 bit numerator and denominator defined in guitar pro time signatures
		unsigned char start_of_repeat;	//If nonzero, indicates that this measure is the start of a repeat (measure 0 is this by default)
		unsigned char num_of_repeats;	//If nonzero, indicates the end of a repeat as well as how many repeats
	};

	struct eof_guitar_pro_struct
	{
		unsigned long numtracks;			//The number of tracks loaded from the guitar pro file
		char **names;						//An array of strings, representing the native name of each loaded track
		EOF_PRO_GUITAR_TRACK **track;		//An array of pro guitar track pointers, representing the imported note data of each loaded track
		EOF_TEXT_EVENT * text_event[EOF_MAX_TEXT_EVENTS];	//An array of pro guitar text event structures, representing the section markers imported for each loaded track
		struct eof_gp_measure *measure;		//An array of measure data from the Guitar Pro file
		unsigned long measures;				//The number of elements in the above array
		unsigned long text_events;			//The size of the text_event[] array
	};

	void eof_gp_debug_log(PACKFILE *inf, char *text);
		//Does nothing in the EOF build
	EOF_SONG *eof_import_gp(const char * fn);
		//Currently parses through the specified GP file and outputs its details to stdout
	struct eof_guitar_pro_struct *eof_load_gp(const char * fn, char *undo_made);
		//Parses the specified guitar pro file, returning a structure of track information and a populated pro guitar track for each
		//Returns NULL on error
		//NOTE:  Beats are added to the current project if there aren't as many as defined in the GP file.
		//If the user opts to import the GP file's time signatures, an undo state will be made if undo_made is not NULL and *undo_made is zero.  The referenced memory will then be set to nonzero
	int eof_unwrap_gp_track(struct eof_guitar_pro_struct *gp, unsigned long track, char import_ts);
		//Unwrap the specified track in the guitar pro structure into a new pro guitar track
		//If the track being unwrapped is 0, text events will be unwrapped and gp's text events array will be replaced if there are any text events
		//If import_ts is nonzero, the active project's time signatures are updated to reflect those of the unwrapped transcription
		//Returns nonzero on error
	char eof_copy_notes_in_beat_range(EOF_PRO_GUITAR_TRACK *source, unsigned long startbeat, unsigned long numbeats, EOF_PRO_GUITAR_TRACK *dest, unsigned long destbeat);
		//Copies the notes within the specified range of beats in the source track to the same number of beats starting at the specified beat in the destination track
		//Returns zero on error
#endif

void pack_ReadWORDLE(PACKFILE *inf, unsigned *data);
	//Read a little endian format 2 byte integer from the specified file.  If data isn't NULL, the value is stored into it.
void pack_ReadDWORDLE(PACKFILE *inf, unsigned long *data);
	//Read a little endian format 4 byte integer from the specified file.  If data isn't NULL, the value is stored into it.
int eof_read_gp_string(PACKFILE *inf, unsigned *length, char *buffer, char readfieldlength);
	//Read a string, prefixed by a one-byte string length, from the specified file
	//If readfieldlength is nonzero, a 4 byte field length that prefixes the string length is also read
	//If length is not NULL, the string length is returned through it.
	//Buffer must be able to store at least 256 bytes to avoid overflowing.
int eof_gp_parse_bend(PACKFILE *inf);
	//Parses the bend at the current file position, outputting debug logging appropriately
	//Returns nonzero if there is an error parsing, such as end of file being reached unexpectedly

#endif


