#pragma once
#include "../stdafx.h"

class FileReader
{

public:

	FileReader(bool debug);
	~FileReader();

	bool ReadFile(const string& path);
	bool ReadLine(string& out);
	bool GetEof();

private:

	bool m_DebugMode = false;
	ifstream* m_IFStream = nullptr;

};

