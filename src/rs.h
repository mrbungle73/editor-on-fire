#ifndef EOF_RS_H
#define EOF_RS_H

int eof_is_string_muted(EOF_SONG *sp, unsigned long track, unsigned long note);
	//Returns nonzero if all used strings in the note are fret hand muted

unsigned long eof_build_chord_list(EOF_SONG *sp, unsigned long track, unsigned long **results);
	//Parses the given pro guitar track, building a list of all unique chords
	//Memory is allocated to *results to return the list, referring to each note by number
	//The function returns the number of unique chords contained in the list
	//If there are no chords in the pro guitar track, or upon error, *results is set to NULL and 0 is returned

unsigned long eof_build_section_list(EOF_SONG *sp, unsigned long **results, unsigned long track);
	//Parses the given chart, building a list of all unique section markers (case insensitive),
	//from the perspective of the specified track (ie. a bass-specific section is not found when a guitar track is specified)
	//Memory is allocated to *results to return the list, referring to each text event by number
	//The function returns the number of unique sections contained in the list
	//If there are no sections in the pro guitar track, or upon error, *results is set to NULL and 0 is returned

int eof_export_rocksmith_track(EOF_SONG * sp, char * fn, unsigned long track);
	//Writes the specified pro guitar track in Rocksmith's XML format, if the track is populated
	//fn is expected to point to an array at least 1024 bytes in size.  It's filename is altered to reflect the track's name (ie /PART REAL_GUITAR.xml)

void eof_pro_guitar_track_fix_fingerings(EOF_PRO_GUITAR_TRACK *tp, char *undo_made);
	//Checks all notes in the track and duplicates finger arrays of notes with complete finger definitions to matching notes without complete finger definitions
	//If any note has invalid fingering, it is cleared and will be allowed to be set by a valid fingering from a matching note.
	//If *undo_made is zero, this function will create an undo state before modifying the chart and will set the referenced variable to nonzero

int eof_pro_guitar_note_fingering_valid(EOF_PRO_GUITAR_TRACK *tp, unsigned long note);
	//Returns 0 if the fingering is invalid for the specified note (partially defined, or a finger specified for a string that is played open)
	//Returns 1 if the fingering is fully defined for the specified note
	//Returns 2 if the fingering is undefined for the specified note

void eof_song_fix_fingerings(EOF_SONG *sp, char *undo_made);
	//Runs eof_pro_guitar_track_fix_fingerings() on all pro guitar tracks in the specified chart
	//If *undo_made is zero, this function will create an undo state before modifying the chart and will set the referenced variable to nonzero

extern unsigned char *eof_fret_range_tolerances ;
	//A dynamically allocated array that defines the fretting hand's range for each fret on the guitar neck, numbered where fret 1's range is defined at eof_fret_range_tolerances[1]
void eof_generate_hand_positions(EOF_SONG *sp, unsigned long track, char difficulty);
	//If there are any hand positions defined for the specified track difficulty, the user is prompted before clearing them and building new ones
	//The most efficient set of hand position changes calculated using a recursive algorithm (eof_remaining_hand_changes_needed) and added to the track
int eof_generate_hand_positions_current_track_difficulty(void);
	//Calls eof_generate_hand_positions() specifying the current track difficulty
	//Returns 0 on error
int eof_note_can_be_played_within_fret_tolerance(EOF_PRO_GUITAR_TRACK *tp, unsigned long note, unsigned char *current_low, unsigned char *current_high);
	//Examines the highest and lowest used frets of the specified note, and whether the updated highest and lowest fret would be within the specified tolerance for fret difference (ie. can be played without moving the fret hand)
	//If the note is considered playable without requiring an addition fret hand position change, current_high and current_low are updated by reference and nonzero is returned
	//if the new difference between the high and low fret are out of the tolerance range, zero is returned and the calling function should place a fret hand position for the previously checked notes
	//The tolerance for each given fret hand position is stored in eof_fret_range_tolerances[], which must have enough definitions as there are frets for the specified pro guitar track, plus 1 to account that entry 0 is unused
void eof_build_fret_range_tolerances(EOF_PRO_GUITAR_TRACK *tp, unsigned char difficulty);
	//Rebuild the array referenced by eof_fret_range_tolerances by examining the specified track difficulty
	//For each note examined, the range of frets used is considered to be playable and the range for the note's lowest used fret position is updated accordingly
	//If any particular fret is not used as the lowest fret for any chords, the fret range is defaulted to 4

#endif