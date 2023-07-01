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

/**
 * \mainpage PGK Projekt zaliczeniowy
 * <h1> Temat projektu: Pole przyszpieszeń </h1>
 * <h1> O Projekcie </h1>
 * <h2> Użyte narzędzia </h2>
 *  W projekcie zostały użyte biblioteki: <a href="https://www.wxwidgets.org"> wxWidgets </a> \n
 *  oraz <a href=https://github.com/codeplea/tinyexpr> tinyexpr </a> \n 
 * Dokumentacja została sporządzona przy użyciu narzędzia <a href="https://www.doxygen.nl/"> Doxygen </a> \n
 * Projekt został napisany przy użyciu Visual Studio 2022. \n
 * Użyty standard języka to C++17 \n
 * <h2> Dokumentacja </h2>
 * Dokumentacja kodu źródłowego jest sporządzona w pełni w języku angielskim ze względu na prostotę w posługiwaniu się określeniami technicznymi.
 * <h1> Twórcy </h1>
 * <a href=https://github.com/KezQu> Jakub Mikusek </a> \n
 * <a href=https://github.com/kastre3381> Kacper Tracz </a> \n
 * <a href=https://github.com/Amnesyy> Marcelin Raczek </a> \n
 * <h1> Repozytorium </h1>
 * <a href=https://github.com/KezQu/PGK-Zaliczeni-wka> Mało profesjonalne ;D </a>
 */



/**
 * @brief Class inheriting from wxBuilder's generated frame
 * 
 */
class MFrame : public BFrame
{
private:
	/**
	 * @brief 2D array containing values of function at given point
	 * 
	 */
	std::vector<std::vector<float>> _cachedData;
	/**
	 * @brief Pair with smallest and biggest function value 
	 * 
	 */
	std::pair<float, float> minMax = { FLT_MAX,FLT_MIN };
	/**
	 * @brief Size of the map with hardcoded value of 1000x1000
	 * 
	 */
	std::pair<int, int> mapSize = { 2000,2000 };
	/**
	 * @brief Size of the panel
	 * 
	 */
	std::pair<int, int> _panelSize;
	/**
	 * @brief wxImage containing the original countour image
	 * 
	 */
	wxImage contourImg_org;
	/**
	 * @brief wxImage containing the copy of countour image
	 * 
	 */
	wxImage contourImg_cpy;
	/**
	 * @brief wxImage containing the original vectors image
	 * 
	 */
	wxImage vectorImg_org;
	/**
	 * @brief wxImage containing the copy of vectors image
	 * 
	 */
	wxImage vectorImg_cpy;
	/**
	 * @brief wxImage containing image in reade to print state
	 * 
	 */
	wxImage printable;
	/**
	 * @brief User's input representing t, coresponding to vector's size
	 * 
	 */
	int t_val;
	/**
	 * @brief Inserted function, x^2+y^2 by default
	 * 
	 */
	std::string fun_formula = "x^2+y^2";
	/**
	 * @brief Countour's image pixel by pixel
	 * 
	 */
	byte* _contourData;
	/**
	 * @brief Vector's image pixel by pixel
	 * 
	 */
	byte* _vectorData;
	/**
	 * @brief Type of image being drawn in the moment
	 * 
	 */
	enum {
		NONE = 0,
		CONTOUR = 1,
		VECTOR = 2
	} _bmp = NONE;
	/**
	 * @brief Frequency of vectors being drawn
	 * 
	 */
	float _grain = mapSize.first > mapSize.second ? mapSize.second / 100. : mapSize.first / 100.;
	/**
	 * @brief Flag checking if function have been inserted
	 * 
	 */
	bool _functionLoaded = false;
public:
	/**
	 * @brief Construct a new MFrame object
	 * 
	 * @param parent 
	 */
	MFrame(wxWindow* parent);
	/**
	 * @brief Destroy the MFrame object
	 * 
	 */
	~MFrame();

	//void mainPanelOnUpdateUI(wxUpdateUIEvent& event) override;
	/**
	 * @brief Handling of all size related events including resize
	 * 
	 * @param event 
	 */
	void bmpPanelOnSize(wxSizeEvent& event) override;
	
	/**
	 * @brief Handling the insertion of new function in text box
	 * 
	 * @param event 
	 */
	void submitButtonOnButtonClick(wxCommandEvent& event) override;
	/**
	 * @brief Drawing the countour map
	 * 
	 * @param event 
	 */
	void contourButtonOnButtonClick(wxCommandEvent& event) override;
	/**
	 * @brief Drawing the vector map
	 * 
	 * @param event 
	 */
	void vectorButtonOnButtonClick(wxCommandEvent& event) override;
	
	/**
	 * @brief Handling the click of save button
	 * 
	 * @param event 
	 */
	void saveButtonOnButtonClick(wxCommandEvent& event) override;

	/**
	 * @brief Setup for contour map, calculating color values
	 * 
	 */
	void CalculateContour();
	/**
	 * @brief Setup for vector map, calculating their size
	 * 
	 */
	void CalculateVector();
	/**
	 * @brief Drawing hole window 
	 * 
	 */
	void Repaint();
	/**
	 * @brief Setup for animation, 
	 * 
	 * @param generated 
	 */
	void CalcAnimation(bool generated);
	/**
	 * @brief Animating the vectors
	 * 
	 */
	void Animate();
	/**
	 * @brief Drawing of back mesh
	 * 
	 * @param dc 
	 */
	void meshDraw(wxMemoryDC& dc);
	/**
	 * @brief Saving the image to file
	 * 
	 */
	void SaveToFile();
};

