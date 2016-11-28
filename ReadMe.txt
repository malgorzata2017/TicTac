I have had many requests for complete source code. I didn't provide code that you can compile into an application. I may do this someday, but there is a lot of code involved, and for now it is not available to the public. I did include the code that is the base for the DLL so that one can see how a neural net is created. 

The following information should be sufficient for you to load the DLL into your own application so that you can analyse how it works. The files referred to are available at the link... http://www.magma.ca/~gtaylor/GeneticAlgorithm.htm

The code in NNetSrc.Zip compiles to a DLL. This DLL is found in "PlayTTT.Zip" If you wanted to recreate the DLL you must include the code and headers from the package NNetSrc.Zip and select to create a new "Win32 Dynamic-Link Library" in MSVC. Furthermore, you must create an exports file of file type .def and include it in the project.
 You may call it "NeuralNet.def", and place the following in it...

LIBRARY NeuralNet
DESCRIPTION "Impliments a BPN neural network"
EXPORTS
	InitNet
	GetNetMove
	KillNet


*****************************************************************
The following exported functions are defined in the file Bpn.h...

 extern int GetNetMove(int *board);
 extern int InitNet(char *weights);
 extern void KillNet(void);

The DLL is called from the program PlayTTT.exe, in the package PlayTTT.Zip.

Of course, you need to know how to employ the functions exported from the DLL. InitNet() passes the name of a file with the weights with which to initialize the neural net. In the case of PlayTTT, that file is called ttt.bpn

During game play, GetNetMove() is called with an array representing the current state of the game board - 'O' to play. This is a simple integer array with 18 elements.

Imagine you have a board defined as follows...

char Board[] = {'X', ' ', ' ',

                ' ', 'O', ' ',

                ' ', ' ', 'X'};  

In the following code the array 'int invecs[18]' is prepared to be passed to the neural net with the current position in game Board[], a 9 element array with 'X', 'O', or ' ' (space) representing the current state...

 n = 0;
 for(i = 0; i < 9; i++)
 {
     if(Board[i] == ' ')
     {
        invecs[n++] = 0;
        invecs[n++] = 0;
     }else if(Board[i] == 'X')
     {
        invecs[n++] = 0;
        invecs[n++] = 1;
     }else if(Board[i] == 'O')
     {
        invecs[n++] = 1;
        invecs[n++] = 0;
     }
 }

GetNeuralNetMove() returns an int with a value from 0 thru 8, representing his 'O' move on the board of 9 squares.

You must remember that the neural network was trained to provide strategic moves only. It has no training to deal with obvious winning or drawn positions. In fact, in PlayTTT, all boards with with O to play an obvious win, or requiring O to block an obvious potential win by X, or obvious drawn positions, are filtered out. They are handled by higher level code and not sent to the neural network. The whole idea was to see if the neural net could play strategies, such as setting up the two-way split needed to defeat the more sophisticated player, or defending himself from such strategy. 


To load the DLL into an application you would use the following code...

// Functions imported from NeuralNet.dll
// extern int GetNetMove(int *board);
// extern void InitNet(void);
// extern void KillNet(void);
// Declare some function pointers for the DLL

typedef int (__cdecl *NEURALNET_INIT)(char*);
typedef void (__cdecl *NEURALNET_KILL)(void);
typedef int (__cdecl *NEURALNET_GET)(int*);
HINSTANCE hMyDLL;
NEURALNET_INIT InitNeuralNet;
NEURALNET_KILL KillNeuralNet;
NEURALNET_GET GetNeuralNetMove;


BOOL LoadNeuralNetDll(void)
{
 hMyDLL = LoadLibrary((LPCTSTR)"NeuralNet.dll"); 
 if(hMyDLL != NULL)
 {
    InitNet = (NEURALNET_INIT)GetProcAddress((HMODULE)hMyDLL, (LPCSTR)"InitNet");
    KillNet = (NEURALNET_KILL)GetProcAddress((HMODULE)hMyDLL, (LPCSTR)"KillNet");
    GetNetMove = (NEURALNET_GET)GetProcAddress((HMODULE)hMyDLL, (LPCSTR)"GetNetMove");

    if(InitNet == NULL)goto LOADDLL_ERROR;
    if(KillNet == NULL)goto LOADDLL_ERROR;
    if(GetNetMove == NULL)goto LOADDLL_ERROR;

    // Initialize the neural network with the weights it developed from training...
    if(!InitNet("ttt.bpn"))goto LOADDLL_ERROR;
    return TRUE;
 }else
 {
LOADDLL_ERROR:
    return FALSE;
 }
}

To unload the DLL you do...
 
void KillNeuralNetDll(void)
{
 if(hMyDLL != NULL)
 {
    KillNet();
    FreeLibrary((HMODULE)hMyDLL);
    hMyDLL = NULL;
 }
}


I hope this is of some help to you.

George W. Taylor
March 23, 2006