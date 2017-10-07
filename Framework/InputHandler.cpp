#include "InputHandler.h"
#include <regex>

#include "Classes/Inventory.h"
#include "Classes/Area.h"
#include "Classes/Interactable.h"

InputHandler::InputHandler(bool debugMode) : m_DebugMode(debugMode)
{
	Initialize();
}

InputHandler::~InputHandler()
{
}

void InputHandler::Initialize()
{
	//All the functions for interaction and whatnot

	//**************************
	//Moving to a different area
	//**************************
	auto moveFunc = [](vector<string>& args)->bool {

		//Safety check, make sure there's an actual direction-
		if (args.size() == 0)
			return false;
		else if (args[0].size() == 0)
			return false;

		//Move in the appropriate direction, check the first letter
		char dir = args[0][0];
		AREADIRECTION direction;
		Area* targetArea = nullptr;
		Area* curArea = GAME->GetCurrentArea();
		//Switch to check
		switch (dir)
		{
		case 'n':
			direction = AREADIRECTION::NORTH;
			targetArea = curArea->GetConnectedArea(direction);
			break;
		case 'e':
			direction = AREADIRECTION::EAST;
			targetArea = curArea->GetConnectedArea(direction);
			break;
		case 's':
			direction = AREADIRECTION::SOUTH;
			targetArea = curArea->GetConnectedArea(direction);
			break;
		case 'w':
			direction = AREADIRECTION::WEST;
			targetArea = curArea->GetConnectedArea(direction);
			break;
		}

		//If targetarea is valid and not locked, move there
		if (targetArea != nullptr)
		{
			if (!curArea->IsLocked(direction))
			{
				GAME->SetCurrentArea(targetArea);
				system("cls"); //Risky, this only works on Windows
				targetArea->Enter();
				return true;
			}
			else
			{
				//Random fail message
				vector<string> randomFail = { "The way is shut.", "You can't go there.", "Something is stopping you from going there.", "That route is blocked.", "That appears to be locked." };
				srand(size_t(time(NULL)));
				OUTPUT->Log(randomFail[rand() % randomFail.size()] + "\n\n");
				return false;
			}
		}
		else 
		{
			OUTPUT->Log("There really is nothing to see there.\n\n");
			return false;
		}

	};
	//Add to InputActions
	m_InputActionsMap["move"] = moveFunc;
	m_InputActionsMap["go"] = moveFunc;
	m_InputActionsMap["travel"] = moveFunc;
	m_InputActionsMap["walk"] = moveFunc;
	
	//**************************
	//Examining an area or interactable
	//**************************
	auto examineFunc = [](vector<string>& args)->bool {
		//Ignore 'at'
		if (args[0] == "at") args.erase(args.begin());
		//Make sure there's still some args left
		if (args.size() <= 0) return false;

		//First check if it's the area we're examining
		Area* curArea = GAME->GetCurrentArea();
		vector<string> areaSynonyms = { "area", "surroundings", "environment", "room", "surrounding", "whereabouts", "around", "everywhere", "dungeon" };
		for (auto possibleAreaName : areaSynonyms)
		{
			//If this is a word we use for the area, examine it and return true
			if (args[0] == possibleAreaName)
			{
				OUTPUT->Log(curArea->Examine());
				return true;
			}
		}

		//Get the full itemname
		string itemname = "";
		for (size_t i = 0; i < args.size(); ++i)
		{
			itemname += args[i];
			if (args.size() > 1 && i != (args.size() - 1)) itemname += " ";
		}

		//Check if it's an item we're trying to examine that is inside of our current area
		auto interactables = curArea->GetInteractablesArr();
		//Add to this the items in our inventory (after all, it doesn't matter for examine, the end result is the same)
		auto invInteractables = GAME->GetInventory()->GetInteractables();
		interactables.insert(invInteractables.begin(), invInteractables.end()); //CAREFUL -- Some keys in the inventory might already be in the area (this shouldnt happen by design, but still)

		for (auto it : interactables)
		{
			Interactable* interactable = it.second;

			//See if one of the identifiers matches
			for (auto identifier : interactable->GetIdentifiers())
			{
				if (identifier == args[0] || identifier == itemname)
				{;
					//Output
					OUTPUT->Log(interactable->GetRandomDescription() + "\n\n");
					return true;
				}
			}
			//Check ID and name last because the player is the least likely to use them
			if (interactable->GetID() == args[0] || interactable->GetName() == itemname)
			{
				//Output
				OUTPUT->Log(interactable->GetRandomDescription() + "\n");
				return true;
			}
		}

		//Assume something went wrong, return false
		OUTPUT->Log("You're not sure what you're supposed to look at.\n\n");
		return false;

	};
	m_InputActionsMap["examine"] = examineFunc;
	m_InputActionsMap["look"] = examineFunc;
	m_InputActionsMap["inspect"] = examineFunc;
	m_InputActionsMap["investigate"] = examineFunc;

	//**************************
	//Picking up an interactable
	//**************************
	auto pickupFunc = [](vector<string>& args)->bool {
		//Do we even have space to pick anything up?
		if (!GAME->GetInventory()->HasSpace())
		{
			OUTPUT->Log("You can't carry anything else.\n\n");
			return false;
		}

		//Ignore 'up'
		if (args[0] == "up") args.erase(args.begin());
		//Make sure there's still some args left
		if (args.size() <= 0) return false;

		//Get the items present in the area
		Area* curArea = GAME->GetCurrentArea();
		//Get the full itemname
		string itemname = "";
		for (size_t i = 0; i < args.size(); ++i)
		{
			itemname += args[i];
			if (args.size() > 1 && i != (args.size() - 1)) itemname += " ";
		}

		//Go through the items, find the one we want and pick it up if we can
		for (auto it : curArea->GetInteractablesArr())
		{
			for (auto identifier : it.second->GetIdentifiers())
			{
				if (identifier == itemname)
				{
					//Does this give us a different item? (eg pick up rock, find key)
					bool hasPickupItem = it.second->HasPickupItem();
					if (hasPickupItem)
					{
						string pickupItem = it.second->GetPickupItem();
						if (pickupItem != "")
						{
							//Get the item
							Interactable* item = CONTENT->GetInteractable(pickupItem);
							//If it's valid, destroy this item (eg rock) and give us the one we picked up to our inventory (eg key)s
							if (item)
							{
								string pickuptext = it.second->GetPickupText();
								if (pickuptext != "") OUTPUT->Log(it.second->GetPickupText() + "\n");
								GAME->GetInventory()->PickupItem(item, true);
								return true;
							}
						}
						//If empty string it already gave us that item
						else return false;
					}

					//Can we pick this item up?
					if (!it.second->CanPickup())
					{
						//Random failmessage
						vector<string> failMessages = { "You can't pick that up.", "That's stuck in place.", "Why would you even want that?" };
						srand(int(time(NULL)));

						OUTPUT->Log(failMessages[rand() % failMessages.size()] + "\n\n");
						return false;
					}

					//Pick up the item
					GAME->GetInventory()->PickupItem(it.second);
					return true;
				}
			}
		}

		OUTPUT->Log("That doesn't seem to be here.\n\n");
		//Something must have gone wrong, return false
		return false;
	};
	m_InputActionsMap["pickup"] = pickupFunc;
	m_InputActionsMap["pick"] = pickupFunc;
	m_InputActionsMap["grab"] = pickupFunc;
	m_InputActionsMap["take"] = pickupFunc;

	//**************************
	//Dropping an interactable
	//**************************
	auto dropFunc = [](vector<string>& args)->bool {
		//Get the items present in our inventory
		auto interactables = GAME->GetInventory()->GetInteractables();
		//Get the full itemname
		string itemname = "";
		for (size_t i = 0; i < args.size(); ++i)
		{
			itemname += args[i];
			if (args.size() > 1 && i != (args.size() - 1)) itemname += " ";
		}

		//Go through the items, find the one we want and drop it
		for (auto it : interactables)
		{
			for (auto identifier : it.second->GetIdentifiers())
			{
				if (identifier == itemname)
				{
					//Drop the item
					GAME->GetInventory()->DropItem(it.second);
					return true;
				}
			}
		}

		//Something must have gone wrong, return false
		return false;
	};
	m_InputActionsMap["drop"] = dropFunc;

	//**************************
	//Using one interactable (on another)
	//**************************
	auto useFunc = [&](vector<string>& args)->bool {
		//Check if it's a single use or a use x on y kind of deal here, look for the word 'on' or 'with'
		size_t itemEnd = numeric_limits<size_t>::max();
		for (size_t i = 0; i < args.size(); ++i)
		{
			if (args[i] == "on" || args[i] == "with")
			{
				args.erase(args.begin() + i);
				itemEnd = i;
				break;
			}
		}

		//Full item name 1 and 2
		string fullItem1 = "";
		Interactable* pItem1 = nullptr;
		string fullItem2 = "";
		Interactable* pItem2 = nullptr;

		//If there's an itemend, that means there are 2 items, so get both (eg. use key on lock)
		if (itemEnd != numeric_limits<size_t>::max())
		{
			//Item 1
			for (size_t i = 0; i < itemEnd; ++i)
			{
				fullItem1 += args[i];
				if (args.size() > 1 && i != (args.size() - 1)) fullItem1 += " ";
			}

			//Item 2
			for (size_t i = itemEnd; i < args.size(); ++i)
			{
				fullItem2 += args[i];
				if (args.size() > 1 && i != (args.size() - 1)) fullItem2 += " ";
			}
		}
		//Otherwise only one item (eg. use tomato)
		else
		{
			for (size_t i = 0; i < args.size(); ++i)
			{
				fullItem1 += args[i];
				if (args.size() > 1 && i != (args.size() - 1)) fullItem1 += " ";
			}
		}

		//Trim the names (there's spaces in there, we don't want those)
		TrimString(fullItem1);
		TrimString(fullItem2);

		//Items in the area
		auto interactables = GAME->GetCurrentArea()->GetInteractablesArr();
		//Items in our inventory (after all, it doesn't matter for use, the end result is the same)
		auto invInteractables = GAME->GetInventory()->GetInteractables();
		//Add these to the area interactables for a complete list
		interactables.insert(invInteractables.begin(), invInteractables.end()); //CAREFUL -- Some keys in the inventory might already be in the area (this shouldnt happen by design, but still)

		//Go through the items, find the one(s) we want
		if (itemEnd == numeric_limits<size_t>::max())
		{
			bool doneLooking = false;

			//Find the first item (anywhere)
			for (auto it : interactables)
			{
				for (auto identifier : it.second->GetIdentifiers())
				{		
					if (identifier == fullItem1) 
					{
						pItem1 = it.second;
						doneLooking = true;
						break;
					}
				}

				if (doneLooking) break; //Don't do more loops than we need
			}

			//If this is still false, we didn't find the item - no point in going on
			if (!doneLooking)
			{
				OUTPUT->Log("There is no '" + fullItem1 + "'.\n\n");
				return false;
			}
		}
		else
		{
			bool doneLooking = false;

			//Find the first item
			for (auto it : interactables)
			{
				for (auto identifier : it.second->GetIdentifiers())
				{
					if (identifier == fullItem1)
					{
						pItem1 = it.second;
						doneLooking = true;
						break;
					}
				}

				if (doneLooking) break; //Don't do more loops than we need
			}

			//If this is still false, we didn't find the item - no point in going on
			if (!doneLooking)
			{
				OUTPUT->Log("There is no '" + fullItem1 + "'.\n\n");
				return false;
			}
			doneLooking = false; //Reset

			//Find the second item anywhere (inventory OR area)
			for (auto it : interactables)
			{
				for (auto identifier : it.second->GetIdentifiers())
				{
					//Item 2
					if (identifier == fullItem2) 
					{
						pItem2 = it.second;
						doneLooking = true;
						break;
					}
				}

				if (doneLooking) break; //Don't do more loops than we need
			}

			//If this is still false, we didn't find the item - no point in going on
			if (!doneLooking)
			{
				OUTPUT->Log("There is no '" + fullItem2 + "'.\n\n");
				return false;
			}
		}

		//Actually use the item(s)
		return pItem1->Use(pItem2);
	};
	m_InputActionsMap["use"] = useFunc;

	//Done
	if (m_DebugMode) OUTPUT->Log("Finished loading interaction functions.\n", OUTPUT_COLOR::GREEN);
}

void InputHandler::ParseInput(const string& input)
{
	if (m_DebugMode) OUTPUT->Log("\n[DEBUG] Entered: " + input + "\n", OUTPUT_COLOR::GREEN);

	//Split the string into arguments, where each word is an argument, seperated by delim (default " ")
	//Only if there is an actual string to split
	if (input == "") return;
	vector<string> arguments = SplitString(input);
	//Process the command and arguments
	ProcessCommand(arguments);
}

void InputHandler::Tick()
{
	getline(cin, m_Input);
	OUTPUT->Log("\n");
	ParseInput(m_Input);
}

vector<string> InputHandler::SplitString(const string& str, const string& delim)
{
	//Commandstring to lowercase
	string command = str;
	transform(command.begin(), command.end(), command.begin(), ::tolower);

	vector<string> result;
	size_t pos = 0;

	//Loop for args
	while ((pos = command.find(delim)) != string::npos)
	{
		result.push_back(command.substr(0, pos));
		command.erase(0, pos + delim.length());
	}

	//Last one wont be included in while loop
	result.push_back(command);

	return result;
}

vector<string> InputHandler::SplitString(const string& str)
{
	return SplitString(str, " ");
}

void InputHandler::ProcessCommand(const vector<string>& cmd)
{
	//No need to process if no potential command/args are present
	int argcount = cmd.size();
	string key = cmd[0];
	
	if (key == "help")
	{
		//Special exception, this is the help and the only command that doesn't need any args
		OUTPUT->Log("Controls:\n-Help\n-Suicide\n-Inventory\n-Go <direction>\n-Look (around) (at) <itemname>\n-Examine (area, room, ..)\n-Examine <itemname>\n-Use <itemname> (on <itemname>)\n-Pickup <itemname>\n-Drop <itemname>\n\n", OUTPUT_COLOR::YELLOW);
		return;
	}
	else if (key == "suicide")
	{
		//Special exception, suicide
		GAME->KillPlayer();
		return;
	}
	else if (key == "inventory" || key == "items" || key == "inv")
	{
		//Special exceptionm check inventory
		GAME->GetInventory()->LookAtInventory();
		return;
	}
	else if (argcount <= 1) 
	{
		OUTPUT->Log("You're not sure what good this would do.\n\n"); 
		return;
	}

	vector<string> args = cmd;
	args.erase(args.begin());

	//Run the command, first arg is the key for our InputActionsMap
	map<string, function<bool(vector<string>)>>::const_iterator it = m_InputActionsMap.find(key);
	//If the key exists
	if (it != m_InputActionsMap.end())
	{
		bool success = m_InputActionsMap[key](args);
		if (m_DebugMode && !success) OUTPUT->Log("Something went wrong processing a command!\n\n", OUTPUT_COLOR::RED);
	}
	else
	{
		OUTPUT->Log("You're not sure what good this would do.\n\n");
	}
}

//Helper function
void InputHandler::TrimString(string& str)
{
	if (str.size() == 0) return;

	size_t first = str.find_first_not_of(' ');
 	size_t last = str.find_last_not_of(' ');
	str = str.substr(first, (last - first + 1));
}
