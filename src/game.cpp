#ifdef __WIN32
  #include <SDL.h>
  #include <SDL_image.h>
  #include <SDL_ttf.h>
#else
  #include <SDL2/SDL.h>
  #include <SDL2/SDL_image.h>
  #include <SDL2/SDL_ttf.h>
#endif


#include <string.h>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <chrono>

///////////////////////////////////////////////////////////////////////////////////////////////////////
// These new types caused extreme headache if declared and defined in each cooresponding header file !
///////////////////////////////////////////////////////////////////////////////////////////////////////
struct coord {
  int x;
  int y;
};

enum potion_state {
  NO_POTION,
  ENERGETIC_POTION,
  DISINFECTANT_GEL
};

enum virus_state {
  NO_VIRUS,
  WEAK_VIRUS,
  COVID19_VIRUS
};

enum direction {
  UP = 0,
  DOWN = 1,
  LEFT = 2,
  RIGHT = 3
};

enum user_input {
  START_POINT,
  END_POINT,
  PLAYER_DIRECTION,
  WEAK_VIRUS_POSITION,
  COVID19_VIRUS_POSITION,
  ENERGETIC_POTION_POSITION,
  DISINFECTANT_GEL_POSITION,
  INITIALIZING_AFTER_INPUT,
  DONE_INPUT
};

#include "SDL_FUNCs.h"
#include "labyrinthe.h"
#include "player.h"
#include "potion.h"
#include "virus.h"

int main() {

  /*--------------------------------------------------- Initialization -------------------------------------------*/
  // Initialize PNG loading
  int imgFlags = IMG_INIT_PNG;
  if (!( IMG_Init(imgFlags) & imgFlags)) {
    printf("SDL_image could not initialze! SDL_image Error: %s\n", IMG_GetError());
    return 1;
  }

  // Initializing SDL_ttf
  if (TTF_Init() == -1) {
    printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
    return 1;
  }
  /*---------------------------------------------------------------------------------------------------------------*/

  /*---------------------------------------------- Window, Renderer and Event--------------------------------------*/
  bool quit = false;

  SDL_Window* gWindow = SDL_CreateWindow(   "Maze Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                            0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);

  SDL_Renderer* gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);

  SDL_Event e;

  TTF_Font* gFont = NULL;
  SDL_Color textColor = {0xFF, 0xFF, 0xFF};

  // this holds the time (seconds) between the last two frames
  double deltaTime;

  // usefull for calculating deltaTime
  auto previousTimerCall = std::chrono::high_resolution_clock::now();

  // once start Position is chosen, determine possible directions for the player to head in
  bool possibleDirection[] = {false, false, false, false};    // UP, DOWN, LEFT, RIGHT

  // vector holding viruses
  std::vector<virus> virus_vector;

  // vector holding potions
  std::vector<potion> potion_vector;
  /*----------------------------------------------------------------------------------------------------------------*/

  /*-------------------------------------------------- Loading Media -----------------------------------------------*/
  // loading font style
  gFont = TTF_OpenFont("media/KarmaFuture.ttf", 28);
  if (gFont == NULL) {
    printf("Could not load font! SDL_ttf Error: %s\n", TTF_GetError());
    return 1;
  }

  // loading start postion text texture
  TextureWrapper startTT;
  if (!startTT.loadFromRenderedText(gRenderer, "Choose \nStarting Position!", gFont, textColor))
    return 1;

  // loadign end position text texture
  TextureWrapper endTT;
  if (!endTT.loadFromRenderedText(gRenderer, "Choose \nEnding Position!", gFont, textColor))
    return 1;

  // loading initial player direction text texture
  TextureWrapper dirTT;
  if (!dirTT.loadFromRenderedText(gRenderer, "Choose \nInitial Player Direction!", gFont, textColor))
    return 1;

  // loading Weak Virus position text texture
  TextureWrapper weakVirusTT;
  if (!weakVirusTT.loadFromRenderedText(gRenderer, "Choose \nInitial Position(s) of Weak Virus(es)!", gFont, textColor))
    return 1;

  // loading Covid19 Virus position text texture
  TextureWrapper covid19TT;
  if (!covid19TT.loadFromRenderedText(gRenderer, "Choose \nInitial Position(s) of Covid19 Virus(es)!", gFont, textColor))
    return 1;

  // loading Energetic Potion position text texture
  TextureWrapper enerPotionTT;
  if (!enerPotionTT.loadFromRenderedText(gRenderer, "Choose \nPosition(s) of Energetic Potion(s)!", gFont, textColor))
    return 1;

  // loading Disinfectant Gel position text texture
  TextureWrapper disGelTT;
  if (!disGelTT.loadFromRenderedText(gRenderer, "Choose \nPosition(s) of Disinfectant Gel!", gFont, textColor))
    return 1;

  // initializing timer text texture
  TextureWrapper timerTT;

  // loading Press Space text texture
  TextureWrapper spaceTT;
  if (!spaceTT.loadFromRenderedText(gRenderer, "To continue,\nPress Space!", gFont, textColor))
    return 1;

  // loading game won text texture
  TextureWrapper gameWonTT;
  if (!gameWonTT.loadFromRenderedText(gRenderer, "You Won!", gFont, textColor))
    return 1;

  // loading game lost text texture
  TextureWrapper gameLostTT;
  if (!gameLostTT.loadFromRenderedText(gRenderer, "You Lost!", gFont, textColor))
    return 1;

  // loading press Alt + F4 to quit text texture
  TextureWrapper quitTT;
  if (!quitTT.loadFromRenderedText(gRenderer, "Press ALT + F4 to Quit!", gFont, textColor))
    return 1;

  //988f86
  // laoding characters texture
  TextureWrapper charactersTexture(4, 2, 250);
  if (!charactersTexture.loadFromFile(gRenderer, "media/Textures/characters.png"))
    return 1;

  // loading wall texture
  TextureWrapper wallTexture;
  if (!wallTexture.loadFromFile(gRenderer, "media/Textures/wall.png"))
    return 1;


  // loading end point texture
  TextureWrapper endPointTexture(1, 2, 250);
  if(!endPointTexture.loadFromFile(gRenderer, "media/Textures/end_point.png"))
    return 1;

  // loading weak virus texture
  TextureWrapper weakVirusTexture(1, 2, 500);
  if(!weakVirusTexture.loadFromFile(gRenderer, "media/Textures/weak_virus.png"))
    return 1;

  // loading covid19 virus texture
  TextureWrapper covid19Texture(1, 2, 250);
  if (!covid19Texture.loadFromFile(gRenderer, "media/Textures/covid_virus.png"))
    return 1;

  // loading disinfectant gel texture
  TextureWrapper disinfectantGelTexture;
  if(!disinfectantGelTexture.loadFromFile(gRenderer, "media/Textures/covid_potion.png"))
    return 1;

  // loading energetic potion texture
  TextureWrapper energeticPotionTexture;
  if(!energeticPotionTexture.loadFromFile(gRenderer, "media/Textures/energetic_potion.png"))
    return 1;

  // loading unhighlighted direction texture
  TextureWrapper unhighlightedDirTexture;
  if(!unhighlightedDirTexture.loadFromFile(gRenderer, "media/Textures/unhighlighted_direction.png"))
    return 1;

  // loading highlighted direction texture
  TextureWrapper highlightedDirTexture;
  if(!highlightedDirTexture.loadFromFile(gRenderer, "media/Textures/highlighted_direction.png"))
    return 1;
  /*----------------------------------------------------------------------------------------------------------------*/

  /*----------------------------- Create and, if verbose, render the maze creation ---------------------------------*/
  Labyrinthe object(25, 25, {0, 0}, false, gRenderer);
  object.setWallTexture(&wallTexture);
  //object.add_random_rooms(4);
  int SCREEN_WIDTH, SCREEN_HEIGHT;
  SDL_GetRendererOutputSize(gRenderer, &SCREEN_WIDTH, &SCREEN_HEIGHT);

  // create player object, initialize members later
  player uPlayer;
  uPlayer.setHp(MAX_HP);
  /*----------------------------------------------------------------------------------------------------------------*/

  /*------------------------------------------- Handling user inpuut -----------------------------------------------*/
  // input flags
  bool mouseButtonDown = false;
  bool keyPressed = false;

  // what is the current input the user is asked to chose/enter ?
  user_input currentUserInput = START_POINT;

  // character type chosen by user
  character_type uPlayerChar;

  // initial position for the palyer to start from
  coord uPlayerInitPos;

  // initial direction for the player to start from
  direction uPlayerDir;

  double covidTimer = 0;

  bool previousKeyState = false;
  bool doneUserInput = false;
  bool isTimerSet = false;
  bool gameLost = false;
  bool gameWon = false;

  virus::speed = 5;
  virus::maze = &object;

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Main Menu
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  /*while(!quit) {
    while(SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }

    // update screen
    SDL_RenderPresent(gRenderer);
  }*/

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Game loop
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  while (!quit) {

    while(SDL_PollEvent(&e) != 0) {

      if (e.type == SDL_QUIT) {

        quit = true;

      } else if (e.type == SDL_MOUSEBUTTONDOWN) {

        mouseButtonDown = true;

      }

    } // END EVENT WHILE

    // usefull for detecting a KEY press (holdong key down will generate nothing)
    if (!previousKeyState) {
      keyPressed = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_SPACE];
      previousKeyState = keyPressed;
    } else {
      keyPressed = false;
      previousKeyState = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_SPACE];
    }

    // Clear the screen
    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(gRenderer);

    // Render maze
    object.renderMaze();

    switch (currentUserInput) {

      case START_POINT :
                        {
                          int x, y;
                          SDL_GetMouseState(&x, &y);

                          // temperary objects to hold some data
                          int caseSize = object.getCaseSize();
                          int tempX = x/caseSize, tempY = y/caseSize;

                          // if mouse cursor is inside the maze
                          if (  (tempX != 0) && (tempX < object.getCols()+1) &&
                                (tempY != 0) && (tempY < object.getRows()+1)    ) {

                            if (object(tempY-1, tempX-1).state == PASS) {

                              // highlight the maze case where the cursor is pointing to
                              charactersTexture.renderAnimated( gRenderer, tempX*caseSize,
                                                                tempY*caseSize, caseSize, 0, deltaTime, 7);

                              if (mouseButtonDown) {
                                object.setStartPoint({tempY-1, tempX-1});
                                uPlayer.setStartPosition({tempY-1, tempX-1});
                                currentUserInput = END_POINT;

                                // determine possible directions for the initial encountring
                                if (object(object.getStartPoint().x-1, object.getStartPoint().y).state == PASS ) {
                                  possibleDirection[UP] = true;
                                }
                                if (object(object.getStartPoint().x+1, object.getStartPoint().y).state == PASS ) {
                                  possibleDirection[DOWN] = true;
                                }
                                if (object(object.getStartPoint().x, object.getStartPoint().y-1).state == PASS ) {
                                  possibleDirection[LEFT] = true;
                                }
                                if (object(object.getStartPoint().x, object.getStartPoint().y+1).state == PASS ) {
                                  possibleDirection[RIGHT] = true;
                                }

                                mouseButtonDown = false;
                              }

                            }

                          }

                          startTT.render(gRenderer, (object.getCols()+3)*object.getCaseSize(), object.getCaseSize()*2);
                          break;
                        }

      case END_POINT :
                        {
                          // previous renders
                          charactersTexture.renderAnimated( gRenderer, (uPlayer.getPosition().y+1)*object.getCaseSize(),
                                                            (uPlayer.getPosition().x+1)*object.getCaseSize(),
                                                            object.getCaseSize(), 0, deltaTime, 7);
                          int x, y;
                          SDL_GetMouseState(&x, &y);

                          // temperary objects to hold some data
                          int caseSize = object.getCaseSize();
                          int tempX = x/caseSize, tempY = y/caseSize;

                          // if mouse cursor is inside the maze
                          if (  (tempX != 0) && (tempX < object.getCols()+1) &&
                                (tempY != 0) && (tempY < object.getRows()+1)    ) {

                            if (object(tempY-1, tempX-1).state == PASS) {

                              // highlight the maze case where the cursor is pointing to
                              endPointTexture.renderAnimated(   gRenderer, tempX*caseSize,
                                                                tempY*caseSize, caseSize, 0, deltaTime, 4);

                              if (  mouseButtonDown && (object.getStartPoint().x != tempY-1
                                    || object.getStartPoint().y != tempX-1) ) {
                                object.setEndPoint({tempY-1, tempX-1});
                                currentUserInput = PLAYER_DIRECTION;
                                mouseButtonDown = false;
                              }

                            }

                          }

                          endTT.render(gRenderer, (object.getCols()+3)*object.getCaseSize(), object.getCaseSize()*2);
                          break;
                        }

      case PLAYER_DIRECTION :
                              {
                                // previous renders
                                charactersTexture.renderAnimated( gRenderer, (uPlayer.getPosition().y+1)*object.getCaseSize(),
                                                                  (uPlayer.getPosition().x+1)*object.getCaseSize(),
                                                                  object.getCaseSize(), 0, deltaTime, 7);
                                endPointTexture.renderAnimated(   gRenderer, (object.getEndPoint().y+1)*object.getCaseSize(),
                                                                  (object.getEndPoint().x+1)*object.getCaseSize(), object.getCaseSize(), 0, deltaTime, 4);

                                int x, y;
                                SDL_GetMouseState(&x, &y);

                                // temperary objects to hold some data
                                int caseSize = object.getCaseSize();
                                int tempX = x/caseSize, tempY = y/caseSize;
                                coord cursorCase = {tempY-1, tempX-1};
                                coord startPoint = object.getStartPoint();

                                // render arrows indicating possible initial directions
                                if (possibleDirection[UP]) {
                                  // render UP arrow
                                  unhighlightedDirTexture.render( gRenderer, (startPoint.y+1)*caseSize, (startPoint.x)*caseSize, caseSize,
                                                                  NULL, -90.0);
                                }
                                if (possibleDirection[DOWN]) {
                                  // render DOWN arrow
                                  unhighlightedDirTexture.render( gRenderer, (startPoint.y+1)*caseSize, (startPoint.x+2)*caseSize, caseSize,
                                                                  NULL, +90.0);
                                }
                                if (possibleDirection[LEFT]) {
                                  // render LEFT arrow
                                  unhighlightedDirTexture.render( gRenderer, (startPoint.y)*caseSize, (startPoint.x+1)*caseSize, caseSize,
                                                                  NULL, 0.0, NULL, SDL_FLIP_HORIZONTAL);
                                }
                                if (possibleDirection[RIGHT]) {
                                  // render RIGHT direction
                                  unhighlightedDirTexture.render(gRenderer, (startPoint.y+2)*caseSize, (startPoint.x+1)*caseSize, caseSize);
                                }

                                // highligh the arrow the cursor if pointing to
                                if (  (cursorCase.x == startPoint.x-1) &&
                                      (cursorCase.y == startPoint.y) &&
                                      (possibleDirection[UP])  ) {

                                        highlightedDirTexture.render( gRenderer, tempX*caseSize, tempY*caseSize, caseSize,
                                                                      NULL, -90.0);

                                        // if user clicks on this arrow, chose this direction as initial direction
                                        if (mouseButtonDown) {
                                          uPlayerDir = UP;
                                          currentUserInput = WEAK_VIRUS_POSITION;
                                          mouseButtonDown = false;
                                        }

                                } else if ( (cursorCase.x == startPoint.x+1) &&
                                            (cursorCase.y == startPoint.y) &&
                                            (possibleDirection[DOWN]) ) {

                                          highlightedDirTexture.render( gRenderer, tempX*caseSize, tempY*caseSize, caseSize,
                                                                        NULL, +90.0);

                                        if (mouseButtonDown) {
                                          uPlayerDir = DOWN;
                                          currentUserInput = WEAK_VIRUS_POSITION;
                                          mouseButtonDown = false;
                                        }

                                } else if ( (cursorCase.x == startPoint.x) &&
                                            (cursorCase.y == startPoint.y-1) &&
                                            (possibleDirection[LEFT]) ) {

                                          highlightedDirTexture.render( gRenderer, tempX*caseSize, tempY*caseSize, caseSize,
                                                                        NULL, 0.0, NULL, SDL_FLIP_HORIZONTAL);


                                        if (mouseButtonDown) {
                                          uPlayerDir = LEFT;
                                          currentUserInput = WEAK_VIRUS_POSITION;
                                          mouseButtonDown = false;
                                        }
                                } else if ( (cursorCase.x == startPoint.x) &&
                                            (cursorCase.y == startPoint.y+1) &&
                                            (possibleDirection[RIGHT])  ) {

                                          highlightedDirTexture.render( gRenderer, tempX*caseSize, tempY*caseSize, caseSize);


                                        if (mouseButtonDown) {
                                          uPlayerDir = RIGHT;
                                          currentUserInput = WEAK_VIRUS_POSITION;
                                          mouseButtonDown = false;
                                        }
                                }

                                dirTT.render(gRenderer, (object.getCols()+3)*object.getCaseSize(), object.getCaseSize()*2);
                                break;
                              }

      case WEAK_VIRUS_POSITION :
                            {

                              // previous renders
                              charactersTexture.renderAnimated( gRenderer, (uPlayer.getPosition().y+1)*object.getCaseSize(),
                                                                (uPlayer.getPosition().x+1)*object.getCaseSize(),
                                                                object.getCaseSize(), 0, deltaTime, 7);

                              endPointTexture.renderAnimated(   gRenderer, (object.getEndPoint().y+1)*object.getCaseSize(),
                                                                (object.getEndPoint().x+1)*object.getCaseSize(), object.getCaseSize(), 0, deltaTime, 4);

                              for (int i=0; i<virus_vector.size(); ++i) {
                                weakVirusTexture.renderAnimated(  gRenderer, (virus_vector[i].getPosition().y+1)*object.getCaseSize(),
                                                                  (virus_vector[i].getPosition().x+1)*object.getCaseSize(),
                                                                  object.getCaseSize(), 0, deltaTime, 2, &virus_vector[i].animationTimer,
                                                                  &virus_vector[i].animationIndex);
                              }

                              if (keyPressed) {

                                  currentUserInput = COVID19_VIRUS_POSITION;
                                  break;

                              }

                              int x, y;
                              SDL_GetMouseState(&x, &y);

                              // temperary objects to hold some data
                              int caseSize = object.getCaseSize();
                              int tempX = x/caseSize, tempY = y/caseSize;

                              // if mouse cursor is inside the maze
                              if (  (tempX != 0) && (tempX < object.getCols()+1) &&
                                    (tempY != 0) && (tempY < object.getRows()+1)    ) {

                                if (  (object(tempY-1, tempX-1).state == PASS) &&
                                      (object(tempY-1, tempX-1).potion == NO_POTION) ) {

                                  // highlight the maze case where the cursor is pointing to
                                  weakVirusTexture.renderAnimated(  gRenderer, tempX*caseSize,
                                                                    tempY*caseSize, caseSize, 0, deltaTime, 2);

                                  if (mouseButtonDown) {
                                    virus_vector.push_back(virus({tempY-1, tempX-1}, WEAK_VIRUS));
                                    mouseButtonDown = false;
                                  }
                                }
                              }

                              weakVirusTT.render(gRenderer, (object.getCols()+3)*object.getCaseSize(), object.getCaseSize()*2);
                              spaceTT.render( gRenderer, (object.getCols()+3)*object.getCaseSize(),
                                              object.getCaseSize()*(object.getRows()+1) - spaceTT.getHeight());
                              break;
                            }

      case COVID19_VIRUS_POSITION :
                              {
                                // previous renders
                                charactersTexture.renderAnimated( gRenderer, (uPlayer.getPosition().y+1)*object.getCaseSize(),
                                                                  (uPlayer.getPosition().x+1)*object.getCaseSize(),
                                                                  object.getCaseSize(), 0, deltaTime, 7);

                                endPointTexture.renderAnimated(   gRenderer, (object.getEndPoint().y+1)*object.getCaseSize(),
                                                                  (object.getEndPoint().x+1)*object.getCaseSize(), object.getCaseSize(), 0, deltaTime, 4);

                                for (int i=0; i<virus_vector.size(); ++i) {
                                  switch (virus_vector[i].getNature()) {

                                    case COVID19_VIRUS: covid19Texture.renderAnimated(  gRenderer, (virus_vector[i].getPosition().y+1)*object.getCaseSize(),
                                                                      (virus_vector[i].getPosition().x+1)*object.getCaseSize(),
                                                                      object.getCaseSize(), 0, deltaTime, 2, &virus_vector[i].animationTimer,
                                                                      &virus_vector[i].animationIndex);
                                                        break;

                                    case WEAK_VIRUS:    weakVirusTexture.renderAnimated(  gRenderer, (virus_vector[i].getPosition().y+1)*object.getCaseSize(),
                                                                      (virus_vector[i].getPosition().x+1)*object.getCaseSize(),
                                                                      object.getCaseSize(), 0, deltaTime, 2, &virus_vector[i].animationTimer,
                                                                      &virus_vector[i].animationIndex);
                                                        break;

                                  }

                                }


                                if (keyPressed) {

                                    currentUserInput = ENERGETIC_POTION_POSITION;
                                    break;

                                }

                                int x, y;
                                SDL_GetMouseState(&x, &y);

                                // temperary objects to hold some data
                                int caseSize = object.getCaseSize();
                                int tempX = x/caseSize, tempY = y/caseSize;

                                // if mouse cursor is inside the maze
                                if (  (tempX != 0) && (tempX < object.getCols()+1) &&
                                      (tempY != 0) && (tempY < object.getRows()+1)    ) {

                                  if (  (object(tempY-1, tempX-1).state == PASS) &&
                                        (object(tempY-1, tempX-1).virus == NO_VIRUS) &&
                                        (object(tempY-1, tempX-1).potion == NO_POTION) ) {

                                    // highlight the maze case where the cursor is pointing to
                                    covid19Texture.renderAnimated(  gRenderer, tempX*caseSize, tempY*caseSize,
                                                                    caseSize, 0, deltaTime, 3);

                                    if (mouseButtonDown) {
                                      virus_vector.push_back(virus({tempY-1, tempX-1}, COVID19_VIRUS));
                                      mouseButtonDown = false;
                                    }
                                  }
                                }

                                covid19TT.render(gRenderer, (object.getCols()+3)*object.getCaseSize(), object.getCaseSize()*2);
                                spaceTT.render( gRenderer, (object.getCols()+3)*object.getCaseSize(),
                                                object.getCaseSize()*(object.getRows()+1) - spaceTT.getHeight());
                                break;
                              }

      case ENERGETIC_POTION_POSITION :
                                      {

                                        // previous renders
                                        charactersTexture.renderAnimated( gRenderer, (uPlayer.getPosition().y+1)*object.getCaseSize(),
                                                                          (uPlayer.getPosition().x+1)*object.getCaseSize(),
                                                                          object.getCaseSize(), 0, deltaTime, 7);
                                        endPointTexture.renderAnimated(   gRenderer, (object.getEndPoint().y+1)*object.getCaseSize(),
                                                                          (object.getEndPoint().x+1)*object.getCaseSize(), object.getCaseSize(), 0, deltaTime, 4);

                                        for (int i=0; i<virus_vector.size(); ++i) {
                                          switch (virus_vector[i].getNature()) {

                                            case COVID19_VIRUS: covid19Texture.renderAnimated(  gRenderer, (virus_vector[i].getPosition().y+1)*object.getCaseSize(),
                                                                              (virus_vector[i].getPosition().x+1)*object.getCaseSize(),
                                                                              object.getCaseSize(), 0, deltaTime, 2, &virus_vector[i].animationTimer,
                                                                              &virus_vector[i].animationIndex);
                                                                break;

                                            case WEAK_VIRUS:    weakVirusTexture.renderAnimated(  gRenderer, (virus_vector[i].getPosition().y+1)*object.getCaseSize(),
                                                                              (virus_vector[i].getPosition().x+1)*object.getCaseSize(),
                                                                              object.getCaseSize(), 0, deltaTime, 2, &virus_vector[i].animationTimer,
                                                                              &virus_vector[i].animationIndex);
                                                                break;

                                          }

                                        }

                                        for (int i=0; i<potion_vector.size(); ++i) {
                                          energeticPotionTexture.render( gRenderer, (potion_vector[i].getPosition().y+1)*object.getCaseSize(),
                                                                                      (potion_vector[i].getPosition().x+1)*object.getCaseSize(),
                                                                                      object.getCaseSize());
                                        }


                                        if (keyPressed) {

                                          currentUserInput = DISINFECTANT_GEL_POSITION;
                                          break;

                                        }

                                        int x, y;
                                        SDL_GetMouseState(&x, &y);

                                        // temperary objects to hold some data
                                        int caseSize = object.getCaseSize();
                                        int tempX = x/caseSize, tempY = y/caseSize;

                                        // if mouse cursor is inside the maze
                                        if (  (tempX != 0) && (tempX < object.getCols()+1) &&
                                            (tempY != 0) && (tempY < object.getRows()+1)    ) {

                                          if (  (object(tempY-1, tempX-1).state == PASS) &&
                                                (object(tempY-1, tempX-1).virus == NO_VIRUS) &&
                                                (object(tempY-1, tempX-1).potion == NO_POTION) ) {

                                            // highlight the maze case where the cursor is pointing to
                                            energeticPotionTexture.render( gRenderer, tempX*caseSize, tempY*caseSize, caseSize);

                                            if (mouseButtonDown) {
                                              potion_vector.push_back(potion({tempY-1, tempX-1}, ENERGETIC_POTION));
                                              mouseButtonDown = false;
                                            }
                                          }
                                        }

                                        enerPotionTT.render(gRenderer, (object.getCols()+3)*object.getCaseSize(), object.getCaseSize()*2);
                                        spaceTT.render( gRenderer, (object.getCols()+3)*object.getCaseSize(),
                                                        object.getCaseSize()*(object.getRows()+1) - spaceTT.getHeight());
                                        break;
                                        }

      case DISINFECTANT_GEL_POSITION :
                                          {
                                            // previous renders
                                            charactersTexture.renderAnimated( gRenderer, (uPlayer.getPosition().y+1)*object.getCaseSize(),
                                                                              (uPlayer.getPosition().x+1)*object.getCaseSize(),
                                                                              object.getCaseSize(), 0, deltaTime, 7);
                                            endPointTexture.renderAnimated(   gRenderer, (object.getEndPoint().y+1)*object.getCaseSize(),
                                                                              (object.getEndPoint().x+1)*object.getCaseSize(), object.getCaseSize(), 0, deltaTime, 4);

                                            for (int i=0; i<virus_vector.size(); ++i) {
                                              switch (virus_vector[i].getNature()) {

                                                case COVID19_VIRUS: covid19Texture.renderAnimated(  gRenderer, (virus_vector[i].getPosition().y+1)*object.getCaseSize(),
                                                                                  (virus_vector[i].getPosition().x+1)*object.getCaseSize(),
                                                                                  object.getCaseSize(), 0, deltaTime, 2, &virus_vector[i].animationTimer,
                                                                                  &virus_vector[i].animationIndex);
                                                                    break;

                                                case WEAK_VIRUS:    weakVirusTexture.renderAnimated(  gRenderer, (virus_vector[i].getPosition().y+1)*object.getCaseSize(),
                                                                                  (virus_vector[i].getPosition().x+1)*object.getCaseSize(),
                                                                                  object.getCaseSize(), 0, deltaTime, 2, &virus_vector[i].animationTimer,
                                                                                  &virus_vector[i].animationIndex);
                                                                    break;

                                              }

                                            }

                                            for (int i=0; i<potion_vector.size(); ++i) {
                                              switch (potion_vector[i].getNature()) {
                                                case ENERGETIC_POTION : energeticPotionTexture.render( gRenderer, (potion_vector[i].getPosition().y+1)*object.getCaseSize(),
                                                                                          (potion_vector[i].getPosition().x+1)*object.getCaseSize(),
                                                                                          object.getCaseSize());
                                                                        break;

                                                case DISINFECTANT_GEL : disinfectantGelTexture.render( gRenderer, (potion_vector[i].getPosition().y+1)*object.getCaseSize(),
                                                                                          (potion_vector[i].getPosition().x+1)*object.getCaseSize(),
                                                                                          object.getCaseSize());
                                                                        break;
                                              }
                                            }

                                            if (keyPressed) {
                                              currentUserInput = INITIALIZING_AFTER_INPUT;
                                              break;
                                            }
                                            int x, y;
                                            SDL_GetMouseState(&x, &y);

                                            // temperary objects to hold some data
                                            int caseSize = object.getCaseSize();
                                            int tempX = x/caseSize, tempY = y/caseSize;

                                            // if mouse cursor is inside the maze
                                            if (  (tempX != 0) && (tempX < object.getCols()+1) &&
                                                  (tempY != 0) && (tempY < object.getRows()+1)    ) {

                                              if (  (object(tempY-1, tempX-1).state == PASS) &&
                                                    (object(tempY-1, tempX-1).virus == NO_VIRUS) &&
                                                    (object(tempY-1, tempX-1).potion == NO_POTION) ) {

                                                // highlight the maze case where the cursor is pointing to
                                                disinfectantGelTexture.render(gRenderer, tempX*caseSize, tempY*caseSize, caseSize);

                                                if (mouseButtonDown) {
                                                  potion_vector.push_back(potion({tempY-1, tempX-1}, DISINFECTANT_GEL));
                                                  mouseButtonDown = false;
                                                }
                                              }
                                            }

                                            disGelTT.render(gRenderer, (object.getCols()+3)*object.getCaseSize(), object.getCaseSize()*2);
                                            spaceTT.render( gRenderer, (object.getCols()+3)*object.getCaseSize(),
                                                            object.getCaseSize()*(object.getRows()+1) - spaceTT.getHeight());
                                            break;
                                          }

      case INITIALIZING_AFTER_INPUT :   // initializing user player's members
                                        uPlayer.setMaze(&object);
                                        uPlayer.setCharacter(OTHER);
                                        uPlayer.setDirection(uPlayerDir);
                                        player::speed = 7;

                                        doneUserInput = true;
                                        currentUserInput = DONE_INPUT;

                                        break;


      default :                   break;

    } // ENS SWITCH


    // where the actual gameplay rendering resides
    if (doneUserInput) {

      // updates + render timer
      if (isTimerSet) {
        covidTimer += deltaTime;
        if (covidTimer > 60) {
          gameLost = true;
          break;
        }

        // /changing flaot to int to string
        std::ostringstream os;
        os << static_cast<int>(covidTimer);
        if (!timerTT.loadFromRenderedText(gRenderer, os.str(), gFont, textColor))
          return 1;

        timerTT.render(gRenderer, (object.getCols()+3)*object.getCaseSize(), object.getCaseSize()*2);

      }

      if (uPlayer.getHp() <= 0) {
        gameLost = true;
        break;
      }

      // render endpoint
      endPointTexture.renderAnimated(   gRenderer, (object.getEndPoint().y+1)*object.getCaseSize(),
                                        (object.getEndPoint().x+1)*object.getCaseSize(), object.getCaseSize(), 0, deltaTime, 4);



      // Potions Update Game Status + Rendering
      for (int i=0; i<potion_vector.size(); ++i) {

            switch (potion_vector[i].getNature()) {

              case ENERGETIC_POTION : if ( (potion_vector[i].getPosition().x == uPlayer.getPosition().x) && (potion_vector[i].getPosition().y == uPlayer.getPosition().y) ) {
                                        if (uPlayer.getHp() != MAX_HP) {
                                          uPlayer.setHp(uPlayer.getHp()+1);
                                          potion_vector.erase(potion_vector.begin()+i);
                                        }
                                      } else { // else Render Potion
                                        energeticPotionTexture.render( gRenderer, (potion_vector[i].getPosition().y+1)*object.getCaseSize(),
                                                                                  (potion_vector[i].getPosition().x+1)*object.getCaseSize(),
                                                                                  object.getCaseSize());
                                      }

                                      break;

              case DISINFECTANT_GEL :  if ( (potion_vector[i].getPosition().x == uPlayer.getPosition().x) && (potion_vector[i].getPosition().y == uPlayer.getPosition().y) ) {
                                                  if (isTimerSet) {
                                                    isTimerSet = false;
                                                    covidTimer = 0;
                                                    potion_vector.erase(potion_vector.begin()+i);
                                                  }
                                                } else { // else render Potion
                                                  disinfectantGelTexture.render(  gRenderer, (potion_vector[i].getPosition().y+1)*object.getCaseSize(),
                                                                                  (potion_vector[i].getPosition().x+1)*object.getCaseSize(),
                                                                                  object.getCaseSize());
                                                }

                                                break;
            }
          }

      // Viruses Update Position + Update Game Status + Rendering
      for (int i=0; i<virus_vector.size(); ++i) {

                virus_vector[i].updatePosition(deltaTime);

                switch(virus_vector[i].getNature()) {
                  case WEAK_VIRUS : if ( (virus_vector[i].getPosition().x == uPlayer.getPosition().x) && (virus_vector[i].getPosition().y == uPlayer.getPosition().y) ) {
                                      uPlayer.setHp(uPlayer.getHp()-1);
                                      virus_vector.erase(virus_vector.begin()+i);
                                    } else { // else render virus
                                      weakVirusTexture.renderAnimated(  gRenderer, (virus_vector[i].getPosition().y+1)*object.getCaseSize(),
                                                                        (virus_vector[i].getPosition().x+1)*object.getCaseSize(),
                                                                        object.getCaseSize(), 0, deltaTime, 2, &virus_vector[i].animationTimer,
                                                                        &virus_vector[i].animationIndex);
                                    }
                                    break;

                  case COVID19_VIRUS :  if ( (virus_vector[i].getPosition().x == uPlayer.getPosition().x) && (virus_vector[i].getPosition().y == uPlayer.getPosition().y) ) {
                                          uPlayer.setHp(uPlayer.getHp()-3);
                                          if (!isTimerSet) {
                                            isTimerSet = true;
                                          }
                                          virus_vector.erase(virus_vector.begin()+i);
                                        } else {  // render virus
                                          covid19Texture.renderAnimated(  gRenderer, (virus_vector[i].getPosition().y+1)*object.getCaseSize(),
                                                                          (virus_vector[i].getPosition().x+1)*object.getCaseSize(),
                                                                          object.getCaseSize(), 0, deltaTime, 3, &virus_vector[i].animationTimer,
                                                                          &virus_vector[i].animationIndex);
                                        }
                                        break;
                }
              }

      // updates user player's direction according to user input
      direction userInutDir;
      const Uint8* keyboardstate = SDL_GetKeyboardState(NULL);
      if (keyboardstate[SDL_SCANCODE_UP]) {
        uPlayer.setDirection(UP);
      } else if (keyboardstate[SDL_SCANCODE_DOWN]) {
        uPlayer.setDirection(DOWN);
      } else if (keyboardstate[SDL_SCANCODE_LEFT]) {
        uPlayer.setDirection(LEFT);
      } else if (keyboardstate[SDL_SCANCODE_RIGHT]) {
        uPlayer.setDirection(RIGHT);
      }

      uPlayer.updatePosition(deltaTime);  // updates user player's position accordign to dir and speed
      switch(uPlayer.getCharacter()) {    // renders user player depending on its character type
        case OTHER :  charactersTexture.renderAnimated( gRenderer, (uPlayer.getPosition().y+1)*object.getCaseSize(),
                                                        (uPlayer.getPosition().x+1)*object.getCaseSize(), object.getCaseSize(),
                                                        0, deltaTime, 7);
                      break;
      }

      // checks if player has reached end point
      if ( (uPlayer.getPosition().x == object.getEndPoint().x) && (uPlayer.getPosition().y == object.getEndPoint().y) ) {
        gameWon = true;
        break;
      }

    }

    // render healthbar
    if (uPlayer.getHp() > 0) {
      for (int i=0; i<uPlayer.getHp(); ++i) {
        SDL_Rect highlightRect{ (object.getCols()+3)*object.getCaseSize() + i*(object.getCaseSize()/2 + 2),
                                object.getCaseSize()/4,
                                object.getCaseSize()/2, object.getCaseSize()/2  };
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderFillRect(gRenderer, &highlightRect);
      }
    }

    // Updates screen
    SDL_RenderPresent(gRenderer);

    auto timeNow = std::chrono::high_resolution_clock::now();
    deltaTime = std::chrono::duration<double>(timeNow - previousTimerCall).count();

    previousTimerCall = timeNow;

    mouseButtonDown = false;

  }// END GAME LOOP
  /*---------------------------------------------------------------------------------------------------------------*/

  /*-------------------------------------------------- End game scene ---------------------------------------------*/
  while (!quit) {
    while(SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }

    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(gRenderer);

    if (gameWon) {
      gameWonTT.render(gRenderer, (SCREEN_WIDTH - gameWonTT.getWidth())/2, (SCREEN_HEIGHT - gameWonTT.getHeight())/2);
      quitTT.render(gRenderer, (SCREEN_WIDTH - quitTT.getWidth())/2, ((SCREEN_HEIGHT - quitTT.getHeight()))/2 + gameWonTT.getHeight());
    }

    if (gameLost) {
      gameLostTT.render(gRenderer, (SCREEN_WIDTH - gameLostTT.getWidth())/2, (SCREEN_HEIGHT - gameLostTT.getHeight())/2);
      quitTT.render(gRenderer, (SCREEN_WIDTH - quitTT.getWidth())/2, ((SCREEN_HEIGHT - quitTT.getHeight()))/2 + gameLostTT.getHeight());
    }

    SDL_RenderPresent(gRenderer);
  }
  /*---------------------------------------------------------------------------------------------------------------*/

  /*------------------------------------------ Deallocationg Memory Space -----------------------------------------*/
  SDL_DestroyRenderer(gRenderer);
  SDL_DestroyWindow(gWindow);
  gRenderer = NULL;
  gWindow = NULL;

  // Quiting SDL subsystems
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
  /*---------------------------------------------------------------------------------------------------------------*/

  return 0;
}
