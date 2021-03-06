/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
//
#include "ui_local.h"

#define SLUGROCK_FRAMEL	"menu/art/frame2_l"
#define SLUGROCK_FRAMER	"menu/art/frame1_r"
#define SLUGROCK_BACK0	"menu/art/back_0"
#define SLUGROCK_BACK1	"menu/art/back_1"
#define SLUGROCK_ACCEPT0	"menu/art/accept_0"
#define SLUGROCK_ACCEPT1	"menu/art/accept_1"

static char* slugrock_artlist[] =
{
	SLUGROCK_FRAMEL,	
	SLUGROCK_FRAMER,
	SLUGROCK_BACK0,
	SLUGROCK_BACK1,
	NULL
};

#define ID_BACK	 100

typedef struct
{
	menuframework_s	menu;
	menutext_s		banner;
	menubitmap_s	framel;
	menubitmap_s	framer;
	menubitmap_s	back;
	char			info[MAX_INFO_STRING];
	int				numlines;
	menutext_s		server;
	menutext_s		client;
	menutext_s		bots;
	menulist_s			g_forceWeaponMode;
	menulist_s			g_forceRedWeaponMode;
	menulist_s			g_forceBlueWeaponMode;
	menuradiobutton_s	g_healthRegen;
	menulist_s			cg_weaponMode;
	menulist_s			bot_weaponMode;
	menubitmap_s	apply;
} slugrock_t;
static slugrock_t	s_slugrock;

char *g_forceWeaponMode_names[] = { "no", "alternate", "random", "rl", "rg", "both", 0 };

typedef struct {
	const char	*name;
	const char	*longname;
	const char	*names[10];
	const char	*longnames[10];
	void		*menuitem;
	int			type;
} slugrockCvar_t;

slugrockCvar_t cvar_list[] = {
	{
		"g_forceWeaponMode",
		"Force the weapon mode:",
		{ "no", "alternate", "random", "rl", "rg", "both", "select", 0 },
		{ "No", "Alternate", "Random", "Rocket Launcher", "Railgun", "Both", "Select", 0 },
		&s_slugrock.g_forceWeaponMode,
		MTYPE_SPINCONTROL
	},

	{
		"g_forceRedWeaponMode",
		"Force the red team weapon mode:",
		{ "no", "alternate", "random", "rl", "rg", "both", "select", 0 },
		{ "No", "Alternate", "Random", "Rocket Launcher", "Railgun", "Both", "Select", 0 },
		&s_slugrock.g_forceRedWeaponMode,
		MTYPE_SPINCONTROL
	},

	{
		"g_forceBlueWeaponMode",
		"Force the blue team weapon mode:",
		{ "no", "alternate", "random", "rl", "rg", "both", "select", 0 },
		{ "No", "Alternate", "Random", "Rocket Launcher", "Railgun", "Both", "Select", 0 },
		&s_slugrock.g_forceBlueWeaponMode,
		MTYPE_SPINCONTROL
	},

	{
		"g_healthRegen",
		"Regenerate health:",
		{ "0", "1", 0 }, 
		{ NULL }, 
		&s_slugrock.g_healthRegen,
		MTYPE_RADIOBUTTON
	},

	{
		"cg_weaponMode",
		"Weapon mode:",
		{ "alternate", "random", "rl", "rg", "both", 0 }, 
		{ "Alternate", "Random", "Rocket Launcher", "Railgun", "Both", 0 },
		&s_slugrock.cg_weaponMode,
		MTYPE_SPINCONTROL
	},

	{
		"bot_weaponMode",
		"Weapon mode:",
		{ "alternate", "random", "rl", "rg", "both", 0 }, 
		{ "Alternate", "Random", "Rocket Launcher", "Railgun", "Both", 0 },
		&s_slugrock.bot_weaponMode,
		MTYPE_SPINCONTROL
	},

	{ NULL }
};

/*
=================
SlugRock_Event
=================
*/
static void SlugRock_Event( void* ptr, int event )
{
	switch (((menucommon_s*)ptr)->id)
	{
		case ID_BACK:
			if (event != QM_ACTIVATED)
				break;

			UI_PopMenu();
			break;
	}
}

/*
=================
SlugRock_GetCurrentValueIndex
=================
*/
int SlugRock_GetCurrentValueIndex( int id ) {
	int		n;
	char	cvar_value[MAX_INFO_VALUE];

	trap_Cvar_VariableStringBuffer( cvar_list[id].name, cvar_value, sizeof(cvar_value) );

	for(n=0; cvar_list[id].names[n]; n++) {
		if ( !Q_stricmp(cvar_list[id].names[n], cvar_value) ) {
			return n;
		}
	}
	return 0;
}

/*
=================
SlugRock_MenuDraw
=================
*/
static void SlugRock_MenuDraw( void )
{
	char			info[MAX_INFO_STRING];
	uiClientState_t cs;
	int				team;

	if (s_slugrock.g_forceWeaponMode.curvalue == 0) {
		s_slugrock.g_forceBlueWeaponMode.generic.flags &= ~QMF_GRAYED;
		s_slugrock.g_forceRedWeaponMode.generic.flags &= ~QMF_GRAYED;
		s_slugrock.cg_weaponMode.generic.flags &= ~QMF_GRAYED;
		s_slugrock.bot_weaponMode.generic.flags &= ~QMF_GRAYED;
	}
	else {
		s_slugrock.g_forceBlueWeaponMode.generic.flags |= QMF_GRAYED;
		s_slugrock.g_forceRedWeaponMode.generic.flags |= QMF_GRAYED;
		s_slugrock.cg_weaponMode.generic.flags |= QMF_GRAYED;
		s_slugrock.bot_weaponMode.generic.flags |= QMF_GRAYED;
	}

	trap_GetClientState( &cs );
	trap_GetConfigString( CS_PLAYERS + cs.clientNum, info, MAX_INFO_STRING );
	team = atoi( Info_ValueForKey( info, "t" ) );
	
	if (s_slugrock.g_forceRedWeaponMode.curvalue == 0) {
		s_slugrock.cg_weaponMode.generic.flags &= ~QMF_GRAYED;
	}
	else if ( team == TEAM_RED ) {
		s_slugrock.cg_weaponMode.generic.flags |= QMF_GRAYED;
	}
	
	if (s_slugrock.g_forceBlueWeaponMode.curvalue == 0) {
		s_slugrock.cg_weaponMode.generic.flags &= ~QMF_GRAYED;
	}
	else if ( team == TEAM_BLUE ) {
		s_slugrock.cg_weaponMode.generic.flags |= QMF_GRAYED;
	}

	Menu_Draw( &s_slugrock.menu );
}

/*
=================
SlugRock_MenuKey
=================
*/
static sfxHandle_t SlugRock_MenuKey( int key )
{
	return ( Menu_DefaultKey( &s_slugrock.menu, key ) );
}

/*
=================
SlugRock_Cache
=================
*/
void SlugRock_Cache( void )
{
	int	i;

	// touch all our pics
	for (i=0; ;i++)
	{
		if (!slugrock_artlist[i])
			break;
		trap_R_RegisterShaderNoMip(slugrock_artlist[i]);
	}
}

/*
=================
SlugRock_ApplyChanges
=================
*/
void SlugRock_ApplyChanges( void* ptr, int event ) {
	int		i, id;

	if ( event != QM_ACTIVATED )
		return;

	for(id=0; cvar_list[id].name; id++) {
		switch (cvar_list[id].type) {
			case MTYPE_SPINCONTROL:
				i = ((menulist_s*)cvar_list[id].menuitem)->curvalue;
				break;
			case MTYPE_RADIOBUTTON:
				i = ((menuradiobutton_s*)cvar_list[id].menuitem)->curvalue;
				break;
		}
		trap_Cvar_Set( cvar_list[id].name, cvar_list[id].names[i] );
	}

	UI_PopMenu();
}

/*
=================
SlugRock_AddItem
=================
*/
void SlugRock_AddItem( int id, int y, int x_offset ) {
	switch(cvar_list[id].type) {
		case MTYPE_SPINCONTROL:
		((menulist_s*)cvar_list[id].menuitem)->generic.type		= cvar_list[id].type;
		((menulist_s*)cvar_list[id].menuitem)->generic.flags	= QMF_PULSEIFFOCUS|QMF_SMALLFONT|QMF_CENTER_JUSTIFY;
		((menulist_s*)cvar_list[id].menuitem)->generic.x		= SCREEN_WIDTH/2+x_offset;
		((menulist_s*)cvar_list[id].menuitem)->generic.y		= y;
		((menulist_s*)cvar_list[id].menuitem)->generic.name		= cvar_list[id].longname;
		((menulist_s*)cvar_list[id].menuitem)->generic.id		= id;
		((menulist_s*)cvar_list[id].menuitem)->generic.callback	= SlugRock_Event;
		((menulist_s*)cvar_list[id].menuitem)->itemnames		= cvar_list[id].longnames;
		((menulist_s*)cvar_list[id].menuitem)->curvalue			= SlugRock_GetCurrentValueIndex( id );
		break;
		case MTYPE_RADIOBUTTON:
		((menuradiobutton_s*)cvar_list[id].menuitem)->generic.type		= cvar_list[id].type;
		((menuradiobutton_s*)cvar_list[id].menuitem)->generic.flags		= QMF_PULSEIFFOCUS|QMF_SMALLFONT|QMF_CENTER_JUSTIFY;
		((menuradiobutton_s*)cvar_list[id].menuitem)->generic.x			= SCREEN_WIDTH/2+x_offset;
		((menuradiobutton_s*)cvar_list[id].menuitem)->generic.y			= y;
		((menuradiobutton_s*)cvar_list[id].menuitem)->generic.name		= cvar_list[id].longname;
		((menuradiobutton_s*)cvar_list[id].menuitem)->generic.id		= id;
		((menuradiobutton_s*)cvar_list[id].menuitem)->generic.callback	= SlugRock_Event;
		((menuradiobutton_s*)cvar_list[id].menuitem)->curvalue			= SlugRock_GetCurrentValueIndex( id );
		break;
	}
	Menu_AddItem( &s_slugrock.menu, (void*) cvar_list[id].menuitem );
}

/*
=================
UI_SlugRockMenu
=================
*/
void UI_SlugRockMenu( void )
{
	int				y;
	int				id;

	// zero set all our globals
	memset( &s_slugrock, 0 ,sizeof(slugrock_t) );

	SlugRock_Cache();

	s_slugrock.menu.draw			= SlugRock_MenuDraw;
	s_slugrock.menu.key				= SlugRock_MenuKey;
	s_slugrock.menu.wrapAround		= qtrue;
	s_slugrock.menu.fullscreen		= qtrue;

	s_slugrock.banner.generic.type  = MTYPE_BTEXT;
	s_slugrock.banner.generic.x		= 320;
	s_slugrock.banner.generic.y		= 16;
	s_slugrock.banner.string		= "SLUGROCK SETTINGS";
	s_slugrock.banner.color			= color_white;
	s_slugrock.banner.style			= UI_CENTER;

	s_slugrock.framel.generic.type  = MTYPE_BITMAP;
	s_slugrock.framel.generic.name  = SLUGROCK_FRAMEL;
	s_slugrock.framel.generic.flags = QMF_INACTIVE;
	s_slugrock.framel.generic.x		= 0;  
	s_slugrock.framel.generic.y		= 78;
	s_slugrock.framel.width  	    = 256;
	s_slugrock.framel.height  		= 329;

	s_slugrock.framer.generic.type  = MTYPE_BITMAP;
	s_slugrock.framer.generic.name  = SLUGROCK_FRAMER;
	s_slugrock.framer.generic.flags = QMF_INACTIVE;
	s_slugrock.framer.generic.x		= 376;
	s_slugrock.framer.generic.y		= 76;
	s_slugrock.framer.width  	    = 256;
	s_slugrock.framer.height  		= 334;

	s_slugrock.back.generic.type	 = MTYPE_BITMAP;
	s_slugrock.back.generic.name     = SLUGROCK_BACK0;
	s_slugrock.back.generic.flags    = QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_slugrock.back.generic.callback = SlugRock_Event;
	s_slugrock.back.generic.id		 = ID_BACK;
	s_slugrock.back.generic.x		 = 0;
	s_slugrock.back.generic.y		 = 480-64;
	s_slugrock.back.width  			 = 128;
	s_slugrock.back.height  		 = 64;
	s_slugrock.back.focuspic         = SLUGROCK_BACK1;

	id = 0;
	y = 100;

	s_slugrock.server.generic.type		= MTYPE_PTEXT;
	s_slugrock.server.generic.y			= y;
	s_slugrock.server.generic.flags		= QMF_LEFT_JUSTIFY|QMF_INACTIVE;
	s_slugrock.server.generic.x			= SCREEN_WIDTH/2;
	s_slugrock.server.string			= "SERVER";
	s_slugrock.server.color				= color_orange;
	s_slugrock.server.style				= UI_CENTER|UI_SMALLFONT;
	y += 24;

	while (cvar_list[id].name[0] == 'g') {
		SlugRock_AddItem( id, y, 50 );
		id++;
		y += SMALLCHAR_HEIGHT;
	}

	y += 16;
	s_slugrock.client.generic.type		= MTYPE_PTEXT;
	s_slugrock.client.generic.y			= y;
	s_slugrock.client.generic.flags		= QMF_LEFT_JUSTIFY|QMF_INACTIVE;
	s_slugrock.client.generic.x			= SCREEN_WIDTH/2;
	s_slugrock.client.string			= "CLIENT";
	s_slugrock.client.color				= color_orange;
	s_slugrock.client.style				= UI_CENTER|UI_SMALLFONT;
	y += 24;

	while (cvar_list[id].name[0] == 'c') {
		SlugRock_AddItem( id, y, 0 );
		id++;
		y += SMALLCHAR_HEIGHT;
	}

	y += 16;
	s_slugrock.bots.generic.type		= MTYPE_PTEXT;
	s_slugrock.bots.generic.y			= y;
	s_slugrock.bots.generic.flags		= QMF_LEFT_JUSTIFY|QMF_INACTIVE;
	s_slugrock.bots.generic.x			= SCREEN_WIDTH/2;
	s_slugrock.bots.string				= "BOTS";
	s_slugrock.bots.color				= color_orange;
	s_slugrock.bots.style				= UI_CENTER|UI_SMALLFONT;
	y += 24;

	while (cvar_list[id].name && cvar_list[id].name[0] == 'b') {
		SlugRock_AddItem( id, y, 0 );
		id++;
		y += SMALLCHAR_HEIGHT;
	}

	s_slugrock.apply.generic.type     = MTYPE_BITMAP;
	s_slugrock.apply.generic.name     = SLUGROCK_ACCEPT0;
	s_slugrock.apply.generic.flags    = QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_slugrock.apply.generic.callback = SlugRock_ApplyChanges;
	s_slugrock.apply.generic.x        = 640;
	s_slugrock.apply.generic.y        = 480-64;
	s_slugrock.apply.width  		  = 128;
	s_slugrock.apply.height  		  = 64;
	s_slugrock.apply.focuspic         = SLUGROCK_ACCEPT1;

	Menu_AddItem( &s_slugrock.menu, (void*) &s_slugrock.banner );
	Menu_AddItem( &s_slugrock.menu, (void*) &s_slugrock.framel );
	Menu_AddItem( &s_slugrock.menu, (void*) &s_slugrock.framer );
	Menu_AddItem( &s_slugrock.menu, (void*) &s_slugrock.back );
	Menu_AddItem( &s_slugrock.menu, (void*) &s_slugrock.apply );
	Menu_AddItem( &s_slugrock.menu, (void*) &s_slugrock.server );
	Menu_AddItem( &s_slugrock.menu, (void*) &s_slugrock.client );
	Menu_AddItem( &s_slugrock.menu, (void*) &s_slugrock.bots );
	UI_PushMenu( &s_slugrock.menu );
}


