#ifndef _window_h_
#define _window_h_

#include <string>
#include <SDL.h>
#include "vultures_types.h"

#define ROOTWIN vultures_get_window(0)

#define V_LISTITEM_WIDTH  300
#define V_LISTITEM_HEIGHT  52

using std::string;

typedef enum {
	V_WINTYPE_NONE, /* only the root window has this type */
	V_WINTYPE_LEVEL,
	V_WINTYPE_MAP,
	V_WINTYPE_MAIN,
	V_WINTYPE_MENU,
	V_WINTYPE_BUTTON,
	V_WINTYPE_OPTION,
	V_WINTYPE_SCROLLBAR,
	V_WINTYPE_TEXT,
	V_WINTYPE_CONTEXTMENU,
	V_WINTYPE_SCROLLAREA,
	
	/* new-style inventory and object lists (ie multidrop, pickup, loot) */
	V_WINTYPE_OBJWIN,
	V_WINTYPE_OBJITEM,
	V_WINTYPE_OBJITEMHEADER,
	
	V_WINTYPE_ENDING,
	V_WINTYPE_CUSTOM
} window_type;


typedef enum {
	V_EVENT_UNHANDLED,
	V_EVENT_UNHANDLED_REDRAW, /* pass the event on to a parent win and redraw */
	V_EVENT_HANDLED_NOREDRAW, /* don't pass it on and don't redraw */
	V_EVENT_HANDLED_REDRAW,   /* don't pass it on and redraw */
	V_EVENT_HANDLED_FINAL     /* redraw and leave the event dispatcher */
} eventresult;



enum hotspots {
	V_HOTSPOT_NONE = 0,
	/* child windows of the map */
	V_HOTSPOT_SCROLL_UPLEFT,
	V_HOTSPOT_SCROLL_UP,
	V_HOTSPOT_SCROLL_UPRIGHT,
	V_HOTSPOT_SCROLL_LEFT,
	V_HOTSPOT_SCROLL_RIGHT,
	V_HOTSPOT_SCROLL_DOWNLEFT,
	V_HOTSPOT_SCROLL_DOWN,
	V_HOTSPOT_SCROLL_DOWNRIGHT,
	V_WIN_MINIMAP,
	V_WIN_STATUSBAR,
	V_WIN_TOOLBAR1,
	V_WIN_TOOLBAR2,
	V_WIN_ENHANCE,

	/* child windows of the statusbar */
	V_HOTSPOT_BUTTON_LOOK,
	V_HOTSPOT_BUTTON_EXTENDED,
	V_HOTSPOT_BUTTON_MAP,
	V_HOTSPOT_BUTTON_SPELLBOOK,
	V_HOTSPOT_BUTTON_INVENTORY,
	V_HOTSPOT_BUTTON_DISCOVERIES,
	V_HOTSPOT_BUTTON_MESSAGES,
	V_HOTSPOT_BUTTON_OPTIONS,
	V_HOTSPOT_BUTTON_IFOPTIONS,
	V_HOTSPOT_BUTTON_HELP,

	V_MENU_ACCEPT,
	V_MENU_CANCEL = -1,

	V_INV_NEXTPAGE,
	V_INV_PREVPAGE,
	V_INV_CLOSE
};



class window
{

public:
	window(window *p);
	window();
	virtual ~window();

	virtual bool draw() = 0;
	virtual eventresult event_handler(window* target, void* result, SDL_Event* event) = 0;
	virtual window* replace_win(window *win);
	virtual void set_caption(string str);
	virtual void hide();
	virtual void layout() {};
	virtual void update_background(void);
	
	window_type get_wintype() { return v_type; };
	int get_nh_type() { return nh_type; };
	int get_id() { return id; };
	void *get_menu_id() { return menu_id_v; };
	
	void draw_windows();
	window *walk_winlist(bool *descend);
	window *get_window_from_point(point mouse);
	bool intersects_invalid();

	int get_w() { return w; }
	int get_h() { return h; }

	bool need_redraw;
	bool visible;
	
	/* if context_is_text is false, text items are assumed to be headers and
	 * get spaced ~1/2 line further apart  */
	bool content_is_text;
	
	virtual window* find_accel(char accel);


// protected:

	int id;
	int nh_type;                   /* type assigned by nethack */
	window_type v_type;
	string caption;
	char accelerator;


	/* absolute coords; calculated before drawing */
	int abs_x, abs_y;

	/* coords relative to parent */
	int x, y;
	int w, h;

	
	union {
		void *menu_id_v;
		int menu_id;
	};
	
	window *first_child, *last_child;
	window *parent;
	window *sib_next, *sib_prev;
	
protected:
	bool autobg;
	SDL_Surface *background;
};


window * vultures_get_window(int winid);

#endif