/*=========================================================================

  OpenGC - The Open Source Glass Cockpit Project
  Please see our web site at http://www.opengc.org

  Original:
  https://learnopengl.com/In-Practice/Text-Rendering

  Improved Version:
  https://github.com/johnWRS/LearnOpenGLTextRenderingImprovement

  Should Replace FTGL Dependency in xpopengc

  See Copyright.txt or http://www.opengc.org/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even 
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
  PURPOSE.  See the above copyright notice for more information.

  =========================================================================*/

#ifndef ogcFreetype_h
#define ogcFreetype_h

#include "ogcGLHeaders.h"

//FreeType Headers
#include <ft2build.h>
#include FT_FREETYPE_H

//Some STL headers
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

///Wrap everything in a namespace, that we can use common
///function names like "print" without worrying about
///overlapping with anyone else's code.
namespace OpenGC {
    
  using namespace glm;
  using namespace std;
  
  class Freetype {


  public:
    Freetype(char* name, int size);
    ~Freetype();
    
    void RenderText(std::string text, float x, float y, float scalex, float scaley);

  protected:
    /* Font Loading and Texture Generation */
    int LoadFont();

    /** The name of the font */
    char* m_Name;

    /** The initial pixel size of the font */
    int m_Size;

    /* maximum character number to load (restrict to ascii set) */
    long unsigned int max_char = 256;

    /* Font is already loaded */
    bool m_Loaded;
   
    /// Holds all state information relevant to a character as loaded using FreeType
    struct Character {
      unsigned int TextureID; // ID handle of the glyph texture
      glm::ivec2   Size;      // Size of glyph
      glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
      unsigned int Advance;   // Horizontal offset to advance to next glyph
    };

    std::map<GLchar, Character> Characters;
 

  };
    
}

#endif
