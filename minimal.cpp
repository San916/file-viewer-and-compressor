/////////////////////////////////////////////////////////////////////////////
// Name:        minimal.cpp
// Purpose:     Minimal wxWidgets sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// The path to the root of the tree should be split into ROOT_DIR_NAME and ROOT_HOME_FOLDER_NAME
//     ROOT_HOME_FOLDER_NAME contains the name of the root of the tree (e.g. "Documents")
//     ROOT_DIR_NAME contains the name of everything coming before the root of the tree (e.g. "/Users/username/")

#define ROOT_DIR_NAME "/Users/"
#define ROOT_HOME_FOLDER_NAME ""

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#define WX_FRAME_WIDTH 1248
#define WX_FRAME_HEIGHT 768

#define FRAME_UPPER_RIGHT_W 224

#define FRAME_LEFT_W 448
#define FRAME_RIGHT_W (WX_FRAME_WIDTH-FRAME_LEFT_W)

#define FRAME_UPPER_H (WX_FRAME_HEIGHT/2)
#define FRAME_UPPER_RIGHT_H FRAME_UPPER_H
#define FRAME_RIGHT_LOWER_H (WX_FRAME_HEIGHT-FRAME_UPPER_RIGHT_H)

//------------------------------------------------
// File Tree Panel(Left Panel)
//------------------------------------------------
#define TREE_PANEL_W FRAME_LEFT_W
#define TREE_PANEL_H WX_FRAME_HEIGHT

#define TREE_PANEL_X 0
#define TREE_PANEL_Y 0

//------------------------------------------------
// File List Panel(Upper Panel)
//------------------------------------------------
#define FILE_LIST_PANEL_W (FRAME_RIGHT_W-300)
#define FILE_LIST_PANEL_H FRAME_UPPER_H

#define FILE_LIST_PANEL_X FRAME_LEFT_W
#define FILE_LIST_PANEL_Y 0

//------------------------------------------------
// Button Panel(Upper Right Panel)
//------------------------------------------------
#define BUTTON_LIST_PANEL_W 300
#define BUTTON_LIST_PANEL_H FRAME_UPPER_RIGHT_H

#define BUTTON_LIST_PANEL_X (FRAME_LEFT_W+FILE_LIST_PANEL_W)
#define BUTTON_LIST_PANEL_Y 0

//------------------------------------------------
// File Contents Panel(Lower Right Panel)
//------------------------------------------------
#define CONTENTS_PANEL_W FRAME_RIGHT_W
#define CONTENTS_PANEL_H FRAME_RIGHT_LOWER_H

#define CONTENTS_PANEL_X FRAME_LEFT_W
#define CONTENTS_PANEL_Y FILE_LIST_PANEL_H

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include "wx/grid.h"
#include "wx/treectrl.h"
//#include "wx_pch.h"
#include "wx/msgdlg.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/fs_zip.h"
#include "wx/zipstrm.h"
#include "wx/wfstream.h"

// for directory (files)
#include <iostream>
#include <iomanip>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include <sstream>

using namespace std;

#include "lzw.h"
#include "client.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows it is in resources and even
// though we could still include the XPM here it would be unused)
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

string converter(uint8_t *str);

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------
wxImage *cellImageFolder = NULL;
wxImage *cellImageFile = NULL;
wxBitmap *cellBitmapFolder = NULL;
wxBitmap *cellBitmapFile = NULL;

class FileGridCellRenderer : public wxGridCellStringRenderer
{
public:
    virtual void Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, const wxRect& rect, int row, int col, bool isSelected);
};

void FileGridCellRenderer::Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, const wxRect& rect, int row, int col, bool isSelected)
{
        dc.DrawBitmap(*cellBitmapFile, rect.x+1, rect.y+1);
}

class FolderGridCellRenderer : public wxGridCellStringRenderer
{
public:
    virtual void Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, const wxRect& rect, int row, int col, bool isSelected);
};

void FolderGridCellRenderer::Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, const wxRect& rect, int row, int col, bool isSelected)
{
        dc.DrawBitmap(*cellBitmapFolder, rect.x+2, rect.y+2);
}

//int main() {
//    MyFrame *myFrame = new MyFrame();
//
//    myFrame->Show();
//    myFrame->run();
//
//}

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit() wxOVERRIDE;
};

// MyFrame extends wxFrame. It is a child class
// wxFrame has the base of a GUI application
// Define a new frame type: this is going to be our main frame
int gridRowAmount = 0;

class MyFrame : public wxFrame {
private:
//    wxStaticText *lblID;
//    wxTextCtrl *txtID;
//
//    wxStaticText *lblPwd;
//    wxTextCtrl *txtPwd;
//
//    wxButton *btnLogin;
//    wxButton *btnCancel;
//
//    wxTextCtrl *txtChat;
//    wxStaticText *lblChat;
//    wxButton *btnChat;
//
//    wxStaticText *lblMessage;
    
    //--------------------------------------------------------------------------
    // Member variables
    //--------------------------------------------------------------------------
    
    wxPanel *mainPanel;
    string fullpath;
    
    wxButton *compressButton;
    wxButton *decompressButton;
    
    //--------------------------------------------------------------------------
    // Left Panel
    //--------------------------------------------------------------------------
    wxPanel *treePanel;
    wxTreeCtrl *tree;
    wxTreeItemId rootId;
    void recShowAllDir(string curDirPath, wxTreeItemId curRootID, int depth, string tabs);
    
    //--------------------------------------------------------------------------
    // Upper Panel
    //--------------------------------------------------------------------------
    wxPanel *fileListPanel;

    string rootNameOnly;
    string rootParentDirPathOnLeftPanel;

    string curDirPathOnUpperPanel;

    wxGrid *grid;

    //--------------------------------------------------------------------------
    // Upper Right Panel
    //--------------------------------------------------------------------------
    wxPanel *buttonPanel;
    
    //--------------------------------------------------------------------------
    // Lower Right Panel
    //--------------------------------------------------------------------------
    wxPanel *contentsPanel;
    wxTextCtrl *txtContents;
    
public:
    // constructor(s)
    MyFrame(const wxString& title); // constructor
    virtual ~MyFrame();
    
    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

//    void login(wxCommandEvent & WXUNUSED(event));
//    void loginCancel(wxCommandEvent & WXUNUSED(event));
//    void ChatEnter(wxCommandEvent & WXUNUSED(event));
    bool makeUpperRightPanel(wxString fullPath);

//    wxPanel *chatPanel;
//    wxPanel *loginPanel;
    void OnCompressLeftClick(wxCommandEvent& event);
    void OnDecompressLeftClick(wxCommandEvent& event);

    void OnGridLeftClick(wxGridEvent& ev);
    void OnGridRightClick(wxGridEvent& ev);
    void OnMouseLeftClick(wxMouseEvent& ev);
    void OnPopupClick(wxCommandEvent &ev);

    void getAllInfoFromFolder(vector<string> &names, vector<bool> &isFolder, string path, int *totalChunks, int *totalFiles, int *totalFolders, long *totalSize);

    void IdlePlay(wxIdleEvent& evt);
private:
    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Minimal_About = wxID_ABOUT
};

const int ID_LOGIN = 101;
const int ID_CANCEL = 102;
const int ID_CHAT = 103;
const int ID_BUTTON_COMPRESSED = 104;
const int ID_BUTTON_DECOMPRESSED = 105;

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
    EVT_GRID_CELL_LEFT_CLICK(MyFrame::OnGridLeftClick)
    EVT_GRID_CELL_RIGHT_CLICK(MyFrame::OnGridRightClick)
//    EVT_LEFT_DOWN(MyFrame::OnMouseLeftClick)
    EVT_BUTTON (ID_BUTTON_COMPRESSED, MyFrame::OnCompressLeftClick)
    EVT_BUTTON (ID_BUTTON_DECOMPRESSED, MyFrame::OnDecompressLeftClick)
wxEND_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
wxIMPLEMENT_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    //--------------------------------------------------------------------------
    // LZW encoding
    //--------------------------------------------------------------------------
//    // open input file for encoding
//    ifstream inputToEncode;
//    inputToEncode.open ("input.bin");
//    
//    if (inputToEncode.fail()) {
//        cout << "main()::encoding: error - failed to open : input.bin" << endl;
//        exit(1);
//    }
//    
//    // create output file for encoding
//    string encodedOutputFilename = "output.lzw";
//    ofstream outputEncoded;
//    outputEncoded.open(encodedOutputFilename.c_str());
//    if (outputEncoded.fail()) {
//        cout << "main()::encoding: error - failed to open : " << encodedOutputFilename << endl;
//        exit(1);
//    }
//    
//    encode(inputToEncode, outputEncoded);
//    inputToEncode.close();
//    outputEncoded.close();
    
//    wxFileSystem::AddHandler(new wxZipFSHandler);
//
//    wxFileSystem fs;
//    //    wxFSFile *zip = fs.OpenFile( "d:\\test.zip#zip:test.txt");
//    wxFSFile *zip = fs.OpenFile("/Users/sankang/a.txt.zip#zip:a.txt");
//    cout << "MyApp::OnInit(): zip  = " << zip << endl;
//    if(zip!=NULL)
//    {
//        wxInputStream *in = zip->GetStream();
//        if ( in != NULL )
//        {
//            wxFileOutputStream out( "/Users/sankang/b.txt" );
//            out.Write(*in);
//            out.Close();
//        }
//        delete zip;
//    }
//    exit(1);
    
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    MyFrame *frame = new MyFrame("Minimal wxWidgets App");

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

MyFrame *thisFrame;

/**
 * Get the size of a file.
 * @return The filesize, or 0 if the file does not exist.
 */
size_t getFileSize(const char* filename) {
    struct stat st;
    if (stat(filename, &st) != 0) {
        return 0;
    }
    return st.st_size;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------
// the constructor of a child class needs to call the parents constructor
// frame constructor
MyFrame::MyFrame(const wxString& title) : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(WX_FRAME_WIDTH, WX_FRAME_HEIGHT)) {
    thisFrame = this;
    
    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, "&About\tF1", "Show about dialog");

    fileMenu->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#else // !wxUSE_MENUS
    // If menus are not available add a button to access the about box
    wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* aboutBtn = new wxButton(this, wxID_ANY, "About...");
    aboutBtn->Bind(wxEVT_BUTTON, &MyFrame::OnAbout, this);
    sizer->Add(aboutBtn, wxSizerFlags().Center());
#endif // wxUSE_MENUS/!wxUSE_MENUS

    
// a statusbar is the application sending the user a message
#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWidgets!");
#endif // wxUSE_STATUSBAR
    
#ifdef WIN32
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
#endif

    mainPanel = new wxPanel(this, -1,
                            wxPoint(0, 0),
                            wxSize(WX_FRAME_WIDTH, WX_FRAME_HEIGHT));

    //--------------------------------------------------------------------------
    // Left Panel
    //--------------------------------------------------------------------------

    treePanel = new wxPanel(mainPanel, wxID_ANY,
                            wxPoint(TREE_PANEL_X, TREE_PANEL_Y),
                            wxSize(TREE_PANEL_W, TREE_PANEL_H));
    tree = new wxTreeCtrl(treePanel, wxID_ANY,
                          wxPoint( 0, 0 ),
                          wxSize(TREE_PANEL_W, TREE_PANEL_H));
    tree->Bind (wxEVT_LEFT_DOWN, &MyFrame::OnMouseLeftClick, this);

    rootNameOnly = ROOT_HOME_FOLDER_NAME;
    rootId = tree->AddRoot(rootNameOnly);
    
    // Build file tree
    rootParentDirPathOnLeftPanel = ROOT_DIR_NAME;
    curDirPathOnUpperPanel = rootParentDirPathOnLeftPanel;
    curDirPathOnUpperPanel += rootNameOnly;

    recShowAllDir(curDirPathOnUpperPanel, rootId, 0, "");

    //--------------------------------------------------------------------------
    // Upper Panel
    //--------------------------------------------------------------------------
    cellImageFolder = new wxImage();
    cellImageFile = new wxImage();
#ifdef WIN32
    if (cellImageFolder->LoadFile(wxT("folder.bmp"))) {
        cellBitmapFolder = new wxBitmap(*cellImageFolder);
    } else {
        wxLogError(_T("folder.bmp didn't load, does it exist?"));
    }
#else
    if (cellImageFolder->LoadFile(wxT("../../folder.bmp"))) {
        cellBitmapFolder = new wxBitmap(*cellImageFolder);
    }
    else {
        wxLogError(_T("../../folder.bmp didn't load, does it exist?"));
    }
#endif

#ifdef WIN32
    if (cellImageFile->LoadFile(wxT("file.bmp"))) {
        cellBitmapFile = new wxBitmap(*cellImageFile);
    } else {
        wxLogError(_T("file.bmp didn't load, does it exist?"));
    }
#else
    if (cellImageFile->LoadFile(wxT("../../file.bmp"))) {
        cellBitmapFile = new wxBitmap(*cellImageFile);
    }
    else {
        wxLogError(_T("../../file.bmp didn't load, does it exist?"));
    }
#endif

    fileListPanel = new wxPanel(mainPanel, wxID_ANY,
                                         wxPoint(FILE_LIST_PANEL_X, FILE_LIST_PANEL_Y),
                                         wxSize(FILE_LIST_PANEL_W, FILE_LIST_PANEL_H));
    
    grid = new wxGrid(fileListPanel, wxID_ANY,
                              wxPoint( 0, 0 ),
                              wxSize(FILE_LIST_PANEL_W, FILE_LIST_PANEL_H));
    // this will create a grid and, by default, an associated grid
    // table for strings
    grid->CreateGrid( 0, 0 );
    
    grid->SetRowLabelSize(0);

    grid->AppendCols(3);

    grid->EnableEditing(false);

    grid->SetColLabelValue(0, _("")); // from wxSmith
    grid->SetColLabelValue(1, _("Name")); // from wxSmith
    grid->SetColLabelValue(2, _("Size")); // from wxSmith

    grid->SetColSize(0, 20);
    grid->SetColSize(1, 400);
    grid->SetColSize(2, 100);
    
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(curDirPathOnUpperPanel.c_str())) != NULL) {
        /* print all the files and directories within directory */
        int i = 0;
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] != '.') {
                grid->AppendRows(1);
                gridRowAmount++;
                if (ent->d_type == DT_DIR) {
//                    printf ("%s\n", ent->d_name);
                    grid->SetCellRenderer(i, 0, new FolderGridCellRenderer);
                }
                else {
                    grid->SetCellRenderer(i, 0, new FileGridCellRenderer);
                }

                grid->SetCellValue( i, 1, ent->d_name);
                
                string fullpath = curDirPathOnUpperPanel;
                fullpath += "/";
                fullpath += ent->d_name;
                
                int fileSize = getFileSize(fullpath.c_str());
                
                char buf[80];
                sprintf(buf, "%d bytes", fileSize);
                
                printf ("%s: %d\n", ent->d_name, fileSize);
                
                grid->SetCellAlignment(i, 2, wxALIGN_RIGHT, wxALIGN_CENTRE);
                grid->SetCellValue( i, 2, buf);

                i++;
//                }
            }
            
        }
        closedir (dir);
        
    }
    else {
        /* could not open directory */
        perror ("");
        //return EXIT_FAILURE;
        exit(1);
    }

    //--------------------------------------------------------------------------
    // Lower Right Panel
    //--------------------------------------------------------------------------

    contentsPanel = new wxPanel(mainPanel, wxID_ANY,
                                         wxPoint(CONTENTS_PANEL_X, CONTENTS_PANEL_Y),
                                         wxSize(CONTENTS_PANEL_W, CONTENTS_PANEL_H));

    txtContents = new wxTextCtrl(contentsPanel, -1, wxT(""),
                                             wxPoint(0, 0),
                                             wxSize(CONTENTS_PANEL_W, CONTENTS_PANEL_H),
                                             wxTE_MULTILINE);
    //--------------------------------------------------------------------------
    // Upper Right Panel
    //--------------------------------------------------------------------------
    buttonPanel = new wxPanel(mainPanel, wxID_ANY,
                              wxPoint(BUTTON_LIST_PANEL_X, BUTTON_LIST_PANEL_Y),
                              wxSize(BUTTON_LIST_PANEL_W, BUTTON_LIST_PANEL_H));

    compressButton = new wxButton(buttonPanel, ID_BUTTON_COMPRESSED, _T("Compress"),
                                  wxPoint(0, 0),
                                  wxSize(150, 20), 0);
    decompressButton = new wxButton(buttonPanel, ID_BUTTON_DECOMPRESSED, _T("Decompress"),
                                  wxPoint(150, 0),
                                  wxSize(150, 20), 0);
    compressButton->Disable();
    decompressButton->Disable();
}

MyFrame::~MyFrame() {
    delete cellImageFolder;
    delete cellImageFile;
    delete cellBitmapFolder;
    delete cellBitmapFile;
}
void MyFrame::OnCompressLeftClick(wxCommandEvent& event) {
    cout << "void OnCompressLeftClick: compress button pressed" << endl;
    cout << "void OnCompressLeftClick: fullpath = " << fullpath << endl;
    if (fullpath.substr(fullpath.size()-4) == ".lzw") {
        cout << "void OnCompressLeftClick: cannot compress, it is already compressed" << endl;
    }
}
void MyFrame::OnDecompressLeftClick(wxCommandEvent& event) {
    cout << "void OnDecompressLeftClick: decompress button pressed" << endl;
    cout << "void OnCompressLeftClick: fullpath = " << fullpath << endl;
    if (fullpath.substr(fullpath.size()-4) != ".lzw") {
        cout << "void OnCompressLeftClick: cannot decompress, it is already decompressed" << endl;
    }
}

void MyFrame::recShowAllDir(string curDirPath, wxTreeItemId curRootID, int depth, string tabs) {
//    std::cout << "recShowAllDir(" << curDirPath << ")" << std::endl;
    DIR *dir;
    struct dirent *ent;
//    std::cout << curDirPath.c_str() << std::endl;

    dir = opendir(curDirPath.c_str());
    if (dir != NULL) { // dir is not NULL for some reason // fixed
        /* print all the files and directories within directory */
        int i = 0;
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] != '.') {
                wxTreeItemId curChildID = tree->AppendItem(curRootID, ent->d_name);
                if (ent->d_type == DT_DIR) {
                    string subDirFullPath = curDirPath;
                    subDirFullPath += "/";
                    subDirFullPath += ent->d_name;
                    recShowAllDir(subDirFullPath, curChildID, depth+1, tabs+"    ");
                }
            }
        }
        closedir (dir);
    }
    else {
//        exit(1);
    }
}

// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format
                 (
                    "Welcome to %s!\n"
                    "\n"
                    "This is the minimal wxWidgets sample\n"
                    "running under %s.",
                    wxVERSION_STRING,
                    wxGetOsDescription()
                 ),
                 "About wxWidgets minimal sample",
                 wxOK | wxICON_INFORMATION,
                 this);
}

#define MAX_LEN_BUF 80
void MyFrame::OnGridLeftClick(wxGridEvent& ev) {
    fullpath = curDirPathOnUpperPanel;
    fullpath += "/";
    fullpath += grid->GetCellValue( ev.GetRow(), ev.GetCol() );
    
    if (fullpath.substr(fullpath.size()-4) == ".lzw") {
        cout << "MyFrame::OnGridLeftClick: cannot compress, it is already compressed" << endl;
        compressButton->Disable();
        decompressButton->Enable();
    }
    else {
        compressButton->Enable();
        decompressButton->Disable();
    }
    
    DIR *dir;
    if ((dir = opendir(fullpath.c_str())) != NULL) {
        // when the fullPath is for a dir then dont open
        closedir(dir);
    }
    else {
        FILE *fpIn = fopen(fullpath.c_str(), "r");

        size_t totalNumBytesRead = 0;
        
        txtContents->Clear();
        
        if (fullpath.substr(fullpath.size()-4) == ".txt") { // file is .txt so print in ascii
            cout << "fullpath.substr(fullpath.size()-4) == .txt" << endl;
            char buf[MAX_LEN_BUF+1];
            while (true) {
                size_t numBytesRead = fread(buf, 1, 80, fpIn);
                //printf("numBytesRead: %d\n", numBytesRead);
                buf[numBytesRead] = '\0';
                if (numBytesRead == 0 && feof(fpIn)) {
                    break;
                }
                totalNumBytesRead += numBytesRead;
                printf("%s", buf);
                txtContents->AppendText(buf);
            }
        }
        else { // file isnt a .txt file so print in binary
            cout << "fullpath.substr(fullpath.size()-4) != .txt" << endl;

            unsigned char buf[MAX_LEN_BUF+1];
            while (true) {
                size_t numBytesRead = fread(buf, 1, 80, fpIn);
                //printf("numBytesRead: %d\n", numBytesRead);
                buf[numBytesRead] = '\0';
                if (numBytesRead == 0 && feof(fpIn)) {
                    break;
                }
                totalNumBytesRead += numBytesRead;
                for (int i = 0; i < numBytesRead; i++) {
                    cout << hex << setw(2) << setfill('0') << buf[i] << endl;
                }
                txtContents->AppendText(buf);
            }
        }
        
        printf("\ntotalNumBytesRead: %d\n", totalNumBytesRead);

        fclose(fpIn);
    }
    
    // you must call Skip() if you want the default processing
    // to occur in wxGrid
    
    ev.Skip();
}

#define ID_COMPRESS        2001
#define ID_DECOMPRESS    2002

void MyFrame::OnGridRightClick(wxGridEvent& ev) {
    wxString cellName = grid->GetCellValue(ev.GetRow(), ev.GetCol());
    void *data = &cellName;
    
    //void *data = reinterpret_cast<void *>(ev.GetItem().GetData());
    wxMenu mnu;
    mnu.SetClientData( data ); // only accepts void pointers
    //mnu.Append(ID_SOMETHING,     "Do something");
    mnu.Append(ID_COMPRESS,     "Compress");
    //mnu.Enable(ID_SOMETHING, false);
    mnu.Append(ID_DECOMPRESS,     "Decompress");

    string name = cellName.ToStdString();

    std::size_t found = name.find(".lzw");

    if (found == name.length() - 4) {
        mnu.Enable(ID_COMPRESS, false);
    }
    else {
        mnu.Enable(ID_DECOMPRESS, false);
    }
    
    mnu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MyFrame::OnPopupClick), NULL, this);
    PopupMenu(&mnu);
    mnu.Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(MyFrame::IdlePlay));
}

#define MAX_LEN_PATH 4096
#define MAX_LEN_NAME 256

struct HEADER_FOLDER {
    uint8_t chunkType; // file chunk: 0, folder chunk: 1, end of folder chunk: 2
    char name[MAX_LEN_NAME+1]; // max len of filename = 256
};

struct HEADER_FOLDER_END {
    uint8_t chunkType; // file chunk: 0, folder chunk: 1, end of folder chunk: 2
};

struct HEADER_FILE {
    uint8_t chunkType; // file chunk: 0, folder chunk: 1, end of folder chunk: 2
    char name[MAX_LEN_NAME+1]; // max len of filename = 256
    uint64_t originalSize; // 4,294,967,296 ^ 2 <-- good enough
    uint64_t compressedDataSize;
};

struct HEADER_GLOBAL {
    uint32_t numChunks;
    uint32_t numFiles;
    uint32_t numFolders;
    uint64_t sizeOfUncompressed;
};
// 00 00 00 00 00 00 00 10 = 4,294,967,296 ^ 2
//                    0x10
//cout << sizeof(FILE_HEADER) << endl;

void MyFrame::OnPopupClick(wxCommandEvent &ev) {
    void *data=static_cast<wxMenu *>(ev.GetEventObject())->GetClientData();
    wxString *cellNamePtr = (wxString *)data;
    std::cout << "MyFrame::OnPopupClick(): cellName: " << *cellNamePtr << std::endl;

    ifstream inputToEncode;
    fstream outputEncoded;
//curDirPathOnUpperPanel
    std:: cout << "MyFrame::OnPopupClick(): curDirPathOnUpperPanel + cellName: " << curDirPathOnUpperPanel + "/" + *cellNamePtr << std::endl;
    
    DIR *dir;
    struct dirent *ent;
    
    dir = opendir(curDirPathOnUpperPanel.c_str());

    string nameOfFolder;
    bool isFile = true;
    if (ev.GetId() == ID_COMPRESS) {
        
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name == (*cellNamePtr) && ent->d_type == DT_DIR) { // folder
                isFile = false;
                nameOfFolder = ent->d_name;
            }
        }

        // ----------------------------------------------------------------------------------------------------
        // TODO: use model described here to allow for non .txt files to be compressed and decompressed
        // ----------------------------------------------------------------------------------------------------

        // The !isFile section is inconsequential in this version of the app

        if (!isFile) { // folder
            wxString outputEncodedFilenameInWxString = curDirPathOnUpperPanel + "/" + *cellNamePtr + ".lzw";
            string outputEncodedFilename = outputEncodedFilenameInWxString.ToStdString();

            outputEncoded.open(outputEncodedFilename.c_str(), std::ios_base::in | std::ios_base::out | ios::ios_base::binary | ios::ios_base::app);
            outputEncoded.close();

            vector<string> names;
            vector<bool> isFolder;
            // global header
            int totalChunks;
            int totalFiles;
            int totalFolders;
            long totalSize;
            totalChunks = 0;
            totalFiles = 0;
            totalFolders = 0;
            totalSize = 0;
            
            string fullpathOfFolderToCompress = curDirPathOnUpperPanel.c_str();
            fullpathOfFolderToCompress += "/";
            fullpathOfFolderToCompress += nameOfFolder;
            getAllInfoFromFolder(names, isFolder, fullpathOfFolderToCompress, &totalChunks, &totalFiles, &totalFolders, &totalSize);
  
            cout << "MyFrame::OnPopupClick(): totalChunks = " << totalChunks << endl;
            cout << "MyFrame::OnPopupClick(): totalFiles = " << totalFiles << endl;
            cout << "MyFrame::OnPopupClick(): totalFolders = " << totalFolders << endl;
            cout << "MyFrame::OnPopupClick(): totalSize = " << totalSize << endl;
            
            HEADER_GLOBAL headerGlobal;
            headerGlobal.numChunks = totalChunks;
            headerGlobal.numFiles = totalFiles;
            headerGlobal.numFolders = totalFolders + 1; // include the root folder
            headerGlobal.sizeOfUncompressed = totalSize;
            outputEncoded.open(outputEncodedFilename.c_str(), std::ios_base::in | std::ios_base::out | ios::ios_base::binary | ios::ios_base::app);
            outputEncoded.write((const char *)&headerGlobal, sizeof(HEADER_GLOBAL));
            outputEncoded.close();

            HEADER_FOLDER rootFolder;
            rootFolder.chunkType = 1;
            strncpy(rootFolder.name, (*cellNamePtr).ToStdString().c_str(), MAX_LEN_NAME);
            outputEncoded.open(outputEncodedFilename.c_str(), std::ios_base::in | std::ios_base::out | ios::ios_base::binary | ios::ios_base::app);
            outputEncoded.write((const char *)&rootFolder, sizeof(HEADER_FOLDER));
            outputEncoded.close();
            
            for (int i = 0; i < totalChunks; i++) {
                if (isFolder.at(i)) { // it is a folder
                    cout << "MyFrame::OnPopupClick(): encoding: it is a folder" << endl;
                    HEADER_FOLDER subfolder;
                    subfolder.chunkType = 1;
                    strncpy(subfolder.name, (names[i].substr(curDirPathOnUpperPanel.size())).c_str(), MAX_LEN_NAME);
                    outputEncoded.open(outputEncodedFilename.c_str(), std::ios_base::in | std::ios_base::out | ios::ios_base::binary | ios::ios_base::app);
                    outputEncoded.write((const char *)&subfolder, sizeof(HEADER_FOLDER));
                    outputEncoded.close();
                }
                else { // it is a file
                    cout << "MyFrame::OnPopupClick(): encoding: it is a file" << endl;
                    HEADER_FILE headerFile;
                    headerFile.chunkType = 0;
                    // extract file name from relative path
                    // ex)
                    //     names[i] -> "test/a.txt"
                    int indexLastSlash = -1;
                    for (int j = names[i].length()-1; j >= 0 && indexLastSlash == -1; j--) {
                        if (names[i][j] == '/') {
                            indexLastSlash = j;
                        }
                    }
                    string filenameOnly = names[i].substr(indexLastSlash+1);
                    strncpy(headerFile.name, filenameOnly.c_str(), MAX_LEN_NAME);
                    headerFile.originalSize = getFileSize(names[i].c_str());
                    cout << "MyFrame::OnPopupClick(): originalSize: " << headerFile.originalSize << endl;
                    headerFile.compressedDataSize = 0; // update later

                    outputEncoded.open(outputEncodedFilename.c_str(), std::ios_base::in | std::ios_base::out | ios::ios_base::binary | ios::ios_base::app);
                    outputEncoded.write((const char *)&headerFile, sizeof(HEADER_FILE));
                    outputEncoded.close();

                    inputToEncode.open(names[i]);
                     
                    cout << "void MyFrame::OnPopupClick(wxCommandEvent &ev): "<< curDirPathOnUpperPanel + "/" + *cellNamePtr << endl;
                     
                    outputEncoded.open(outputEncodedFilename.c_str(), std::ios_base::in | std::ios_base::out | ios::ios_base::binary | ios::ios_base::ate);
                    outputEncoded.seekp(0, ios::end); // to append
                    int writePointerBeforeEncode = outputEncoded.tellp();
                    cout << "MyFrame::OnPopupClick(): writePointerBeforeEncode: "<< writePointerBeforeEncode << endl;
                    encode(inputToEncode, (std::fstream &)outputEncoded);
                    int writePointerAfterEncode = outputEncoded.tellp();
                    cout << "MyFrame::OnPopupClick(): writePointerAfterEncode: "<< writePointerAfterEncode << endl;
                    
                    uint64_t compressedSize = writePointerAfterEncode - writePointerBeforeEncode;
                    cout << "MyFrame::OnPopupClick(): compressedSize: "<< compressedSize << endl;
                    outputEncoded.seekp(-1*(compressedSize+8), ios::end); // compressedSize+8 is the distance from the end
                    //char buf[] = "hello";
                    //outputEncoded.write(buf, 5);
                    cout << "MyFrame::OnPopupClick(): encoding: debugging 1" << endl;
                    outputEncoded.write((const char *)&compressedSize, sizeof(uint64_t));
                    cout << "MyFrame::OnPopupClick(): encoding: debugging 2" << endl;
                    inputToEncode.close();
                    outputEncoded.close();
                    cout << "MyFrame::OnPopupClick(): encoding: debugging 3" << endl;
                }
                string curPath = names[i];
                cout << "MyFrame::OnPopupClick(): curPath: "<< curPath << endl;
                
                if (i < totalChunks-1) {
                    string nextPath = names[i+1];
                    cout << "MyFrame::OnPopupClick(): nextPath: "<< nextPath << endl;
                    int count = 0;
                    

                    //--------------------------------------------------------------
                    // Check if curPath and nextPath have the same parent folders
                    // Sibling Case
                    //     Example:
                    //          curPath : "users/sankang/test1/curPath” <- folder
                    //          nextPath: continue"users/sankang/test1/test1aFile.txt" <- file
                    //     Example:
                    //          curPath : "users/sankang/test1/curPath” <- folder
                    //          nextPath: "users/sankang/test1/test1aFolder” <- folder
                    //--------------------------------------------------------------
                    cout << "MyFrame::OnPopupClick(): siblingCase Started: " << endl;

                    bool siblingCase = false;

                    string siblingCaseTempCurPath = curPath;    // use these to avoid changing curPath and nextPath
                    string siblingCaseTempNextPath = nextPath;

                    for (int j = curPath.size()-1; j >= 0; j--) { // find last slash to compare
                        if (curPath[j] == '/') {
                            siblingCaseTempCurPath = siblingCaseTempCurPath.substr(0, j);
                            cout << "MyFrame::OnPopupClick: siblingCaseTempCurPath: " << siblingCaseTempCurPath << endl;
                            break;
                        }
                    }

                    for (int j = nextPath.size()-1; j >= 0; j--) { // find last slash to compare
                        if (nextPath[j] == '/') {
                            siblingCaseTempNextPath = siblingCaseTempNextPath.substr(0, j);
                            cout << "MyFrame::OnPopupClick: siblingCaseTempNextPath: " << siblingCaseTempNextPath << endl;
                            break;
                        }
                    }
                
                
                    if (siblingCaseTempCurPath == siblingCaseTempNextPath) { // compare substrs
                        siblingCase = true;
                        
                        if (isFolder.at(i)) { // if curPath is a folder, then make a folder end header
                                HEADER_FOLDER_END folderEnd;
                                folderEnd.chunkType = 2;
                                outputEncoded.open(outputEncodedFilename.c_str(), std::ios_base::in | std::ios_base::out | ios::ios_base::binary | ios::ios_base::app);
                                outputEncoded.write((const char *)&folderEnd, sizeof(HEADER_FOLDER_END));
                                outputEncoded.close();
                        }
                        cout << "MyFrame::OnPopupClick(): siblingCase: true" << endl;
                    }
                    
                    //--------------------------------------------------------------
                    // Check if curPath and nextPath have the same parent folders but nextPath has folders inbetween the shared parent
                    // Child Case
                    //     Example:
                    //          curPath : "users/sankang/test1/test2” <- folder
                    //          nextPath: "users/sankang/test1/test2/test1aFile.txt" <- file
                    //     Example:
                    //          curPath : "users/sankang/test1/test2” <- folder
                    //          nextPath: "users/sankang/test1/test2/test1aFolder” <- folder
                    //--------------------------------------------------------------
                    cout << "MyFrame::OnPopupClick(): childCase Started: " << endl;

                    bool childCase = false;

                    string childCaseTempCurPath = names[i];    // use these to avoid changing curPath and nextPath
                    string childCaseTempNextPath = names[i+1];

                    while (true) {
                        for (int j = childCaseTempNextPath.size()-1; j >= 0; j--) { // find last slash to compare
                            if (childCaseTempNextPath[j] == '/') {
                                childCaseTempNextPath = childCaseTempNextPath.substr(0, j);
                                cout << "MyFrame::OnPopupClick(): childCaseTempNextPath: " << childCaseTempNextPath << endl;
                                break;
                            }
                        }
                        if (childCaseTempCurPath == childCaseTempNextPath) { // compare substrs
                            childCase = true;
                            cout << "MyFrame::OnPopupClick(): childCase: true" << endl;
                            break;
                        }
                        if (childCaseTempNextPath.size() < childCaseTempCurPath.size()) {
                            cout << "MyFrame::OnPopupClick(): childCaseTempNextPath.size() < childCaseTempCurPath.size()" << endl;
                            break;
                        }
                    }
                    
                    //--------------------------------------------------------------
                    // Check if the current path is a folder and Makes a FOLDER_END header
                    //--------------------------------------------------------------
                    int indexLastSlash = -1; // get to the last slash
                    for (int j = curPath.length()-1; j >= 0 && indexLastSlash == -1; j--) {
                        if (curPath[j] == '/') {
                            indexLastSlash = j;
                        }
                    }
                    
                    DIR *dir;
                    struct dirent *ent;
                    
                    dir = opendir(curPath.substr(0, indexLastSlash).c_str());
                    while ((ent = readdir(dir)) != NULL) {
                        if (ent->d_name == curPath && ent->d_type == DT_DIR) { // folder
                            HEADER_FOLDER_END folderEnd;
                            folderEnd.chunkType = 2;
                            outputEncoded.open(outputEncodedFilename.c_str(), std::ios_base::in | std::ios_base::out | ios::ios_base::binary | ios::ios_base::app);
                            outputEncoded.write((const char *)&folderEnd, sizeof(HEADER_FOLDER_END));
                            outputEncoded.close();
                        }
                    }
                    
                    bool endOfCurPath = false;
                    bool endOfNextPath = false;

                    if (nextPath[0] == NULL) {
                        siblingCase = true;
                    }
                    
                    while (!siblingCase && !childCase) {
                        // curdirpath = users
                        // foldertocompress = users/a
                        // nameoffile1 = users/a/b/c <-- file or folder
                        // nameoffile2 = users/a/z <-- file or folder
                        //
                        // take away the last name (c and z) then compare the remaining paths
                        //
                        // they arent the same so it means that the two congruent files have different paths
                        // make a folder end header
                        //
                        // nameoffile3 = users/a/b/c/d <-- file or folder
                        // nameoffile4 = users/a/c <-- file or folder
                        //
                        // while loop will be used and can make multiple folder end headers
                        //
                        // 3 folder end headers are needed
                        
                        //----------------------------------------------------------
                        //----------------------------------------------------------
                        //----------------------------------------------------------
                        // See if and FOLDER_END headers are needed
                        // Case 1: Normal Case
                        //    curPath : "users/sankang/test1"
                        //    nextPath: "users/sankang/b.txt"
                        // Case 2: Special Case
                        //----------------------------------------------------------
                        //----------------------------------------------------------
                        //----------------------------------------------------------

                        //    curPath : "users/sankang/test1"
                        //    nextPath: "users/sankang/b.txt"
                        int curPathIndexFirstSlash = -1; // get to the first slash
                        int nextPathIndexFirstSlash = -1; // get to the first slash for nextPath
                        for (int j = 0; //j < curPathIndexFirstSlash &&
                             curPathIndexFirstSlash == -1; j++) {
                            // j is at the end of curPath so there is no '/'
                            cout << "MyFrame::OnPopupClick(): curPath[j]: "<< curPath[j] << endl;
                            cout << "MyFrame::OnPopupClick(): curPath[j]: j:" << j << endl;
                            if (j == curPath.size()-1) {
                                endOfCurPath = true; // break because if there is no slash, then it is a file
                                curPathIndexFirstSlash = j;
                                cout << "MyFrame::OnPopupClick(): endOfCurPath = true" << endl;
                                break;
                            }
                            else if (curPath[j] == '/') {
                                curPathIndexFirstSlash = j+1;
                                break;
                            }
                        }
    //                    if (endOfCurPath) {
    //                        break;
    //                    }
                        if (!endOfNextPath) {
                            for (int j = 0; //j < nextPathIndexFirstSlash &&
                                 nextPathIndexFirstSlash == -1; j++) {
                                cout << "MyFrame::OnPopupClick(): nextPath[j]: "<< nextPath[j] << endl;
                                cout << "MyFrame::OnPopupClick(): nextPath[j]: j:" << j << endl;
                                if (j == nextPath.size()-1) { // j is at the end of nextPath so there is no '/'
                                    nextPathIndexFirstSlash = j; // the '/' is always after the name, so put nextPathIndexFirstSlash at one after the end of nextPath
                                    endOfNextPath = true;
                                    cout << "MyFrame::OnPopupClick(): endOfNextPath = true" << endl;
                                    break;
                                }
                                else if (nextPath[j] == '/') { // if j isnt at the end of nextPath
                                    nextPathIndexFirstSlash = j+1;
                                    break;
                                }
                            }
                        }
                        
                        cout << "MyFrame::OnPopupClick: endOfCurPath: " << endOfCurPath << endl;
                        cout << "MyFrame::OnPopupClick: endOfNextPath: " << endOfNextPath << endl;
                        // normal case 1
                        //    curPath : "users/sankang/test1"
                        //    nextPath: "users/sankang/b.txt"
                        //
                        //    curPath : users/sankang/b/a.txt
                        //    nextPath: users/sankang/b
                        // obscure case 1
                        //    curPath : "users/sankang/test1/a.txt" and a.txt is the last in test1
                        //    nextPath: "users/sankang/b.txt"
                        if (curPath.substr(0, curPathIndexFirstSlash) == nextPath.substr(0, nextPathIndexFirstSlash)
    //                        || (curPath.substr(0, curPathIndexFirstSlash) + "/")  == nextPath.substr(0, nextPathIndexFirstSlash) ||
    //                        curPath.substr(0, curPathIndexFirstSlash) == (nextPath.substr(0, nextPathIndexFirstSlash) + "/")
    //                        ||(curPath.substr(0, curPathIndexFirstSlash) + "/") == (nextPath.substr(0, nextPathIndexFirstSlash) + "/")
                            ) {
                            //
                            // normal case
                            //   compare a substr from 0 to the first slashes of both paths
                            //      1st loop:
                            //         "users/" = "users/"
                            //      2nd loop:
                            //         "sankang/" = "sankang/"
                            //      3rd loop:
                            //        "text1" != "b.txt"
                            //   since there is a difference in the third it makes a FOLDER_END header
                            //
                            // obscure case
                            //   compare a substr from 0 to the first slashes of both paths
                            //      1st loop:
                            //         "users/" = "users/"
                            //      2nd loop:
                            //         "sankang/" = "sankang/"
                            //      3rd loop:
                            //        "text1/" != ("b.txt" + "/")
                            //   since there is a difference in the third it makes a FOLDER_END header
                            cout << "MyFrame::OnPopupClick: if (curPath.substr(0, curPathIndexFirstSlash) == nextPath.substr(0, nextPathIndexFirstSlash)" << endl;

                            count++;
                            curPath = curPath.substr(curPathIndexFirstSlash);
                            nextPath = nextPath.substr(nextPathIndexFirstSlash);
                        }
                        // reached the end of the next path but not the cur path
                        // so it needs to add more FOLDER_END headers
                        //
                        //    curPath : "users/sankang/test1/a/a.txt" and a.txt is the last in test1
                        //    nextPath: "users/sankang/b/b.txt"
                        //      1st loop:
                        //         "users/" = "users/"
                        //      2nd loop:
                        //         "sankang/" = "sankang/"
                        //      3rd loop:
                        //         endOfNextPath == true && endOfCurPath == false
                        //         at the moment there are no FOLDER_END headers, but it needs 2
                        else if (!endOfCurPath && endOfNextPath) {
                            cout << "MyFrame::OnPopupClick: !endOfCurPath && endOfNextPath" << endl;
                            count++;
                            HEADER_FOLDER_END folderEnd;
                            folderEnd.chunkType = 2;
                            outputEncoded.open(outputEncodedFilename.c_str(), std::ios_base::in | std::ios_base::out | ios::ios_base::binary | ios::ios_base::app);
                            outputEncoded.write((const char *)&folderEnd, sizeof(HEADER_FOLDER_END));
                            outputEncoded.close();
                            curPath = curPath.substr(curPathIndexFirstSlash);
                        }
                        else { // not equal to each other
                            if (endOfCurPath && endOfNextPath) {
                                cout << "MyFrame::OnPopupClick: endOfCurPath && endOfNextPath" << endl;
                                break;
                            }
                            else {
                                cout << "MyFrame::OnPopupClick: !endOfCurPath && !endOfNextPath" << endl;
                                curPath = curPath.substr(curPathIndexFirstSlash);
                                nextPath = nextPath.substr(nextPathIndexFirstSlash);
                            }
                            cout << "MyFrame::OnPopupClick: FolderEnd created" << endl;
                            HEADER_FOLDER_END folderEnd;
                            folderEnd.chunkType = 2;
                            outputEncoded.open(outputEncodedFilename.c_str(), std::ios_base::in | std::ios_base::out | ios::ios_base::binary | ios::ios_base::app);
                            outputEncoded.write((const char *)&folderEnd, sizeof(HEADER_FOLDER_END));
                            outputEncoded.close();
                            // users/sankang/test1/a/b.txt
                            // users/sankang/test2
    //                        break; //        01234567890
                            
                        }
    //                    if (endOfCurPath && endOfNextPath) {
    //                        break;
    //                    }
                        //----------------------------------------------------------

                            
    //                    if (names[j].substr(indexLastSlash) != names[j+1].substr(curDirPathOnUpperPanel.size())) {
    //                        HEADER_FOLDER_END folderEnd;
    //                        folderEnd.chunkType = 2;
    //                        outputEncoded.open(outputEncodedFilename.c_str(), std::ios_base::in | std::ios_base::out | ios::ios_base::binary | ios::ios_base::app);
    //                        outputEncoded.write((const char *)&folderEnd, sizeof(HEADER_FOLDER_END));
    //                        outputEncoded.close();
    //                    }
    //                    else { // the two paths are the same
    //                        break;
    //                    }
    //                    k++;
                    }
                }
                else {
                    cout << "MyFrame::OnPopupClick(): end of chunks, no next path" << endl;
//                    HEADER_FOLDER_END folderEnd;
//                    folderEnd.chunkType = 2;
//                    outputEncoded.open(outputEncodedFilename.c_str(), std::ios_base::in | std::ios_base::out | ios::ios_base::binary | ios::ios_base::app);
//                    outputEncoded.write((const char *)&folderEnd, sizeof(HEADER_FOLDER_END));
//                    outputEncoded.close();
                    string pathForFolderEnd = curPath;
                    cout << "MyFrame::OnPopupClick(): no next path: pathForFolderEnd: " << pathForFolderEnd << endl;
                    DIR *dir;
                    struct dirent *ent;
                    string pathToCheckForFile;
                    for (int j = pathForFolderEnd.size()-1; j >= 0; j--) {
                        if (pathForFolderEnd[j] == '/') {
                            pathToCheckForFile = pathForFolderEnd.substr(0, j);
                            cout << "MyFrame::OnPopupClick(): no next path: pathToCheckForFile: " << pathToCheckForFile << endl;
                            break;
                        }
                    }
                    
                    dir = opendir(pathToCheckForFile.c_str());
                    while ((ent = readdir(dir)) != NULL) {
                        cout << "MyFrame::OnPopupClick(): no next path: debugging 1" << endl;
                        bool done = false;
                        cout << "MyFrame::OnPopupClick(): no next path: ent->d_name: " << ent->d_name << endl;
                        string temp = pathForFolderEnd;
                        int entSize = strlen(ent->d_name);
                        temp = temp.substr(temp.size()-entSize);
                        cout << "MyFrame::OnPopupClick(): no next path: temp: " << temp << endl;
                        if (ent->d_name == temp && ent->d_type != DT_DIR) {
                            cout << "MyFrame::OnPopupClick(): no next path: pathForFolderEnd is a file" << endl;
                            for (int j = pathForFolderEnd.size()-1; j >= 0; j--) {
                                if (pathForFolderEnd[j] == '/') {
                                    pathForFolderEnd = pathForFolderEnd.substr(0, j);
                                    done = true;
                                    cout << "MyFrame::OnPopupClick(): no next path: pathForFolderEnd: " << pathForFolderEnd << endl;
                                    break;
                                }
                            }
                        }
                        else if (ent->d_name == temp && ent->d_type == DT_DIR) {
                            cout << "MyFrame::OnPopupClick(): no next path: pathForFolderEnd isnt a file" << endl;
//                            HEADER_FOLDER_END folderEnd;
//                            folderEnd.chunkType = 2;
//                            outputEncoded.open(outputEncodedFilename.c_str(), std::ios_base::in | std::ios_base::out | ios::ios_base::binary | ios::ios_base::app);
//                            outputEncoded.write((const char *)&folderEnd, sizeof(HEADER_FOLDER_END));
//                            outputEncoded.close();
                            break;
                        }


                        if (done) {
                            cout << "MyFrame::OnPopupClick(): no next path: broke: " << endl;

                            break;
                        }
                    }
                    pathForFolderEnd = pathForFolderEnd.substr(fullpathOfFolderToCompress.size());
                    cout << "MyFrame::OnPopupClick(): no next path: pathForFolderEnd: " << pathForFolderEnd << endl;
                    bool done = false;
                    if (pathForFolderEnd[0] == '/') {
                        pathForFolderEnd = pathForFolderEnd.substr(1);
                    }
                    while (true) {
                        for (int j = 0; j < pathForFolderEnd.size(); j++) {
                            if (pathForFolderEnd[j] == '/' && j != 0) {
                                cout << "MyFrame::OnPopupClick(): no next path: pathForFolderEnd: " << pathForFolderEnd << endl;
                                pathForFolderEnd = pathForFolderEnd.substr(j);
                                HEADER_FOLDER_END folderEnd;
                                folderEnd.chunkType = 2;
                                outputEncoded.open(outputEncodedFilename.c_str(), std::ios_base::in | std::ios_base::out | ios::ios_base::binary | ios::ios_base::app);
                                outputEncoded.write((const char *)&folderEnd, sizeof(HEADER_FOLDER_END));
                                outputEncoded.close();
                                cout << "MyFrame::OnPopupClick(): no next path: Folder_end made" << endl;
                            }
                            else if (j == pathForFolderEnd.size()-1) {
                                cout << "MyFrame::OnPopupClick(): no next path: j == pathForFolderEnd.size()-1" << endl;
                                HEADER_FOLDER_END folderEnd;
                                folderEnd.chunkType = 2;
                                outputEncoded.open(outputEncodedFilename.c_str(), std::ios_base::in | std::ios_base::out | ios::ios_base::binary | ios::ios_base::app);
                                outputEncoded.write((const char *)&folderEnd, sizeof(HEADER_FOLDER_END));
                                outputEncoded.close();
                                cout << "MyFrame::OnPopupClick(): no next path: Folder_end made" << endl;
                                done = true;
                                break;
                            }
                            cout << "MyFrame::OnPopupClick(): no next path: pathForFolderEnd: " << pathForFolderEnd << endl;
                        }
                        if (done) {
                            break;
                        }
                    }
                    
                    break;
                }

            }
            
            // end of root folder
            HEADER_FOLDER_END folderEnd;
            folderEnd.chunkType = 2;
            outputEncoded.open(outputEncodedFilename.c_str(), std::ios_base::in | std::ios_base::out | ios::ios_base::binary | ios::ios_base::app);
            outputEncoded.write((const char *)&folderEnd, sizeof(HEADER_FOLDER_END));
            outputEncoded.close();

        }
        else { // file
            cout << "MyFrame::OnPopupClick: File Compression Started" << endl;

            cout << "void MyFrame::OnPopupClick(wxCommandEvent &ev): " << curDirPathOnUpperPanel + "/" + *cellNamePtr + ".lzw" << endl;

            string outputEncodedFilename = curDirPathOnUpperPanel;
            outputEncodedFilename += "/";
            outputEncodedFilename += cellNamePtr->ToStdString().substr(0, (*cellNamePtr).size() - 4);
            outputEncodedFilename += ".lzw";

            outputEncoded.open(outputEncodedFilename.c_str(), std::ios_base::in | std::ios_base::out | ios::ios_base::binary | ios::ios_base::app);

            string inputEncodedFilename = curDirPathOnUpperPanel;
            inputEncodedFilename += "/";
            inputEncodedFilename += cellNamePtr->ToStdString();
            inputToEncode.open(inputEncodedFilename.c_str());
                      
            outputEncoded.close();
            outputEncoded.open(outputEncodedFilename.c_str(), std::ios_base::in | std::ios_base::out | ios::ios_base::binary | ios::ios_base::ate);
            
            outputEncoded.seekp(0, ios::end); // to append
            int writePointerBeforeEncode = outputEncoded.tellp();
            encode(inputToEncode, outputEncoded);
            int writePointerAfterEncode = outputEncoded.tellp();
            uint64_t compressedSize = writePointerAfterEncode - writePointerBeforeEncode;
            outputEncoded.seekp(compressedSize+8, ios::end); // to append
            outputEncoded.write((const char *)&compressedSize, sizeof(uint64_t));

            inputToEncode.close();
            outputEncoded.close();
        }
            // refresh the upper right panel to see the new lzw file
        makeUpperRightPanel(curDirPathOnUpperPanel);
        
        cout << "MyFrame::OnPopupClick: Compression Done!!!" << endl;
    }
    else if (ev.GetId() == ID_DECOMPRESS) {

        // Only supports the decompression of .txt files so far

        cout << "MyFrame::OnPopupClick(): Decompression started" << endl;
        wxString inputEncodedFilenameInWxString = curDirPathOnUpperPanel + "/" + *cellNamePtr;
        cout << "MyFrame::OnPopupClick(): inputEncodedFilenameInWxString = " << inputEncodedFilenameInWxString.ToStdString() << endl;

        string inputEncodedFilename = inputEncodedFilenameInWxString.ToStdString();

        ifstream inputToDecode;
        ofstream outputDecoded;

        inputToDecode.open(inputEncodedFilename.c_str());
        
        string curFolder = curDirPathOnUpperPanel;
        curFolder += "/";

        string curFileFullPath = inputEncodedFilename.substr(0, inputEncodedFilename.size() - 4) + ".txt";

        cout << "MyFrame::OnPopupClick(): curFileFullPath: " << curFileFullPath << endl;

        ofstream curFile;
        curFile.open(curFileFullPath);

        inputToDecode.seekg(0, ios::end);
        int file_size = inputToDecode.tellg();

        inputToDecode.close();
        inputToDecode.open(inputEncodedFilename.c_str());

        cout << "MyFrame::OnPopupClick(): file_size: " << file_size << endl;

        decode(inputToDecode, curFile, file_size);

        curFile.close();
        
        inputToDecode.close();

            //------------------------------------------------------------------
            // check if the file is .lzw
            //------------------------------------------------------------------
            //        "abc.bcd.txt"
            //         01234567890
            //              i:3210
                    
            // originalPrefix: "abc.bcd"
//            wxString decodedOutputFilename = *cellNamePtr;
//            decodedOutputFilename = decodedOutputFilename.substr(0, decodedOutputFilename.length()-4);
//
//            string originalPrefix = decodedOutputFilename.ToStdString().substr(0, decodedOutputFilename.length() - 4);
//            string originalPostfix = decodedOutputFilename.ToStdString().substr(decodedOutputFilename.length() - 4); // ".txt"
//
//            cout << "MyFrame::OnPopupClick(): Decompression step 4" << endl;
//
//
//            cout << "MyFrame::OnPopupClick(): originalPrefix = |" << originalPrefix << "|" << endl;
//            cout << "MyFrame::OnPopupClick(): originalPostfix = |" << originalPostfix << "|" << endl;
//            exit(1);
//            //------------------------------------------------------------------
//
//            inputToDecode.open (curDirPathOnUpperPanel + "/" + *cellNamePtr);
//
//            decodedOutputFilename = *cellNamePtr;
//            decodedOutputFilename = decodedOutputFilename.substr(0, decodedOutputFilename.length()-4);
//
////            bool isFolder = true;
////            if (decodedOutputFilename[decodedOutputFilename.length()-4] == '.') { // not 100%
////                isFolder = false;
////            }
//
//            // Algorithm: avoid making duplicates // putting " n" after name
//            // File to decode: "abc.bcd.txt.lzw"
//            // Remove '.lzw' -> result: "abc.bcd.txt"
//            // Desitination File: "abc.bcd.txt"
//            // Remove the everything in the front up to the first "." -> result: "abc"
//            // Before the file is made for decoding, check if there are any other files with the same prefix "abc" (the part before the first ".")
//            //     ex)
//            //        "abc.bcd.txt" <- duplicated!!!
//            //        "abc 1.bcd.txt" <- duplicated!!!
//            //        "abc 2.bcd.txt" <- duplicated!!!
//            //     Goal: We must make the filename like "abc 3.bcd.txt"
//            // Extract the string between "abc" and the first '.'
//            //     ex)
//            //        "abc.bcd.txt"   -> postfix: ""
//            //        "abc 1.bcd.txt" -> postfix: " 1"
//            //        "abc 2.bcd.txt" -? postfix: " 2"
//            // For each postfix from above to find out the highest version number:
//            //     Get the current version number
//            //        Case 1) when the length of postfix is 0:
//            //                the current version is 1
//            //        Case 2) when the length of postfix is greator than 0:
//            //                remove the first space character and convert the rest to int
//            //                the integer is the current version number
//            //     Compare the current version number with the current maximum version number
//            //     If the current version number is greater than the current maximum version number
//            //        Set the current maximum version number to the current version number
//            // The next version number is [the current maximum version number + 1]
//            //     ex) int nextVersionNumber = maxVersionNumber + 1;
//            // Append the next version number to the prefix and assemble the full path
//            //     ex)
//            //         prefix: "abc"
//            //         nextVersionNumber: 3
//            //         rest: ".bcd.txt"
//            //
//            //         fullname = prefix + " " + nextVersionNumber + rest -> "abc 3.bcd.txt"
//            //         fullpath -> "...\...\abc 3.bcd.txt"
//            // outputDecoded.open (curDirPathOnUpperPanel + "/" + decodedOutputFilename + "");
//            //----------------------------------------------------------------------
//            // additional feature
//            //----------------------------------------------------------------------
//            // if there are postfixed 1 and 3 without a postfix 2, then make 2 the next postfix
//            DIR *dir;
//
//            struct dirent *ent;
//
//            if ((dir = opendir(curDirPathOnUpperPanel.c_str())) == NULL) {
//                cout << "error: " << curDirPathOnUpperPanel << " does not exist" << endl;
//                exit(1);
//            }
//
//            bool duplicate = false;
//
//            cout << "MyFrame::OnPopupClick(): Decompression step 1" << endl;
//
//            int i = 0;
//            while ((ent = readdir(dir)) != NULL) { // this is to see if there are any duplicates
//                if (ent->d_name == decodedOutputFilename) {
//                    duplicate = true;
//                    break;
//                }
//            }
//            closedir(dir);
//
//    //        char curChar[2];
//    //        curChar[1] = '\0';
//            cout << "MyFrame::OnPopupClick(): Decompression step 2" << endl;
//
//
//            i = 0;
//            while (true) {
//                if (decodedOutputFilename.at(decodedOutputFilename.length() - i - 1) == '.') {
//                    break;
//                }
//                i++;
//            }
//
//            cout << "MyFrame::OnPopupClick(): decodedOutputFilename: " << decodedOutputFilename << endl;
//
//
//            cout << "MyFrame::OnPopupClick(): Decompression step 3" << endl;
//
//
//
//
//            i = 0;
//
//            if (duplicate) {
//                if ((dir = opendir(curDirPathOnUpperPanel.c_str())) == NULL) {
//                    cout << "error: " << curDirPathOnUpperPanel << " does not exist" << endl;
//                    exit(1);
//                }
//
//                //------------------------------------------------------------------
//                // Extract the string between "abc" and the first '.'
//                //------------------------------------------------------------------
//                vector<string> postfix;
//                string curFilename = "";
//
//                while ((ent = readdir(dir)) != NULL) { // this is to see if there are any duplicates
//                    if (ent->d_type != DT_DIR) {
//                        // check if the current filename begins with original prefix and ends with original postfix
//                        // convert c_str to class string
//                        curFilename = "";
//                        curFilename = curFilename + ent->d_name;
//                        cout << "MyFrame::OnPopupClick(): curFilename = |" << curFilename << "|" << endl;
//
//                        // curFilename: "abc 2.bcd.txt"
//                        if (curFilename.substr(0, originalPrefix.length()) == originalPrefix &&
//                            curFilename.substr(curFilename.length() - originalPostfix.length()) == originalPostfix) {
//                            cout << "MyFrame::OnPopupClick(): matched!!!" << endl;
//
//                            // curFilename: "abc 2.bcd.txt" -> curPostfix: " 2"
//                            int lenCurPostfix = curFilename.length() - originalPrefix.length() - originalPostfix.length();
//                            string curPostfix = curFilename.substr(originalPrefix.length(), lenCurPostfix);
//                            postfix.push_back(curPostfix);
//                            cout << "MyFrame::OnPopupClick(): curPostfix = |" << curPostfix << "|" << endl;
//
//                        }
//                    }
//                    else { // is folder
//
//                    }
//                }
//                closedir(dir);
//                //------------------------------------------------------------------
//                int highestPostFix = -1;
//                for (int i = 0; i < postfix.size(); i++) {
//                    int curPostfixInInt = 1;
//                    //        Case 1) when the length of postfix is 0:
//                    //                the current version is 1
//                    //        Case 2) when the length of postfix is greator than 0:
//                    //                remove the first space character and convert the rest to int
//                    //                the integer is the current version number
//                    if (postfix.at(i).length() > 0) {
//                        // " 2", " 123", ...
//                        curPostfixInInt = atoi(postfix.at(i).substr(1).c_str());
//                    }
//
//                    if (curPostfixInInt > highestPostFix) {
//                        highestPostFix = curPostfixInInt;
//                    }
//                    cout << "highestPostFix = " << highestPostFix << endl;
//                }
//
//                // The next version number is [the current maximum version number + 1]
//                //     ex) int nextVersionNumber = maxVersionNumber + 1;
//                int nextVersionNumber = highestPostFix + 1;
//
//                // Append the next version number to the prefix and assemble the full path
//                //     ex)
//                //         prefix: "abc"
//                //         nextVersionNumber: 3
//                //         rest: ".bcd.txt"
//                //
//                //         fullname = prefix + " " + nextVersionNumber + rest -> "abc 3.bcd.txt"
//                //         fullpath -> "...\...\abc 3.bcd.txt"
//                // outputDecoded.open (curDirPathOnUpperPanel + "/" + decodedOutputFilename + "");
//
//                char buf[256];
//                sprintf(buf, "%s%s%d%s", originalPrefix.c_str(), " ", nextVersionNumber, originalPostfix.c_str());
//                string nextVersionFilename = buf;
//                decodedOutputFilename = nextVersionFilename;
//
//                cout << "decodedOutputFilename = " << decodedOutputFilename << endl;
//                //outputDecoded << "abcde" << endl;
//        //        exit(1);
//                outputDecoded.open (curDirPathOnUpperPanel + "/" + decodedOutputFilename + "");
//
//                decode(inputToDecode, outputDecoded);
//            }
//
//        }
//        else { // is folder
//            cout << "MyFrame::OnPopupClick(): Decompressing folder started" << endl;
//            inputToDecode.open (curDirPathOnUpperPanel + "/" + *cellNamePtr);
//            HEADER_GLOBAL headerGlobal;
////            char * numChunks = new char[5];
////            char * numFiles = new char[5];
////            char * numFolders = new char[5];
////            char * sizeOfUncompressed = new char[9];
////            inputToDecode.read(numChunks, 4);
////            inputToDecode.read(numFiles, 4);
////            inputToDecode.read(numFolders, 4);
////            inputToDecode.read(sizeOfUncompressed, 8);
//            inputToDecode.read((char *)&headerGlobal, sizeof(HEADER_GLOBAL));
//
//            cout << "MyFrame::OnPopupClick: headerGlobal.numChunks = " << headerGlobal.numChunks << endl;
//            cout << "MyFrame::OnPopupClick: headerGlobal.numFiles = " << headerGlobal.numFiles << endl;
//            cout << "MyFrame::OnPopupClick: headerGlobal.numFolders = " << headerGlobal.numFolders << endl;
//            cout << "MyFrame::OnPopupClick: headerGlobal.sizeOfUncompressed = " << headerGlobal.sizeOfUncompressed << endl;
//
////            for (int i = 0; i < numChunks; i++) {
////
////            }
////            struct HEADER_FOLDER {
////                uint8_t chunkType; // file chunk: 0, folder chunk: 1, end of folder chunk: 2
////                char name[MAX_LEN_NAME+1]; // max len of filename = 256
////            };
////
////            struct HEADER_FOLDER_END {
////                uint8_t chunkType; // file chunk: 0, folder chunk: 1, end of folder chunk: 2
////            };
////
////            struct HEADER_FILE {
////                uint8_t chunkType; // file chunk: 0, folder chunk: 1, end of folder chunk: 2
////                char name[MAX_LEN_NAME+1]; // max len of filename = 256
////                uint64_t originalSize; // 4,294,967,296 ^ 2 <-- good enough
////                uint64_t compressedDataSize;
////            };
////
////            struct HEADER_GLOBAL {
////                uint32_t numChunks;
////                uint32_t numFiles;
////                uint32_t numFolders;
////                uint64_t sizeOfUncompressed;
////            };
//
////            string curFullPath;
//            string curFullPath = curDirPathOnUpperPanel + "/" + cellNamePtr->ToStdString();
//            curFullPath = curFullPath.substr(0, curFullPath.size() - 4);
//            mkdir(curFullPath.c_str(), 0775);
////            while (true) {
//            for (int i = 0; i < headerGlobal.numChunks; i++) {
//                cout << "MyFrame::OnPopupClick: Step 5" << endl;
//                char * chunkType = new char[2];
//                inputToDecode.read(chunkType, 1);
//                cout << "MyFrame::OnPopupClick: chunkType: " << chunkType[0] << endl;
//
//                if (*chunkType == '0') { // is file
//                    HEADER_FILE tempHeaderFile;
//                    inputToDecode.read((char *)&tempHeaderFile, sizeof(tempHeaderFile));
////                    char *file = new char[tempHeaderFile.compressedDataSize];
////                    inputToDecode.read(file, tempHeaderFile.compressedDataSize);
//
////                    char ch = 0x01000001; // 0x65
////                    decode(inputToDecode, outputDecoded);
//                    string curItemFullPath = curFullPath;
//                    curItemFullPath += "/";
//                    curItemFullPath += tempHeaderFile.name;
//                    cout << "MyFrame::OnPopupClick: curItemFullPath: " << curItemFullPath << endl;
//                    ofstream fout;
//                    fout.open(curItemFullPath.c_str()); // gives the full path including the name
//                    decode(inputToDecode, outputDecoded, tempHeaderFile.compressedDataSize);
//                    fout.close();
//                    for (int i = curItemFullPath.size() - 1; i >= 0; i++) {
//                        if (curItemFullPath[i] == '/') {
//                            curItemFullPath = curItemFullPath.substr(0, i-1);
//                            break;
//                        }
//                    }
//                }
//                else if (*chunkType == '1') { // is folder
//                    HEADER_FOLDER tempHeaderFolder;
//                    inputToDecode.read((char *)&tempHeaderFolder, sizeof(tempHeaderFolder));
////                    const char *homeDir = getenv("HOME");
////                    char finalPath[256];
////
////                    sprintf(final, "%s/Desktop/%s", homeDir, game_name);
////                    cout << "final: " << final << endl;
//
//                    // java
////                    mkdir(curFullPath + "/" + tempHeaderFolder.name, 0775);
//
//                    string curTempFullPath = curFullPath;
//                    curTempFullPath += "/";
//                    curTempFullPath += tempHeaderFolder.name;
//                    mkdir(curTempFullPath.c_str(), 0775);
//
////                    char curTempFullPath[1024];
////                    sprintf(curTempFullPath, "%s/%s", curFullPath.c_str(), tempHeaderFolder.name);
////                    mkdir(curTempFullPath, 0775);
//
//                    curFullPath = curTempFullPath;
//                }
//                else if (*chunkType == '2') { // is end of folder
//                    for (int i = curFullPath.size() - 1; i >= 0; i++) {
//                        if (curFullPath[i] == '/') {
//                            curFullPath = curFullPath.substr(0, i-1);
//                            break;
//                        }
//                    }
//                }
////                else {}
//            }
//        }
//
//        inputToDecode.close();
//        outputDecoded.close();
//
//        makeUpperRightPanel(curDirPathOnUpperPanel);
    }
}
            
void MyFrame::OnMouseLeftClick(wxMouseEvent& ev) {
    printf("MyFrame::OnMouseLeftClick()\n");

    wxPoint mouse_position = ev.GetPosition();
    int temp_num = wxTREE_HITTEST_ONITEMLABEL;
    
    wxTreeItemId id = tree->HitTest(mouse_position, temp_num);
    
    if (id)
    {
        wxString data = tree->GetItemText(id);
        tree->SelectItem(id);

        std::cout << "data: " << data << std::endl;
        
        string fullPath = data.ToStdString();
        
        wxTreeItemId parentId = id;

        while (true) {
            parentId = tree->GetItemParent(parentId);
            if (parentId != 0) {
                if (tree->GetItemParent(parentId) != 0) {
                    wxString parentData = tree->GetItemText(parentId);
                    fullPath = "/" + fullPath;
                    fullPath = parentData.ToStdString() + fullPath;
                }
            }
            else {
                break;
            }
        }
       
        if (fullPath == rootNameOnly) {
            fullPath = rootParentDirPathOnLeftPanel + fullPath;
        }
        else {
            fullPath = "/" + fullPath;
            fullPath = rootNameOnly + fullPath;
            fullPath = rootParentDirPathOnLeftPanel + fullPath;
        }

        if (makeUpperRightPanel(fullPath)) {
            curDirPathOnUpperPanel = fullPath;
        }
        std::cout << "OnMouseLeftClick:  fullPath: " << fullPath << std::endl;
        std::cout << "OnMouseLeftClick:  curDirPathOnUpperPanel: " << curDirPathOnUpperPanel << std::endl;
    }
    else
        ev.Skip();
}
bool MyFrame::makeUpperRightPanel(wxString fullPath) {
    grid->SetDefaultCellOverflow(false);

    std::cout << "makeUpperRightPanel:" << fullPath.c_str() << std::endl;
    
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(fullPath.c_str())) == NULL) {
        return false;
    }

    if ((ent = readdir(dir)) != NULL) {
        delete grid;
    }

    grid = new wxGrid(fileListPanel, wxID_ANY,
                      wxPoint( 0, 0 ),
                      wxSize(FILE_LIST_PANEL_W, FILE_LIST_PANEL_H));
    // this will create a grid and, by default, an associated grid
    // table for strings
    grid->CreateGrid( 0, 0 );
    
    grid->SetRowLabelSize(0);
    
    grid->AppendCols(3);
    
    grid->EnableEditing(false);
    
    grid->SetColLabelValue(0, _("")); // from wxSmith
    grid->SetColLabelValue(1, _("Name")); // from wxSmith
    grid->SetColLabelValue(2, _("Size")); // from wxSmith
    
    grid->SetColSize(0, 20);
    grid->SetColSize(1, 400);
    grid->SetColSize(2, 100);

    /* print all the files and directories within directory */
    int i = 0;
    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_name[0] != '.') {
            grid->AppendRows(1);
            gridRowAmount++;
            if (ent->d_type == DT_DIR) {
                //                    printf ("%s\n", ent->d_name);
                grid->SetCellRenderer(i, 0, new FolderGridCellRenderer);
            }
            else {
                grid->SetCellRenderer(i, 0, new FileGridCellRenderer);
            }
            
            grid->SetCellValue( i, 1, ent->d_name);
            
            string curFullpath = fullPath.ToStdString();
            curFullpath += "/";
            curFullpath += ent->d_name;

            int fileSize = getFileSize(curFullpath.c_str());
            
            char buf[80];
            sprintf(buf, "%d bytes", fileSize);
            
            printf ("%s: %d\n", ent->d_name, fileSize);
            
            grid->SetCellAlignment(i, 2, wxALIGN_RIGHT, wxALIGN_CENTRE);
            grid->SetCellValue( i, 2, buf);

            grid->SetRowSize(i, 20);

            i++;
            //                }
        }
    }

    fileListPanel->Refresh();

    closedir (dir);
    return true;
}

void MyFrame::getAllInfoFromFolder(vector<string> &names, vector<bool> &isFolder, string path, int *totalChunks, int *totalFiles, int *totalFolders, long *totalSize) {
    DIR *dir;
    struct dirent *ent;
    dir = opendir(path.c_str());
    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_name[0] != '.') {
            (*totalChunks)++;
            string curItemFullPath = path + "/";
            curItemFullPath += ent->d_name;
            names.push_back(curItemFullPath);
            cout << "MyFrame::getAllInfoFromFolder curItemFullPath:" << curItemFullPath.c_str() << endl;
            if (ent->d_type != DT_DIR) {
                (*totalFiles)++;
                isFolder.push_back(false);
                (*totalSize) += getFileSize(curItemFullPath.c_str());
                cout << "MyFrame::getAllInfoFromFolder getFileSize(curItemFullPath.c_str()):" << getFileSize(curItemFullPath.c_str()) << endl;
            }
            else {
                (*totalFolders)++;
                isFolder.push_back(true);
                getAllInfoFromFolder(names, isFolder, curItemFullPath, totalChunks, totalFiles, totalFolders, totalSize);
            }
        }
    }
}
void MyFrame::IdlePlay(wxIdleEvent& evt) {
    fileListPanel->Refresh();
}
