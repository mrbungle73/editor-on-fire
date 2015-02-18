#include <allegro.h>
#include <stdio.h>
#include "../agup/agup.h"
#include "proc.h"

#ifdef USEMEMWATCH
#include "../memwatch.h"
#endif

#define EOF_MSG_BUTTONFOCUS D_USER
//Allegro's GUI system reserves bit flags 0 through 6 for its own use, the others are available for
//program use.  Define a bitflag for use to filter dialog focus changing events that use the mouse
//click from those that don't

int eof_verified_edit_proc(int msg, DIALOG *d, int c)
{
	int i;
	char * string = NULL;
	int key_list[32] = {KEY_BACKSPACE, KEY_DEL, KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN, KEY_ESC, KEY_ENTER};
	int match = 0;

	if(msg == MSG_CHAR)
	{
		for(i = 0; i < 8; i++)
		{
			if(c >> 8 == key_list[i])			//If the input is permanently allowed
			{
				return d_agup_edit_proc(msg, d, c);	//Immediately allow the input character to be returned
			}
		}

		/* see if key is an allowed key */
		string = (char *)(d->dp2);
		if(string == NULL)	//If the accepted characters list is NULL for some reason
			match = 1;	//Implicitly accept the input character instead of allowing a crash
		else
		{
			for(i = 0; string[i] != '\0'; i++)	//Search all characters of the accepted characters list
			{
				if(string[i] == (c & 0xff))
				{
					match = 1;
					break;
				}
			}
		}

		if(!match)			//If there was no match
			return D_USED_CHAR;	//Drop the character
	}
	if(msg == MSG_WANTFOCUS)
	{	//If this field wants focus
		if(d->flags & EOF_MSG_BUTTONFOCUS)
		{	//If a custom button focus flag was set by the click event
			d->flags &= ~EOF_MSG_BUTTONFOCUS;	//Clear it
			return D_WANTFOCUS;	//And have the dialog system switch input focus
		}
		return D_O_K;	//Otherwise deny it, because simply mousing over it can cause this message
	}
	if(msg == MSG_CLICK)
	{	//If this field is explicitly clicked on
		d->flags |= EOF_MSG_BUTTONFOCUS;	//Set this custom flag to indicate a mouse click initiated the WANTFOCUS message
		return d_agup_edit_proc(msg,d,c);	//Allow the dialog system to carry out the click processing
	}

	return d_agup_edit_proc(msg, d, c);	//Allow the input character to be returned
}
