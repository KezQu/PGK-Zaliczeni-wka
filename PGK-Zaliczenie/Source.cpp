#include <wx/wxprec.h>
#include "MFrame.h"

class MyApp : public wxApp
{
public:
	virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
	SetProcessDPIAware();
	MFrame* frame = new MFrame(NULL);
	frame->Show(true);
	return true;
}