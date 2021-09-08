#ifndef SDL_FUNCS_H
#define SDL_FUNCS_H

#ifdef __WIN32
  #include <SDL.h>
  #include <SDL_image.h>
  #include <SDL_ttf.h>
#else
  #include <SDL2/SDL.h>
  #include <SDL2/SDL_image.h>
  #include <SDL2/SDL_ttf.h>
#endif


#include <stdio.h>
#include <string>

enum buttonSprites {
    MOUSE_OUT = 0,
    MOUSE_OVER = 1,
    MOUSE_DOWN = 2,
    MOUSE_UP = 3,
    TOTAL_SPRITES = 4
};

class TextureWrapper {
private:

  // hardware texture
  SDL_Texture* mTexture;

  //  image dimensions
  int mWidth;
  int mHeight;

  // holds sprite size
  int spriteSize;

  // number of animation frames per texture type
  int animationFrames;

  // number of texture types
  int textureTypes;

  // usefull for animation
  double* mAnimationTimer = NULL;
  int* mAnimationIndex = NULL;

  // vector for holding SDL_Rect(s) for animation purposes
  // vecto structure :  element 1 -> element animationFrames : holds the animation sprites for the first texture type
  //                    element animationFrames + 1 -> element 2*animationFrames : `` for the second texture type
  std::vector<SDL_Rect> animationRects;

public:


  TextureWrapper() : mTexture{NULL}, mWidth{0}, mHeight{0}, animationFrames{0}, textureTypes{1} {}
  TextureWrapper(int nbTextures, int nbFrames, int size) : mTexture{NULL}, mWidth{0}, mHeight{0} {
    textureTypes = nbTextures;
    animationFrames = nbFrames;
    spriteSize = size;

    mAnimationTimer = new double;
    mAnimationIndex = new int;

    *mAnimationTimer = 0;
    *mAnimationIndex = 0;

    // initializing SDL_Rect(s) for different texture types
    for (int i=0; i<textureTypes; ++i) {
      for (int j=0; j<animationFrames; ++j) {
        animationRects.push_back(SDL_Rect{j*spriteSize, i*spriteSize, spriteSize, spriteSize});
      }
    }
  }

  // deallocate memory
  ~TextureWrapper() {
    free();
    delete mAnimationTimer;
    delete mAnimationIndex;
  }

  bool loadFromFile(SDL_Renderer* pRenderer, std::string path) {

    // free previously allocated members
    free();

    // finall texture that will be assigned to pTexture
    SDL_Texture* newTexture = NULL;

    SDL_Surface* tempSurface = NULL;
    tempSurface = IMG_Load(path.c_str());

    if (tempSurface == NULL) {
      printf("Failed @loadFromFile. IMG Error: %s", IMG_GetError());
      return false;
    }


    // Setting color key (transparent pixel when rendering over other textures)
    if (SDL_SetColorKey( tempSurface, SDL_TRUE, SDL_MapRGB( tempSurface -> format, 0x00, 0xFF, 0xFF) ) ) {
      printf("Failed @loadFromFile. SDL Error: %s", SDL_GetError());
      return false;
    }

    newTexture = SDL_CreateTextureFromSurface(pRenderer, tempSurface);
    if (newTexture == NULL) {
      printf("Failed @loadFromFile. SDL Error: %s", SDL_GetError());
      return false;
    }

    // if everything went right, set texture width and height to the image's dimensions
    mWidth = tempSurface->w; mHeight = tempSurface->h;

    // get rid of temporary surface memory allocation space
    SDL_FreeSurface(tempSurface);

    // set the SDL_Texture* member to newTexture
    mTexture = newTexture;

    return true;

  }

  // creates image from font string
  bool loadFromRenderedText(SDL_Renderer* pRenderer, std::string text, TTF_Font* fontStyle, SDL_Color textColor) {

    // free preexisting texture
    free();

    // Render text surface
    SDL_Surface* newSurface = TTF_RenderText_Blended_Wrapped( fontStyle, text.c_str(), textColor, 400);

    if ( newSurface == NULL) {
      printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
      return false;
    } else {

      // Create texture from text surface
      mTexture = SDL_CreateTextureFromSurface(pRenderer, newSurface);
      if (mTexture == NULL) {

        printf("Unable to load texture from surface! SDL Error: %s\n", SDL_GetError());
        return false;

      } else {

        // get texture dimensions
        mWidth = newSurface->w;
        mHeight = newSurface->h;

      }

    }

  // freeing allcated surface memory.
  SDL_FreeSurface(newSurface);
  return true;
  }

  // deallocating texture
  void free() {

    if (mTexture != NULL) {

      // deallocate previous texture pointer and set it to NULL
      SDL_DestroyTexture(mTexture);
      mTexture = NULL;

      // set width and height to default values (zeros)
      mWidth = 0;
      mHeight = 0;
    }

  }

  // color modulation
  void setColor(Uint8 R, Uint8 G, Uint8 B) {
    SDL_SetTextureColorMod(mTexture, R, G, B);
  }

  // alpha modulation
  void setAlpha(Uint8 A) {
    SDL_SetTextureAlphaMod(mTexture, A);
  }

  void setSpriteSize(int size) {
    spriteSize = size;
  }

  void setAnimationFrames(int nbFrames) {
    animationFrames = nbFrames;
  }

  void setTextureTypes(int nbTextures) {
    textureTypes = nbTextures;
  }
  // blend mode - necessary for alpha modulation
  void setBlendMode(SDL_BlendMode blending) {
    SDL_SetTextureBlendMode(mTexture, blending);
  }

  // Renders the attached texture
  void render(  SDL_Renderer* pRenderer, int x = 0, int y = 0, int size = 0, SDL_Rect* selectFrom = NULL, const double angle = 0.0,
                SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE) {

    // set rendering space and position
    SDL_Rect newTextureRect = {x, y, mWidth, mHeight};

    if ((selectFrom != NULL) && (size == 0)) {

      newTextureRect.w = selectFrom->w;
      newTextureRect.h = selectFrom->h;

    } else if (size != 0) {
      newTextureRect.w = size;
      newTextureRect.h = size;
    }

    SDL_RenderCopyEx(pRenderer, mTexture, selectFrom, &newTextureRect, angle, center, flip);

  }

  // render texture animated
  void renderAnimated(  SDL_Renderer* pRenderer, int x, int y, int sizeToRender, int textureIndex, float delatTime,
                        double animationSpeed, double* pAnimationTimer = NULL, int* pAnimationIndex = NULL, const double angle = 0.0,
                        SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE) {

                          int* animationIndex;
                          double* animationTimer;

                          if ((pAnimationTimer != NULL) && (pAnimationIndex != NULL)) {
                            animationTimer = pAnimationTimer;
                            animationIndex = pAnimationIndex;
                          } else {
                            animationTimer = mAnimationTimer;
                            animationIndex = mAnimationIndex;
                          }

                          *animationTimer += delatTime;

                          // pass to the next framev after ceertain amount of time
                          if ( static_cast<int>((*animationTimer)*animationSpeed) != 0) {
                            if ( ++(*animationIndex) >= animationFrames) {
                              *animationIndex = 0;
                            }
                            *animationTimer = 0;
                          }

                        // render corresponding texture frame at (x, y) coordonates
                        SDL_Rect renderTo{x, y, sizeToRender, sizeToRender};
                        SDL_RenderCopyEx( pRenderer, mTexture, &animationRects[(*animationIndex)+(textureIndex*animationFrames)],
                                          &renderTo, angle, center, flip);
                      }

  int getWidth() const {
    return mWidth;
  }

  int getHeight() const {
    return mHeight;
  }

};

#endif
