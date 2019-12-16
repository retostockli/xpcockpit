/* This is the a320_mcdu_keys.c code which contains code for how to communicate with my Airbus
   A320 MCDU hardware (pushbuttons & LEDs only) connected to the OpenCockpits IOCARDS USB device.
   To be used with QPAC V2.0 Basic A320 Airbus!

   Note that the display module is part of the OpenGC subproject.

   Copyright (C) 2014 Hans Jansen

   This program is free software: you can redistribute it and/or modify it under the 
   terms of the GNU General Public License as published by the Free Software Foundation, 
   either version 3 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program.  
   If not, see <http://www.gnu.org/licenses/>. 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "libiocards.h"
#include "serverdata.h"
#include "a320_mcdu_keys.h"

// Note: the key sequence is dependent on your wiring of the MCDU !!

int nkeys = 89; // number of MCDU keys + 1
enum mcdu_keys { // nuxx = unused key nr. xx
//	nu00,
	nu01,	nu02,	lsk2l,	lsk1l,
	lsk4l,	lsk3l,	lsk6l,	lsk5l,
	nu09,	nu10,	nu11,	nu12,
	nu13,	nu14,	nu15,	nu16,
	key8,	key0,	key2,	key5,
	slup,	sldn,	prog,	rnav,
	key7,	keydec,	key1,	key4,
	sllt,	slrt,	dir,	fpln,
	keyu,	keyz,	keyk,	keyp,
	keya,	keyf,	perf,	fpred,
	key9,	keypm,	key3,	key6,
	airpt,	blnka,	nu47,	nu48,
	keyw,	keysp,	keym,	keyr,
	keyc,	keyh,	data,	atcc,
	keyv,	keysl,	keyl,	keyq,
	keyb,	keyg,	init,	sfpln,
	keyy,	keyclr,	keyo,	keyt,
	keye,	keyj,	nu71,	nu72,
	keyx,	keyovf,	keyn,	keys,
	keyd,	keyi,	blnkm,	menu,
	nu81,	nu82,	lsk2r,	lsk1r,
	lsk4r,	lsk3r,	lsk6r,	lsk5r
	};

char* MCDUPrefix[2] = { "AirbusFBW/MCDU1", "AirbusFBW/MCDU2" };
char* MCDUcommand[89] = { // Keys-card input codes into X-Plane Commands
//	"(not used)",									//	 0
	"(not used)",	"(not used)",		"LSK2L",		"LSK1L",	//  1 -  8
	"LSK4L",	"LSK3L",		"LSK6L",		"LSK5L",
	"(not used)",	"(not used)",		"(not used)",		"(not used)",	//  9 - 16
	"(not used)",	"(not used)",		"(not used)",		"(not used)",
	"Key8",		"Key0",			"Key2",			"Key5",		// 17 - 24
	"SlewUp",	"SlewDown",		"Prog",			"RadNav",
	"Key7",		"KeyDecimal",		"Key1",			"Key4",		// 25 - 32
	"SlewLeft",	"SlewRight",		"DirTo",		"Fpln",
	"KeyU",		"KeyZ",			"KeyK",			"KeyP",		// 33 - 40
	"KeyA",		"KeyF",			"Perf",			"(FUEL PRED)",			// Fpred ?
	"Key9",		"KeyPM",		"Key3",			"Key6",		// 41 - 48
	"Airport",	"(blank near AIRPORT)",	"(not used)",		"(not used)",
	"KeyW",		"KeySpace",		"KeyM",			"KeyR",		// 49 - 56
	"KeyC",		"KeyH",			"Data",			"(ATC COMM)",			// Atc ?
	"KeyV",		"KeySlash",		"KeyL",			"KeyQ",		// 57 - 64
	"KeyB",		"KeyG",			"Init",			"(SEC F-PLN)",			// Sfpln ?
	"KeyY",		"KeyClear",		"KeyO",			"KeyT",		// 65 - 72
	"KeyE",		"KeyJ",			"(not used)",		"(not used)",
	"KeyX",		"KeyOverfly",		"KeyN",			"KeyS",		// 73 - 80
	"KeyD",		"KeyI",			"(blank near MENU)",	"Menu",
	"(not used)",	"(not used)",		"LSK2R",		"LSK1R",	// 81 - 88
	"LSK4R",	"LSK3R",		"LSK6R",		"LSK5R"
};

int *cmnd[2][89]; // pointers for the valid commands
int init_flag = 1; // initialization phase marker
int mcdu_select = 0; // pseudo_device number for commands array
// Note: key #79 (blnkm: blank near MENU) configured for switching between MCDU#1 and MCDU#2

extern int verbose;

void a320_mcdu_keys(void)
{

  int i, ret = 0;
  int device = 2; // for left-hand MCDU; 3 for right-hand one - see .ini file!
  int key;
  char cmdName[90];

  // Note: this dataref is used by the a320_pedestal_mip.c module
  int *current_mcdu = link_dataref_int ("xpserver/current_mcdu");

    int *mcdu1_data = link_dataref_cmd_once ("AirbusFBW/MCDU1Data");
    int *mcdu1_dirto = link_dataref_cmd_once ("AirbusFBW/MCDU1DirTo");
    int *mcdu1_fpln = link_dataref_cmd_once ("AirbusFBW/MCDU1Fpln");
    int *mcdu1_init = link_dataref_cmd_once ("AirbusFBW/MCDU1Init");
    int *mcdu1_key0 = link_dataref_cmd_once ("AirbusFBW/MCDU1Key0");
    int *mcdu1_key1 = link_dataref_cmd_once ("AirbusFBW/MCDU1Key1");
    int *mcdu1_key2 = link_dataref_cmd_once ("AirbusFBW/MCDU1Key2");
    int *mcdu1_key3 = link_dataref_cmd_once ("AirbusFBW/MCDU1Key3");
    int *mcdu1_key4 = link_dataref_cmd_once ("AirbusFBW/MCDU1Key4");
    int *mcdu1_key5 = link_dataref_cmd_once ("AirbusFBW/MCDU1Key5");
    int *mcdu1_key6 = link_dataref_cmd_once ("AirbusFBW/MCDU1Key6");
    int *mcdu1_key7 = link_dataref_cmd_once ("AirbusFBW/MCDU1Key7");
    int *mcdu1_key8 = link_dataref_cmd_once ("AirbusFBW/MCDU1Key8");
    int *mcdu1_key9 = link_dataref_cmd_once ("AirbusFBW/MCDU1Key9");
    int *mcdu1_keya = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyA");
    int *mcdu1_keyb = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyB");
    int *mcdu1_keyc = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyC");
    int *mcdu1_keyclear = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyClear");
    int *mcdu1_keyd = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyD");
    int *mcdu1_keydecimal = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyDecimal");
    int *mcdu1_keye = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyE");
    int *mcdu1_keyf = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyF");
    int *mcdu1_keyg = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyG");
    int *mcdu1_keyh = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyH");
    int *mcdu1_keyi = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyI");
    int *mcdu1_keyj = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyJ");
    int *mcdu1_keyk = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyK");
    int *mcdu1_keyl = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyL");
    int *mcdu1_keym = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyM");
    int *mcdu1_keyn = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyN");
    int *mcdu1_keyo = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyO");
    int *mcdu1_keyoverfly = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyOverfly");
    int *mcdu1_keyp = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyP");
    int *mcdu1_keypm = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyPM");
    int *mcdu1_keyq = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyQ");
    int *mcdu1_keyr = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyR");
    int *mcdu1_keys = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyS");
    int *mcdu1_keyslash = link_dataref_cmd_once ("AirbusFBW/MCDU1KeySlash");
    int *mcdu1_keyspace = link_dataref_cmd_once ("AirbusFBW/MCDU1KeySpace");
    int *mcdu1_keyt = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyT");
    int *mcdu1_keyu = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyU");
    int *mcdu1_keyv = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyV");
    int *mcdu1_keyw = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyW");
    int *mcdu1_keyx = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyX");
    int *mcdu1_keyy = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyY");
    int *mcdu1_keyz = link_dataref_cmd_once ("AirbusFBW/MCDU1KeyZ");
    int *mcdu1_lsk1l = link_dataref_cmd_once ("AirbusFBW/MCDU1LSK1L");
    int *mcdu1_lsk1r = link_dataref_cmd_once ("AirbusFBW/MCDU1LSK1R");
    int *mcdu1_lsk2l = link_dataref_cmd_once ("AirbusFBW/MCDU1LSK2L");
    int *mcdu1_lsk2r = link_dataref_cmd_once ("AirbusFBW/MCDU1LSK2R");
    int *mcdu1_lsk3l = link_dataref_cmd_once ("AirbusFBW/MCDU1LSK3L");
    int *mcdu1_lsk3r = link_dataref_cmd_once ("AirbusFBW/MCDU1LSK3R");
    int *mcdu1_lsk4l = link_dataref_cmd_once ("AirbusFBW/MCDU1LSK4L");
    int *mcdu1_lsk4r = link_dataref_cmd_once ("AirbusFBW/MCDU1LSK4R");
    int *mcdu1_lsk5l = link_dataref_cmd_once ("AirbusFBW/MCDU1LSK5L");
    int *mcdu1_lsk5r = link_dataref_cmd_once ("AirbusFBW/MCDU1LSK5R");
    int *mcdu1_lsk6l = link_dataref_cmd_once ("AirbusFBW/MCDU1LSK6L");
    int *mcdu1_lsk6r = link_dataref_cmd_once ("AirbusFBW/MCDU1LSK6R");
    int *mcdu1_menu = link_dataref_cmd_once ("AirbusFBW/MCDU1Menu");
    int *mcdu1_perf = link_dataref_cmd_once ("AirbusFBW/MCDU1Perf");
    int *mcdu1_prog = link_dataref_cmd_once ("AirbusFBW/MCDU1Prog");
    int *mcdu1_radnav = link_dataref_cmd_once ("AirbusFBW/MCDU1RadNav");
    int *mcdu1_slewdown = link_dataref_cmd_once ("AirbusFBW/MCDU1SlewDown");
    int *mcdu1_slewleft = link_dataref_cmd_once ("AirbusFBW/MCDU1SlewLeft");
    int *mcdu1_slewright = link_dataref_cmd_once ("AirbusFBW/MCDU1SlewRight");
    int *mcdu1_slewup = link_dataref_cmd_once ("AirbusFBW/MCDU1SlewUp");

    int *mcdu2_data = link_dataref_cmd_once ("AirbusFBW/MCDU2Data");
    int *mcdu2_dirto = link_dataref_cmd_once ("AirbusFBW/MCDU2DirTo");
    int *mcdu2_fpln = link_dataref_cmd_once ("AirbusFBW/MCDU2Fpln");
    int *mcdu2_init = link_dataref_cmd_once ("AirbusFBW/MCDU2Init");
    int *mcdu2_key0 = link_dataref_cmd_once ("AirbusFBW/MCDU2Key0");
    int *mcdu2_key1 = link_dataref_cmd_once ("AirbusFBW/MCDU2Key1");
    int *mcdu2_key2 = link_dataref_cmd_once ("AirbusFBW/MCDU2Key2");
    int *mcdu2_key3 = link_dataref_cmd_once ("AirbusFBW/MCDU2Key3");
    int *mcdu2_key4 = link_dataref_cmd_once ("AirbusFBW/MCDU2Key4");
    int *mcdu2_key5 = link_dataref_cmd_once ("AirbusFBW/MCDU2Key5");
    int *mcdu2_key6 = link_dataref_cmd_once ("AirbusFBW/MCDU2Key6");
    int *mcdu2_key7 = link_dataref_cmd_once ("AirbusFBW/MCDU2Key7");
    int *mcdu2_key8 = link_dataref_cmd_once ("AirbusFBW/MCDU2Key8");
    int *mcdu2_key9 = link_dataref_cmd_once ("AirbusFBW/MCDU2Key9");
    int *mcdu2_keya = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyA");
    int *mcdu2_keyb = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyB");
    int *mcdu2_keyc = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyC");
    int *mcdu2_keyclear = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyClear");
    int *mcdu2_keyd = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyD");
    int *mcdu2_keydecimal = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyDecimal");
    int *mcdu2_keye = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyE");
    int *mcdu2_keyf = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyF");
    int *mcdu2_keyg = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyG");
    int *mcdu2_keyh = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyH");
    int *mcdu2_keyi = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyI");
    int *mcdu2_keyj = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyJ");
    int *mcdu2_keyk = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyK");
    int *mcdu2_keyl = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyL");
    int *mcdu2_keym = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyM");
    int *mcdu2_keyn = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyN");
    int *mcdu2_keyo = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyO");
    int *mcdu2_keyoverfly = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyOverfly");
    int *mcdu2_keyp = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyP");
    int *mcdu2_keypm = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyPM");
    int *mcdu2_keyq = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyQ");
    int *mcdu2_keyr = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyR");
    int *mcdu2_keys = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyS");
    int *mcdu2_keyslash = link_dataref_cmd_once ("AirbusFBW/MCDU2KeySlash");
    int *mcdu2_keyspace = link_dataref_cmd_once ("AirbusFBW/MCDU2KeySpace");
    int *mcdu2_keyt = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyT");
    int *mcdu2_keyu = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyU");
    int *mcdu2_keyv = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyV");
    int *mcdu2_keyw = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyW");
    int *mcdu2_keyx = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyX");
    int *mcdu2_keyy = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyY");
    int *mcdu2_keyz = link_dataref_cmd_once ("AirbusFBW/MCDU2KeyZ");
    int *mcdu2_lsk1l = link_dataref_cmd_once ("AirbusFBW/MCDU2LSK1L");
    int *mcdu2_lek1r = link_dataref_cmd_once ("AirbusFBW/MCDU2LSK1R");
    int *mcdu2_lsk2l = link_dataref_cmd_once ("AirbusFBW/MCDU2LSK2L");
    int *mcdu2_lsk2r = link_dataref_cmd_once ("AirbusFBW/MCDU2LSK2R");
    int *mcdu2_lsk3l = link_dataref_cmd_once ("AirbusFBW/MCDU2LSK3L");
    int *mcdu2_lsk3r = link_dataref_cmd_once ("AirbusFBW/MCDU2LSK3R");
    int *mcdu2_lsk4l = link_dataref_cmd_once ("AirbusFBW/MCDU2LSK4L");
    int *mcdu2_lsk4r = link_dataref_cmd_once ("AirbusFBW/MCDU2LSK4R");
    int *mcdu2_lsk5l = link_dataref_cmd_once ("AirbusFBW/MCDU2LSK5L");
    int *mcdu2_lsk5r = link_dataref_cmd_once ("AirbusFBW/MCDU2LSK5R");
    int *mcdu2_lsk6l = link_dataref_cmd_once ("AirbusFBW/MCDU2LSK6L");
    int *mcdu2_lsk6r = link_dataref_cmd_once ("AirbusFBW/MCDU2LSK6R");
    int *mcdu2_menu = link_dataref_cmd_once ("AirbusFBW/MCDU2Menu");
    int *mcdu2_perf = link_dataref_cmd_once ("AirbusFBW/MCDU2Perf");
    int *mcdu2_prog = link_dataref_cmd_once ("AirbusFBW/MCDU2Prog");
    int *mcdu2_radnav = link_dataref_cmd_once ("AirbusFBW/MCDU2RadNav");
    int *mcdu2_slewdown = link_dataref_cmd_once ("AirbusFBW/MCDU2SlewDown");
    int *mcdu2_slewleft = link_dataref_cmd_once ("AirbusFBW/MCDU2SlewLeft");
    int *mcdu2_slewright = link_dataref_cmd_once ("AirbusFBW/MCDU2SlewRight");
    int *mcdu2_slewup = link_dataref_cmd_once ("AirbusFBW/MCDU2SlewUp");

  if (init_flag) {
    if (*current_mcdu == INT_MISS) *current_mcdu = 0;	// preselect left MCDU
    *mcdu1_menu = keys_input(device, menu);		// preset menu pages
    *mcdu2_menu = keys_input(device, menu);
    init_flag = 0;

  } else {

    ret = keys_input(device, blnkm); // determine left/right MCDU
    if (ret == 1) {
      mcdu_select = 1 - mcdu_select; // switch left/right MCDU
      *current_mcdu = mcdu_select;
    }
    // switch one of the FM1/FM2 lights accordingly
    // (this is done in module a320_pedestal_mip.c)

    if (*current_mcdu == 0) {
      *mcdu1_data = keys_input(device, data);
      *mcdu1_dirto = keys_input(device, dir);
      *mcdu1_fpln = keys_input(device, fpln);
      *mcdu1_init = keys_input(device, init);
      *mcdu1_key0 = keys_input(device, key0);
      *mcdu1_key1 = keys_input(device, key1);
      *mcdu1_key2 = keys_input(device, key2);
      *mcdu1_key3 = keys_input(device, key3);
      *mcdu1_key4 = keys_input(device, key4);
      *mcdu1_key5 = keys_input(device, key5);
      *mcdu1_key6 = keys_input(device, key6);
      *mcdu1_key7 = keys_input(device, key7);
      *mcdu1_key8 = keys_input(device, key8);
      *mcdu1_key9 = keys_input(device, key9);
      *mcdu1_keya = keys_input(device, keya);
      *mcdu1_keyb = keys_input(device, keyb);
      *mcdu1_keyc = keys_input(device, keyc);
      *mcdu1_keyclear = keys_input(device, keyclr);
      *mcdu1_keyd = keys_input(device, keyd);
      *mcdu1_keydecimal = keys_input(device, keydec);
      *mcdu1_keye = keys_input(device, keye);
      *mcdu1_keyf = keys_input(device, keyf);
      *mcdu1_keyg = keys_input(device, keyg);
      *mcdu1_keyh = keys_input(device, keyh);
      *mcdu1_keyi = keys_input(device, keyi);
      *mcdu1_keyj = keys_input(device, keyj);
      *mcdu1_keyk = keys_input(device, keyk);
      *mcdu1_keyl = keys_input(device, keyl);
      *mcdu1_keym = keys_input(device, keym);
      *mcdu1_keyn = keys_input(device, keyn);
      *mcdu1_keyo = keys_input(device, keyo);
      *mcdu1_keyoverfly = keys_input(device, keyovf);
      *mcdu1_keyp = keys_input(device, keyp);
      *mcdu1_keypm = keys_input(device, keypm);
      *mcdu1_keyq = keys_input(device, keyq);
      *mcdu1_keyr = keys_input(device, keyr);
      *mcdu1_keys = keys_input(device, keys);
      *mcdu1_keyslash = keys_input(device, keysl);
      *mcdu1_keyspace = keys_input(device, keysp);
      *mcdu1_keyt = keys_input(device, keyt);
      *mcdu1_keyu = keys_input(device, keyu);
      *mcdu1_keyv = keys_input(device, keyv);
      *mcdu1_keyw = keys_input(device, keyw);
      *mcdu1_keyx = keys_input(device, keyx);
      *mcdu1_keyy = keys_input(device, keyy);
      *mcdu1_keyz = keys_input(device, keyz);
      *mcdu1_lsk1l = keys_input(device, lsk1l);
      *mcdu1_lsk1r = keys_input(device, lsk1r);
      *mcdu1_lsk2l = keys_input(device, lsk2l);
      *mcdu1_lsk2r = keys_input(device, lsk2r);
      *mcdu1_lsk3l = keys_input(device, lsk3l);
      *mcdu1_lsk3r = keys_input(device, lsk3r);
      *mcdu1_lsk4l = keys_input(device, lsk4l);
      *mcdu1_lsk4r = keys_input(device, lsk4r);
      *mcdu1_lsk5l = keys_input(device, lsk5l);
      *mcdu1_lsk5r = keys_input(device, lsk5r);
      *mcdu1_lsk6l = keys_input(device, lsk6l);
      *mcdu1_lsk6r = keys_input(device, lsk6r);
      *mcdu1_menu = keys_input(device, menu);
      *mcdu1_perf = keys_input(device, perf);
      *mcdu1_prog = keys_input(device, prog);
      *mcdu1_radnav = keys_input(device, rnav);
      *mcdu1_slewdown = keys_input(device, sldn);
      *mcdu1_slewleft = keys_input(device, sllt);
      *mcdu1_slewright = keys_input(device, slrt);
      *mcdu1_slewup = keys_input(device, slup);
    } else {
      *mcdu2_data = keys_input(device, data);
      *mcdu2_dirto = keys_input(device, dir);
      *mcdu2_fpln = keys_input(device, fpln);
      *mcdu2_init = keys_input(device, init);
      *mcdu2_key0 = keys_input(device, key0);
      *mcdu2_key1 = keys_input(device, key1);
      *mcdu2_key2 = keys_input(device, key2);
      *mcdu2_key3 = keys_input(device, key3);
      *mcdu2_key4 = keys_input(device, key4);
      *mcdu2_key5 = keys_input(device, key5);
      *mcdu2_key6 = keys_input(device, key6);
      *mcdu2_key7 = keys_input(device, key7);
      *mcdu2_key8 = keys_input(device, key8);
      *mcdu2_key9 = keys_input(device, key9);
      *mcdu2_keya = keys_input(device, keya);
      *mcdu2_keyb = keys_input(device, keyb);
      *mcdu2_keyc = keys_input(device, keyc);
      *mcdu2_keyclear = keys_input(device, keyclr);
      *mcdu2_keyd = keys_input(device, keyd);
      *mcdu2_keydecimal = keys_input(device, keydec);
      *mcdu2_keye = keys_input(device, keye);
      *mcdu2_keyf = keys_input(device, keyf);
      *mcdu2_keyg = keys_input(device, keyg);
      *mcdu2_keyh = keys_input(device, keyh);
      *mcdu2_keyi = keys_input(device, keyi);
      *mcdu2_keyj = keys_input(device, keyj);
      *mcdu2_keyk = keys_input(device, keyk);
      *mcdu2_keyl = keys_input(device, keyl);
      *mcdu2_keym = keys_input(device, keym);
      *mcdu2_keyn = keys_input(device, keyn);
      *mcdu2_keyo = keys_input(device, keyo);
      *mcdu2_keyoverfly = keys_input(device, keyovf);
      *mcdu2_keyp = keys_input(device, keyp);
      *mcdu2_keypm = keys_input(device, keypm);
      *mcdu2_keyq = keys_input(device, keyq);
      *mcdu2_keyr = keys_input(device, keyr);
      *mcdu2_keys = keys_input(device, keys);
      *mcdu2_keyslash = keys_input(device, keysl);
      *mcdu2_keyspace = keys_input(device, keysp);
      *mcdu2_keyt = keys_input(device, keyt);
      *mcdu2_keyu = keys_input(device, keyu);
      *mcdu2_keyv = keys_input(device, keyv);
      *mcdu2_keyw = keys_input(device, keyw);
      *mcdu2_keyx = keys_input(device, keyx);
      *mcdu2_keyy = keys_input(device, keyy);
      *mcdu2_keyz = keys_input(device, keyz);
      *mcdu2_lsk1l = keys_input(device, lsk1l);
      *mcdu2_lek1r = keys_input(device, lsk1r);
      *mcdu2_lsk2l = keys_input(device, lsk2l);
      *mcdu2_lsk2r = keys_input(device, lsk2r);
      *mcdu2_lsk3l = keys_input(device, lsk3l);
      *mcdu2_lsk3r = keys_input(device, lsk3r);
      *mcdu2_lsk4l = keys_input(device, lsk4l);
      *mcdu2_lsk4r = keys_input(device, lsk4r);
      *mcdu2_lsk5l = keys_input(device, lsk5l);
      *mcdu2_lsk5r = keys_input(device, lsk5r);
      *mcdu2_lsk6l = keys_input(device, lsk6l);
      *mcdu2_lsk6r = keys_input(device, lsk6r);
      *mcdu2_menu = keys_input(device, menu);
      *mcdu2_perf = keys_input(device, perf);
      *mcdu2_prog = keys_input(device, prog);
      *mcdu2_radnav = keys_input(device, rnav);
      *mcdu2_slewdown = keys_input(device, sldn);
      *mcdu2_slewleft = keys_input(device, sllt);
      *mcdu2_slewright = keys_input(device, slrt);
      *mcdu2_slewup = keys_input(device, slup);
    }
  }

}

