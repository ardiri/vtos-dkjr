/*
 * @(#)helio.h
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

#ifndef _HELIO_H
#define _HELIO_H

// system includes
#include <system.h>
#include "resource/gfx.h"

// application constants
#define GAME_FPS  8                  // 8 fps (adequate for animations)
#define MAX_CHOPS 13
#define MAX_BIRDS 8

typedef struct
{
  struct 
  {
    BOOLEAN   gamePlaying;              // is there a game in play?
    BOOLEAN   gamePaused;               // is the game currently paused?
    BOOLEAN   gameWait;                 // are we waiting for user? 
    USHORT    gameAnimationCount;       // a ticking counter
    USHORT    gameScore;                // the score of the player
    USHORT    highScore[2];             // a high score list (score only)
    USHORT    gameLives;                // the number of lives the player has

    struct 
    {
      BYTE    gameType;                 // what type of game are we playing?
      USHORT  gameLevel;                // what level are we at?
      BOOLEAN bonusAvailable;           // is there a bonus available?
      BOOLEAN bonusScoring;             // are we currently in BONUS mode?

      USHORT  dropWait;                 // the delay between falling positions
      USHORT  dropPosition;             // the position of the drop object

      BOOLEAN keyHide;                  // do we need to hide the key?
      SHORT   keyDirection;             // what direction is the key swinging?
      USHORT  keyWait;                  // the delay between swinging positions
      USHORT  keyPosition;              // the position of the key swing

      USHORT  dkjrCount;                // the frame count for the player
      USHORT  dkjrPosition;             // the position of DK 
      USHORT  dkjrNewPosition;          // the desired position of DK 
      USHORT  dkjrHangWait;             // the delay dkjr is on a vine
      USHORT  dkjrJumpWait;             // the delay dkjr is in the air

      USHORT  cageCount;                // the protection level of the cage

      USHORT  chopCount;                // the number of chops on the screen
      USHORT  chopWait[MAX_CHOPS];      // the delay between chop movement
      USHORT  chopPosition[MAX_CHOPS];  // the position of the chop

      USHORT  birdCount;                // the number of birds on the screen
      USHORT  birdWait[MAX_BIRDS];      // the delay between bird movement
      USHORT  birdPosition[MAX_BIRDS];  // the position of the bird
    } dkjr;

  } game;
  
} PreferencesType;

// local includes
#include "device.h"
#include "help.h"
#include "game.h"
#include "animate.h"
#include "resource.h"

// functions
extern void    InitApplication(void);
extern BOOLEAN ApplicationHandleEvent(EvtType *);
extern void    ApplicationDisplayDialog(ObjectID);
extern void    EventLoop(void);
extern void    EndApplication(void);
extern BOOLEAN HelioMain(WORD, void *);

#endif 
