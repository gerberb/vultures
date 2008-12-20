extern "C" {
	#include "hack.h"
	
	extern "C" short glyph2tile[];
}

#include "vultures_gra.h"
#include "vultures_gfl.h"
#include "vultures_win.h"
#include "vultures_txt.h"
#include "vultures_sdl.h"
#include "vultures_mou.h"
#include "vultures_tile.h"


#include "map.h"
#include "hotspot.h"
#include "levelwin.h"
#include "textwin.h"


map::map(levelwin *p) : window(p)
{
	int i;
	SDL_Surface *image;

	v_type = V_WINTYPE_MAP;
	autobg = 1;
	need_redraw = 1;
	w = 640;
	h = 480;
	x = (parent->w - w) / 2;
	y = (parent->h - h) / 2;
	abs_x = parent->abs_x + x;
	abs_y = parent->abs_y + y;

	/* Load map parchment */
	mapbg = vultures_load_graphic(V_FILENAME_MAP_PARCHMENT);

	/* level title */
	textwin *txt = new textwin(this, "");
	txt->x = w/2;
	txt->y = 48;

	/* close button */
	hotspot *close = new hotspot(this, 598, 11, 28, 24, 1, "Close map");
	close->abs_x = abs_x + close->x;
	close->abs_y = abs_y + close->y;

	/* Load map symbols */
	image = vultures_load_graphic(V_FILENAME_MAP_SYMBOLS);
	if (image == NULL)
		return;
	else
	{
		for (i = 0; i < V_MAX_MAP_SYMBOLS; i++)
			vultures_map_symbols[i] = vultures_get_img_src(
				(i%40)*VULTURES_MAP_SYMBOL_WIDTH,
				(i/40)*VULTURES_MAP_SYMBOL_HEIGHT,
				(i%40)*VULTURES_MAP_SYMBOL_WIDTH + (VULTURES_MAP_SYMBOL_WIDTH - 1),
				(i/40)*VULTURES_MAP_SYMBOL_HEIGHT+ (VULTURES_MAP_SYMBOL_HEIGHT - 1),
				image);
		SDL_FreeSurface(image);
	}
}

map::~map()
{
	int i;

	/* free the map symbols */
	for (i = 0; i < V_MAX_MAP_SYMBOLS; i++)
		SDL_FreeSurface(vultures_map_symbols[i]);
	
	SDL_FreeSurface(mapbg);
}

bool map::draw()
{
	int map_x, map_y;
	int i, j;
	window * txt = first_child;
	char buffer[256];

	/* Draw parchment */
	vultures_put_img(abs_x, abs_y, mapbg);

	/* Draw the level name */
#ifdef VULTURESCLAW
	describe_level(buffer, TRUE);
#else
	if (!describe_level(buffer))
		sprintf(buffer, "%s, level %d ", dungeons[u.uz.dnum].dname, depth(&u.uz));
#endif
	txt->set_caption(buffer);
	txt->x = (w - vultures_text_length(V_FONT_HEADLINE, txt->caption)) / 2;
	txt->abs_x = abs_x + txt->x;
	txt->abs_y = abs_y + txt->y;
	vultures_put_text_shadow(V_FONT_HEADLINE, txt->caption, vultures_screen, txt->abs_x, txt->abs_y, CLR32_BROWN, CLR32_BLACK_A50);

	/* Find upper left corner of map on parchment */
	map_x = abs_x + 39;
	map_y = abs_y + 91;

	/* Draw map on parchment, and create hotspots */
	for (i = 0; i < ROWNO; i++)
		for (j = 1; j < COLNO; j++)
		{
			int map_glyph = levwin->get_glyph(MAP_GLYPH, j, i);
			bool is_dark = (levwin->get_glyph(MAP_DARKNESS, j, i) == 2);
			
			if (map_glyph != NO_GLYPH &&
				(map_glyph != cmap_to_glyph(S_stone) ||
				(level.locations[j][i].seenv && is_dark))) {
				vultures_put_img(
						map_x+VULTURES_MAP_SYMBOL_WIDTH*j,
						map_y+VULTURES_MAP_SYMBOL_HEIGHT*i,
						vultures_map_symbols[glyph2tile[map_glyph]]); 
			}
		}

	vultures_invalidate_region(abs_x, abs_y, w, h);

	return false;
}


void map::toggle(void)
{
	static window *mapwin = NULL;

	if (!mapwin)
	{
		mapwin = new map(levwin);

		mapwin->x = (mapwin->parent->w - mapwin->w) / 2;
		mapwin->y = (mapwin->parent->h - mapwin->h) / 2;

		vultures_winid_map = mapwin->id;
	}
	else
	{
		mapwin->hide();
		delete mapwin;
		mapwin = NULL;
		vultures_winid_map = 0;
	}
}


eventresult map::event_handler(window* target, void* result, SDL_Event* event)
{
	point mouse, mappos;
	string ttext;

	mouse = vultures_get_mouse_pos();
	mappos.x = (mouse.x - abs_x - 39) / VULTURES_MAP_SYMBOL_WIDTH;
	mappos.y = (mouse.y - abs_y - 91) / VULTURES_MAP_SYMBOL_HEIGHT;

	if (mappos.x < 1 || mappos.x >= COLNO ||
		mappos.y < 0 || mappos.y >= ROWNO)
	{
		mappos.x = -1;
		mappos.y = -1;
	}

	switch (event->type)
	{
		case SDL_MOUSEBUTTONUP:
			/* handler != target if the user clicked on the X in the upper right corner */
			if (this != target && target->menu_id == 1)
			{
				toggle();
				vultures_mouse_invalidate_tooltip(1);
				return V_EVENT_HANDLED_REDRAW;
			}

			/* only handle clicks on valid map locations */
			if (mappos.x != -1)
				return levwin->handle_click(result, event->button.button, mappos);

			break;

		case SDL_TIMEREVENT:
			if (event->user.code < HOVERTIMEOUT)
				return V_EVENT_HANDLED_NOREDRAW;

			/* draw the tooltip for the close button */
			if (this != target && target->menu_id == 1)
				vultures_mouse_set_tooltip(target->caption);
			/* draw a tooltip for the map location */
			else if (mappos.x != -1) {
				ttext = levwin->map_square_description(mappos, 1);
				if(!ttext.empty())
					vultures_mouse_set_tooltip(ttext);
			}

			break;

		case SDL_MOUSEMOTION:
			vultures_set_mcursor(V_CURSOR_NORMAL);
			break;

		case SDL_VIDEORESIZE:
			x = (parent->w - w) / 2;
			y = (parent->h - h) / 2;
			break;
	}

	return V_EVENT_HANDLED_NOREDRAW;
}