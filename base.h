#include <iostream>
#include <fcntl.h>
#include <io.h>
#include <fstream>
#include <string>
#include <windows.h>
#include <winbase.h>
#include <vector>
#include <algorithm>

#include <Winuser.h>
#include <strsafe.h>
#include <cstring>

#include "injector.h"
#include "UpdateCol.h"





void RedirectIOToConsole();
void Evaluate();
DWORD WPM_NextCard();
bool WPM_CheckMatchTop(DWORD, int);
bool WPM_CheckMatchF(DWORD);
bool WPM_CheckMatchBot(DWORD, int);
void WPM_MoveCard(int);
void Reveal();
void PutCard();


void RunLoop();

//void WPM_PlaceAce(DWORD);


void UpdateAce();
void UpdateField();
void UpdateFlip();
void UpdateColTop(int);
void UpdateColBot(int);
void UpdateUnreveal(int);