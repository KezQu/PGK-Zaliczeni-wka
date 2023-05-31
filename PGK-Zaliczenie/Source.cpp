#include <wx/wxprec.h>
#include "BasicFrame.h"

class MyApp : public wxApp
{
public:
	virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
	SetProcessDPIAware();
	BFrame* frame = new BFrame(NULL);
	frame->Show(true);
	return true;
}