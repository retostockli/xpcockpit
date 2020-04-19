/*=============================================================================

  This is the ogcA320EWDAnnunciator.cpp file, part of the OpenGC subproject
  of the XpIoCards project (https://sourceforge.net/projects/xpiocards/)

  === Airbus A320 style Engine/Warning Display - Annunciator Area ===

  Created:
    Date:        2011-11-14
    Author:      Hans Jansen
    last change: 2020-02-06
    (see ogcSkeletonGauge.cpp for more details)

  Copyright (C) 2011-2020 Hans Jansen (hansjansen@users.sourceforge.net)
  and/or                  Reto Stöckli (stockli@users.sourceforge.net)

  This program is free software: you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>. 

===============================================================================

  The OpenGC subproject has been derived from:
    OpenGC - The Open Source Glass Cockpit Project
    Copyright (c) 2001-2003 Damion Shelton

=============================================================================*/

#include <math.h>

#include "ogcA320EWD.h"
#include "ogcA320EWDAnnunciator.h"

namespace OpenGC {

  A320EWDAnnunciator::A320EWDAnnunciator () {
    printf("A320EWDAnnunciator constructed\n");

    m_Font = m_pFontManager->LoadFont ((char*) "CockpitScreens.ttf");

    m_PhysicalPosition.x = 0;
    m_PhysicalPosition.y = 0;

    m_PhysicalSize.x = 0;
    m_PhysicalSize.y = 0;

    m_Scale.x = 1.0;
    m_Scale.y = 1.0;
  }

  A320EWDAnnunciator::~A320EWDAnnunciator () {}

  void A320EWDAnnunciator::Render () {

    bool coldAndDark = true;

    GaugeComponent::Render ();

      // The datarefs we want to use on this instrument

    // Note: this dataref is created and maintained by the a320_overhead.c module
    int *cold_and_dark = link_dataref_int ("xpserver/cold_and_dark");
    if (*cold_and_dark == INT_MISS) coldAndDark = true; else coldAndDark = (*cold_and_dark != 0) ? true : false;

    unsigned char *ewd_1a = link_dataref_byte_arr ("AirbusFBW/EWD1aText", 36, -1); if (strlen((char*)ewd_1a) > 36) ewd_1a[36] = '\0';
    unsigned char *ewd_1b = link_dataref_byte_arr ("AirbusFBW/EWD1bText", 36, -1); if (strlen((char*)ewd_1b) > 36) ewd_1b[36] = '\0';
    unsigned char *ewd_1g = link_dataref_byte_arr ("AirbusFBW/EWD1gText", 36, -1); if (strlen((char*)ewd_1g) > 36) ewd_1g[36] = '\0';
    unsigned char *ewd_1r = link_dataref_byte_arr ("AirbusFBW/EWD1rText", 36, -1); if (strlen((char*)ewd_1r) > 36) ewd_1r[36] = '\0';
    unsigned char *ewd_1w = link_dataref_byte_arr ("AirbusFBW/EWD1wText", 36, -1); if (strlen((char*)ewd_1w) > 36) ewd_1w[36] = '\0';
    unsigned char *ewd_2a = link_dataref_byte_arr ("AirbusFBW/EWD2aText", 36, -1); if (strlen((char*)ewd_2a) > 36) ewd_2a[36] = '\0';
    unsigned char *ewd_2b = link_dataref_byte_arr ("AirbusFBW/EWD2bText", 36, -1); if (strlen((char*)ewd_2b) > 36) ewd_2b[36] = '\0';
    unsigned char *ewd_2g = link_dataref_byte_arr ("AirbusFBW/EWD2gText", 36, -1); if (strlen((char*)ewd_2g) > 36) ewd_2g[36] = '\0';
    unsigned char *ewd_2r = link_dataref_byte_arr ("AirbusFBW/EWD2rText", 36, -1); if (strlen((char*)ewd_2r) > 36) ewd_2r[36] = '\0';
    unsigned char *ewd_2w = link_dataref_byte_arr ("AirbusFBW/EWD2wText", 36, -1); if (strlen((char*)ewd_2w) > 36) ewd_2w[36] = '\0';
    unsigned char *ewd_3a = link_dataref_byte_arr ("AirbusFBW/EWD3aText", 36, -1); if (strlen((char*)ewd_3a) > 36) ewd_3a[36] = '\0';
    unsigned char *ewd_3b = link_dataref_byte_arr ("AirbusFBW/EWD3bText", 36, -1); if (strlen((char*)ewd_3b) > 36) ewd_3b[36] = '\0';
    unsigned char *ewd_3g = link_dataref_byte_arr ("AirbusFBW/EWD3gText", 36, -1); if (strlen((char*)ewd_3g) > 36) ewd_3g[36] = '\0';
    unsigned char *ewd_3r = link_dataref_byte_arr ("AirbusFBW/EWD3rText", 36, -1); if (strlen((char*)ewd_3r) > 36) ewd_3r[36] = '\0';
    unsigned char *ewd_3w = link_dataref_byte_arr ("AirbusFBW/EWD3wText", 36, -1); if (strlen((char*)ewd_3w) > 36) ewd_3w[36] = '\0';
    unsigned char *ewd_4a = link_dataref_byte_arr ("AirbusFBW/EWD4aText", 36, -1); if (strlen((char*)ewd_4a) > 36) ewd_4a[36] = '\0';
    unsigned char *ewd_4b = link_dataref_byte_arr ("AirbusFBW/EWD4bText", 36, -1); if (strlen((char*)ewd_4b) > 36) ewd_4b[36] = '\0';
    unsigned char *ewd_4g = link_dataref_byte_arr ("AirbusFBW/EWD4gText", 36, -1); if (strlen((char*)ewd_4g) > 36) ewd_4g[36] = '\0';
    unsigned char *ewd_4r = link_dataref_byte_arr ("AirbusFBW/EWD4rText", 36, -1); if (strlen((char*)ewd_4r) > 36) ewd_4r[36] = '\0';
    unsigned char *ewd_4w = link_dataref_byte_arr ("AirbusFBW/EWD4wText", 36, -1); if (strlen((char*)ewd_4w) > 36) ewd_4w[36] = '\0';
    unsigned char *ewd_5a = link_dataref_byte_arr ("AirbusFBW/EWD5aText", 36, -1); if (strlen((char*)ewd_5a) > 36) ewd_5a[36] = '\0';
    unsigned char *ewd_5b = link_dataref_byte_arr ("AirbusFBW/EWD5bText", 36, -1); if (strlen((char*)ewd_5b) > 36) ewd_5b[36] = '\0';
    unsigned char *ewd_5g = link_dataref_byte_arr ("AirbusFBW/EWD5gText", 36, -1); if (strlen((char*)ewd_5g) > 36) ewd_5g[36] = '\0';
    unsigned char *ewd_5r = link_dataref_byte_arr ("AirbusFBW/EWD5rText", 36, -1); if (strlen((char*)ewd_5r) > 36) ewd_5r[36] = '\0';
    unsigned char *ewd_5w = link_dataref_byte_arr ("AirbusFBW/EWD5wText", 36, -1); if (strlen((char*)ewd_5w) > 36) ewd_5w[36] = '\0';
    unsigned char *ewd_6a = link_dataref_byte_arr ("AirbusFBW/EWD6aText", 36, -1); if (strlen((char*)ewd_6a) > 36) ewd_6a[36] = '\0';
    unsigned char *ewd_6b = link_dataref_byte_arr ("AirbusFBW/EWD6bText", 36, -1); if (strlen((char*)ewd_6b) > 36) ewd_6b[36] = '\0';
    unsigned char *ewd_6g = link_dataref_byte_arr ("AirbusFBW/EWD6gText", 36, -1); if (strlen((char*)ewd_6g) > 36) ewd_6g[36] = '\0';
    unsigned char *ewd_6r = link_dataref_byte_arr ("AirbusFBW/EWD6rText", 36, -1); if (strlen((char*)ewd_6r) > 36) ewd_6r[36] = '\0';
    unsigned char *ewd_6w = link_dataref_byte_arr ("AirbusFBW/EWD6wText", 36, -1); if (strlen((char*)ewd_6w) > 36) ewd_6w[36] = '\0';
    unsigned char *ewd_7a = link_dataref_byte_arr ("AirbusFBW/EWD7aText", 36, -1); if (strlen((char*)ewd_7a) > 36) ewd_7a[36] = '\0';
    unsigned char *ewd_7b = link_dataref_byte_arr ("AirbusFBW/EWD7bText", 36, -1); if (strlen((char*)ewd_7b) > 36) ewd_7b[36] = '\0';
    unsigned char *ewd_7g = link_dataref_byte_arr ("AirbusFBW/EWD7gText", 36, -1); if (strlen((char*)ewd_7g) > 36) ewd_7g[36] = '\0';
    unsigned char *ewd_7r = link_dataref_byte_arr ("AirbusFBW/EWD7rText", 36, -1); if (strlen((char*)ewd_7r) > 36) ewd_7r[36] = '\0';
    unsigned char *ewd_7w = link_dataref_byte_arr ("AirbusFBW/EWD7wText", 36, -1); if (strlen((char*)ewd_7w) > 36) ewd_7w[36] = '\0';

    if (!coldAndDark) {

      // The instrument display
      m_pFontManager->SetSize(m_Font, 14, 14);

      glColor3ub (COLOR_AMBER);
      m_pFontManager->Print (10, 119, (const char*) ewd_1a, m_Font); // Line 1 amber
      m_pFontManager->Print (10, 100, (const char*) ewd_2a, m_Font); // Line 2 amber
      m_pFontManager->Print (10,  81, (const char*) ewd_3a, m_Font); // Line 3 amber
      m_pFontManager->Print (10,  62, (const char*) ewd_4a, m_Font); // Line 4 amber
      m_pFontManager->Print (10,  43, (const char*) ewd_5a, m_Font); // Line 5 amber
      m_pFontManager->Print (10,  24, (const char*) ewd_6a, m_Font); // Line 6 amber
      m_pFontManager->Print (10,   5, (const char*) ewd_7a, m_Font); // Line 7 amber
      glColor3ub (COLOR_CYAN);
      m_pFontManager->Print (10, 119, (const char*) ewd_1b, m_Font); // Line 1 blue
      m_pFontManager->Print (10, 100, (const char*) ewd_2b, m_Font); // Line 2 blue
      m_pFontManager->Print (10,  81, (const char*) ewd_3b, m_Font); // Line 3 blue
      m_pFontManager->Print (10,  62, (const char*) ewd_4b, m_Font); // Line 4 blue
      m_pFontManager->Print (10,  43, (const char*) ewd_5b, m_Font); // Line 5 blue
      m_pFontManager->Print (10,  24, (const char*) ewd_6b, m_Font); // Line 6 blue
      m_pFontManager->Print (10,   5, (const char*) ewd_7b, m_Font); // Line 7 blue
      glColor3ub (COLOR_GREEN);
      m_pFontManager->Print (10, 119, (const char*) ewd_1g, m_Font); // Line 1 green
      m_pFontManager->Print (10, 100, (const char*) ewd_2g, m_Font); // Line 2 green
      m_pFontManager->Print (10,  81, (const char*) ewd_3g, m_Font); // Line 3 green
      m_pFontManager->Print (10,  62, (const char*) ewd_4g, m_Font); // Line 4 green
      m_pFontManager->Print (10,  43, (const char*) ewd_5g, m_Font); // Line 5 green
      m_pFontManager->Print (10,  24, (const char*) ewd_6g, m_Font); // Line 6 green
      m_pFontManager->Print (10,   5, (const char*) ewd_7g, m_Font); // Line 7 green
      glColor3ub (COLOR_RED);
      m_pFontManager->Print (10, 119, (const char*) ewd_1r, m_Font); // Line 1 red
      m_pFontManager->Print (10, 100, (const char*) ewd_2r, m_Font); // Line 2 red
      m_pFontManager->Print (10,  81, (const char*) ewd_3r, m_Font); // Line 3 red
      m_pFontManager->Print (10,  62, (const char*) ewd_4r, m_Font); // Line 4 red
      m_pFontManager->Print (10,  43, (const char*) ewd_5r, m_Font); // Line 5 red
      m_pFontManager->Print (10,  24, (const char*) ewd_6r, m_Font); // Line 6 red
      m_pFontManager->Print (10,   5, (const char*) ewd_7r, m_Font); // Line 7 red
      glColor3ub (COLOR_WHITE);
      m_pFontManager->Print (10, 119, (const char*) ewd_1w, m_Font); // Line 1 white
      m_pFontManager->Print (10, 100, (const char*) ewd_2w, m_Font); // Line 2 white
      m_pFontManager->Print (10,  81, (const char*) ewd_3w, m_Font); // Line 3 white
      m_pFontManager->Print (10,  62, (const char*) ewd_4w, m_Font); // Line 4 white
      m_pFontManager->Print (10,  43, (const char*) ewd_5w, m_Font); // Line 5 white
      m_pFontManager->Print (10,  24, (const char*) ewd_6w, m_Font); // Line 6 white
      m_pFontManager->Print (10,   5, (const char*) ewd_7w, m_Font); // Line 7 white

    } // End if (!coldAndDark)

  } // End Render()

}
