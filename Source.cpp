#include "include/AllIncludes.h"
#include <stdlib.h>



ManagerUI::ManagerUI(RenderWindow* wind)
{
	window = wind;
}

void ManagerUI::Draw()
{
	if (!window)
	{
		std::cout << "ManagerUI::Draw; Unvalid window ref!" << '\n';
		return;
	}
	for (int i = 0; i < WidgetsArr.size(); i++)
		if(WidgetsArr[i]->Visible)
			WidgetsArr[i]->DrawWidget(window);
}

void ManagerUI::AddWidget(WidgetBase* Widget)
{
	WidgetsArr.push_back(Widget);
}

void ManagerUI::SendEvent(Event& ev, int ShiftX)
{
	event = ev;

	if (event.type == Event::MouseButtonPressed)
		if (event.mouseButton.button == Mouse::Left)
		{
			Vector2i MouseLoc(event.mouseButton.x+ShiftX, event.mouseButton.y);
			for (int i = 0; i < WidgetsArr.size(); i++)
			{
				if (!WidgetsArr[i]->Visible)
					continue;
				WidgetsArr[i]->MouseInAreab = WidgetsArr[i]->IsMouseInArea(MouseLoc);
				if (WidgetsArr[i]->MouseInAreab)
				{
					WidgetsArr[i]->OnPressed();
					ev.type = Event::EventType::JoystickConnected;
				}
					
			}
				
		}

	if (event.type == Event::MouseButtonReleased)
		if (event.mouseButton.button == Mouse::Left)
		{
			Vector2i MouseLoc(event.mouseButton.x+ShiftX, event.mouseButton.y);
			for (int i = 0; i < WidgetsArr.size(); i++)
			{
				if (!WidgetsArr[i]->Visible)
					continue;
				if (WidgetsArr[i]->MouseInAreab && WidgetsArr[i]->IsMouseInArea(MouseLoc))
				{
					WidgetsArr[i]->OnClickBase();
					WidgetsArr[i]->OnUnpressed();
				}
				if (WidgetsArr[i]->MouseInAreab && !WidgetsArr[i]->IsMouseInArea(MouseLoc))
					WidgetsArr[i]->OnUnpressed();
				WidgetsArr[i]->MouseInAreab = false;
			}
		}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool Math::InRectArea(Vector2i Pos, Vector2f Loc, Vector2f Size)
{
	if (Pos.x >= Loc.x && Pos.x <= Loc.x + Size.x)
		if (Pos.y >= Loc.y && Pos.y <= Loc.y + Size.y)
			return true;
	return false;
}

Vector3f Math::rgbTohsv(Vector3i rgb)
{
	Vector3f out((float)rgb.x / 255, (float)rgb.y/ 255, (float)rgb.z / 255);
	//std::cout <<  << '\n';
	float maxV = std::max(out.x, out.y);
	maxV= std::max(maxV, out.z);
	float minV = std::min(out.x, out.y);
	minV = std::min(minV, out.z);  
	float delta = maxV - minV;
	if (delta == 0)
		out.x = 0;
	else if (maxV == out.x)
		out.x =60*((int)((out.y - out.z) / delta) % 6);
	else if (maxV == out.y)
		out.x = 60 * (((out.z - out.x) / delta) +2);
	else if (maxV == out.z)
		out.x = 60 * (((out.x - out.y) / delta) + 4);
	if (maxV == 0)
		out.y = 0;
	else
		out.y = (int)(100*delta / maxV);
	out.z = (int)(maxV*100);
	out.x = (int)out.x;
	return out;
}

Vector3i Math::hsvTorgb(Vector3f hsv)
{
	float s = hsv.y / 100;
	float v = hsv.z / 100;
	float C = s * v;
	float X = C * (1 - abs(fmod(hsv.x / 60.0, 2) - 1));
	float m = v - C;
	float r, g, b;
	if (hsv.x >= 0 && hsv.x < 60) {
		r = C, g = X, b = 0;
	}
	else if (hsv.x >= 60 && hsv.x < 120) {
		r = X, g = C, b = 0;
	}
	else if (hsv.x >= 120 && hsv.x < 180) {
		r = 0, g = C, b = X;
	}
	else if (hsv.x >= 180 && hsv.x < 240) {
		r = 0, g = X, b = C;
	}
	else if (hsv.x >= 240 && hsv.x < 300) {
		r = X, g = 0, b = C;
	}
	else {
		r = C, g = 0, b = X;
	}
	return Vector3i((r + m) * 255, (g + m) * 255, (b + m) * 255);
}

Vector3i Math::EditInHSV(Vector3i rgb, Vector3i hsvChange)
{
	Vector3f hsv = Math::rgbTohsv(rgb);
	hsv = Vector3f(hsv.x += hsvChange.x, hsv.y += hsvChange.y, hsv.z += hsvChange.z);
	hsv.x = std::min(std::max(0, (int)hsv.x), 360);
	hsv.y = std::min(std::max(0, (int)hsv.y), 100);
	hsv.z = std::min(std::max(0, (int)hsv.z), 100);
	return Math::hsvTorgb(hsv);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


WidgetBase::WidgetBase(sf::Vector2f Pos, Vector2f Size, Color Col, Text txt1):Loc(Pos), Size(Size), Col(Col),
txt(txt1)
{
	txt.setOrigin(txt.getLocalBounds().left, txt.getLocalBounds().top);
	Vector2f txtshift((Size.x-txt.getLocalBounds().width)/2, (Size.y - txt.getLocalBounds().height) / 2);
	txt.setPosition(Vector2f(Loc.x + txtshift.x, Loc.y + txtshift.y));
}

void WidgetBase::DrawWidget(sf::RenderWindow* window)
{}

void WidgetBase::SetLocation(sf::Vector2f Pos)
{}

bool WidgetBase::IsMouseInArea(sf::Vector2i pos)
{return false;}

void WidgetBase::OnClick()
{}

void WidgetBase::OnPressed()
{}

void WidgetBase::OnUnpressed()
{}


void WidgetBase::SetOnClick(void (*Receive)(WidgetBase*))
{
	ReceiveClick = Receive;
}

void WidgetBase::OnClickBase()
{
	OnClick();
	if (ReceiveClick)
		(*ReceiveClick)(this);
}

void WidgetBase::SetLocationBase(sf::Vector2f Pos)
{
	Loc = Pos;
	Vector2f txtshift((Size.x - txt.getLocalBounds().width) / 2, (Size.y - txt.getLocalBounds().height) / 2);
	txt.setPosition(Vector2f(Loc.x + txtshift.x, Loc.y + txtshift.y));
	this->SetLocation(Loc);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


Button::Button() {}
Button::Button(Vector2f Pos, Vector2f Size, Color Col, Text txt1):WidgetBase(Pos, Size, Col, txt1)
{
	ButtonShape.setPosition(Pos);
	ButtonShape.setSize(Size);
	ButtonShape.setFillColor(Col);
}

void Button::DrawWidget(RenderWindow* window)
{
	ButtonShape.setFillColor(Col);
	window->draw(ButtonShape);
	window->draw(txt);
}

void Button::SetLocation(sf::Vector2f Pos)
{
	ButtonShape.setPosition(Pos);
}

bool Button::IsMouseInArea(sf::Vector2i pos)
{
	return Math::InRectArea(pos, Loc, ButtonShape.getSize());
}

void Button::OnPressed()
{
	Vector3i rgb(Col.r, Col.g, Col.b);
	rgb = Math::EditInHSV(rgb, Vector3i(0, 0, -20));
	Col = Color(rgb.x, rgb.y, rgb.z);
}

void Button::OnUnpressed()
{
	Vector3i rgb(Col.r, Col.g, Col.b);
	rgb = Math::EditInHSV(rgb, Vector3i(0, 0, 20));
	Col = Color(rgb.x, rgb.y, rgb.z);
}
