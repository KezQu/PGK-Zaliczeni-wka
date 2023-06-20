#include "MFrame.h"
#include "tinyexpr.h"
#define CONTOURCOUNT 15

MFrame::MFrame(wxWindow* parent)
	:BFrame(parent)
{
	_contourData = static_cast<byte*>(std::calloc(sizeof(byte), mapSize.first * mapSize.second * 3));
	std::memset(_contourData, 150, mapSize.first * mapSize.second * 3 * sizeof(byte));
	_vectorData = static_cast<byte*>(std::calloc(sizeof(byte), mapSize.first * mapSize.second * 3));
	std::memset(_vectorData, 150, mapSize.first * mapSize.second * 3 * sizeof(byte));

	wxImage::AddHandler(new wxBMPHandler);
	wxImage::AddHandler(new wxPNGHandler);
}

MFrame::~MFrame()
{
	free(_contourData);
	free(_vectorData);
}

//void MFrame::mainPanelOnUpdateUI(wxUpdateUIEvent& event)
//{
//	Generate();
//}

void MFrame::bmpPanelOnSize(wxSizeEvent& event)
{
	bmpPanel->GetSize(&_panelSize.first, &_panelSize.second);
	_panelSize.first = _panelSize.first > 0 ? _panelSize.first : 1;
	_panelSize.second = _panelSize.second > 0 ? _panelSize.second : 1;
	Repaint();
}

void MFrame::submitButtonOnButtonClick(wxCommandEvent& event)
{
	minMax = { FLT_MAX,FLT_MIN };
	_cachedData.resize(mapSize.first);
	contourImg_org.Destroy();
	contourImg_cpy.Destroy();
	vectorImg_org.Destroy();
	vectorImg_cpy.Destroy();
	if (FuncInsert->GetValue().ToStdString().length() == 0)
	{
		wxMessageDialog(NULL, "Please enter the function formula!", "Warning", wxOK).ShowModal();
		return;
	}
	fun_formula = FuncInsert->GetValue().ToStdString().c_str();


	if (Tinsert->GetValue().ToStdString().length() == 0)
	{
		wxMessageDialog(NULL, "Please enter the t value!", "Warning", wxOK).ShowModal();
		return;
	}

	t_val = std::stof(Tinsert->GetValue().ToStdString());

	//#pragma omp parallel for
	for (int i = 0; i < mapSize.first; i++)
	{
		_cachedData[i].resize(mapSize.second);
		for (int j = 0; j < mapSize.second; j++)
		{
			double x, y, t;
			te_variable vars[] = { {"x",&x}, {"y",&y}, {"t",&t} };
			int err;
			t = t_val;
			te_expr* expr = te_compile(fun_formula.c_str(), vars, 3, &err);
			x = i / _grain;
			y = j / _grain;
			float fVal = te_eval(expr);
			if (fVal < minMax.first)
				minMax.first = fVal;
			if (fVal > minMax.second)
				minMax.second = fVal;
			_cachedData[i][j] = fVal;
			te_free(expr);
		}
	}

	fun_formula.clear();
	CalculateContour();
	CalculateVector();
	_functionLoaded = true;
}

void MFrame::contourButtonOnButtonClick(wxCommandEvent& event)
{
	_bmp = CONTOUR;
	Repaint();
}

void MFrame::vectorButtonOnButtonClick(wxCommandEvent& event)
{
	_bmp = VECTOR;
	Repaint();
}


void MFrame::saveButtonOnButtonClick(wxCommandEvent& event)
{
	std::string cpath = std::filesystem::current_path().generic_string();

	wxFileDialog sfDialog(this, _("Save BMP file"), _(cpath), _(""), _("BMP files (*.bmp)|*.bmp|PNG files (*.png)|*.png"), wxFD_SAVE);
	if (sfDialog.ShowModal() == wxID_CANCEL)
		return;
	//wxFileOutputStream output_stream(saveFileDialog.GetPath());
	if (!contourImg_org.IsOk() || !vectorImg_org.IsOk())
	{
		wxLogError("Cannot save current bitmaps in file '%s'.", sfDialog.GetPath());
		return;
	}
	else
	{
		wxImage mergedBmps(contourImg_org.GetSize().Scale(1, 2));
		mergedBmps.Paste(contourImg_org, 0, 0);
		mergedBmps.Paste(vectorImg_org, 0, contourImg_org.GetHeight());
		if (!mergedBmps.IsOk())
		{
			wxLogError("Failed merging bitmaps '%s'.", sfDialog.GetPath());
			return;
		}
		mergedBmps.SaveFile(sfDialog.GetPath());
	}
}

void MFrame::animateButtonOnToggleButton(wxCommandEvent& event)
{
}



void MFrame::CalculateContour()
{
	double contourHeight = (minMax.second - minMax.first) / static_cast<double>(CONTOURCOUNT);
#pragma omp parallel for
	for (int y = 0; y < mapSize.second - 1; y++)
		for (int x = 0; x < mapSize.first - 1; x++)
		{
			if (static_cast<int>(_cachedData[x + 1][y + 1] / contourHeight) == static_cast<int>(_cachedData[x + 1][y] / contourHeight)
				&& static_cast<int>(_cachedData[x + 1][y + 1] / contourHeight) == static_cast<int>(_cachedData[x][y + 1] / contourHeight)
				&& static_cast<int>(_cachedData[x][y] / contourHeight) == static_cast<int>(_cachedData[x + 1][y] / contourHeight)
				&& static_cast<int>(_cachedData[x][y] / contourHeight) == static_cast<int>(_cachedData[x][y + 1] / contourHeight))
				for (int i = 0; i < 3; i++)
					_contourData[(x + mapSize.first * y) * 3 + i] = 255;
			else
				for (int i = 0; i < 3; i++)
					_contourData[(x + mapSize.first * y) * 3 + i] = 0;
		}

	contourImg_org.Create(mapSize.first, mapSize.second, _contourData, true);
	contourImg_org = contourImg_org.Mirror(false);
}



void MFrame::CalculateVector()
{
	auto normalize = [&](float val) { return (val - minMax.first) / (minMax.second - minMax.first); };
#pragma omp parallel for
	for (int y = 0; y < mapSize.second - 1; y++)
		for (int x = 0; x < mapSize.first - 1; x++)
		{
			float normalisedVal = normalize(_cachedData[x][y]);
			if (normalisedVal < 0.5)
			{
				_vectorData[(x + mapSize.first * y) * 3 + 0] = 255 * 0;
				_vectorData[(x + mapSize.first * y) * 3 + 1] = 255 * 2 * normalisedVal;
				_vectorData[(x + mapSize.first * y) * 3 + 2] = 255 * (1 - 2 * normalisedVal);
			}
			else
			{
				_vectorData[(x + mapSize.first * y) * 3 + 0] = 255 * 0.5 * normalisedVal;
				_vectorData[(x + mapSize.first * y) * 3 + 1] = 255 * 2 * (1 - normalisedVal);
				_vectorData[(x + mapSize.first * y) * 3 + 2] = 255 * 0;
			}
		}




	vectorImg_org.Create(mapSize.first, mapSize.second, _vectorData, true);
	vectorImg_org = vectorImg_org.Mirror(false);
}



void MFrame::Repaint()
{
	wxClientDC cdc(wxWindow::GetWindowChild(bmpPanel->GetId()));
	wxBufferedDC bdc(&cdc);

	//bdc.SetBackground(wxColour(230, 219, 172));
	bdc.SetDeviceOrigin(0, _panelSize.second);
	bdc.Clear();

	if (_bmp == CONTOUR && _functionLoaded)
	{
		
		contourImg_cpy = contourImg_org.Copy();
		contourImg_cpy = contourImg_cpy.Rescale(_panelSize.first, _panelSize.second, wxIMAGE_QUALITY_HIGH);
		//contourImg_cpy = contourImg_cpy.ConvertToMono(255,255,255); // zamiast odcieni szarości poziomice są czarne
		wxBitmap contourBmp(contourImg_cpy);
		bdc.DrawBitmap(contourBmp, 0, -_panelSize.second);

		wxPen pen1(wxColor(156, 156, 156), .3);
		bdc.SetPen(pen1);

		float stepX = _panelSize.first / static_cast<float>(mapSize.first / 5);
		for (int x = 0; x < mapSize.first; x += _grain)
			bdc.DrawLine(x * stepX, 0, x * stepX, -mapSize.second);
		float stepY = _panelSize.second / static_cast<float>(mapSize.second / 5);
		for (int y = 0; y < mapSize.second; y += _grain)
			bdc.DrawLine(0, -y * stepY, mapSize.first, -y * stepY);

	}
	else if (_bmp == VECTOR && _functionLoaded)
	{
		vectorImg_cpy = vectorImg_org.Copy();
		vectorImg_cpy = vectorImg_cpy.Rescale(_panelSize.first, _panelSize.second, wxIMAGE_QUALITY_HIGH);
		wxBitmap vectorBmp(vectorImg_cpy);
		bdc.DrawBitmap(vectorBmp, 0, -_panelSize.second);
		if (FuncInsert->GetValue().ToStdString().length() == 0)
		{
			wxMessageDialog(NULL, "Please enter the function formula!", "Warning", wxOK).ShowModal();
			return;
		}
		fun_formula = FuncInsert->GetValue().ToStdString().c_str();

		if (Tinsert->GetValue().ToStdString().length() == 0)
		{
			wxMessageDialog(NULL, "Please enter the t value!", "Warning", wxOK).ShowModal();
			return;
		}
		t_val = std::stof(Tinsert->GetValue().ToStdString());

		wxPen pen1(*wxBLACK, .3);
		bdc.SetPen(pen1);

		float stepX = _panelSize.first / static_cast<float>(mapSize.first / 5);
		for (int x = 0; x < mapSize.first; x += _grain)
			bdc.DrawLine(x * stepX, 0, x * stepX, -mapSize.second);
		float stepY = _panelSize.second / static_cast<float>(mapSize.second / 5);
		for (int y = 0; y < mapSize.second; y += _grain)
			bdc.DrawLine(0, -y * stepY, mapSize.first, -y * stepY);

		for (int x = 25; x < GetClientSize().x - 220; x += 40)
		{
			for (int y = 25; y < GetClientSize().y - 30; y += 40)
			{	
				wxPen pen(*wxWHITE, 3);
				bdc.SetPen(pen);
				double h = 10.e-5;
				double _x, _y, _t;
				te_variable vars[] = { {"x",&_x}, {"y",&_y}, {"t",&_t} };
				int err;
				_t = t_val;

				err = 0;
				_x = (double)x + h;
				_y = (double)y;
				_t = t_val;
				te_expr* expr = te_compile(fun_formula.c_str(), vars, 3, &err);
				float fVal_xh = te_eval(expr);
				te_free(expr);

				err = 0;
				_x = (double)x - h;
				_y = (double)y;
				_t = t_val;
				expr = te_compile(fun_formula.c_str(), vars, 3, &err);
				float fVal_xh2 = te_eval(expr);
				te_free(expr);

				err = 0;
				_x = (double)x;
				_y = (double)y + h;
				_t = t_val;
				expr = te_compile(fun_formula.c_str(), vars, 3, &err);
				float fVal_yh = te_eval(expr);
				te_free(expr);

				err = 0;
				_x = (double)x;
				_y = (double)y - h;
				_t = t_val;
				expr = te_compile(fun_formula.c_str(), vars, 3, &err);
				float fVal_yh2 = te_eval(expr);
				te_free(expr);

				double x_end = (fVal_xh - fVal_xh2) / (2. * h);
				double y_end = (fVal_yh - fVal_yh2) / (2. * h);

				float dziel = std::sqrt(std::pow(x_end, 2) + std::pow(y_end, 2)) / 20.;

				bdc.DrawLine(x, -y, x + x_end / dziel, -y - y_end / dziel);

				bdc.DrawCircle(x + x_end / dziel, -y - y_end / dziel, 3);

				
				
				
				double x_to_rotate = x + x_end / dziel * 0.7;
				double y_to_rotate = - y - y_end / dziel * 0.7;

				double x_to_rotate_around = x + x_end / dziel;
				double y_to_rotate_around = -y - y_end / dziel;

				double x_prime1 = x_to_rotate_around * std::cos(M_PI / 6.) - y_to_rotate_around * std::sin(M_PI / 6.);
				double y_prime1 = y_to_rotate_around * std::cos(M_PI / 6.) + x_to_rotate_around * std::sin(M_PI / 6.);

				double x_prime2 = x_to_rotate_around * std::cos(-3.1415926535897932384626433832795 / 6.) - y_to_rotate_around * std::sin(-M_PI / 6.);
				double y_prime2 = y_to_rotate_around * std::cos(-M_PI / 6.) + x_to_rotate_around * std::sin(-M_PI / 6.);

				x_prime1 += x_to_rotate_around;
				x_prime2 += x_to_rotate_around;
				y_prime1 -= y_to_rotate_around;
				y_prime2 -= y_to_rotate_around;

				double dziel1 = std::sqrt(std::pow(x_prime1 - (x + x_end / dziel), 2) + std::pow(y_prime1 - (y + y_end / dziel), 2));

				//bdc.DrawLine(x + x_end / dziel, -y - y_end / dziel, x_prime1, -y_prime1);
				//bdc.DrawLine(x + x_end / dziel, -y - y_end / dziel, x_prime2, -y_prime2);

			}
		}
	}
	


}

void MFrame::SaveToFile()
{
}

void MFrame::CalcAnimation(bool generated)
{
}

void MFrame::Animate()
{
}
