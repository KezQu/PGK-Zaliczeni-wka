#include "MFrame.h"
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

int calculate(std::string s) {
	struct Op {
		char c;
		char precidence;
		int (*fct) (int oper1, int oper2);
	} ops[] = {
	  {'+', 1, [](int oper1, int oper2) -> int { return oper1 + oper2; }},
	  {'-', 1, [](int oper1, int oper2) -> int { return oper1 - oper2; }},
	  {'*', 2, [](int oper1, int oper2) -> int { return oper1 * oper2; }},
	  {'/', 2, [](int oper1, int oper2) -> int { return oper1 / oper2; }} };

	std::stack <Op*> operators;
	std::stack <int> operands;

	// Function that will process the top operator from the stack
	auto process_op = [](std::stack<Op*>& operators, std::stack<int>& operands) -> void {
		int operand2 = operands.top();
		operands.pop();
		int operand1 = operands.top();
		operands.pop();
		Op* op = operators.top();
		operators.pop();
		operands.push(op->fct(operand1, operand2));
	};

	const char* str = s.c_str();
	while (*str) {
		// Skip whitespace
		if (iswspace(*str)) { ++str; }
		// Reading operand
		else if (iswdigit(*str)) {
			char* end_pos;
			int operand = strtol(str, &end_pos, 10);
			str = end_pos;
			operands.push(operand);
		}
		else {
			// Find the corresponding operator
			for (auto& op : ops) {
				if (*str == op.c) {
					// Process any pending operators with a higher or equal precidence
					while (!operators.empty() && operators.top()->precidence >= op.precidence) {
						process_op(operators, operands);
					}
					operators.push(&op);
					++str;
					break;
				}
			}
		}
	}

	// Flush any remaining operators
	while (!operators.empty()) {
		process_op(operators, operands);
	}

	return operands.top();
}
void MFrame::submitButtonOnButtonClick(wxCommandEvent& event)
{
	_cachedData.resize(mapSize.first);

#pragma omp parallel for
	for (int x = 0; x < mapSize.first; x++)
	{
		_cachedData[x].resize(mapSize.second);
		for (int y = 0; y < mapSize.second; y++)
		{
			float _x = x / _grain;
			float _y = y / _grain;
			//float fVal = _x*_x + _y*_y;
			float fVal = std::cbrt(_x * _y / 80.) +  std::sin(_y * std::sqrt(_x ) / 80.);
			//float fVal = std::cbrt(_y * std::sqrt(_x ) / 100.) + std::sin(_x / (_y + 1));
			//float fVal = std::pow((_x - 50) , 2) + std::sin(_y) + _y;
			//float fVal = std::sin(_x ) + std::cos(_y );
			//float fVal = std::sin((_x  * _x + _y * _y ) / 10.);
			//float fVal = (_x / 10. * std::sin(2 * _y / 10.)) - 1.;
			//float fVal = (_x * (2 * _x  - 7) * (2 * _y + 1) + 2 * _y );
			if (fVal < minMax.first)
				minMax.first = fVal;
			if (fVal > minMax.second)
				minMax.second = fVal;

			_cachedData[x][y] = fVal;
		}
	}
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
//
#pragma omp parallel for
	for (int y = 0; y < mapSize.second-1; y++)
		for (int x = 0; x < mapSize.first-1; x++)
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
			if(normalisedVal < 0.5)
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
		//contourImg_cpy = contourImg_cpy.ConvertToMono(255,255,255); // zamiast odcieni szaro�ci poziomice s� czarne
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
	float stepX = _panelSize.first / static_cast<float>(mapSize.first / 5);
	for (int x = 0; x < mapSize.first / 5; x += _grain)
		bdc.DrawLine(x * stepX, 0, x * stepX, -5);
	float stepY = _panelSize.second / static_cast<float>(mapSize.second / 5);
	for (int y = 0; y < mapSize.second / 5; y += _grain)
		bdc.DrawLine(0, -y * stepY, 5, -y * stepY);
}

void MFrame::CalcAnimation(bool generated)
{
}

void MFrame::Animate()
{
}

bool any(char ch, std::vector<char> vec){
	for(auto &i: vec)
		if(ch==i) return true;
	return false;
}

float readOperator(float x, float y, char op){
	switch(op){
		case '+': return x+y;
		case '-': return x-y;
		case '/': return x/y;
		case '*': return x*y;
	}
}

float String2Fun(std::string input, int x, int y)
{	
	std::vector<char> possibleOperators{'+', '-', '/', '*'};
    std::map<char, int> ops
    {
        {'(', -1},
        {'+', 0},
        {'-', 0},
        {'/', 1},
        {'*', 1},
        {')', 10}
    };
	auto comp = ops.key_comp();
    std::vector<char> operators;
    std::vector<char> queue;
    for (auto &ch : input) {
		switch (ch) {
		case '+':
		case '-':
			while (operators.size() && ops[operators.back()] > -1) {
				queue.push_back(operators.back());
				operators.pop_back();
			}
			operators.push_back(ch);
            break;
		case '/':
		case '*':
			while (operators.size() && ops[operators.back()] > 0) {
				queue.push_back(operators.back());
				operators.pop_back();
			}
			operators.push_back(ch);
            break;
		case '(':
			operators.push_back(ch);
            break;
		case ')':
			while (ops[operators.back()] > -1) {
				queue.push_back(operators.back());
				operators.pop_back();
				if (operators.size() == 0 && (operators.size()+queue.size() +1 != input.size())) {
					//some exception handling (error window or smthng can pop up)
					std::cout<<"Parenthesis mismatch1\n";
					
				}
			}
			operators.pop_back();
            break;
		default:
            queue.push_back(ch);
        }
	}
    while (operators.size()>0) {
        queue.push_back(operators.back());
        operators.pop_back();
		
    }
	float val;
	std::vector<float> stack;
	for(const auto& it: queue){
		if(!any(it, possibleOperators)){
			if(it=='x'){
				stack.push_back(x);
			}else if(it=='y'){
				stack.push_back(y);
			}else{
				stack.push_back(it-48);//atoi does not work for some reason
			}
		}else{
			val=readOperator(stack[stack.size()-2], stack[stack.size()-1], it);
			stack.pop_back();
			stack.pop_back();
			stack.push_back(val);
		}
	}
	return stack[0];
}