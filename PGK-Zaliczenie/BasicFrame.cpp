///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "BasicFrame.h"

///////////////////////////////////////////////////////////////////////////

BFrame::BFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );

	mainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer51;
	bSizer51 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );

	bmpPanel = new wxPanel( mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	bmpPanel->SetMinSize( wxSize( 10,10 ) );

	bSizer6->Add( bmpPanel, 1, wxEXPAND | wxALL, 0 );


	bSizer51->Add( bSizer6, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );

	bSizer5->SetMinSize( wxSize( 198,-1 ) );
	m_staticText1 = new wxStaticText( mainPanel, wxID_ANY, wxT("Insert function"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	m_staticText1->SetFont( wxFont( 14, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Cascadia Code") ) );

	bSizer5->Add( m_staticText1, 0, wxALIGN_CENTER|wxALL, 5 );

	FuncInsert = new wxTextCtrl( mainPanel, wxID_ANY, wxT("x^2+y^2"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( FuncInsert, 0, wxALL|wxEXPAND, 5 );

	m_staticText2 = new wxStaticText( mainPanel, wxID_ANY, wxT("Pass value for t"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	m_staticText2->SetFont( wxFont( 14, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Cascadia Code") ) );

	bSizer5->Add( m_staticText2, 0, wxALIGN_CENTER|wxALL, 5 );

	Tinsert = new wxTextCtrl( mainPanel, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( Tinsert, 0, wxALL|wxEXPAND, 5 );

	submitButton = new wxButton( mainPanel, wxID_ANY, wxT("Submit"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( submitButton, 0, wxALL|wxEXPAND, 5 );

	loadingBar = new wxGauge( mainPanel, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	loadingBar->SetValue( 0 );
	loadingBar->Hide();

	bSizer5->Add( loadingBar, 0, 0, 5 );


	bSizer5->Add( 0, 0, 1, 0, 5 );

	contourButton = new wxButton( mainPanel, wxID_ANY, wxT("Show Contour Map"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( contourButton, 0, wxALL|wxEXPAND, 5 );

	vectorButton = new wxButton( mainPanel, wxID_ANY, wxT("Show Vector Field"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( vectorButton, 0, wxALL|wxEXPAND, 5 );


	bSizer5->Add( 0, 0, 1, wxEXPAND, 5 );

	saveButton = new wxButton( mainPanel, wxID_ANY, wxT("Save to file"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( saveButton, 0, wxALL|wxEXPAND, 5 );


	bSizer51->Add( bSizer5, 0, wxEXPAND, 0 );


	mainPanel->SetSizer( bSizer51 );
	mainPanel->Layout();
	bSizer51->Fit( mainPanel );
	bSizer1->Add( mainPanel, 1, wxALL|wxEXPAND, 0 );


	this->SetSizer( bSizer1 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	mainPanel->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( BFrame::mainPanelOnUpdateUI ), NULL, this );
	bmpPanel->Connect( wxEVT_SIZE, wxSizeEventHandler( BFrame::bmpPanelOnSize ), NULL, this );
	submitButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BFrame::submitButtonOnButtonClick ), NULL, this );
	contourButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BFrame::contourButtonOnButtonClick ), NULL, this );
	vectorButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BFrame::vectorButtonOnButtonClick ), NULL, this );
	saveButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BFrame::saveButtonOnButtonClick ), NULL, this );
}

BFrame::~BFrame()
{
	// Disconnect Events
	mainPanel->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( BFrame::mainPanelOnUpdateUI ), NULL, this );
	bmpPanel->Disconnect( wxEVT_SIZE, wxSizeEventHandler( BFrame::bmpPanelOnSize ), NULL, this );
	submitButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BFrame::submitButtonOnButtonClick ), NULL, this );
	contourButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BFrame::contourButtonOnButtonClick ), NULL, this );
	vectorButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BFrame::vectorButtonOnButtonClick ), NULL, this );
	saveButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BFrame::saveButtonOnButtonClick ), NULL, this );
}
