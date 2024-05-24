/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org

  Original:
  https://learnopengl.com/In-Practice/Text-Rendering

  Modified by Reto Stockli to work without shaders but direct vertex texturing.

  Should Replace FTGL Dependency in xpopengc

  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

  =========================================================================*/

//Include our header file.
#include "ogcFreetype.h"

namespace OpenGC {

  Freetype
  ::Freetype(char* name, int size)
  {

    m_Name = new char[512];

    strcpy(m_Name, name);

    m_Size = size;

    m_Loaded = false;

  }

  Freetype
  ::~Freetype() {
  }

  // Load the font. Cannot be done at object initialization since we also
  // perform OpenGL operations.
  int Freetype
  ::LoadFont()
  {

    // FreeType
    // --------
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft) != 0)
      {
        printf("ERROR::FREETYPE: Could not init FreeType Library\n");
        return -1;
      }
	
    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, m_Name, 0, &face) != 0) {
      printf("ERROR::FREETYPE: Failed to load font\n");
      return -1;
    }
    else {
      // set size to load glyphs as
      //FT_Set_Pixel_Sizes(face, m_Size, m_Size);
      FT_Set_Pixel_Sizes(face, 0, m_Size);

      // disable byte-alignment restriction
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      FT_UInt index;
      FT_ULong c = FT_Get_First_Char(face, &index);

      /* loop through available characters, but do not load above the standard ascii set */
      while (c < max_char) {

	  // Load character glyph 
	  if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0)
            {
	      printf("ERROR::FREETYTPE: Failed to load Glyph\n");
	      continue;
            }
	  
	  // generate texture
	  unsigned int texture;
	  glGenTextures(1, &texture);
	  glBindTexture(GL_TEXTURE_2D, texture);

	  /* Make a RGBA texture where the glyph bitmap is alpha and all other channels are 1.0 */
	  GLubyte pixels[face->glyph->bitmap.width*face->glyph->bitmap.rows*4];
	  for (unsigned int j=0;j<face->glyph->bitmap.rows;j++) {
	    for (unsigned int i=0;i<face->glyph->bitmap.width;i++) {
	      pixels[j*4*face->glyph->bitmap.width+i*4+0] = 255;
	      pixels[j*4*face->glyph->bitmap.width+i*4+1] = 255;
	      pixels[j*4*face->glyph->bitmap.width+i*4+2] = 255;
	      pixels[j*4*face->glyph->bitmap.width+i*4+3] =
		face->glyph->bitmap.buffer[j*face->glyph->bitmap.width+i];
	    }
	  }

	  glTexImage2D(GL_TEXTURE_2D,
		       0,
		       GL_RGBA,
		       face->glyph->bitmap.width,
		       face->glyph->bitmap.rows,
		       0,
		       GL_RGBA,
		       GL_UNSIGNED_BYTE,
		       pixels
		       );

	  // Set texture options
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
	  // now store character for later use
	  Character character = {
	    texture,
	    glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
	    glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
	    static_cast<unsigned int>(face->glyph->advance.x)
	  };
	  Characters.insert(std::pair<unsigned char, Character>(c, character));
	  glBindTexture(GL_TEXTURE_2D, 0);

	  /* retrieve next available character */
	  c = FT_Get_Next_Char(face, c, &index);
	  if (index == 0) break; // if FT_Get_Next_Char write 0 to index then
	  // have no more characters in font face

        }
      
    }
    
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    
    printf("Freetype: Finished loading font: %s\n",m_Name);

    return 0;
    
  }

  // render line of text
  // -------------------
  void
  Freetype
  ::RenderText(std::string text, float x, float y, float scalex, float scaley)
  {

    if (!m_Loaded) {

      if (LoadFont() != 0) {
	printf("Error Loading Freetype Font: %s \n",m_Name);
      }

      m_Loaded = true;

    }

    if (m_Loaded) {

      glEnable(GL_TEXTURE_2D);
    
      // iterate through all characters
      std::string::const_iterator c;
      for (c = text.begin(); c != text.end(); c++) 
	{
	  
	  Character ch = Characters[*c];

	  GLfloat xpos = x + ch.Bearing.x * scalex;
	  GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scaley;

	  GLfloat w = ch.Size.x * scalex;
	  GLfloat h = ch.Size.y * scaley;

	  glBindTexture(GL_TEXTURE_2D, ch.TextureID);
	  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	  //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	  //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	  glBegin(GL_TRIANGLES);
	  glTexCoord2f(0.0f, 0.0f); glVertex2f(xpos,   ypos+h);
	  glTexCoord2f(0.0f, 1.0f); glVertex2f(xpos,   ypos);
	  glTexCoord2f(1.0f, 1.0f); glVertex2f(xpos+w, ypos);
	  glTexCoord2f(0.0f, 0.0f); glVertex2f(xpos,   ypos+h);
	  glTexCoord2f(1.0f, 1.0f); glVertex2f(xpos+w, ypos);
	  glTexCoord2f(1.0f, 0.0f); glVertex2f(xpos+w, ypos+h);
	  glEnd();
	  
	  glBindTexture(GL_TEXTURE_2D,0);
	  
	  x += (ch.Advance >> 6) * scalex; 
	
	}
   
      glDisable (GL_TEXTURE_2D);

    }
 
  }

}
