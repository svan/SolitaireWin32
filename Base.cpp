#include "base.h"


#define PROCESS_NAME "sole.exe"

// Injected ddls, should all be in 1 or create a PIPE. How?
#define DLL_NAME "Fdll.dll" // move stack to fld
#define DLL_NAME2 "Fdll2.dll" // Send leftclick on stack
#define DLL_NAME3 "Fdll3.dll" // ?? not used
#define DLL_NAME4 "Fdll4.dll" // Send rightclick
#define DLL_NAME5 "Fdll5.dll" // move fld to fld
#define DLL_NAME6 "Fdll6.dll" // Move Bot to fld
#define DLL_NAME7 "Fdll7.dll" // Send leftclick on unrevealed card




//// These should be declared inside objects-structures. How?
//
static int CurCardValue; //holds current card
static int CurCardSlot; //Holds position of bot card

static byte fieldBuffO[7][13] = { 0, 0 };  //holds play field hex of card.  
static byte fieldBuffOrev[7][13] = { 0, 0 }; //holds 0x00 for unrevealed, 0x80 for revealed. Both should be combined into a 3d array IMO.
static byte fldStack[7] = { 0 }; // Holds each columns size. Usefull to avoid long searches.
//
//// Holds the playfield as global so other functions can access it easily.


using namespace std;

void ReadInputLoop()
{
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    while(true)
	{
	SetConsoleTextAttribute(hConsole, 7); //grey
    cout << ">";
	Evaluate(); 
    }

}

void Evaluate()
{

	string line = "inject";
	string line1 = "flip";
	string line2 = "runloop";
	string input1 = "";

	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	while(true)
	{
    getline(cin, input1);

	if ( input1 == line) // ==inject
	{
		LoadDll(PROCESS_NAME, DLL_NAME2);
		SetConsoleTextAttribute(hConsole, 10); //green
		cout << "Injected "  "into " << endl;
		break;
	}
	else if ( input1 == line1) // ==flip
	{
		SetConsoleTextAttribute(hConsole, 10);
		UpdateFlip();
		break;
	}
	else if ( input1 == line2) // ==RunLoop
	{
		SetConsoleTextAttribute(hConsole, 10);
		RunLoop();
		break;
	}
	else
	{
		cout << "default " << endl;
		break;
	}
	}

}

void RunLoop()
{
	for (int y = 0; y < 30; y++) // Run through y cards
	{

		UpdateField();
		UpdateFlip();

		//Check unrevealed
		for (int x = 0; x < 7; x++)
		{
			UpdateUnreveal(x); //Checks 7 columns unrevealed
		}
		
		if (WPM_CheckMatchF(CurCardValue) == TRUE) //Check the flip card match
		{
		  
		  UpdateField(); 
		  cout << "CMF moved ";
		}


		//top column scan
		for (int x = 0; x < 7; x++) 
		{
			UpdateColTop(x); //Writes current column card to Global int CurCardValue
			if (WPM_CheckMatchTop(CurCardValue, x) == TRUE) //Check the top of each column match
			{
				
				//UpdateAce();
				UpdateField();
				cout << "CMtop moved ";
			}
		}


		//bottom column scan
		for (int x = 0; x < 7; x++)
		{
			UpdateColBot(x); //Writes current column card to Global int CurCardValue
			if (CurCardValue != 0x55)
			{
			if (WPM_CheckMatchBot(CurCardValue, x) == TRUE) //Check the bottom of each column match
			{
				
				//UpdateAce();
				UpdateField();
				cout << "CMbot moved ";
			}
			}
		}



		LoadDll(PROCESS_NAME, DLL_NAME4); //Send rightclick
		LoadDll(PROCESS_NAME, DLL_NAME2); //Send leftclick on stack


	}

}

void UpdateColTop(int x)
{
	//fldStack[u] 7 members hold stack size
	//fieldBuff0[col][row] holds content of field

	  int cardcolm = fldStack[x] - 1;
	  CurCardValue = fieldBuffO[x][cardcolm];
			
}

void UpdateColBot(int x)
{
	CurCardValue = 0x55; //So it doesnt mess other stuff up

	for (int row = 0; row < fldStack[x]; row++)
	{
	  if (fieldBuffOrev[x][row] == 0x80)
	  {
	  CurCardValue = fieldBuffO[x][row];
	  CurCardSlot = row;
	  break;
	  } // perhaps set CurCardValue to something else if not 00
	}			
}

void UpdateUnreveal(int x)
{
	HWND hwnd;       // Holding the window title
    HANDLE handle;   // Holding the handle to the process
    DWORD pid;       // Holding the PID of the process

    hwnd = FindWindow(0, "Solitaire");
    GetWindowThreadProcessId(hwnd, &pid);
    handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);


	int row;
	  row = fldStack[x];
	  row = row - 1;

	  if (fieldBuffOrev[x][row] == 0x00)
	  {
						byte xcol1 = x;
						byte xcol3 = row;
	                    WriteProcessMemory(handle,(LPVOID) (0x01006D3D),&xcol1,1,0);
						WriteProcessMemory(handle,(LPVOID) (0x01006D3F),&xcol3,1,0);
						LoadDll(PROCESS_NAME, DLL_NAME7);
	  }


}

void UpdateField()
{
	//Start Preperation
    HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	HWND hwnd;       // Holding the window title
    HANDLE handle;   // Holding the handle to the process
    DWORD pid;       // Holding the PID of the process

    hwnd = FindWindow(0, "Solitaire");
    GetWindowThreadProcessId(hwnd, &pid);
    handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    //END Preperation

	DWORD poi1, poi2;
	byte *fieldBuff = new byte[2996]; //0x258 2996
	//byte fieldBuffO[7][13] = { 0, 0 }; declared global
	//byte fldStack[7] = { 0 };

	ReadProcessMemory(handle,(LPCVOID) (0x01007170),&poi1,sizeof(poi1),0);
	ReadProcessMemory(handle,(LPCVOID) (poi1 + 0x84),&poi2,sizeof(poi2),0);
	ReadProcessMemory(handle,(void *) (poi2 + 0x1C),fieldBuff,2996,0); //REAL

	for (int u = 0; u < 7; u++)
	{
		int p = fieldBuff[(472 * u)];
		fldStack[u] = p;
		//printf("%02X", fldStack[u]);
		//printf(" Cards\n");
	}

	for (int col = 0; col < 7; col++)
	{
		for (int i = 0, row = 0; i < fldStack[col]; i++, row++)
		{
        int pp = (i * 12) + 8 + (472*col);
		fieldBuffO[col][row] = fieldBuff[pp];
		}
	}

	//here starts 0x80 field
	for (int col = 0; col < 7; col++)
	{
		for (int i = 0, row = 0; i < fldStack[col]; i++, row++)
		{
        int pp = (i * 12) + 9 + (472*col);
        //printf("%02X", (aceBuff[pp] + (400*col)));
		//printf(" ");
		fieldBuffOrev[col][row] = fieldBuff[pp];
		}
	}
	//end 80 field
	
	//printf("\n\n");

	for (int col = 0; col < 7; col++)
	{
	  for (int row = 0; row < fldStack[col]; row++)
	  {
			//printf("%02X", fieldBuffOrev[col][row]);
			//GetCard(aceBuff0[col][row]);
			//printf(" ");  
	  }
	  //printf("\n");
	}


	//printf("\n\n");

	for (int col = 0; col < 7; col++)
	{
	  for (int row = 0; row < fldStack[col]; row++)
	  {
			//printf("%02X", fieldBuffO[col][row]);
			//GetCard(aceBuff0[col][row]);
			//printf(" ");  
	  }
	  //printf("\n");
	}
	cout << "Updated Field ";

}

void UpdateFlip()
{
  	//Start Preperation
    HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	HWND hwnd;       // Holding the window title
    HANDLE handle;   // Holding the handle to the process
    DWORD pid;       // Holding the PID of the process

    hwnd = FindWindow(0, "Solitaire");
    GetWindowThreadProcessId(hwnd, &pid);
    handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    //END Preperation

	byte stackcardN[284] = { 0 };

	DWORD basew;
	DWORD ccard;

    ReadProcessMemory(handle,(LPCVOID) (0x01007170),&basew,sizeof(basew),0);
	ReadProcessMemory(handle,(LPCVOID) (basew + 0x70),&ccard,sizeof(ccard),0);
	ReadProcessMemory(handle,(void *) (ccard + 0x1C),stackcardN,284,0); //current card of stack

	int CurCard = stackcardN[0];
	int CurCardForm = (CurCard-1) * 12 + 8; // -4 = 0
	if (CurCardForm == -4)
	{ 
		//LoadDll(PROCESS_NAME, DLL_NAME2);
		return; 
	}
	CurCardValue = stackcardN[CurCardForm];

	printf("Upcard: ");
	printf("%d", CurCardValue); printf(" "); printf("%0x", CurCardValue);
	//printf("%02X", CurCardValue);
	printf("\n");

}

bool WPM_CheckMatchTop(DWORD cardid, int colmNum) 
{
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	HWND hwnd;       // Holding the window title
    HANDLE handle;   // Holding the handle to the process
    DWORD pid;       // Holding the PID of the process

    hwnd = FindWindow(0, "Solitaire");
    GetWindowThreadProcessId(hwnd, &pid);
    handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	//cout << "(Hex of Card) ";
	//printf("%02X", cardid);
	//printf("\n");

	//Ace Logic
	if (cardid == 0x00)
	{
	  //WriteProcessMemory(handle,(LPVOID) (0x01006D3E),3,1,0);
	  LoadDll(PROCESS_NAME, DLL_NAME4);
	  return 1;
	}
	if (cardid == 0x01)
	{
      //WriteProcessMemory(handle,(LPVOID) (0x01006D3E),4,1,0);
	  LoadDll(PROCESS_NAME, DLL_NAME4);
	  return 1;
	}
	if (cardid == 0x02)
	{
     // WriteProcessMemory(handle,(LPVOID) (0x01006D3E),5,1,0);
	  LoadDll(PROCESS_NAME, DLL_NAME4);
	  return 1;
	}
	if (cardid == 0x03)
	{
      //WriteProcessMemory(handle,(LPVOID) (0x01006D3E),6,1,0);
	  LoadDll(PROCESS_NAME, DLL_NAME4);
	  return 1;
	}
	//end Ace Logic

	byte xcol1 = colmNum;

	//static byte fieldBuffO[7][13] = { 0, 0 };
    //static byte fldStack[7] = { 0 };

	for (int col = 0; col < 7; col++)
	{
		int row = fldStack[col];
		row = row - 1;

				
				if (cardid%4 == 0 && row != -1)
				{
					if(((cardid + 0x05) == fieldBuffO[col][row]) || ((cardid + 0x06) == fieldBuffO[col][row]))
					{
						byte xcol2 = col;
	                    WriteProcessMemory(handle,(LPVOID) (0x01006D3D),&xcol1,1,0);
						WriteProcessMemory(handle,(LPVOID) (0x01006D3E),&xcol2,1,0);
						LoadDll(PROCESS_NAME, DLL_NAME5);
						printf("Moved to: %d", col);
						printf(" %d", row);
						printf(" \n");
						return 1;
					}
					
				}
				if (cardid%4 == 1 && row != -1)
				{
					if(((cardid + 0x03) == fieldBuffO[col][row]) || ((cardid + 0x06) == fieldBuffO[col][row]))
					{
						byte xcol2 = col;
					    WriteProcessMemory(handle,(LPVOID) (0x01006D3D),&xcol1,1,0);
						WriteProcessMemory(handle,(LPVOID) (0x01006D3E),&xcol2,1,0);
						LoadDll(PROCESS_NAME, DLL_NAME5);
						printf("Moved to: %d", col);
						printf(" %d", row);
						printf(" \n");
						return 1;
					}
					
				}
				if (cardid%4 == 2 && row != -1)
				{
					if(((cardid + 0x02) == fieldBuffO[col][row]) || ((cardid + 0x05) == fieldBuffO[col][row]))
					{
						byte xcol2 = col;
	                    WriteProcessMemory(handle,(LPVOID) (0x01006D3D),&xcol1,1,0);
						WriteProcessMemory(handle,(LPVOID) (0x01006D3E),&xcol2,1,0);
						LoadDll(PROCESS_NAME, DLL_NAME5);
						printf("Moved to: %d", col);
						printf(" %d", row);
						printf(" \n");
						return 1;
					}
					
				}
				if (cardid%4 == 3 && row != -1)
				{
					if(((cardid + 0x02) == fieldBuffO[col][row]) || ((cardid + 0x03) == fieldBuffO[col][row]))
					{
						byte xcol2 = col;
	                    WriteProcessMemory(handle,(LPVOID) (0x01006D3D),&xcol1,1,0);
						WriteProcessMemory(handle,(LPVOID) (0x01006D3E),&xcol2,1,0);
						LoadDll(PROCESS_NAME, DLL_NAME5);
						printf("Moved to: %d", col);
						printf(" %d", row);
						printf(" \n");
						return 1;
					}
					
				}

				if ( row == -1 && ( cardid == 0x30 || cardid == 0x31 || cardid == 0x32 || cardid == 0x33 ))
			    {
					    byte xcol2 = col;
	                    WriteProcessMemory(handle,(LPVOID) (0x01006D3D),&xcol1,1,0);
						WriteProcessMemory(handle,(LPVOID) (0x01006D3E),&xcol2,1,0);
						LoadDll(PROCESS_NAME, DLL_NAME5);
		        }

			
		
		    
	}
	return 0;
} 

bool WPM_CheckMatchBot(DWORD cardid, int colmNum) 
{
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	HWND hwnd;       // Holding the window title
    HANDLE handle;   // Holding the handle to the process
    DWORD pid;       // Holding the PID of the process

    hwnd = FindWindow(0, "Solitaire");
    GetWindowThreadProcessId(hwnd, &pid);
    handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	//cout << "(Hex of Card) ";
	//printf("%02X", cardid);
	//printf("\n");

	byte xcol1 = colmNum;

	//static byte fieldBuffO[7][13] = { 0, 0 };
    //static byte fldStack[7] = { 0 };

	for (int col = 0; col < 7; col++)
	{
		int row = fldStack[col];
		row = row - 1;

				
				if (cardid%4 == 0 && row != -1)
				{
					if(((cardid + 0x05) == fieldBuffO[col][row]) || ((cardid + 0x06) == fieldBuffO[col][row]))
					{
						byte xcol2 = col;
						byte xcol3 = CurCardSlot;
	                    WriteProcessMemory(handle,(LPVOID) (0x01006D3D),&xcol1,1,0);
						WriteProcessMemory(handle,(LPVOID) (0x01006D3E),&xcol2,1,0);
						WriteProcessMemory(handle,(LPVOID) (0x01006D3F),&xcol3,1,0);
						LoadDll(PROCESS_NAME, DLL_NAME6);
						printf("Moved to: %d", col);
						printf(" %d", row);
						printf(" \n");
						return 1;
					}
					
				}
				if (cardid%4 == 1 && row != -1)
				{
					if(((cardid + 0x03) == fieldBuffO[col][row]) || ((cardid + 0x06) == fieldBuffO[col][row]))
					{
						byte xcol2 = col;
						byte xcol3 = CurCardSlot;
					    WriteProcessMemory(handle,(LPVOID) (0x01006D3D),&xcol1,1,0);
						WriteProcessMemory(handle,(LPVOID) (0x01006D3E),&xcol2,1,0);
						WriteProcessMemory(handle,(LPVOID) (0x01006D3F),&xcol3,1,0);
						LoadDll(PROCESS_NAME, DLL_NAME6);
						printf("Moved to: %d", col);
						printf(" %d", row);
						printf(" \n");
						return 1;
					}
					
				}
				if (cardid%4 == 2 && row != -1)
				{
					if(((cardid + 0x02) == fieldBuffO[col][row]) || ((cardid + 0x05) == fieldBuffO[col][row]))
					{
						byte xcol2 = col;
						byte xcol3 = CurCardSlot;
	                    WriteProcessMemory(handle,(LPVOID) (0x01006D3D),&xcol1,1,0);
						WriteProcessMemory(handle,(LPVOID) (0x01006D3E),&xcol2,1,0);
						WriteProcessMemory(handle,(LPVOID) (0x01006D3F),&xcol3,1,0);
						LoadDll(PROCESS_NAME, DLL_NAME6);
						printf("Moved to: %d", col);
						printf(" %d", row);
						printf(" \n");
						return 1;
					}
					
				}
				if (cardid%4 == 3 && row != -1)
				{
					if(((cardid + 0x02) == fieldBuffO[col][row]) || ((cardid + 0x03) == fieldBuffO[col][row]))
					{
						byte xcol2 = col;
						byte xcol3 = CurCardSlot;
	                    WriteProcessMemory(handle,(LPVOID) (0x01006D3D),&xcol1,1,0);
						WriteProcessMemory(handle,(LPVOID) (0x01006D3E),&xcol2,1,0);
						WriteProcessMemory(handle,(LPVOID) (0x01006D3F),&xcol3,1,0);
						LoadDll(PROCESS_NAME, DLL_NAME6);
						printf("Moved to: %d", col);
						printf(" %d", row);
						printf(" \n");
						return 1;
					}
					
				}

				if ( (row == -1) && ( cardid == 0x30 || cardid == 0x31 || cardid == 0x32 || cardid == 0x33 ) && (CurCardSlot > 0))
			    {
					    byte xcol2 = col;
						byte xcol3 = CurCardSlot;
	                    WriteProcessMemory(handle,(LPVOID) (0x01006D3D),&xcol1,1,0);
						WriteProcessMemory(handle,(LPVOID) (0x01006D3E),&xcol2,1,0);
						WriteProcessMemory(handle,(LPVOID) (0x01006D3F),&xcol3,1,0);
						LoadDll(PROCESS_NAME, DLL_NAME6);
		        }

			
		
		    
	}
	return 0;
} 

bool WPM_CheckMatchF(DWORD cardid) 
{
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	HWND hwnd;       // Holding the window title
    HANDLE handle;   // Holding the handle to the process
    DWORD pid;       // Holding the PID of the process

    hwnd = FindWindow(0, "Solitaire");
    GetWindowThreadProcessId(hwnd, &pid);
    handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	//cout << "(Hex of Card) ";
	//printf("%02X", cardid);
	//printf("\n");

	byte poi1 = 0xff;
	byte xcol = 0x00;

	//Ace Logic
	if (cardid == 0x00)
	{
	 // WriteProcessMemory(handle,(LPVOID) (0x01006D3E),3,1,0);
	  LoadDll(PROCESS_NAME, DLL_NAME4);
	  return 1;
	}
	if (cardid == 0x01)
	{
      //WriteProcessMemory(handle,(LPVOID) (0x01006D3E),4,1,0);
	  LoadDll(PROCESS_NAME, DLL_NAME4);
	  return 1;
	}
	if (cardid == 0x02)
	{
      //WriteProcessMemory(handle,(LPVOID) (0x01006D3E),5,1,0);
	  LoadDll(PROCESS_NAME, DLL_NAME4);
	  return 1;
	}
	if (cardid == 0x03)
	{
      //WriteProcessMemory(handle,(LPVOID) (0x01006D3E),6,1,0);
	  LoadDll(PROCESS_NAME, DLL_NAME4);
	  return 1;
	}
	//end Ace Logic

	for (int col = 0; col < 7; col++)
	{
		int row = fldStack[col];
		row = row - 1;

				if (cardid%4 == 0 && row != -1)
				{
					if(((cardid + 0x05) == fieldBuffO[col][row]) || ((cardid + 0x06) == fieldBuffO[col][row]))
					{
						byte xcol = col;
	                    WriteProcessMemory(handle,(LPVOID) (0x01006D3D),&poi1,1,0);
						WriteProcessMemory(handle,(LPVOID) (0x01006D3E),&xcol,1,0);
						LoadDll(PROCESS_NAME, DLL_NAME);
						printf("Moved to: %d", col);
						printf(" %d", row);
						printf(" \n");
						return 1;
					}
					
				}
				if (cardid%4 == 1 && row != -1)
				{
					if(((cardid + 0x03) == fieldBuffO[col][row]) || ((cardid + 0x06) == fieldBuffO[col][row]))
					{
						byte xcol = col;
					    WriteProcessMemory(handle,(LPVOID) (0x01006D3D),&poi1,1,0);
						WriteProcessMemory(handle,(LPVOID) (0x01006D3E),&xcol,1,0);
						LoadDll(PROCESS_NAME, DLL_NAME);
						printf("Moved to: %d", col);
						printf(" %d", row);
						printf(" \n");
						return 1;
					}
					
				}
				if (cardid%4 == 2 && row != -1)
				{
					if(((cardid + 0x02) == fieldBuffO[col][row]) || ((cardid + 0x05) == fieldBuffO[col][row]))
					{
						byte xcol = col;
	                    WriteProcessMemory(handle,(LPVOID) (0x01006D3D),&poi1,1,0);
						WriteProcessMemory(handle,(LPVOID) (0x01006D3E),&xcol,1,0);
						LoadDll(PROCESS_NAME, DLL_NAME);
						printf("Moved to: %d", col);
						printf(" %d", row);
						printf(" \n");
						return 1;
					}					
				}
				if (cardid%4 == 3 && row != -1)
				{
					if(((cardid + 0x02) == fieldBuffO[col][row]) || ((cardid + 0x03) == fieldBuffO[col][row]))
					{
						byte xcol = col;
	                    WriteProcessMemory(handle,(LPVOID) (0x01006D3D),&poi1,1,0);
						WriteProcessMemory(handle,(LPVOID) (0x01006D3E),&xcol,1,0);
						LoadDll(PROCESS_NAME, DLL_NAME);
						printf("Moved to: %d", col);
						printf(" %d", row);
						printf(" \n");
						return 1;
					}
					
				}

				if ( row == -1 && ( cardid == 0x30 || cardid == 0x31 || cardid == 0x32 || cardid == 0x33 ))
			    {
					    byte xcol = col;
	                    WriteProcessMemory(handle,(LPVOID) (0x01006D3D),&poi1,1,0);
						WriteProcessMemory(handle,(LPVOID) (0x01006D3E),&xcol,1,0);
						LoadDll(PROCESS_NAME, DLL_NAME);
		        }
			
		
		    
	}
	cout << "CMf none ";
	return 0;
} 

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	enableDebugPrivileges();
	RedirectIOToConsole();

	ReadInputLoop();

   return 0;
}
