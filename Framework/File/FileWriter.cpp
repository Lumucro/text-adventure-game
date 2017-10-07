#include "FileWriter.h"

FileWriter::FileWriter()
{
}

FileWriter::~FileWriter()
{
}

/*
 * HOW ITEMS AND AREAS WILL BE LOADED - IDEA 1
 * Because we dont want to bother to keep putting everything in order or something like that
 * We will just first load everything, including identifiers as what they rely on (for example other areas)
 * When all of these are done, we go over all of them to add pointers to whatever it relies on (or something like that)
 * 
 * PROBLEM
 * Memory usage if we load everything in even if its not needed?
 * May need a way around this
 */