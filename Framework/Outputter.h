#pragma once
#include "stdafx.h"

using namespace std;

enum class OUTPUT_COLOR
{
	RED, GREEN, BLUE, YELLOW, PINK, WHITE
};

class Outputter final
{

public:

	static Outputter* GetInstance();
	static void Destroy();

	void Log(string text);
	void Log(string text, OUTPUT_COLOR col);

private:

	Outputter();
	~Outputter();

	static Outputter* m_Instance;

};

