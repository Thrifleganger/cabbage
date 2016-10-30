/*
  ==============================================================================

    CabbageCodeEditor.cpp
    Created: 16 Oct 2016 2:59:39pm
    Author:  rory

  ==============================================================================
*/

#include "CabbageCodeEditor.h"
#include "../Settings/CabbageSettings.h"
#include "../Utilities/CabbageUtilities.h"
#include "../CabbageMainDocumentWindow.h"

//==============================================================================
CabbageCodeEditorComponent::CabbageCodeEditorComponent(Component* statusBar, ValueTree valueTree, CodeDocument &document, CodeTokeniser *codeTokeniser)
    : CodeEditorComponent(document, codeTokeniser), valueTree(valueTree), statusBar(statusBar)
{
	
    String opcodeFile = File(File::getSpecialLocation(File::currentExecutableFile)).getParentDirectory().getFullPathName();
    opcodeFile += "/opcodes.txt";
    Logger::writeToLog(opcodeFile);

    if(File(opcodeFile).existsAsFile())
        setOpcodeStrings(File(opcodeFile).loadFileAsString());	
	
	document.addListener(this);
	this->setColour(CodeEditorComponent::ColourIds::lineNumberBackgroundId, CabbageSettings::getColourFromValueTree(valueTree, CabbageColourIds::lineNumberBackground, Colour(70,70,70)));
	this->setColour(CodeEditorComponent::ColourIds::lineNumberTextId, CabbageSettings::getColourFromValueTree(valueTree, CabbageColourIds::lineNumbers, Colours::white));
}

void CabbageCodeEditorComponent::updateColourScheme()
{
	struct Type
	{
		const char* name;
		uint32 colour;
	};

	const Type types[] =
	{
		{ "Error",              CabbageSettings::getColourFromValueTree(valueTree, CabbageColourIds::numbers, Colours::grey.darker()).getARGB() },
		{ "Comment",            CabbageSettings::getColourFromValueTree(valueTree, CabbageColourIds::comment, Colours::grey.darker()).getARGB() },
		{ "Keyword",            CabbageSettings::getColourFromValueTree(valueTree, CabbageColourIds::keyword, Colours::grey.darker()).getARGB() },
		{ "Identifier",         CabbageSettings::getColourFromValueTree(valueTree, CabbageColourIds::identifierLiteral, Colours::grey.darker()).getARGB() },
		{ "Integer",            Colours::grey.darker().getARGB() },
		{ "Float",              Colours::grey.darker().getARGB() },
		{ "String",             CabbageSettings::getColourFromValueTree(valueTree, CabbageColourIds::stringLiteral, Colours::grey.darker()).getARGB() },
		{ "Operator",           Colours::grey.darker().getARGB() },
		{ "Bracket",            Colours::grey.darker().getARGB() },
		{ "Punctuation",        Colours::grey.darker().getARGB() },
		{ "Preprocessor Text",  Colours::grey.darker().getARGB() },
		{ "Csd Tag",			CabbageSettings::getColourFromValueTree(valueTree, CabbageColourIds::csdtags, Colours::grey.darker()).getARGB()}
	};

	CodeEditorComponent::ColourScheme cs;

	setColour(CodeEditorComponent::ColourIds::lineNumberBackgroundId, CabbageSettings::getColourFromValueTree(valueTree, CabbageColourIds::lineNumberBackground, Colour(70,70,70)));
	setColour(CodeEditorComponent::ColourIds::lineNumberTextId, CabbageSettings::getColourFromValueTree(valueTree, CabbageColourIds::lineNumbers, Colours::white));
	setColour(CodeEditorComponent::ColourIds::backgroundColourId, CabbageSettings::getColourFromValueTree(valueTree, CabbageColourIds::codeBackground, Colours::white));
	setColour(CaretComponent::ColourIds::caretColourId, CabbageSettings::getColourFromValueTree(valueTree, CabbageColourIds::caret, Colours::white));
	setColour(CodeEditorComponent::ColourIds::highlightColourId, CabbageSettings::getColourFromValueTree(valueTree, CabbageColourIds::selectTextBackground, Colours::white));

	for (int i = 0; i < sizeof (types) / sizeof (types[0]); ++i)  // (NB: numElementsInArray doesn't work here in GCC4.2)
		cs.set (types[i].name, Colour (types[i].colour));

	this->setColourScheme(cs);
}

void CabbageCodeEditorComponent::codeDocumentTextInserted(const String &text,int)
{
    const String lineFromCsd = getDocument().getLine(getDocument().findWordBreakBefore(getCaretPos()).getLineNumber());
    displayOpcodeHelpInStatusBar(lineFromCsd);
}

//==============================================================================
void CabbageCodeEditorComponent::displayOpcodeHelpInStatusBar(String lineFromCsd)
{
    String opcodeHelpString;
    StringArray syntaxTokens, csdLineTokens;
    csdLineTokens.clear();
    csdLineTokens.addTokens(lineFromCsd, " ,\t", "");

    for(int i=0; i<opcodeStrings.size(); i++)
    {
        opcodeHelpString = opcodeStrings[i];
        syntaxTokens.clear();
        syntaxTokens.addTokens(opcodeHelpString, ";", "\"");
        if(syntaxTokens.size()>3)
            for(int x=0; x<csdLineTokens.size(); x++)
            {
                if(syntaxTokens[0].removeCharacters("\"")==csdLineTokens[x].trim())
                {
                    if(syntaxTokens[0].length()>3)
                    {
						if(EditorAndConsoleContentComponent::StatusBar* bar = dynamic_cast<EditorAndConsoleContentComponent::StatusBar*>(statusBar))
							bar->setText(syntaxTokens[2]+syntaxTokens[3]);
                        x=csdLineTokens.size();
                        i=opcodeStrings.size();
                    }
                }
            }
    }
}