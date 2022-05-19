/*
 * @(#)game.c
 *
 * Copyright 1999-2001, Aaron Ardiri (mailto:aaron@ardiri.com)
 * All rights reserved.
 *
 * This file  was generated as part of the "dkjr" program developed for 
 * the Helio Computing Platform designed by vtech:
 *
 *   http://www.vtechinfo.com/
 *
 * The contents of this file is confidential and  proprietary in nature
 * ("Confidential Information"). Redistribution or modification without 
 * prior consent of the original author is prohibited.
 */

#include "helio.h"

// image resources
#include "game.inc"

// interface
static void GameGetSpritePosition(BYTE, BYTE, SHORT *, SHORT *);
static void GameDrawRestore(PreferencesType *);
static void GameAdjustLevel(PreferencesType *);
static void GameIncrementScore(PreferencesType *);
static void GameMovePlayer(PreferencesType *);
static void GameMoveChops(PreferencesType *);
static void GameRemoveChop(PreferencesType *, SHORT);
static void GameMoveBirds(PreferencesType *);
static void GameRemoveBird(PreferencesType *, SHORT);

// global variable structure
typedef struct
{
  GfxWindow *winDigits;                     // scoring digits bitmaps
  GfxWindow *winMisses;                     // the lives notification bitmaps

  GfxWindow *winCages;                      // the cage bitmaps
  GfxWindow *winCageBackup;                 // the area behind the cage
  BOOLEAN   cageChanged;                    // do we need to repaint the cage?
  BOOLEAN   cageBackupAvailable;            // is there a backup available?

  GfxWindow *winKeys;                       // the key bitmaps
  BOOLEAN   keyChanged;                     // do we need to repaint the key?
  BOOLEAN   keyOnScreen;                    // is key bitmap on screen?
  USHORT    keyOldPosition;                 // the *old* position of the key 

  GfxWindow *winDrops;                      // the drop bitmaps
  BOOLEAN   dropChanged;                    // do we need to repaint the drop?
  BOOLEAN   dropOnScreen;                   // is drop bitmap on screen?
  USHORT    dropOldPosition;                // the *old* position of the drop 

  GfxWindow *winBirds;                      // the bird bitmaps
  BOOLEAN   birdChanged[MAX_BIRDS];         // do we need to repaint the bird?
  BOOLEAN   birdOnScreen[MAX_BIRDS];        // is bird bitmap on screen?
  USHORT    birdOnScreenPosition[MAX_BIRDS];// the *old* position of the bird

  GfxWindow *winChops;                      // the chop bitmaps
  BOOLEAN   chopChanged[MAX_CHOPS];         // do we need to repaint the chop?
  BOOLEAN   chopOnScreen[MAX_CHOPS];        // is chop bitmap on screen?
  USHORT    chopOnScreenPosition[MAX_CHOPS];// the *old* position of the chop

  GfxWindow *winKongs;                      // the kong bitmaps
  BOOLEAN   kongChanged;                    // do we need to repaint DK?
  BOOLEAN   kongOnScreen;                   // is DK bitmap on screen?
  USHORT    kongOldPosition;                // the *old* position of DK 

  BYTE      gameType;                       // the type of game active
  BOOLEAN   playerDied;                     // has the player died?
  UBYTE     moveDelayCount;                 // the delay between moves
  UBYTE     moveLast;                       // the last move performed
  UBYTE     moveNext;                       // the next desired move

} GameGlobals;

// globals reference
static GameGlobals *gbls;

/**
 * Initialize the Game.
 */  
void   
GameInitialize()
{
  // create the globals object
  gbls = (GameGlobals *)pmalloc(sizeof(GameGlobals));

  // initialize and load the "bitmap" windows
  {
    SHORT i;

    gbls->winDigits              = (GfxWindow *)pmalloc(sizeof(GfxWindow));
    gbls->winDigits->width       = bitmap00Width;
    gbls->winDigits->height      = bitmap00Height;
    gbls->winDigits->memSize     = bitmap00Size;
    gbls->winDigits->memory      = (void *)bitmap00;
    gbls->winMisses              = (GfxWindow *)pmalloc(sizeof(GfxWindow));
    gbls->winMisses->width       = bitmap01Width;
    gbls->winMisses->height      = bitmap01Height;
    gbls->winMisses->memSize     = bitmap01Size;
    gbls->winMisses->memory      = (void *)bitmap01;
    gbls->winCages               = (GfxWindow *)pmalloc(sizeof(GfxWindow));
    gbls->winCages->width        = bitmap02Width;
    gbls->winCages->height       = bitmap02Height;
    gbls->winCages->memSize      = bitmap02Size;
    gbls->winCages->memory       = (void *)bitmap02;
    gbls->winKeys                = (GfxWindow *)pmalloc(sizeof(GfxWindow));
    gbls->winKeys->width         = bitmap03Width;
    gbls->winKeys->height        = bitmap03Height;
    gbls->winKeys->memSize       = bitmap03Size;
    gbls->winKeys->memory        = (void *)bitmap03;
    gbls->winDrops               = (GfxWindow *)pmalloc(sizeof(GfxWindow));
    gbls->winDrops->width        = bitmap04Width;
    gbls->winDrops->height       = bitmap04Height;
    gbls->winDrops->memSize      = bitmap04Size;
    gbls->winDrops->memory       = (void *)bitmap04;
    gbls->winBirds               = (GfxWindow *)pmalloc(sizeof(GfxWindow));
    gbls->winBirds->width        = bitmap05Width;
    gbls->winBirds->height       = bitmap05Height;
    gbls->winBirds->memSize      = bitmap05Size;
    gbls->winBirds->memory       = (void *)bitmap05;
    gbls->winChops               = (GfxWindow *)pmalloc(sizeof(GfxWindow));
    gbls->winChops->width        = bitmap06Width;
    gbls->winChops->height       = bitmap06Height;
    gbls->winChops->memSize      = bitmap06Size;
    gbls->winChops->memory       = (void *)bitmap06;
    gbls->winKongs               = (GfxWindow *)pmalloc(sizeof(GfxWindow));
    gbls->winKongs->width        = bitmap07Width;
    gbls->winKongs->height       = bitmap07Height;
    gbls->winKongs->memSize      = bitmap07Size;
    gbls->winKongs->memory       = (void *)bitmap07;

    gbls->winCageBackup          = GfxCreateWindow(35, 31);

    gbls->cageBackupAvailable    = FALSE;
    gbls->cageChanged            = TRUE;
    gbls->keyChanged             = TRUE;
    gbls->keyOnScreen            = FALSE;
    gbls->keyOldPosition         = 0;

    gbls->dropChanged            = TRUE;
    gbls->dropOnScreen           = FALSE;
    gbls->dropOldPosition        = 0;

    for (i=0; i<MAX_BIRDS; i++) {
      gbls->birdChanged[i]          = TRUE;
      gbls->birdOnScreen[i]         = FALSE;
      gbls->birdOnScreenPosition[i] = 0;
    }

    for (i=0; i<MAX_CHOPS; i++) {
      gbls->chopChanged[i]          = TRUE;
      gbls->chopOnScreen[i]         = FALSE;
      gbls->chopOnScreenPosition[i] = 0;
    }

    gbls->kongChanged            = TRUE;
    gbls->kongOnScreen           = FALSE;
    gbls->kongOldPosition        = 0;
  }
}

/**
 * Reset the Game.
 * 
 * @param prefs the global preference data.
 * @param gameType the type of game to configure for.
 */  
void   
GameReset(PreferencesType *prefs, BYTE gameType)
{
  // turn off all the "bitmaps"
  FormDrawForm(gameForm);

  // turn on all the "bitmaps"
  {
    GfxRegion region    = { {   0,   0 }, {   0,   0 } };
    GfxRegion scrRegion = { {   0,   0 }, {   0,   0 } };
    SHORT     i;

    //
    // draw the score
    //

    for (i=0; i<4; i++) {

      // what is the rectangle we need to copy?
      GameGetSpritePosition(spriteDigit, i, 
                            &scrRegion.topLeft.x, &scrRegion.topLeft.y);
      scrRegion.extent.x  = 7;
      scrRegion.extent.y  = 12;
      region.topLeft.x    = 8 * scrRegion.extent.x;
      region.topLeft.y    = 0;
      region.extent.x     = scrRegion.extent.x;
      region.extent.y     = scrRegion.extent.y;

      // draw the digit!
      GfxCopyRegion(gbls->winDigits, GfxGetDrawWindow(),
                    &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxPaint);
    }

    //
    // draw the misses
    //

    // what is the rectangle we need to copy?
    GameGetSpritePosition(spriteMiss, 0, 
                          &scrRegion.topLeft.x, &scrRegion.topLeft.y);
    scrRegion.extent.x  = 48;
    scrRegion.extent.y  = 9;
    region.topLeft.x    = 2 * scrRegion.extent.x;
    region.topLeft.y    = 0;
    region.extent.x     = scrRegion.extent.x;
    region.extent.y     = scrRegion.extent.y;

    // draw the miss bitmap!
    GfxCopyRegion(gbls->winMisses, GfxGetDrawWindow(),
                  &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);

    // 
    // draw the cage
    //

    // what is the rectangle we need to copy?
    GameGetSpritePosition(spriteCage, 0, 
                          &scrRegion.topLeft.x, &scrRegion.topLeft.y);
    scrRegion.extent.x  = 35;
    scrRegion.extent.y  = 31;
    region.topLeft.x    = 0;
    region.topLeft.y    = 0;
    region.extent.x     = scrRegion.extent.x;
    region.extent.y     = scrRegion.extent.y;

    // what is the rectangle we need to copy?
    region.topLeft.x    = 0 * scrRegion.extent.x; 
    region.topLeft.y    = 0;
    region.extent.x     = scrRegion.extent.x;
    region.extent.y     = scrRegion.extent.y;

    // draw the cage bitmap!
    GfxCopyRegion(gbls->winCages, GfxGetDrawWindow(),
                  &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);

    // what is the rectangle we need to copy?
    region.topLeft.x    = 4 * scrRegion.extent.x; 
    region.topLeft.y    = 0;
    region.extent.x     = scrRegion.extent.x;
    region.extent.y     = scrRegion.extent.y;

    // draw the cage bitmap!
    GfxCopyRegion(gbls->winCages, GfxGetDrawWindow(),
                  &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);

    // 
    // draw the keys
    //

    for (i=0; i<4; i++) {

      // what is the rectangle we need to copy?
      GameGetSpritePosition(spriteKey, 0, 
                            &scrRegion.topLeft.x, &scrRegion.topLeft.y);
      scrRegion.extent.x  = 28;
      scrRegion.extent.y  = 22;
      region.topLeft.x    = 0;  
      region.topLeft.y    = 0;  
      region.extent.x     = scrRegion.extent.x;
      region.extent.y     = scrRegion.extent.y;

      // what is the rectangle we need to copy?
      region.topLeft.x = i * scrRegion.extent.x; 
      region.topLeft.y = 0;
      region.extent.x  = scrRegion.extent.x;
      region.extent.y  = scrRegion.extent.y;

      // draw the key bitmap!
      GfxCopyRegion(gbls->winKeys, GfxGetDrawWindow(),
                    &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);
    }

    //
    // draw the drops
    //

    for (i=0; i<4; i++) {

      // what is the rectangle we need to copy?
      GameGetSpritePosition(spriteDrop, i, 
                            &scrRegion.topLeft.x, &scrRegion.topLeft.y);
      scrRegion.extent.x  = 10;
      scrRegion.extent.y  = 10;
      region.topLeft.x    = i * scrRegion.extent.x;
      region.topLeft.y    = 0;
      region.extent.x     = scrRegion.extent.x;
      region.extent.y     = scrRegion.extent.y;

      // draw the drop bitmap!
      GfxCopyRegion(gbls->winDrops, GfxGetDrawWindow(),
                    &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);
    }

    //
    // draw the birds
    //

    for (i=0; i<8; i++) {

      // what is the rectangle we need to copy?
      GameGetSpritePosition(spriteBird, i, 
                            &scrRegion.topLeft.x, &scrRegion.topLeft.y);
      scrRegion.extent.x  = 10;
      scrRegion.extent.y  = 10;
      region.topLeft.x    = i * scrRegion.extent.x;
      region.topLeft.y    = 0;
      region.extent.x     = scrRegion.extent.x;
      region.extent.y     = scrRegion.extent.y;

      // draw the bird bitmap!
      GfxCopyRegion(gbls->winBirds, GfxGetDrawWindow(),
                    &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);
    }

    // 
    // draw the chops
    //

    for (i=0; i<13; i++) {

      // what is the rectangle we need to copy?
      GameGetSpritePosition(spriteChop, i, 
                            &scrRegion.topLeft.x, &scrRegion.topLeft.y);
      scrRegion.extent.x  = 10;
      scrRegion.extent.y  = 10;
      region.topLeft.x    = i * scrRegion.extent.x;
      region.topLeft.y    = 0;
      region.extent.x     = scrRegion.extent.x;
      region.extent.y     = scrRegion.extent.y;

      // draw the chop bitmap!
      GfxCopyRegion(gbls->winChops, GfxGetDrawWindow(),
                    &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);

    }

    //
    // draw DKJR
    //

    for (i=0; i<26; i++) {

      // skip #13
      if (i == 13) continue;

      // what is the rectangle we need to copy?
      GameGetSpritePosition(spriteKong, i, 
                            &scrRegion.topLeft.x, &scrRegion.topLeft.y);
      scrRegion.extent.x  = 20;
      scrRegion.extent.y  = 32;
      region.topLeft.x    = (i % 7) * scrRegion.extent.x; 
      region.topLeft.y    = (i / 7) * scrRegion.extent.y; 
      region.extent.x     = scrRegion.extent.x;
      region.extent.y     = scrRegion.extent.y;

      // draw the DK bitmap!
      GfxCopyRegion(gbls->winKongs, GfxGetDrawWindow(),
                    &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);
    }
  }

  // wait a good two seconds :))
  TmrWaitTime(2000);

  // turn off all the "bitmaps"
  FormDrawForm(gameForm);

  // reset the preferences
  GameResetPreferences(prefs, gameType);
}

/**
 * Reset the Game preferences.
 * 
 * @param prefs the global preference data.
 * @param gameType the type of game to configure for.
 */  
void   
GameResetPreferences(PreferencesType *prefs, BYTE gameType)
{
  SHORT i;

  // now we are playing
  prefs->game.gamePlaying          = TRUE;
  prefs->game.gamePaused           = FALSE;
  prefs->game.gameWait             = TRUE;
  prefs->game.gameAnimationCount   = 0;

  // reset score and lives
  prefs->game.gameScore            = 0;
  prefs->game.gameLives            = 3;

  // reset dkjr specific things
  prefs->game.dkjr.gameType        = gameType;
  prefs->game.dkjr.gameLevel       = 1;
  prefs->game.dkjr.bonusAvailable  = TRUE;
  prefs->game.dkjr.bonusScoring    = FALSE;

  prefs->game.dkjr.dropWait        = 0;
  prefs->game.dkjr.dropPosition    = 0;

  prefs->game.dkjr.keyHide         = FALSE;
  prefs->game.dkjr.keyDirection    = 1;
  prefs->game.dkjr.keyPosition     = 0;
  prefs->game.dkjr.keyWait         = 0;

  prefs->game.dkjr.dkjrCount       = 0;
  prefs->game.dkjr.dkjrPosition    = 0;
  prefs->game.dkjr.dkjrNewPosition = 0;
  prefs->game.dkjr.dkjrHangWait    = 0;
  prefs->game.dkjr.dkjrJumpWait    = 0;

  prefs->game.dkjr.cageCount       = 4;

  prefs->game.dkjr.chopCount       = 0;
  memset(prefs->game.dkjr.chopPosition, (UBYTE)0, sizeof(USHORT) * MAX_CHOPS);
  memset(prefs->game.dkjr.chopWait,     (UBYTE)0, sizeof(USHORT) * MAX_CHOPS);

  prefs->game.dkjr.birdCount       = 0;
  memset(prefs->game.dkjr.birdPosition, (UBYTE)0, sizeof(USHORT) * MAX_BIRDS);
  memset(prefs->game.dkjr.birdWait,     (UBYTE)0, sizeof(USHORT) * MAX_BIRDS);

  // reset the "backup" and "onscreen" flags
  gbls->cageBackupAvailable        = FALSE;
  gbls->cageChanged                = TRUE;
  gbls->keyChanged                 = TRUE;
  gbls->keyOnScreen                = FALSE;
  gbls->dropChanged                = TRUE;
  gbls->dropOnScreen               = FALSE;
  for (i=0; i<MAX_BIRDS; i++) {
    gbls->birdChanged[i]           = TRUE;
    gbls->birdOnScreen[i]          = FALSE;
  }
  for (i=0; i<MAX_CHOPS; i++) {
    gbls->chopChanged[i]           = TRUE;
    gbls->chopOnScreen[i]          = FALSE;
  }
  gbls->kongChanged                = TRUE;
  gbls->kongOnScreen               = FALSE;

  gbls->gameType                   = gameType;
  gbls->playerDied                 = FALSE;
  gbls->moveDelayCount             = 0;
  gbls->moveLast                   = moveNone;
  gbls->moveNext                   = moveNone;
}

/**
 * Process key input from the user.
 * 
 * @param prefs the global preference data.
 * @param keyStatus the current key state.
 */  
void   
GameProcessKeyInput(PreferencesType *prefs, UWORD keyStatus)
{
  // the helio device does not have a very nice "key" pattern so
  // playing games using the keys may not be an easy task :) the
  // following is coded as a "prototype", maybe someone will use
  // the "key" capabilities. :))
  //
  // the system is hardcoded as follows:
  //
  //   pageUp       = move up
  //   pageDown     = move down
  //   address book = move left
  //   to do list   = move right
  //   scheduler    = jump
  //
  // :P enjoy
  //
  // -- Aaron Ardiri, 2000

#define ctlKeyUp    keyBitPageUp
#define ctlKeyDown  keyBitPageDown
#define ctlKeyLeft  keyBitHard1
#define ctlKeyRight keyBitHard2
#define ctlKeyJump  keyBitHard3

  keyStatus &= (ctlKeyJump  |
                ctlKeyUp    |
                ctlKeyDown  |
                ctlKeyLeft  |
                ctlKeyRight);

  // did they press at least one of the game keys?
  if (keyStatus != 0) {

    // if they were waiting, we should reset the game animation count
    if (prefs->game.gameWait) { 
      prefs->game.gameAnimationCount = 0;
      prefs->game.gameWait           = FALSE;
    }

    // great! they wanna play
    prefs->game.gamePaused = FALSE;
  }

  // jump for key
  if (
      ((keyStatus & ctlKeyJump) != 0) &&
      ((keyStatus & ctlKeyLeft) != 0) &&
      (
       (gbls->moveDelayCount == 0) || 
       (gbls->moveLast       != moveJumpKey)
      ) 
     ) {

    // adjust the position if possible
    if (prefs->game.dkjr.dkjrPosition == 18) {
      prefs->game.dkjr.dkjrNewPosition = 20;
    }
  }

  // jump
  else
  if (
      ((keyStatus &  ctlKeyJump) != 0) &&
      (
       (gbls->moveDelayCount == 0) || 
       (gbls->moveLast       != moveJump)
      ) &&
      ((prefs->game.dkjr.dkjrPosition % 2) == 0) // on floor
     ) { 
    
    SHORT newPosition = prefs->game.dkjr.dkjrPosition + 1;

    // adjust the position if possible
    if ((newPosition >= 0) && (newPosition < 20) && (newPosition != 13)) 
      prefs->game.dkjr.dkjrNewPosition = newPosition;
  }
 
  // move left
  else
  if (
      ((keyStatus &  ctlKeyLeft) != 0) &&
      (
       (gbls->moveDelayCount == 0) || 
       (gbls->moveLast       != moveLeft)
      )
     ) {

    // adjust the position if possible (lower level)
    if ((prefs->game.dkjr.dkjrPosition < 12) &&
        (prefs->game.dkjr.dkjrPosition >  1)
       ) {

      SHORT newPosition = prefs->game.dkjr.dkjrPosition - 2;

      // adjust the position if possible
      if ((newPosition >= 0) && (newPosition < 20) && (newPosition != 13)) 
        prefs->game.dkjr.dkjrNewPosition = newPosition;
    }

    // adjust the position if possible (upper level)
    else
    if ((prefs->game.dkjr.dkjrPosition > 11) &&
        (prefs->game.dkjr.dkjrPosition < 18)
       ) {

      SHORT newPosition = prefs->game.dkjr.dkjrPosition + 2;

      // adjust the position if possible
      if ((newPosition >= 0) && (newPosition < 20) && (newPosition != 13)) 
        prefs->game.dkjr.dkjrNewPosition = newPosition;
    }
  }

  // move right
  else
  if (
      ((keyStatus & ctlKeyRight) != 0) &&
      (
       (gbls->moveDelayCount == 0) || 
       (gbls->moveLast       != moveRight)
      )
     ) {

    // adjust the position if possible (lower level)
    if ((prefs->game.dkjr.dkjrPosition < 12) &&
        (prefs->game.dkjr.dkjrPosition < 10)
       ) {

      SHORT newPosition = prefs->game.dkjr.dkjrPosition + 2;

      // adjust the position if possible
      if ((newPosition >= 0) && (newPosition < 20) && (newPosition != 13)) 
        prefs->game.dkjr.dkjrNewPosition = newPosition;
    }

    // adjust the position if possible (upper level)
    else
    if ((prefs->game.dkjr.dkjrPosition > 11) &&
        (prefs->game.dkjr.dkjrPosition > 13)
       ) {

      SHORT newPosition = prefs->game.dkjr.dkjrPosition - 2;

      // adjust the position if possible
      if ((newPosition >= 0) && (newPosition < 20) && (newPosition != 13)) 
        prefs->game.dkjr.dkjrNewPosition = newPosition;
    }
  }

  // move up
  else
  if (
      (
       (
        ((keyStatus &  ctlKeyJump) != 0) &&
        (
         (gbls->moveDelayCount == 0) || 
         (gbls->moveLast       != moveJump)
        )
       ) ||
       (
        ((keyStatus &    ctlKeyUp) != 0) && 
        (
         (gbls->moveDelayCount == 0) || 
         (gbls->moveLast       != moveUp)
        )
       )
      )
     ) {

    // adjust the position if possible (move from lower to upper)
    if (prefs->game.dkjr.dkjrPosition == 11) { 
      prefs->game.dkjr.dkjrNewPosition = 12;
    }
  }

  // move down
  else
  if (
      ((keyStatus &  ctlKeyDown) != 0) &&
      (
       (gbls->moveDelayCount == 0) || 
       (gbls->moveLast       != moveDown)
      )
     ) {

    // adjust the position if possible (move from upper to lower)
    if (prefs->game.dkjr.dkjrPosition == 12) { 
      prefs->game.dkjr.dkjrNewPosition = 11;
    }

    // adjust the position if possible (in the air?)
    if ((prefs->game.dkjr.dkjrPosition % 2) == 1) { 

      SHORT newPosition = prefs->game.dkjr.dkjrPosition - 1;

      // adjust the position if possible
      if ((newPosition >= 0) && (newPosition < 20) && (newPosition != 13)) 
        prefs->game.dkjr.dkjrNewPosition = newPosition;
    }
  }
}

/**
 * Process stylus input from the user.
 * 
 * @param prefs the global preference data.
 * @param x the x co-ordinate of the stylus event.
 * @param y the y co-ordinate of the stylus event.
 */  
void   
GameProcessStylusInput(PreferencesType *prefs, SHORT x, SHORT y)
{
  GfxRegion region;
  SHORT     i;

  // lets take a look at all the possible "positions"
  for (i=0; i<21; i++) {

    // get the bounding box of the position
    GameGetSpritePosition(spriteKong, i,
                          &region.topLeft.x, &region.topLeft.y);
    region.extent.x  = 20;
    region.extent.y  = 20;

    // did they tap inside this rectangle?
    if ((i != 13) && 
        ((x >= region.topLeft.x) && (y >= region.topLeft.y) && 
	 (x <= (region.topLeft.x+region.extent.x)) &&
	 (y <= (region.topLeft.y+region.extent.y)))) {

      // ok, this is where we are going to go :)
      prefs->game.dkjr.dkjrNewPosition = i;

      // if they were waiting, we should reset the game animation count
      if (prefs->game.gameWait) { 
        prefs->game.gameAnimationCount = 0;
        prefs->game.gameWait           = FALSE;
      }

      // great! they wanna play
      prefs->game.gamePaused = FALSE;
      break;                                        // stop looking
    }
  }
}

/**
 * Process the object movement in the game.
 * 
 * @param prefs the global preference data.
 */  
void   
GameMovement(PreferencesType *prefs)
{
  SHORT     i, j;
  GfxRegion region  = {{   8,  18 }, { 144, 14 }};

  //
  // the game is NOT paused.
  //

  if (!prefs->game.gamePaused) {

    // animate the key
    if (prefs->game.dkjr.keyWait == 0) {
    
      if ((prefs->game.dkjr.keyPosition  == 0) && 
          (prefs->game.dkjr.keyDirection == -1)) 
        prefs->game.dkjr.keyDirection = 1;
      if ((prefs->game.dkjr.keyPosition  == 3) && 
          (prefs->game.dkjr.keyDirection == 1)) 
        prefs->game.dkjr.keyDirection = -1;
    
      prefs->game.dkjr.keyPosition += prefs->game.dkjr.keyDirection;
      prefs->game.dkjr.keyWait      = 2;
  
      gbls->keyChanged = TRUE;
    }
    else
      prefs->game.dkjr.keyWait--;

    // we must make sure the user is ready for playing 
    if (!prefs->game.gameWait) {

      // we cannot be dead yet :)
      gbls->playerDied = FALSE;

      // are we in bonus mode?
      if ((prefs->game.dkjr.bonusScoring) &&
          (prefs->game.gameAnimationCount % GAME_FPS) < (GAME_FPS >> 1)) {

        BYTE    str[32];
        GfxFont currFont = GfxGetFont();

        strcpy(str, "    * BONUS PLAY *    ");
        GfxSetFont(gfx_palmosBoldFont);
        GfxDrawString(str, strlen(str), 
                      80 - (GfxGetCharsWidth(str, strlen(str)) >> 1), 22, gfxPaint);
        GfxSetFont(currFont);
      }
      else 
        GfxFillRegion(GfxGetDrawWindow(), &region, gfx_white);

      // has the dropping object hit something?
      switch (prefs->game.dkjr.dropPosition) 
      {
        case 1: 
             // upper level (chops)
             {
               i = 0;
               while (i<prefs->game.dkjr.chopCount) {

                 // are they in the right position?
                 if (prefs->game.dkjr.chopPosition[i] == 2) {
            
                   // this is worth 3 points
                   for (j=0; j<3; j++) {
                     GameIncrementScore(prefs);
                   }

                   // we need to remove the chop
                   GameRemoveChop(prefs, i);
                   prefs->game.dkjr.dropWait = 0; // move the drop object 
                 }
                 else i++;
               }
             }
             break;

        case 2:
             // lower level (birds)
             {
               i = 0;
               while (i<prefs->game.dkjr.birdCount) {

                 // are they in the right position?
                 if (prefs->game.dkjr.birdPosition[i] == 4) {
            
                   // this is worth 6 points
                   for (j=0; j<6; j++) {
                     GameIncrementScore(prefs);
                   }

                   // we need to remove the bird
                   GameRemoveBird(prefs, i);
                   prefs->game.dkjr.dropWait = 0; // move the drop object 
                 }
                 else i++;
               }
             }
             break;

        case 3: 
             // lower level (chops)
             {
               i = 0;
               while (i<prefs->game.dkjr.chopCount) {

                 // are they in the right position?
                 if (prefs->game.dkjr.chopPosition[i] == 9) {
            
                   // this is worth 9 points
                   for (j=0; j<9; j++) {
                     GameIncrementScore(prefs);
                   }

                   // we need to remove the chop
                   GameRemoveChop(prefs, i);
                   prefs->game.dkjr.dropWait = 0; // move the drop object 
                 }
                 else i++;
               }
             }
             break;

        case 0: 
        default:
             break;
      }

      // player gets first move
      GameMovePlayer(prefs);

      // enemies move next
      GameMoveBirds(prefs);
      GameMoveChops(prefs);

      // animate the drop
      if (prefs->game.dkjr.dropWait == 0) {

        // it must be at position 1, 2 or 3 to animate
        if ((prefs->game.dkjr.dropPosition > 0) &&
            (prefs->game.dkjr.dropPosition < 4)) {

          prefs->game.dkjr.dropPosition++;
          prefs->game.dkjr.dropWait =
            (gbls->gameType == GAME_A) ? 4 : 3;
  
          gbls->dropChanged = TRUE;
        }
      }
      else
        prefs->game.dkjr.dropWait--;

      // is it time to upgrade the game?
      if (prefs->game.gameAnimationCount >= 
           ((gbls->gameType == GAME_A) ? 0x17f : 0x100)) {

        prefs->game.gameAnimationCount = 0;
        prefs->game.dkjr.gameLevel++;

        // upgrading of difficulty?
        if (
            (gbls->gameType             == GAME_A) &&
            (prefs->game.dkjr.gameLevel > MAX_BIRDS)
           ) {

          gbls->gameType              = GAME_B;
          prefs->game.dkjr.gameLevel -= 2;  // give em a break :)
        }
      } 

      // has the player died in this frame?
      if (gbls->playerDied) {

        SHORT     index;
        GfxRegion region    = { {   0,   0 }, {   0,   0 } };
        GfxRegion scrRegion = { {   0,   0 }, {   0,   0 } };

        // erase the old DK bitmap (he could have just come out of a jump)?
        if (gbls->kongOnScreen) {

          index = gbls->kongOldPosition;

          // what is the rectangle we need to copy?
          GameGetSpritePosition(spriteKong, index, 
                                &scrRegion.topLeft.x, &scrRegion.topLeft.y);
          scrRegion.extent.x  = 20;
          scrRegion.extent.y  = 32;
          region.topLeft.x    = (index % 7) * scrRegion.extent.x; 
          region.topLeft.y    = (index / 7) * scrRegion.extent.y; 
          region.extent.x     = scrRegion.extent.x;
          region.extent.y     = scrRegion.extent.y;

          // invert the old DK bitmap
          GfxCopyRegion(gbls->winKongs, GfxGetDrawWindow(),
                        &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxMask);
          gbls->kongOnScreen    = FALSE;
        }

        // play death sound and flash the player
        for (i=0; i<4; i++) {

          index = prefs->game.dkjr.dkjrPosition;

          // what is the rectangle we need to copy?
          GameGetSpritePosition(spriteKong, index, 
                                &scrRegion.topLeft.x, &scrRegion.topLeft.y);
          scrRegion.extent.x  = 20;
          scrRegion.extent.y  = 32;
          region.topLeft.x    = (index % 7) * scrRegion.extent.x; 
          region.topLeft.y    = (index / 7) * scrRegion.extent.y; 
          region.extent.x     = scrRegion.extent.x;
          region.extent.y     = scrRegion.extent.y;

          // invert the old DK bitmap
          GfxCopyRegion(gbls->winKongs, GfxGetDrawWindow(),
                        &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxInvert);
          
          // play the beep sound
          SndPlaySndEffect(SNDRES2_BEEP);
          TmrWaitTime(500);
        }

        // lose a life :(
        prefs->game.gameLives--;

        // no more lives left: GAME OVER!!
        if (prefs->game.gameLives == 0) {

          // return to main screen
          EvtAppendEvt(EVT_INLAY_SELECT,0,INLAY_EXIT,0,NULL);

          prefs->game.gamePlaying = FALSE;
        }

        // reset player position and continue game
        else {
          GameAdjustLevel(prefs);
          prefs->game.dkjr.bonusScoring = FALSE;
          prefs->game.gameWait          = TRUE;
        }
      }
    }

    // we have to display "GET READY!"
    else {

      // flash on:
      if ((prefs->game.gameAnimationCount % GAME_FPS) < (GAME_FPS >> 1)) {

        BYTE    str[32];
        GfxFont currFont = GfxGetFont();

        strcpy(str, "    * GET READY *    ");
        GfxSetFont(gfx_palmosBoldFont);
        GfxDrawString(str, strlen(str), 
                      80 - (GfxGetCharsWidth(str, strlen(str)) >> 1), 22, gfxPaint);
        GfxSetFont(currFont);
      }

      // flash off:
      else
        GfxFillRegion(GfxGetDrawWindow(), &region, gfx_white);
    }

    // update the animation counter
    prefs->game.gameAnimationCount++;
  }

  //
  // the game is paused.
  //

  else {

    BYTE    str[32];
    GfxFont currFont = GfxGetFont();

    strcpy(str, "    *  PAUSED  *    ");
    GfxSetFont(gfx_palmosBoldFont);
    GfxDrawString(str, strlen(str), 
                  80 - (GfxGetCharsWidth(str, strlen(str)) >> 1), 22, gfxPaint);
    GfxSetFont(currFont);
  }
}

/**
 * Draw the game on the screen.
 * 
 * @param prefs the global preference data.
 */
void   
GameDraw(PreferencesType *prefs)
{
  SHORT     i, index;
  GfxRegion region    = { {   0,   0 }, {   0,   0 } };
  GfxRegion scrRegion = { {   0,   0 }, {   0,   0 } };

  // 
  // RESTORE BITMAPS ON SCREEN (if required)
  //

  GameDrawRestore(prefs);

  // 
  // DRAW INFORMATION/BITMAPS ON SCREEN
  //

  // draw the score
  {
    SHORT base;
 
    base = 1000;  // max score (4 digits)
    for (i=0; i<4; i++) {

      index = (prefs->game.gameScore / base) % 10;

      // what is the rectangle we need to copy?
      GameGetSpritePosition(spriteDigit, i, 
                            &scrRegion.topLeft.x, &scrRegion.topLeft.y);
      scrRegion.extent.x  = 7;
      scrRegion.extent.y  = 12;
      region.topLeft.x    = index * scrRegion.extent.x;
      region.topLeft.y    = 0;
      region.extent.x     = scrRegion.extent.x;
      region.extent.y     = scrRegion.extent.y;

      // draw the digit!
      GfxCopyRegion(gbls->winDigits, GfxGetDrawWindow(),
                    &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxPaint);
      base /= 10;
    }
  }

  // draw the misses that have occurred :( 
  if (prefs->game.gameLives < 3) {
  
    index = 2 - prefs->game.gameLives;

    // what is the rectangle we need to copy?
    GameGetSpritePosition(spriteMiss, 0, 
                          &scrRegion.topLeft.x, &scrRegion.topLeft.y);
    scrRegion.extent.x  = 48;
    scrRegion.extent.y  = 9;
    region.topLeft.x    = index * scrRegion.extent.x;
    region.topLeft.y    = 0;
    region.extent.x     = scrRegion.extent.x;
    region.extent.y     = scrRegion.extent.y;

    // draw the miss bitmap!
    GfxCopyRegion(gbls->winMisses, GfxGetDrawWindow(),
                  &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);
  }

  // no missed, make sure none are shown
  else {
  
    index = 2;  // the miss with *all* three misses

    // what is the rectangle we need to copy?
    GameGetSpritePosition(spriteMiss, 0, 
                          &scrRegion.topLeft.x, &scrRegion.topLeft.y);
    scrRegion.extent.x  = 48;
    scrRegion.extent.y  = 9;
    region.topLeft.x    = index * scrRegion.extent.x;
    region.topLeft.y    = 0;
    region.extent.x     = scrRegion.extent.x;
    region.extent.y     = scrRegion.extent.y;

    // erase the three miss bitmap!
    GfxCopyRegion(gbls->winMisses, GfxGetDrawWindow(),
                  &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxMask);
  }

  // draw the cage on the screen (only if it has changed)
  if (gbls->cageChanged) {

    // what is the rectangle we need to copy?
    GameGetSpritePosition(spriteCage, 0, 
                          &scrRegion.topLeft.x, &scrRegion.topLeft.y);
    scrRegion.extent.x  = 35;
    scrRegion.extent.y  = 31;
    region.topLeft.x    = 0;
    region.topLeft.y    = 0;
    region.extent.x     = scrRegion.extent.x;
    region.extent.y     = scrRegion.extent.y;

    // backup the area behind bitmap
    GfxCopyRegion(GfxGetDrawWindow(), gbls->winCageBackup,
                  &scrRegion, 0, 0, gfxPaint);
    gbls->cageBackupAvailable = TRUE;

    // what is the rectangle we need to copy?
    index = prefs->game.dkjr.cageCount;
    region.topLeft.x    = index * scrRegion.extent.x; 
    region.topLeft.y    = 0;
    region.extent.x     = scrRegion.extent.x;
    region.extent.y     = scrRegion.extent.y;

    // draw the cage bitmap!
    GfxCopyRegion(gbls->winCages, GfxGetDrawWindow(),
                  &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);

    // dont draw until we need to
    gbls->cageChanged = FALSE;
  }

  // draw the key on the screen (only if it has changed)
  if (gbls->keyChanged) {

    // what is the rectangle we need to copy?
    GameGetSpritePosition(spriteKey, 0, 
                          &scrRegion.topLeft.x, &scrRegion.topLeft.y);
    scrRegion.extent.x  = 28;
    scrRegion.extent.y  = 22;
    region.topLeft.x    = 0;  
    region.topLeft.y    = 0;  
    region.extent.x     = scrRegion.extent.x;
    region.extent.y     = scrRegion.extent.y;

    // 
    // erase the previous key 
    // 

    if (gbls->keyOnScreen) {

      index = gbls->keyOldPosition;

      // what is the rectangle we need to copy?
      region.topLeft.x = index * scrRegion.extent.x; 
      region.topLeft.y = 0;
      region.extent.x  = scrRegion.extent.x;
      region.extent.y  = scrRegion.extent.y;

      // erase the key bitmap!
      GfxCopyRegion(gbls->winKeys, GfxGetDrawWindow(),
                    &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxMask);
      gbls->keyOnScreen    = FALSE;
    }

    // 
    // draw the key at the new position
    // 

    // are we supposed to draw it?
    if (!prefs->game.dkjr.keyHide) {

      index = prefs->game.dkjr.keyPosition;

      // what is the rectangle we need to copy?
      region.topLeft.x = index * scrRegion.extent.x; 
      region.topLeft.y = 0;
      region.extent.x  = scrRegion.extent.x;
      region.extent.y  = scrRegion.extent.y;

      // save this location, record key is onscreen
      gbls->keyOnScreen    = TRUE;
      gbls->keyOldPosition = index;

      // draw the key bitmap!
      GfxCopyRegion(gbls->winKeys, GfxGetDrawWindow(),
                    &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);
    }

    // dont draw until we need to
    gbls->keyChanged = FALSE;
  }

  // draw the drop on the screen (only if it has changed)
  if (gbls->dropChanged) {

    // 
    // erase the previous drop 
    // 

    if (gbls->dropOnScreen) {

      index = gbls->dropOldPosition;

      // what is the rectangle we need to copy?
      GameGetSpritePosition(spriteDrop, index, 
                            &scrRegion.topLeft.x, &scrRegion.topLeft.y);
      scrRegion.extent.x  = 10;
      scrRegion.extent.y  = 10;
      region.topLeft.x    = index * scrRegion.extent.x;
      region.topLeft.y    = 0;
      region.extent.x     = scrRegion.extent.x;
      region.extent.y     = scrRegion.extent.y;

      // invert the old drop bitmap
      GfxCopyRegion(gbls->winDrops, GfxGetDrawWindow(),
                    &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxMask);
      gbls->dropOnScreen    = FALSE;
    }

    // 
    // draw the drop at the new position
    // 

    // lets make sure we can see the drop :)
    if (prefs->game.dkjr.dropPosition < 4) {

      index = prefs->game.dkjr.dropPosition;

      // what is the rectangle we need to copy?
      GameGetSpritePosition(spriteDrop, index, 
                            &scrRegion.topLeft.x, &scrRegion.topLeft.y);
      scrRegion.extent.x  = 10;
      scrRegion.extent.y  = 10;
      region.topLeft.x    = index * scrRegion.extent.x;
      region.topLeft.y    = 0;
      region.extent.x     = scrRegion.extent.x;
      region.extent.y     = scrRegion.extent.y;

      // save this location, record drop is onscreen
      gbls->dropOnScreen    = TRUE;
      gbls->dropOldPosition = index;

      // draw the drop bitmap!
      GfxCopyRegion(gbls->winDrops, GfxGetDrawWindow(),
                    &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);
    }

    // dont draw until we need to
    gbls->dropChanged = FALSE;
  }

  // draw the birds
  for (i=0; i<prefs->game.dkjr.birdCount; i++) {

    // draw the bird on the screen (only if it has changed)
    if (gbls->birdChanged[i]) {

      // 
      // erase the previous bird 
      // 

      if (gbls->birdOnScreen[i]) {

        index = gbls->birdOnScreenPosition[i];

        // what is the rectangle we need to copy?
        GameGetSpritePosition(spriteBird, index, 
                              &scrRegion.topLeft.x, &scrRegion.topLeft.y);
        scrRegion.extent.x  = 10;
        scrRegion.extent.y  = 10;
        region.topLeft.x    = index * scrRegion.extent.x;
        region.topLeft.y    = 0;
        region.extent.x     = scrRegion.extent.x;
        region.extent.y     = scrRegion.extent.y;

        // invert the old bird bitmap
        GfxCopyRegion(gbls->winBirds, GfxGetDrawWindow(),
                      &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxMask);
        gbls->birdOnScreen[i]    = FALSE;
      }

      // 
      // draw the bird at the new position
      // 

      index = prefs->game.dkjr.birdPosition[i];

      // what is the rectangle we need to copy?
      GameGetSpritePosition(spriteBird, index, 
                            &scrRegion.topLeft.x, &scrRegion.topLeft.y);
      scrRegion.extent.x  = 10;
      scrRegion.extent.y  = 10;
      region.topLeft.x    = index * scrRegion.extent.x;
      region.topLeft.y    = 0;
      region.extent.x     = scrRegion.extent.x;
      region.extent.y     = scrRegion.extent.y;

      // save this location, record bird is onscreen
      gbls->birdOnScreen[i]         = TRUE;
      gbls->birdOnScreenPosition[i] = index;

      // draw the bird bitmap!
      GfxCopyRegion(gbls->winBirds, GfxGetDrawWindow(),
                    &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);

      // dont draw until we need to
      gbls->birdChanged[i] = FALSE;
    }
  }

  // draw the chops
  for (i=0; i<prefs->game.dkjr.chopCount; i++) {

    // draw the chop on the screen (only if it has changed)
    if (gbls->chopChanged[i]) {

      // 
      // erase the previous chop 
      // 

      if (gbls->chopOnScreen[i]) {

        index = gbls->chopOnScreenPosition[i];

        // what is the rectangle we need to copy?
        GameGetSpritePosition(spriteChop, index, 
                              &scrRegion.topLeft.x, &scrRegion.topLeft.y);
        scrRegion.extent.x  = 10;
        scrRegion.extent.y  = 10;
        region.topLeft.x    = index * scrRegion.extent.x;
        region.topLeft.y    = 0;
        region.extent.x     = scrRegion.extent.x;
        region.extent.y     = scrRegion.extent.y;

        // invert the old chop bitmap
        GfxCopyRegion(gbls->winChops, GfxGetDrawWindow(),
                      &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxMask);
        gbls->chopOnScreen[i]    = FALSE;
      }

      // 
      // draw the chop at the new position
      // 

      index = prefs->game.dkjr.chopPosition[i];

      // what is the rectangle we need to copy?
      GameGetSpritePosition(spriteChop, index, 
                            &scrRegion.topLeft.x, &scrRegion.topLeft.y);
      scrRegion.extent.x  = 10;
      scrRegion.extent.y  = 10;
      region.topLeft.x    = index * scrRegion.extent.x;
      region.topLeft.y    = 0;
      region.extent.x     = scrRegion.extent.x;
      region.extent.y     = scrRegion.extent.y;

      // save this location, record chop is onscreen
      gbls->chopOnScreen[i]         = TRUE;
      gbls->chopOnScreenPosition[i] = index;

      // draw the chop bitmap!
      GfxCopyRegion(gbls->winChops, GfxGetDrawWindow(),
                    &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);

      // dont draw until we need to
      gbls->chopChanged[i] = FALSE;
    }
  }

  // draw DK (only if it has changed)
  if (gbls->kongChanged) {

    // 
    // erase the previous DK
    // 

    if (gbls->kongOnScreen) {

      index = gbls->kongOldPosition;

      // what is the rectangle we need to copy?
      GameGetSpritePosition(spriteKong, index, 
                            &scrRegion.topLeft.x, &scrRegion.topLeft.y);
      scrRegion.extent.x  = 20;
      scrRegion.extent.y  = 32;
      region.topLeft.x    = (index % 7) * scrRegion.extent.x; 
      region.topLeft.y    = (index / 7) * scrRegion.extent.y; 
      region.extent.x     = scrRegion.extent.x;
      region.extent.y     = scrRegion.extent.y;

      // invert the old DK bitmap
      GfxCopyRegion(gbls->winKongs, GfxGetDrawWindow(),
                    &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxMask);
      gbls->kongOnScreen    = FALSE;
    }

    // 
    // draw DK at the new position
    // 

    index = prefs->game.dkjr.dkjrPosition;

    // what is the rectangle we need to copy?
    GameGetSpritePosition(spriteKong, index, 
                          &scrRegion.topLeft.x, &scrRegion.topLeft.y);
    scrRegion.extent.x  = 20;
    scrRegion.extent.y  = 32;
    region.topLeft.x    = (index % 7) * scrRegion.extent.x; 
    region.topLeft.y    = (index / 7) * scrRegion.extent.y; 
    region.extent.x     = scrRegion.extent.x;
    region.extent.y     = scrRegion.extent.y;

    // save this location, record DK is onscreen
    gbls->kongOnScreen    = TRUE;
    gbls->kongOldPosition = index;

    // draw the DK bitmap!
    GfxCopyRegion(gbls->winKongs, GfxGetDrawWindow(),
                  &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxOverlay);

    // dont draw until we need to
    gbls->kongChanged = FALSE;
  }
}

/**
 * Terminate the game.
 */
void   
GameTerminate()
{
  // clean up windows/memory
  GfxDisposeWindow(gbls->winCageBackup);
  pfree(gbls->winDigits);
  pfree(gbls->winMisses);
  pfree(gbls->winCages);
  pfree(gbls->winKeys);
  pfree(gbls->winDrops);
  pfree(gbls->winBirds);
  pfree(gbls->winChops);                        // mem leak - fixed 24-Dec-2000
  pfree(gbls->winKongs);
  pfree(gbls);
}

/**
 * Get the position of a particular sprite on the screen.
 *
 * @param spriteType the type of sprite.
 * @param index the index required in the sprite position list.
 * @param x the x co-ordinate of the position
 * @param y the y co-ordinate of the position
 */
static void
GameGetSpritePosition(BYTE  spriteType, 
                      BYTE  index, 
                      SHORT *x, 
                      SHORT *y)
{
  switch (spriteType) 
  {
    case spriteDigit: 
         {
           *x = 120 + (index * 9);
           *y = 37;
         }
         break;

    case spriteMiss: 
         {
           *x = 106;
           *y = 51;
         }
         break;

    case spriteCage: 
         {
           *x = 3;
           *y = 37;
         }
         break;

    case spriteKey: 
         {
           *x = 50;
           *y = 41;
         }
         break;

    case spriteDrop: 
         {
           SHORT positions[][2] = {
                                   {  84,  62 },
                                   {  84,  71 },
                                   {  85, 100 },
                                   {  84, 117 }
                                 };

           *x = positions[index][0];
           *y = positions[index][1];
         }
         break;

    case spriteBird: 
         {
           SHORT positions[][2] = {
                                   {   4,  97 },
                                   {  14, 107 },
                                   {  39, 106 },
                                   {  61, 108 },
                                   {  83, 109 },
                                   { 105, 108 },
                                   { 124, 107 },
                                   { 146, 100 }
                                 };

           *x = positions[index][0];
           *y = positions[index][1];
         }
         break;

    case spriteChop: 
         {
           SHORT positions[][2] = {
                                   {  41,  88 },
                                   {  57,  89 },
                                   {  82,  87 },
                                   { 104,  90 },
                                   { 125,  87 },
                                   { 150,  90 },
                                   { 145, 124 },
                                   { 128, 126 },
                                   { 105, 125 },
                                   {  84, 127 },
                                   {  62, 128 },
                                   {  37, 126 },
                                   {  17, 128 }
                                 };

           *x = positions[index][0];
           *y = positions[index][1];
         } 
         break;

    case spriteKong: 
         {
           SHORT positions[][2] = {
                                   {  20, 118 },
                                   {  22, 102 },
                                   {  45, 119 },
                                   {  45, 104 },
                                   {  65, 119 },
                                   {  68, 103 },
                                   {  90, 119 },
                                   {  91, 104 },
                                   { 111, 119 },
                                   { 112, 102 },
                                   { 133, 118 },
                                   { 131, 101 },
                                   { 130,  81 },
                                   {   0,   0 },  // not used
                                   { 108,  82 },
                                   { 113,  67 },
                                   {  88,  81 },
                                   {  90,  64 },
                                   {  65,  79 },
                                   {  64,  65 },
                                   {  49,  64 },
                                   {  13,  91 },
                                   {   4, 107 },
                                   {  32,  37 },
                                   {  32,  37 },
                                   {  34,  68 }
                                 };

           *x = positions[index][0];
           *y = positions[index][1];
         }
         break;

    default:
         break;
  }
}

/**
 * Restore the graphics on the screen (as it was before the draw).
 * 
 * @param prefs the global preference data.
 */
static void   
GameDrawRestore(PreferencesType *prefs)
{
  GfxRegion region    = { {   0,   0 }, {   0,   0 } };
  GfxRegion scrRegion = { {   0,   0 }, {   0,   0 } };

  // 
  // RESTORE INFORMATION/BITMAPS ON SCREEN (in reverse order)
  //

  // restore the area behind the cage (if it is available)
  if (gbls->cageChanged &&
      gbls->cageBackupAvailable) {

    // what is the rectangle we need to copy?
    GameGetSpritePosition(spriteCage, 0, 
                          &scrRegion.topLeft.x, &scrRegion.topLeft.y);
    scrRegion.extent.x  = 35;
    scrRegion.extent.y  = 31;
    region.topLeft.x    = 0;
    region.topLeft.y    = 0;
    region.extent.x     = scrRegion.extent.x;
    region.extent.y     = scrRegion.extent.y;

    GfxCopyRegion(gbls->winCageBackup, GfxGetDrawWindow(),
                  &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxPaint);
    gbls->cageBackupAvailable = FALSE;
  }
}

/**
 * Adjust the level (remove birds that are too close and reset positions)
 *
 * @param prefs the global preference data.
 */
static void 
GameAdjustLevel(PreferencesType *prefs)
{
  SHORT i;

  // remove any birds that are too close to start
  i = 0;
  while (i<prefs->game.dkjr.birdCount) {

    BYTE removalPoint = 2;

    // is the bird too close?
    if (prefs->game.dkjr.birdPosition[i] < removalPoint) {
            
      // we need to remove the bird
      GameRemoveBird(prefs, i);
    }
    else i++;
  }

  // remove any chops that are too close to start
  i = 0;
  while (i<prefs->game.dkjr.chopCount) {

    BYTE removalPoint = 9;

    // is the chop too close?
    if (prefs->game.dkjr.chopPosition[i] > removalPoint) {
            
      // we need to remove the chop
      GameRemoveChop(prefs, i);
    }
    else i++;
  }

  // return to start position
  prefs->game.dkjr.dkjrCount       = 0;
  prefs->game.dkjr.dkjrPosition    = 0;
  prefs->game.dkjr.dkjrNewPosition = 0;
  prefs->game.dkjr.dkjrJumpWait    = 0;
  prefs->game.dkjr.dropPosition    = 0;
  gbls->kongChanged                = TRUE;
  gbls->dropChanged                = TRUE;

  // player is not dead :))
  gbls->playerDied                 = FALSE;
}

/**
 * Increment the players score. 
 *
 * @param prefs the global preference data.
 */
static void 
GameIncrementScore(PreferencesType *prefs)
{
  SHORT      i, index;
  GfxRegion  region     = { {   0,   0 }, {   0,   0 } };
  GfxRegion  scrRegion  = { {   0,   0 }, {   0,   0 } };

  // adjust accordingly
  prefs->game.gameScore += prefs->game.dkjr.bonusScoring ? 2 : 1;

  // redraw score bitmap
  {
    SHORT base;
 
    base = 1000;  // max score (4 digits)
    for (i=0; i<4; i++) {

      index = (prefs->game.gameScore / base) % 10;

      // what is the rectangle we need to copy?
      GameGetSpritePosition(spriteDigit, i, 
                            &scrRegion.topLeft.x, &scrRegion.topLeft.y);
      scrRegion.extent.x  = 7;
      scrRegion.extent.y  = 12;
      region.topLeft.x    = index * scrRegion.extent.x;
      region.topLeft.y    = 0;
      region.extent.x     = scrRegion.extent.x;
      region.extent.y     = scrRegion.extent.y;

      // draw the digit!
      GfxCopyRegion(gbls->winDigits, GfxGetDrawWindow(),
                    &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxPaint);
      base /= 10;
    }
  }

  // play the sound
  SndPlaySndEffect(SNDRES5_BEEP);
  TmrWaitTime(125);

  // is it time for a bonus?
  if (
      (prefs->game.gameScore >= 300) &&
      (prefs->game.dkjr.bonusAvailable)
     ) {

    // little fan-fare :)) - the "veewoo" sound was the best i could find :((
    SndPlaySndEffect(SNDRES_VEEWOO);
    TmrWaitTime(500);

    // apply the bonus!
    if (prefs->game.gameLives == 3) 
      prefs->game.dkjr.bonusScoring = TRUE;
    else
      prefs->game.gameLives = 3;

    prefs->game.dkjr.bonusAvailable = FALSE;
  }
}

/**
 * Move the player.
 *
 * @param prefs the global preference data.
 */
static void
GameMovePlayer(PreferencesType *prefs) 
{
  SHORT i;

  // DK in normal gameplay?
  if (prefs->game.dkjr.dkjrPosition < 20) {

    //
    // where does DK want to go today?
    //

    // current position differs from new position?
    if (prefs->game.dkjr.dkjrPosition != prefs->game.dkjr.dkjrNewPosition) {

      // bottom level: 
      if (prefs->game.dkjr.dkjrPosition < 12) {

        // special case, jump to upper level
        if ((prefs->game.dkjr.dkjrPosition    == 11) && 
            (prefs->game.dkjr.dkjrNewPosition  > 11)) {
          gbls->moveNext = moveUp;
        }

        // special case, at end and must jump to position 11
        else
        if ((prefs->game.dkjr.dkjrPosition    == 10) && 
            (prefs->game.dkjr.dkjrNewPosition >= 11)) {
          gbls->moveNext = moveJump;
        }

        // move left:
        else
        if ((prefs->game.dkjr.dkjrPosition    >> 1) > 
            (prefs->game.dkjr.dkjrNewPosition >> 1)) {
          gbls->moveNext = moveLeft;
        }

        // move right:
        else
        if ((prefs->game.dkjr.dkjrPosition    >> 1) < 
            (prefs->game.dkjr.dkjrNewPosition >> 1)) {
          gbls->moveNext = moveRight;
        }

        // jump up:
        else
        if (((prefs->game.dkjr.dkjrNewPosition % 2) == 1) &&
            ((prefs->game.dkjr.dkjrPosition    % 2) == 0)) {
          gbls->moveNext = moveJump;
        }

        // move down:
        else 
        if (((prefs->game.dkjr.dkjrNewPosition % 2) == 0) &&
            ((prefs->game.dkjr.dkjrPosition    % 2) == 1)) {
          gbls->moveNext = moveDown;
        }
      }

      // upper level:
      else {

        // special case: at "hiding spot" - one place to go :))
        if (prefs->game.dkjr.dkjrPosition     == 15) {
          gbls->moveNext = moveDown;
        }

        // special case, move down to lower level
        else
        if ((prefs->game.dkjr.dkjrPosition    == 12) && 
            (prefs->game.dkjr.dkjrNewPosition  < 12)) {
          gbls->moveNext = moveDown;
        }

        // special case, jump for key!
        else
        if ((prefs->game.dkjr.dkjrPosition    == 18) && 
            (prefs->game.dkjr.dkjrNewPosition == 20)) {
          gbls->moveNext = moveJumpKey;
        }

        // move left:
        else
        if ((prefs->game.dkjr.dkjrPosition    >> 1) < 
            (prefs->game.dkjr.dkjrNewPosition >> 1)) {
          gbls->moveNext = moveLeft;
        }

        // move right:
        else
        if ((prefs->game.dkjr.dkjrPosition    >> 1) > 
            (prefs->game.dkjr.dkjrNewPosition >> 1)) {
          gbls->moveNext = moveRight;
        }

        // jump up:
        else
        if (((prefs->game.dkjr.dkjrNewPosition % 2) == 1) &&
            ((prefs->game.dkjr.dkjrPosition    % 2) == 0)) {
          gbls->moveNext = moveJump;
        }

        // move down:
        else 
        if (((prefs->game.dkjr.dkjrNewPosition % 2) == 0) &&
            ((prefs->game.dkjr.dkjrPosition    % 2) == 1)) {
          gbls->moveNext = moveDown;
        }
      }
    }

    // lets make sure they are allowed to do the move
    if (
        (gbls->moveDelayCount == 0) || 
        (gbls->moveLast != gbls->moveNext) 
       ) {
      gbls->moveDelayCount = 
       ((gbls->gameType == GAME_A) ? 4 : 3);
    }
    else {
      gbls->moveDelayCount--;
      gbls->moveNext = moveNone;
    }

    //
    // move DK into the right position based on his desired move
    //

    // is the player suspended in the air/hanging on vine?
    if ((prefs->game.dkjr.dkjrPosition % 2) == 1) {

      // has DK been on vine/in air too long?
      if (prefs->game.dkjr.dkjrHangWait == 0) {

        // lets make sure they dont jump back up :)
        if (prefs->game.dkjr.dkjrNewPosition == prefs->game.dkjr.dkjrPosition)
          prefs->game.dkjr.dkjrNewPosition = prefs->game.dkjr.dkjrPosition - 1;

        // automagically move down :)
        gbls->moveNext = moveDown;
      }
      else {
        prefs->game.dkjr.dkjrHangWait--;
      }

      // has DK just completed a jump over a chop?
      if (
          ((prefs->game.dkjr.dkjrPosition % 2) == 1) &&
          (prefs->game.dkjr.dkjrJumpWait == 1)
         ) {

        for (i=0; i<prefs->game.dkjr.chopCount; i++) {

          if ((
               (prefs->game.dkjr.dkjrPosition > 11) &&        // upper level
               (prefs->game.dkjr.dkjrPosition < 20) &&        
               (gbls->moveLast != moveRight)        &&
               ((((prefs->game.dkjr.dkjrPosition-1) / 2) - 6) == 
                 (5 - prefs->game.dkjr.chopPosition[i]))      // chop position
              ) ||
              (
               (prefs->game.dkjr.dkjrPosition < 12) &&        // lower level
               (gbls->moveLast != moveLeft)         &&
               (((prefs->game.dkjr.dkjrPosition-1) / 2) == 
                 (12 - prefs->game.dkjr.chopPosition[i]))     // chop position
              )) { 

            // increase score
            GameIncrementScore(prefs);
          }
        }
      }
    }
  
    // update counter
    prefs->game.dkjr.dkjrCount++;

    // can the player move?
    if (prefs->game.dkjr.dkjrJumpWait == 0) {

      // which diregionion do they wish to move?
      switch (gbls->moveNext)
      {
        case moveLeft:

             // lower level:
             if (prefs->game.dkjr.dkjrPosition < 12) {

               // lets make sure they can move left
               if (prefs->game.dkjr.dkjrPosition > 1) {

                 // step out into air:
                 if ((prefs->game.dkjr.dkjrPosition % 2) == 1) {
                 
                   // did DK move into a bird?
                   for (i=0; i<prefs->game.dkjr.birdCount; i++) {

                     gbls->playerDied |= 
                       (
                        ((prefs->game.dkjr.dkjrPosition-2) >> 1) == 
                        (prefs->game.dkjr.birdPosition[i]-2)
                       );
                   }

                   // DK did a valid move (no death)
                   if (!gbls->playerDied) {

                     prefs->game.dkjr.dkjrPosition -= 2;
                     gbls->kongChanged              = TRUE;

                     // no vine available:
                     if ((prefs->game.dkjr.dkjrPosition == 3) ||
                         (prefs->game.dkjr.dkjrPosition == 9)) {
                       prefs->game.dkjr.dkjrJumpWait = 4;
                       prefs->game.dkjr.dkjrHangWait = 
                         prefs->game.dkjr.dkjrJumpWait;
                     } 

                     // vine available:
                     else {
                       prefs->game.dkjr.dkjrJumpWait = 0;
                       prefs->game.dkjr.dkjrHangWait = 64;
                     }
                   }
                 }

                 // walking on the ground:
                 else {

                   // did DK move into a chop?
                   for (i=0; i<prefs->game.dkjr.chopCount; i++) {

                     gbls->playerDied |= 
                       (
                        ((prefs->game.dkjr.dkjrPosition >> 1) == 
                        (12 - prefs->game.dkjr.chopPosition[i]))
                       );
                   }

                   // DK did a valid move (no death)
                   if (!gbls->playerDied) {

                     prefs->game.dkjr.dkjrPosition -= 2;
                     gbls->kongChanged              = TRUE;
                   }
                 }
               }
             }

             // upper level:
             else {

               // lets make sure they can move left
               if ((prefs->game.dkjr.dkjrPosition < 18) &&
                   ((prefs->game.dkjr.dkjrPosition % 2) == 0)) {

                 // did DK move into a chop?
                 for (i=0; i<prefs->game.dkjr.chopCount; i++) {

                   gbls->playerDied |= 
                     (
                      (((prefs->game.dkjr.dkjrPosition+2) >> 1) - 6) == 
                      (5 - prefs->game.dkjr.chopPosition[i])
                     );
                 }

                 // DK did a valid move (no death)
                 if (!gbls->playerDied) {

                   prefs->game.dkjr.dkjrPosition += 2;
                   gbls->kongChanged              = TRUE;
                 }
               }
             }
             break;

        case moveRight:

             // lower level:
             if (prefs->game.dkjr.dkjrPosition < 12) {

               // lets make sure they can move right
               if (prefs->game.dkjr.dkjrPosition < 10) {

                 // step out into air:
                 if ((prefs->game.dkjr.dkjrPosition % 2) == 1) {
                 
                   // did DK move into a bird?
                   for (i=0; i<prefs->game.dkjr.birdCount; i++) {

                     gbls->playerDied |= 
                       (
                        (prefs->game.dkjr.dkjrPosition >> 1) == 
                        (prefs->game.dkjr.birdPosition[i]-2)
                       );
                   }

                   // DK did a valid move (no death)
                   if (!gbls->playerDied) {

                     prefs->game.dkjr.dkjrPosition += 2;
                     gbls->kongChanged              = TRUE;

                     // no vine available:
                     if ((prefs->game.dkjr.dkjrPosition == 3) ||
                         (prefs->game.dkjr.dkjrPosition == 9)) {
                       prefs->game.dkjr.dkjrJumpWait = 4;
                       prefs->game.dkjr.dkjrHangWait = 
                         prefs->game.dkjr.dkjrJumpWait;
                     } 

                     // vine available:
                     else {
                       prefs->game.dkjr.dkjrJumpWait = 0;
                       prefs->game.dkjr.dkjrHangWait = 64;
                     }
                   }
                 }

                 // walking on the ground:
                 else {

                   // did DK move into a chop?
                   for (i=0; i<prefs->game.dkjr.chopCount; i++) {

                     gbls->playerDied |= 
                       (
                        ((prefs->game.dkjr.dkjrPosition+2) >> 1) == 
                        (12 - prefs->game.dkjr.chopPosition[i])
                       );
                   }

                   // DK did a valid move (no death)
                   if (!gbls->playerDied) {

                     prefs->game.dkjr.dkjrPosition += 2;
                     gbls->kongChanged              = TRUE;
                   }
                 }
               }
             }

             // upper level:
             else {

               // lets make sure they can move right
               if ((prefs->game.dkjr.dkjrPosition > 12) &&
                   ((prefs->game.dkjr.dkjrPosition % 2) == 0)) {

                 // did DK move into a chop?
                 for (i=0; i<prefs->game.dkjr.chopCount; i++) {

                   gbls->playerDied |= 
                     (
                      ((prefs->game.dkjr.dkjrPosition / 2) - 6) == 
                      (5 - prefs->game.dkjr.chopPosition[i])
                     );
                 }

                 // DK did a valid move (no death)
                 if (!gbls->playerDied) {
                   prefs->game.dkjr.dkjrPosition -= 2;
                   gbls->kongChanged              = TRUE;
                 }
               }
             }
             break;

        case moveJump:
             // we can only jump if we are on the ground
             if (
                 ((prefs->game.dkjr.dkjrPosition % 2) == 0) &&
                  (prefs->game.dkjr.dkjrPosition != 12)
                ) {

               prefs->game.dkjr.dkjrPosition++;
               gbls->kongChanged = TRUE;

               // DK jumped into air?
               if ((prefs->game.dkjr.dkjrPosition == 3)  ||
                   (prefs->game.dkjr.dkjrPosition == 9)  ||
                   (prefs->game.dkjr.dkjrPosition == 17) ||
                   (prefs->game.dkjr.dkjrPosition == 19)) {
                 prefs->game.dkjr.dkjrJumpWait = 4;
                 prefs->game.dkjr.dkjrHangWait = prefs->game.dkjr.dkjrJumpWait;
               }

               // DK landed on a vine
               else {
                 prefs->game.dkjr.dkjrJumpWait = 0;
                 prefs->game.dkjr.dkjrHangWait = 64;
               }
             }
             break;

        case moveJumpKey:

             // go for it!
             prefs->game.dkjr.dkjrJumpWait =
               (gbls->gameType == GAME_A) ? 3 : 2;
             prefs->game.dkjr.dkjrHangWait = prefs->game.dkjr.dkjrJumpWait;
             prefs->game.dkjr.dkjrPosition = 20;
             gbls->kongChanged             = TRUE;
             break;

        case moveUp:

             // we can only move up at pos=11
             if (prefs->game.dkjr.dkjrPosition == 11) {
               prefs->game.dkjr.dkjrPosition++;
               gbls->kongChanged = TRUE;
             }
             break;

        case moveDown:

             // we can only move down if at pos=12, or are off ground
             if ((prefs->game.dkjr.dkjrPosition == 12) ||
                 ((prefs->game.dkjr.dkjrPosition % 2) == 1)) {

               prefs->game.dkjr.dkjrPosition--;
               gbls->kongChanged = TRUE;

               // DK landed on a vine
               if (prefs->game.dkjr.dkjrPosition == 11) {
                 prefs->game.dkjr.dkjrHangWait = 64;
               }
             }
             break;

        default:
             break;
      }

      gbls->moveLast = gbls->moveNext;
      gbls->moveNext = moveNone;
    }
    else {
      prefs->game.dkjr.dkjrJumpWait--;
    }
  }

  // DK reaching for key? falling?
  else {

    BOOLEAN grabbedKey = FALSE;

    // lets determine if DK grabbed key
    grabbedKey = (
                  (prefs->game.dkjr.dkjrPosition == 20) &&
                  (prefs->game.dkjr.keyPosition  == 0)  && 
                  (prefs->game.dkjr.keyWait      <  2) // must be there 1 frame
                 );
    
    // DK did not grab key :(
    if (!grabbedKey) {

      // can the player move?
      if (prefs->game.dkjr.dkjrJumpWait == 0) {

        switch (prefs->game.dkjr.dkjrPosition)
        {
          case 22: // DK missed? 
               gbls->playerDied = TRUE;
               break;

          case 25: // DK is done :)
               GameAdjustLevel(prefs);

               // has the cage been opened?
               if (prefs->game.dkjr.cageCount == 0) {
                 prefs->game.dkjr.cageCount = 4;
                 gbls->cageChanged = TRUE;
               }
               prefs->game.dkjr.keyHide = FALSE;
               gbls->keyChanged         = TRUE;
               break;

          case 24: // DK opening cage!
               {
                 SHORT i, bonus, ticks;

                 // release a lock
                 prefs->game.dkjr.cageCount--;
                 gbls->cageChanged = TRUE;
                 bonus = (prefs->game.dkjr.cageCount == 0) ? 20 : 0;

                 // how many half seconds did it take?
                 ticks = (prefs->game.dkjr.dkjrCount / (GAME_FPS >> 1));
 
                 // < 2.5 secs +20, > 10 secs +5, between? 5..20 :)
                 if (ticks < 5)  bonus += 20; else
                 if (ticks > 20) bonus += 5;  else
                                 bonus += 5 + (20 - ticks);

                 // apply the bonus
                 for (i=0; i<bonus; i++) {
                   GameIncrementScore(prefs);
                 }
               }  

          default:
               prefs->game.dkjr.dkjrPosition++;
               prefs->game.dkjr.dkjrJumpWait = 4;
               gbls->kongChanged             = TRUE;
               break;
        }
      }
      else {
        prefs->game.dkjr.dkjrJumpWait--;
      }
    }

    // DK did grab key :)
    else {

      prefs->game.dkjr.dkjrPosition = 23;   // skip forward
      prefs->game.dkjr.dkjrJumpWait = 4;
      prefs->game.dkjr.keyHide      = TRUE;
      gbls->kongChanged             = TRUE;
      gbls->keyChanged              = TRUE;
    }
  }

  // do we need to play a movement sound? 
  if (gbls->kongChanged)  
    SndPlaySndEffect(SNDRES0_BEEP);
  
  // has the player jumped near the drop object, and it is available?
  if ((prefs->game.dkjr.dkjrPosition == 17) &&
      (prefs->game.dkjr.dropPosition == 0)) {

    prefs->game.dkjr.dropPosition++;
    prefs->game.dkjr.dropWait =
      (gbls->gameType == GAME_A) ? 4 : 3;

    gbls->dropChanged = TRUE;
  }
}

/**
 * Move the birds.
 *
 * @param prefs the global preference data.
 */
static void
GameMoveBirds(PreferencesType *prefs) 
{
  // only do this if the player is still alive
  if (!gbls->playerDied) {

    SHORT i, j;

    // process all birds available
    i = 0;
    while (i<prefs->game.dkjr.birdCount) {

      BOOLEAN removal = FALSE;

      if (prefs->game.dkjr.birdWait[i] == 0) {

        BOOLEAN ok;

        // lets make sure it is not moving into a bird in front of us?
        ok = TRUE;
        for (j=0; j<prefs->game.dkjr.birdCount; j++) {
          ok &= (
                 (prefs->game.dkjr.birdPosition[i]+1 != 
                  prefs->game.dkjr.birdPosition[j])
                );
        }

        // the coast is clear, move!
        if (ok) {

          prefs->game.dkjr.birdPosition[i]++;
          prefs->game.dkjr.birdWait[i] = 
            (gbls->gameType == GAME_A) ? 6 : 5;
  
          // the bird has disapeared offscreen:
          if (prefs->game.dkjr.birdPosition[i] > (MAX_BIRDS-1)) {
            GameRemoveBird(prefs, i); removal = TRUE;
          }
  
          // the bird has just moved position onscreen:
          else {
  
            // has it killed the player?
            gbls->playerDied |= 
              (
                (prefs->game.dkjr.dkjrPosition < 12) &&        // lower level
                ((prefs->game.dkjr.dkjrPosition % 2) == 1) &&  // off the ground
                ((prefs->game.dkjr.dkjrPosition / 2) == 
                   (prefs->game.dkjr.birdPosition[i]-2))       // bird position
              );
    
            gbls->birdChanged[i] = TRUE;
          }
    
          // play a movement sound
          SndPlaySndEffect(SNDRES1_BEEP);
        }
      }
      else {
        prefs->game.dkjr.birdWait[i]--;
      }

      if (!removal) i++;
    }

    // new bird appearing on screen?
    {
      BOOLEAN ok;
      BYTE    birthFactor      = (gbls->gameType == GAME_A) ? 8 : 4;
      BYTE    maxOnScreenBirds = (1 + ((prefs->game.dkjr.gameLevel-1) >> 1));
  
      // we must be able to add a bird (based on level)
      ok = (
            (prefs->game.dkjr.birdCount < maxOnScreenBirds) &&
            (prefs->game.dkjr.birdCount < MAX_BIRDS) &&
            ((DeviceRandom(0) % birthFactor) == 0)
           );
  
      // lets check that there are no birds at index=0
      for (i=0; i<prefs->game.dkjr.birdCount; i++) {
        ok &= (prefs->game.dkjr.birdPosition[i] != 0);
      }
  
      // lets add a new bird :) 
      if (ok) {
        i = prefs->game.dkjr.birdCount++;
        prefs->game.dkjr.birdPosition[i] = 0;
        prefs->game.dkjr.birdWait[i]     =
          (gbls->gameType == GAME_A) ? 6 : 5;
        gbls->birdChanged[i]             = TRUE;
        gbls->birdOnScreen[i]            = FALSE;
        gbls->birdOnScreenPosition[i]    = 0;
      }
    }
  }
}

/**
 * Remove a bird from the game.
 *
 * @param prefs the global preference data.
 * @param birdIndex the index of the bird to remove.
 */
static void
GameRemoveBird(PreferencesType *prefs,  
               SHORT           birdIndex) 
{
  SHORT     index;
  GfxRegion region    = { {   0,   0 }, {   0,   0 } };
  GfxRegion scrRegion = { {   0,   0 }, {   0,   0 } };

  //
  // remove the bitmap from the screen
  //

  if (gbls->birdOnScreen[birdIndex]) {

    index = gbls->birdOnScreenPosition[birdIndex];

    // what is the rectangle we need to copy?
    GameGetSpritePosition(spriteBird, index, 
                          &scrRegion.topLeft.x, &scrRegion.topLeft.y);
    scrRegion.extent.x  = 10;
    scrRegion.extent.y  = 10;
    region.topLeft.x    = index * scrRegion.extent.x;
    region.topLeft.y    = 0;
    region.extent.x     = scrRegion.extent.x;
    region.extent.y     = scrRegion.extent.y;

    // invert the old bird bitmap
    GfxCopyRegion(gbls->winBirds, GfxGetDrawWindow(),
                  &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxMask);
  }
  gbls->birdOnScreen[birdIndex]         = FALSE;
  gbls->birdOnScreenPosition[birdIndex] = 0;     // ensure this is clean

  // we will push the 'bird' out of the array
  //
  // before: 1234567---  after: 1345672---
  //          ^     |                 |
  //                end point         end point

  prefs->game.dkjr.birdCount--;

  // removal NOT from end?
  if (prefs->game.dkjr.birdCount > birdIndex) {

    SHORT i, count;

    count = prefs->game.dkjr.birdCount - birdIndex;

    // shift all elements down
    for (i=birdIndex; i<(birdIndex+count); i++) {
      prefs->game.dkjr.birdPosition[i] = prefs->game.dkjr.birdPosition[i+1];
      prefs->game.dkjr.birdWait[i]     = prefs->game.dkjr.birdWait[i+1];
      gbls->birdChanged[i]             = gbls->birdChanged[i+1];
      gbls->birdOnScreen[i]            = gbls->birdOnScreen[i+1];
      gbls->birdOnScreenPosition[i]    = gbls->birdOnScreenPosition[i+1];
    }
  }
}

/**
 * Move the chops.
 *
 * @param prefs the global preference data.
 */
static void
GameMoveChops(PreferencesType *prefs) 
{
  // only do this if the player is still alive
  if (!gbls->playerDied) {

    SHORT i, j;

    // process all chops available
    i = 0;
    while (i<prefs->game.dkjr.chopCount) {
  
      BOOLEAN removal = FALSE;

      if (prefs->game.dkjr.chopWait[i] == 0) {
  
        BOOLEAN ok;

        // lets make sure it is not moving into a chop in front of us?
        ok = TRUE;
        for (j=0; j<prefs->game.dkjr.chopCount; j++) {
          ok &= (
                 (prefs->game.dkjr.chopPosition[i]+1 != 
                  prefs->game.dkjr.chopPosition[j])
                );
        }

        // the coast is clear, move!
        if (ok) {

          prefs->game.dkjr.chopPosition[i]++;
          prefs->game.dkjr.chopWait[i] = 
            (gbls->gameType == GAME_A) ? 6 : 4;
  
          // the chop has disapeared offscreen:
          if (prefs->game.dkjr.chopPosition[i] > (MAX_CHOPS-1)) {
            GameRemoveChop(prefs, i); removal = TRUE;
          }
  
          // the chop has just moved position onscreen:
          else {
  
            // has it killed the player?
            gbls->playerDied |= 
              (
               (
                (prefs->game.dkjr.dkjrPosition > 11) &&        // upper level
                (prefs->game.dkjr.dkjrPosition < 20) &&        
                ((prefs->game.dkjr.dkjrPosition % 2) == 0) &&  // on the ground
                (((prefs->game.dkjr.dkjrPosition / 2) - 6) == 
                  (5 - prefs->game.dkjr.chopPosition[i]))      // chop position
               ) ||
               (
                (prefs->game.dkjr.dkjrPosition < 12) &&        // lower level
                ((prefs->game.dkjr.dkjrPosition % 2) == 0) &&  // on the ground
                ((prefs->game.dkjr.dkjrPosition / 2) == 
                  (12 - prefs->game.dkjr.chopPosition[i]))     // chop position
               )
              );

            gbls->chopChanged[i] = TRUE;
          }
    
          // play a movement sound
          SndPlaySndEffect(SNDRES1_BEEP);
        }
      }
      else {
        prefs->game.dkjr.chopWait[i]--;
      }

      if (!removal) i++;
    }

    // new chop appearing on screen?
    {
      BOOLEAN ok;
      BYTE    birthFactor      = (gbls->gameType == GAME_A) ? 8 : 4;
      BYTE    maxOnScreenChops = prefs->game.dkjr.gameLevel;
  
      // we must be able to add a chop (based on level)
      ok = (
            (prefs->game.dkjr.chopCount < maxOnScreenChops) &&
            (prefs->game.dkjr.chopCount < MAX_CHOPS) &&
            ((DeviceRandom(0) % birthFactor) == 0)
           );
  
      // lets check that there are no chops at index=0
      for (i=0; i<prefs->game.dkjr.chopCount; i++) {
        ok &= (prefs->game.dkjr.chopPosition[i] != 0);
      }

      // lets add a new chop :) 
      if (ok) {
        i = prefs->game.dkjr.chopCount++;
        prefs->game.dkjr.chopPosition[i] = 0;
        prefs->game.dkjr.chopWait[i]     =
          (gbls->gameType == GAME_A) ? 6 : 4;
        gbls->chopChanged[i]             = TRUE;
        gbls->chopOnScreen[i]            = FALSE;
        gbls->chopOnScreenPosition[i]    = 0;
      }
    }
  }
}

/**
 * Remove a chop from the game.
 *
 * @param prefs the global preference data.
 * @param chopIndex the index of the chop to remove.
 */
static void
GameRemoveChop(PreferencesType *prefs, 
               SHORT           chopIndex) 
{
  SHORT     index;
  GfxRegion region    = { {   0,   0 }, {   0,   0 } };
  GfxRegion scrRegion = { {   0,   0 }, {   0,   0 } };

  //
  // remove the bitmap from the screen
  //

  if (gbls->chopOnScreen[chopIndex]) {

    index = gbls->chopOnScreenPosition[chopIndex];

    // what is the rectangle we need to copy?
    GameGetSpritePosition(spriteChop, index, 
                          &scrRegion.topLeft.x, &scrRegion.topLeft.y);
    scrRegion.extent.x  = 10;
    scrRegion.extent.y  = 10;
    region.topLeft.x    = index * scrRegion.extent.x;
    region.topLeft.y    = 0;
    region.extent.x     = scrRegion.extent.x;
    region.extent.y     = scrRegion.extent.y;

    // invert the old chop bitmap
    GfxCopyRegion(gbls->winChops, GfxGetDrawWindow(),
                  &region, scrRegion.topLeft.x, scrRegion.topLeft.y, gfxMask);
  }
  gbls->chopOnScreen[chopIndex]         = FALSE;
  gbls->chopOnScreenPosition[chopIndex] = 0;     // ensure this is clean

  //
  // update the information arrays
  //

  // we will push the 'chop' out of the array
  //
  // before: 1234567---  after: 1345672---
  //          ^     |                 |
  //                end point         end point

  prefs->game.dkjr.chopCount--;

  // removal NOT from end?
  if (prefs->game.dkjr.chopCount > chopIndex) {

    SHORT i, count;

    count = prefs->game.dkjr.chopCount - chopIndex;

    // shift all elements down
    for (i=chopIndex; i<(chopIndex+count); i++) {
      prefs->game.dkjr.chopPosition[i] = prefs->game.dkjr.chopPosition[i+1];
      prefs->game.dkjr.chopWait[i]     = prefs->game.dkjr.chopWait[i+1];
      gbls->chopChanged[i]             = gbls->chopChanged[i+1];
      gbls->chopOnScreen[i]            = gbls->chopOnScreen[i+1];
      gbls->chopOnScreenPosition[i]    = gbls->chopOnScreenPosition[i+1];
    }
  }
}
