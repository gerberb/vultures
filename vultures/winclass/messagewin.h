#ifndef _messagewin_h_
#define _messagewin_h_

#include "window.h"

/* Message shading: old messages grow darker */
#define V_MAX_MESSAGE_COLORS 16

#define V_MESSAGEBUF_SIZE 512


class messagewin : public window
{
public:
	messagewin(window *p);
	~messagewin();
	virtual bool draw();
	virtual eventresult event_handler(window* target, void* result, SDL_Event* event);
	
	void add_message(string msg);
	void setshown(int first);
	int getshown(void);
	string get_message(int offset, int *age);
	void view_all(void);

private:
	SDL_Surface *bg_img;
	int message_ages[V_MESSAGEBUF_SIZE];
	string message_buf[V_MESSAGEBUF_SIZE];
	int message_cur, message_top;
};

extern messagewin *msgwin;


#endif