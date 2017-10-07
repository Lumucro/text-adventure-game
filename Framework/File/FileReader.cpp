#include "FileReader.h"

#include <fstream>

FileReader::FileReader(bool debug) : m_DebugMode(debug)
{
	m_IFStream = new ifstream();
	if (m_DebugMode) OUTPUT->Log("Creating filereader..\n", OUTPUT_COLOR::GREEN);
}

FileReader::~FileReader()
{
	delete m_IFStream;
	m_IFStream = nullptr;
}

bool FileReader::ReadFile(const string& path)
{
	//Close filestream if it's still open
	if (m_IFStream->is_open())
	{
		m_IFStream->close();
	}

	//Open the other file
	m_IFStream->open(path, ifstream::in);

	//Debug check
	if (!m_IFStream->is_open())
	{
		if (m_DebugMode) OUTPUT->Log("Unable to open file at <" + path + "\n", OUTPUT_COLOR::RED);
	}

	//Success or not
	return (m_IFStream->is_open());
}

bool FileReader::ReadLine(string& out)
{
	//If open and not eof, read a line
	if (m_IFStream->is_open() && !m_IFStream->eof())
	{
		getline(*m_IFStream, out);
		return true;
	}
	
	if (m_DebugMode) OUTPUT->Log("ReadLine failed, possibly not open or at eof.\n", OUTPUT_COLOR::RED);
	return false;
}

bool FileReader::GetEof()
{
	return (!m_IFStream->is_open() || m_IFStream->eof());
}
