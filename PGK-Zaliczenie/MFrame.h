#pragma once
#include "BasicFrame.h"
#include <wx/dcbuffer.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h> 
#include <wx/wfstream.h>
#include <wx/log.h> 

#include <cmath>
#include <limits>
#include <memory>
#include <functional>
#include <cstring>
#include <fstream>
#include <filesystem>

class MFrame : public BFrame
{
private:
	std::vector<std::vector<float>> _cachedData;
	std::pair<float, float> minMax = { FLT_MAX,FLT_MIN };
	std::pair<int, int> mapSize = { 5000,5000 };
	std::pair<int, int> _panelSize;
	wxImage contourImg_org;
	wxImage contourImg_cpy;
	wxImage vectorImg_org;
	wxImage vectorImg_cpy;
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
	void SaveToFile();
	void CalcAnimation(bool generated);
	void Animate();
};

