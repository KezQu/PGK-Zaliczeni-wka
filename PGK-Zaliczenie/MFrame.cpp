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
	if (Tinsert->GetValue().ToStdString().length() == 0)
	{
		wxMessageDialog(NULL, "Please enter the t value!", "Warning", wxOK).ShowModal();
		return;
	}
	fun_formula = FuncInsert->GetValue().ToStdString().c_str();
	t_val = std::stof(Tinsert->GetValue().ToStdString());
	std::atomic<int> progress{ 0 };
	std::thread cachingData([&]() {
		//#pragma omp parallel for
		for (int i = 0; i < mapSize.first; i++)
		{
			_cachedData[i].resize(mapSize.second);
			for (int j = 0; j < mapSize.second; j++)
			{
				double x, y, t;
				te_variable vars[] = { {"x",&x}, {"y",&y}, {"t",&t} };
				int err;
				te_expr* expr = te_compile(fun_formula.c_str(), vars, 3, &err);
				t = t_val * _grain;
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
			progress++;
		}
	});
	loadingBar->Show();
	std::pair<int, int> tmpSBsize;
	submitButton->GetSize(&tmpSBsize.first, &tmpSBsize.second);
	loadingBar->SetPosition(wxPoint(submitButton->GetPosition().x, submitButton->GetPosition().y + tmpSBsize.second));
	while (progress < mapSize.first)
		loadingBar->SetValue(progress / static_cast<double>(mapSize.first) * 100);
	cachingData.join();
	loadingBar->Hide();
	
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
	if (!printable.IsOk())
	{
		wxLogError("Cannot save current bitmaps in file '%s'.", sfDialog.GetPath());
		return;
	}
	else
		printable.SaveFile(sfDialog.GetPath());
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

	wxBitmap contourBmp = wxBitmap(contourImg_org);
	wxMemoryDC mdc(contourBmp);
	
	mdc.SetDeviceOrigin(0, mapSize.second);

	mdc.SetPen(wxPen(*wxBLACK, .3));
	meshDraw(mdc);
	contourImg_org = mdc.GetAsBitmap().ConvertToImage();
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

	wxBitmap vectorBmp = wxBitmap(vectorImg_org);
	wxMemoryDC mdc(vectorBmp);
	
	mdc.SetDeviceOrigin(0, mapSize.second);

	wxPen pen1(*wxBLACK, .3);
	mdc.SetPen(pen1);
	meshDraw(mdc);
	
	for (int x = 2 * _grain; x < mapSize.first; x += 5 * _grain)
	{
		for (int y = 2 * _grain; y < mapSize.second; y += 5 * _grain)
		{

			wxPen pen(*wxBLACK, 2);
			mdc.SetPen(pen);
			long double h = 1e-5;
			long double _x, _y, _t;
			te_variable vars[] = { {"x",&_x}, {"y",&_y}, {"t",&_t} };
			int err{ 0 };
			_t = t_val * _grain;

			auto vecLength = [&](long double x, long double y){
				_x = x;
				_y = y;
				te_expr* expr = te_compile(fun_formula.c_str(), vars, 3, &err);
				long double fVal = te_eval(expr);
				te_free(expr);
				return fVal;
			};

			long double fVal_xh = vecLength(x + h, y);
			long double fVal_xh2 = vecLength(x - h, y);

			long double fVal_yh = vecLength(x, y + h);
			long double fVal_yh2 = vecLength(x, y - h);

			long double x_end = (fVal_xh - fVal_xh2) / (2. * h);
			long double y_end = (fVal_yh - fVal_yh2) / (2. * h);

			long double dziel = std::sqrt(std::pow(x_end, 2) + std::pow(y_end, 2)) / (2 * _grain);
			
			mdc.DrawLine(x, -y, x + x_end / dziel, -y - y_end / dziel);

			std::pair<wxAffineMatrix2D, wxAffineMatrix2D> Tmtx;

			Tmtx.first.Translate(x, -y);
			Tmtx.first.Rotate(M_PI / 12);
			
			Tmtx.second.Translate(x, -y);
			Tmtx.second.Rotate(-M_PI / 12);

			auto Lwing = Tmtx.first.TransformPoint(wxPoint2DDouble(x_end / (2 * dziel), -y_end / (2 * dziel)));
			auto Rwing = Tmtx.second.TransformPoint(wxPoint2DDouble(x_end / (2 * dziel), -y_end / (2 * dziel)));

			mdc.DrawLine(x, -y, Lwing.m_x, Lwing.m_y);
			mdc.DrawLine(x, -y, Rwing.m_x, Rwing.m_y);
		}
	}
	vectorImg_org = mdc.GetAsBitmap().ConvertToImage();
}

void MFrame::Repaint()
{
	wxClientDC cdc(wxWindow::GetWindowChild(bmpPanel->GetId()));
	wxBufferedDC bdc(&cdc);

	//bdc.SetBackground(wxColour(230, 219, 172));
	bdc.SetDeviceOrigin(0, _panelSize.second);
	bdc.Clear();

	if (_functionLoaded)
	{
		printable = wxImage(contourImg_org.GetSize().Scale(1, 2));
		printable.Paste(contourImg_org, 0, 0);
		printable.Paste(vectorImg_org, 0, contourImg_org.GetHeight());
		if (!printable.IsOk())
		{
			wxLogError("Failed merging bitmaps");
			return;
		}
	}
	if (_bmp == CONTOUR && _functionLoaded)
	{
		contourImg_cpy = contourImg_org.Copy();
		contourImg_cpy = contourImg_cpy.Rescale(_panelSize.first, _panelSize.second, wxIMAGE_QUALITY_HIGH);
		wxBitmap contourBmp(contourImg_cpy);
		bdc.DrawBitmap(contourBmp, 0, -_panelSize.second);
	}
	else if (_bmp == VECTOR && _functionLoaded)
	{
		vectorImg_cpy = vectorImg_org.Copy();
		vectorImg_cpy = vectorImg_cpy.Rescale(_panelSize.first, _panelSize.second, wxIMAGE_QUALITY_HIGH);
		wxBitmap vectorBmp(vectorImg_cpy);
		bdc.DrawBitmap(vectorBmp, 0, -_panelSize.second);
	}
}

void MFrame::CalcAnimation(bool generated)
{
}

void MFrame::Animate()
{
}

void MFrame::meshDraw(wxMemoryDC & dc)
{
	for (int x = 0; x < mapSize.first; x += _grain * 10)
		dc.DrawLine(x, 0, x, -mapSize.second);
	for (int y = 0; y < mapSize.second; y += _grain * 10)
		dc.DrawLine(0, -y, mapSize.first, -y);

	dc.DrawCircle(mapSize.first / 2., mapSize.second / 2., 100);
};