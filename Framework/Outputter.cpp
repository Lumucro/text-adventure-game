#include "Outputter.h"
#include "windows.h"

Outputter* Outputter::m_Instance = nullptr;

//Singleton pattern
Outputter* Outputter::GetInstance()
{
	if (!m_Instance)
		m_Instance = new Outputter();

	return m_Instance;
}

void Outputter::Destroy()
{
	if (m_Instance)
		delete m_Instance;
}

Outputter::Outputter()
{
}

Outputter::~Outputter()
{
}

void Outputter::Log(string text)
{
	//Set the color
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	//Print the text
	cout << text;
	//Reset the colors
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_INTENSITY);
}

void Outputter::Log(string text, OUTPUT_COLOR col)
{
	//Set the colors
	switch (col)
	{
	case OUTPUT_COLOR::RED:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		break;
	case OUTPUT_COLOR::GREEN:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		break;
	case OUTPUT_COLOR::BLUE:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		break;
	case OUTPUT_COLOR::YELLOW:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
		break;
	case OUTPUT_COLOR::PINK:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
		break;
	case OUTPUT_COLOR::WHITE:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;
	}

	//Print the text
	cout << text;
	//Reset the colors
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_INTENSITY);
}
