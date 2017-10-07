#pragma once
#include "stdafx.h"
#include <functional>

using namespace std;

class Game;

class InputHandler final
{

public:
	
	InputHandler(bool debugMode);
	~InputHandler();

	void ParseInput(const string& input);
	void Tick();

private:

	//Initialize
	void Initialize();
	//Split string up so we can process commands
	vector<string> SplitString(const string& str, const string& delim);
	vector<string> SplitString(const string& str);
	//Processing commands
	void ProcessCommand(const vector<string>& cmd);

	//Helper
	void TrimString(string& str);
	
	//Debug print everything we enter etc
	bool m_DebugMode = false;
	string m_Input = "";

	//Map of input functions
	//key string is the command (like 'use'); string vector arg is the split text (like 'x on y')
	map<string, function<bool(vector<string>)>> m_InputActionsMap;

};

