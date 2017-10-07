#pragma once
#include "stdafx.h"

enum class AREADIRECTION;
class Interactable;
class Area;
class FileReader;

using namespace std;

class ContentManager final
{

public:

	static ContentManager* GetInstance();
	static void Destroy();

	bool Load(bool debug);
	Area* GetArea(const string& areaRef);
	Interactable* GetInteractable(const string& interactableRef);

private:
	
	//Singleton pattern
	ContentManager();
	~ContentManager();
	//Instance
	static ContentManager* m_Instance;

	//Loading objects (template)
	template<typename T>
	typename enable_if<std::is_same<Interactable, T>::value || std::is_same<Area, T>::value, void>::type
	Load(FileReader* pFileReader, map<string, T*>& objectVector, bool debug);
	
	//Adding properties to objects or areas
	bool AddPropertyToObject(Interactable* pObject, const string& keyRef, const vector<string>& valuesRef, bool debug);
	bool AddPropertyToObject(Area* pObject, const string& keyRef, const vector<string>& valuesRef, bool debug);
	//Adding objects to areas
	bool AddInteractableToArea(Interactable* pInteractable, Area* pArea);
	//Connecting areas
	bool AddAreaConnection(Area* pTargetArea, AREADIRECTION dir, const string& toConnect, const bool locked);
	
	//Maps for areas and objects
	map<string, Area*> m_AreasMap;
	map<string, Interactable*> m_InteractablesMap;

	//Debugmode
	bool m_DebugMode = false;

};
