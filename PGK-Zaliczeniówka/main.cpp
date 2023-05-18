#include <wx/wx.h>
#include "MainFrame.h"

class MyApp : public wxApp {

public:

	virtual bool OnInit();
	virtual int OnExit() { return 0; }

};

IMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
	SetProcessDPIAware();
	MainFrame* mainFrame = new MainFrame(NULL);

	mainFrame->Show(true);
	SetTopWindow(mainFrame);

	return true;
}