#pragma once

#include "BasicFrame.h"

#include <wx/dcbuffer.h>
#include <wx/dcmemory.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h> 
#include <wx/wfstream.h>
#include <wx/log.h> 
#include <wx/affinematrix2d.h>

#include <cmath>
#include <limits>
#include <filesystem>
#include <atomic>
#include <thread>

class MFrame : public BFrame
{
private:
	std::vector<std::vector<float>> _cachedData;
	std::pair<float, float> minMax = { FLT_MAX,FLT_MIN };
	std::pair<int, int> mapSize = { 2000,2000 };
	std::pair<int, int> _panelSize;
	wxImage contourImg_org;
	wxImage contourImg_cpy;
	wxImage vectorImg_org;
	wxImage vectorImg_cpy;
	wxImage printable;
	int t_val;
	std::string fun_formula;
	byte* _contourData;
	byte* _vectorData;
	enum {
		NONE = 0,
		CONTOUR = 1,
		VECTOR = 2
	} _bmp = NONE;
	float _grain = mapSize.first > mapSize.second ? mapSize.second / 100. : mapSize.first / 100.;
	bool _functionLoaded = false;
public:
	MFrame(wxWindow* parent);
	~MFrame();

	//void mainPanelOnUpdateUI(wxUpdateUIEvent& event) override;
	void bmpPanelOnSize(wxSizeEvent& event) override;
	
	void submitButtonOnButtonClick(wxCommandEvent& event) override;
	void contourButtonOnButtonClick(wxCommandEvent& event) override;
	void vectorButtonOnButtonClick(wxCommandEvent& event) override;
	
	void saveButtonOnButtonClick(wxCommandEvent& event) override;
	void animateButtonOnToggleButton(wxCommandEvent& event) override;

	void CalculateContour();
	void CalculateVector();
	void Repaint();
	void CalcAnimation(bool generated);
	void Animate();
	void meshDraw(wxMemoryDC& dc);
};

