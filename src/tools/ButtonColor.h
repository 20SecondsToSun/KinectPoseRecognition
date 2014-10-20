#pragma once

#include "Button.h"

using namespace std;

class ButtonColor: public Button
{
	public:

		ButtonColor(ci::Rectf rect, ci::Color color);
		ButtonColor(ci::Rectf rect, ci::Color color, ci::Font* _font, std::string _label);	

		void		draw();

	private:
		ci::Color		maincolor;
};