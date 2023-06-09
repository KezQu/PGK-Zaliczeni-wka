///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/panel.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/gauge.h>
#include <wx/tglbtn.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class BFrame
///////////////////////////////////////////////////////////////////////////////
class BFrame : public wxFrame
{
	private:

	protected:
		wxPanel* mainPanel;
		wxPanel* bmpPanel;
		wxStaticText* m_staticText1;
		wxTextCtrl* FuncInsert;
		wxStaticText* m_staticText2;
		wxTextCtrl* Tinsert;
		wxButton* submitButton;
		wxGauge* loadingBar;
		wxButton* contourButton;
		wxButton* vectorButton;
		wxButton* saveButton;

		// Virtual event handlers, override them in your derived class
		virtual void mainPanelOnUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void bmpPanelOnSize( wxSizeEvent& event ) { event.Skip(); }
		virtual void submitButtonOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void contourButtonOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void vectorButtonOnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void saveButtonOnButtonClick( wxCommandEvent& event ) { event.Skip(); }


	public:

		BFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Acceleration field"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,600 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~BFrame();

};

