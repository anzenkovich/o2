#include "CppSyntaxParser.h"

#include <fstream>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

string& ltrim(string &str, const string& chars /*= " "*/)
{
	int l = (int)str.length();
	int i = 0;
	for (; i < l; i++)
	{
		if (chars.find(str[i]) == string::npos)
			break;
	}

	str.erase(0, i);

	return str;
}

string& rtrim(string &str, const string& chars /*= " "*/)
{
	int l = (int)str.length();
	int i = l - 1;
	for (; i >= 0; i--)
	{
		if (chars.find(str[i]) == string::npos)
			break;
	}

	str.erase(i + 1);

	return str;
}

string& trim(string &str, const string& chars /*= " "*/)
{
	return ltrim(rtrim(str, chars), chars);
}

bool startsWith(const string& str, const string& starts)
{
	int l1 = (int)str.length(), l2 = (int)starts.length();

	if (l1 < l2)
		return false;

	for (int i = 0; i < l1 && i < l2; i++)
	{
		if (str[i] != starts[i])
			return false;
	}

	return true;
}

bool endsWith(const string& str, const string& ends)
{
	int l1 = (int)str.length(), l2 = (int)ends.length();

	if (l1 < l2)
		return false;

	for (int i = 0; i < l1 && i < l2; i++)
	{
		if (str[l1 - i - 1] != ends[l2 - i - 1])
			return false;
	}

	return true;
}

CppSyntaxParser::CppSyntaxParser()
{
	InitializeParsers();
}

CppSyntaxParser::~CppSyntaxParser()
{
	for (auto x : mParsers)
		delete x;
}

void CppSyntaxParser::InitializeParsers()
{
	mParsers.push_back(new ExpressionParser("namespace", &CppSyntaxParser::ParseNamespace, false, true));
	mParsers.push_back(new ExpressionParser("namespace", &CppSyntaxParser::ParseNamespace, true, true));
	mParsers.push_back(new ExpressionParser("//", &CppSyntaxParser::ParseComment, true, true));
	mParsers.push_back(new ExpressionParser("/*", &CppSyntaxParser::ParseMultilineComment, true, true));
	mParsers.push_back(new ExpressionParser("#pragma", &CppSyntaxParser::ParsePragma, false, true));
	mParsers.push_back(new ExpressionParser("#include", &CppSyntaxParser::ParseInclude, false, true));
	mParsers.push_back(new ExpressionParser("#define", &CppSyntaxParser::ParseDefine, true, true));
	mParsers.push_back(new ExpressionParser("#if", &CppSyntaxParser::ParseIfMacros, true, true));
	mParsers.push_back(new ExpressionParser("meta class", &CppSyntaxParser::ParseMetaClass, true, true));
	mParsers.push_back(new ExpressionParser("class", &CppSyntaxParser::ParseClass, true, true));
	mParsers.push_back(new ExpressionParser("struct", &CppSyntaxParser::ParseStruct, true, true));
	mParsers.push_back(new ExpressionParser("template", &CppSyntaxParser::ParseTemplate, true, true));
	mParsers.push_back(new ExpressionParser("typedef", &CppSyntaxParser::ParseTypedef, true, true));
	mParsers.push_back(new ExpressionParser("enum", &CppSyntaxParser::ParseEnum, true, true));
	mParsers.push_back(new ExpressionParser("using", &CppSyntaxParser::ParseUsing, true, true));
	mParsers.push_back(new ExpressionParser("public:", &CppSyntaxParser::ParsePublicSection, true, false));
	mParsers.push_back(new ExpressionParser("private:", &CppSyntaxParser::ParsePrivateSection, true, false));
	mParsers.push_back(new ExpressionParser("protected:", &CppSyntaxParser::ParseProtectedSection, true, false));
	mParsers.push_back(new ExpressionParser("friend", &CppSyntaxParser::ParseFriend, true, false));
	mParsers.push_back(new ExpressionParser("ATTRIBUTE_COMMENT_DEFINITION", &CppSyntaxParser::ParseAttributeCommentDef, true, false));
	mParsers.push_back(new ExpressionParser("ATTRIBUTE_SHORT_DEFINITION", &CppSyntaxParser::ParseAttributeShortDef, true, false));
	mParsers.push_back(new ExpressionParser("ATTRIBUTES", &CppSyntaxParser::ParseAttributes, true, false));
}

void CppSyntaxParser::ParseFile(SyntaxFile& file, const string& filePath, const TimeStamp& fileEditDate)
{
	file.mPath = filePath;
	file.mLastEditedDate = fileEditDate;

	ifstream fin;
	fin.open(filePath.c_str());
	if (!fin.is_open())
		return;

	file.mData = string((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());

	fin.close();

	ParseSyntaxSection(*file.mGlobalNamespace, file.mData, file, SyntaxProtectionSection::Public);
}

void CppSyntaxParser::ParseSyntaxSection(SyntaxSection& section, const string& source, SyntaxFile& file,
										 SyntaxProtectionSection protectionSection)
{
	section.mLength = (int)source.length();
	section.mData = source;
	section.mFile = &file;

	string skipSymbols = " \r\n\t;";

	int caret = 0;
	while (caret < section.mLength)
	{
		if (skipSymbols.find(source[caret]) != string::npos)
		{
			caret++;
			continue;
		}

		bool parsedByKeywork = false;
		for (auto parser : mParsers)
		{
			const char* keyWord = parser->keyWord;

			int i = 0;
			bool success = true;
			while (keyWord[i] != '\0')
			{
				if (source[i + caret] != keyWord[i])
				{
					success = false;
					break;
				}
				i++;
			}

			if (!success)
				continue;

			ParserDelegate pd = parser->parser;
			(this->*pd)(section, caret, protectionSection);
			parsedByKeywork = true;

			break;
		}

		if (parsedByKeywork)
			continue;

		int blockbegin = caret;
		string block = ReadBlock(section.mData, caret);
		trim(block, " \r\t\n{}");

		if (!block.empty())
			TryParseBlock(section, block, blockbegin, caret, protectionSection);
		else
			caret++;
	}
}

int CppSyntaxParser::GetLineNumber(const string& data, int caret)
{
	int res = 0;
	for (int i = 0; i < caret; i++)
		if (data[i] == '\n')
			res++;

	return res;
}

void CppSyntaxParser::TryParseBlock(SyntaxSection& section, const string& block, int blockBegin, int& caret,
									SyntaxProtectionSection& protectionSection)
{
	if (IsFunction(block))
	{
		auto func = ParseFunction(block, protectionSection, blockBegin, caret);
		func->mFile = section.mFile;
		func->mLine = GetLineNumber(section.mData, caret);
		section.mFunctions.push_back(func);
	}
	else
	{
		auto var = ParseVariable(block, protectionSection, blockBegin, caret);
		var->mFile = section.mFile;
		var->mLine = GetLineNumber(section.mData, caret);
		section.mVariables.push_back(var);
	}
}

bool CppSyntaxParser::IsFunction(const string& data)
{
	int locCaret = 0;
	bool isFunction = false;

	string firstWord = ReadWord(data, locCaret, " \n\r(){}[]");

	if (firstWord.empty())
		return false;

	if (firstWord == "virtual")
		firstWord = ReadWord(data, locCaret, " \n\r(){}[]");

	if (firstWord == "static")
		firstWord = ReadWord(data, locCaret, " \n\r(){}[]");

	if (firstWord == "typename")
		firstWord = ReadWord(data, locCaret, " \n\r(){}[]");

	if (firstWord == "inline")
		firstWord = ReadWord(data, locCaret, " \n\r(){}[]");

	if (GetNextSymbol(data, locCaret, " \n\r\t") == '(')
	{
		string braces = ReadBraces(data, locCaret);
		trim(braces, " \n\t\r()");

		int tmpCaret = 0;
		string word = ReadWord(braces, tmpCaret);

		isFunction = GetNextSymbol(braces, tmpCaret, " \n\r\t") != ':';

		if (!isFunction && startsWith(braces, "std"))
			isFunction = true;
	}
	else
	{
		if (firstWord == "const")
			ReadWord(data, locCaret, " \n\r(){}[]");

		string thirdWord = ReadWord(data, locCaret, " \n\r(){}[]");

		if (thirdWord == "operator")
			thirdWord = ReadWord(data, locCaret, " \n\r(){}");

		if (GetNextSymbol(data, locCaret, " \n\r\t") == '(')
			isFunction = true;
	}

	return isFunction;
}

SyntaxVariable* CppSyntaxParser::ParseVariable(const string& data, SyntaxProtectionSection& protectionSection,
											   int begin, int end)
{
	SyntaxVariable* res = new SyntaxVariable();
	res->mBegin = begin;
	res->mLength = end - begin;
	res->mData = data;

	int caret = 0;
	string typeWord = ReadWord(data, caret, " \n\r(){}[]");
	string typeDefinition = typeWord;

	if (typeWord == "static")
	{
		typeWord = ReadWord(data, caret, " \n\r(){}[]");
		typeDefinition += " " + typeWord;
		res->mIsStatic = true;
	}

	if (typeWord == "const")
	{
		typeWord = ReadWord(data, caret, " \n\r(){}[]");
		res->mType.mIsContant = true;

		typeDefinition += " " + typeWord;
	}

	if (!typeWord.empty() && typeWord[typeWord.length() - 1] == '&')
		res->mType.mIsReference = true;

	if (!typeWord.empty() && typeWord[typeWord.length() - 1] == '*')
		res->mType.mIsPointer = true;

	res->mType.mName = typeWord;

	res->mClassSection = protectionSection;

	if (GetNextSymbol(data, caret, " \n\r\t") == '(')
	{
		string braces = trim(ReadBraces(data, caret), " \r\t\t()");
		string nextBraces = trim(ReadBraces(data, caret), " \r\t\t()");

		int tmpCaret = 0;
		string bracesFirst = ReadWord(braces, tmpCaret);
		tmpCaret += 3;
		res->mName = braces.substr(tmpCaret);

		res->mType.mName += " (" + bracesFirst + "*)(" + nextBraces + ")";
	}
	else
		res->mName = ReadWord(data, caret, " \n\r(){}[]");

	return res;
}

SyntaxFunction* CppSyntaxParser::ParseFunction(const string& data, SyntaxProtectionSection& protectionSection,
											   int begin, int end)
{
	SyntaxFunction* res = new SyntaxFunction();
	res->mBegin = begin;
	res->mLength = end - begin;
	res->mData = data;

	res->mClassSection = protectionSection;

	int caret = 0;
	string typeWord = ReadWord(data, caret, " \n\r(){}[]");

	if (typeWord == "virtual")
	{
		res->mIsVirtual = true;
		typeWord = ReadWord(data, caret, " \n\r(){}[]");
	}

	if (typeWord == "static")
	{
		res->mIsStatic = true;
		typeWord = ReadWord(data, caret, " \n\r(){}[]");
	}

	if (typeWord == "inline")
		typeWord = ReadWord(data, caret, " \n\r(){}[]");

	if (typeWord == "typename")
		typeWord = ReadWord(data, caret, " \n\r(){}[]");

	if (typeWord == "explicit")
		typeWord = ReadWord(data, caret, " \n\r(){}[]");

	if (typeWord == "operator")
	{
		string nextWord = ReadWord(data, caret, " \n\r(){}[]");
		res->mName = typeWord + nextWord;
		res->mReturnType.mName = "void";
	}
	else
	{
		if (GetNextSymbol(data, caret, " \n\r\t") == '(')
		{
			res->mName = typeWord;
			res->mReturnType.mName = "void";
		}
		else
		{
			string typeDefinition = typeWord;

			if (typeWord == "const")
			{
				typeWord = ReadWord(data, caret, " \n\r(){}[]");
				res->mReturnType.mIsContant = true;

				typeDefinition += " " + typeWord;
			}

			if (!typeWord.empty() && typeWord[typeWord.length() - 1] == '&')
				res->mReturnType.mIsReference = true;

			if (!typeWord.empty() && typeWord[typeWord.length() - 1] == '*')
				res->mReturnType.mIsPointer = true;

			res->mReturnType.mName = typeWord;

			res->mName = ReadWord(data, caret, " \n\r(){}[]");

			if (res->mName == "operator")
				res->mName += " " + ReadWord(data, caret, " \n\r(){}");
		}
	}

	string paramsStr = trim(ReadBraces(data, caret), " \n\r\t");
	string afterParamWord = ReadWord(data, caret);

	if (afterParamWord == "const")
		res->mIsContstant = true;

	if (!paramsStr.empty())
	{
		auto paramsArr = Split(paramsStr, ',');
		for (auto& prm : paramsArr)
		{
			trim(prm, " \r\n\t");
			SyntaxProtectionSection tempProtectSection = SyntaxProtectionSection::Public;
			res->mParameters.push_back(ParseVariable(prm, tempProtectSection, begin, end));
		}
	}

	return res;
}

void CppSyntaxParser::ParseNamespace(SyntaxSection& section, int& caret,
									 SyntaxProtectionSection& protectionSection)
{
	int begin = caret;
	caret += (int)strlen("namespace");

	string namespaceName = ReadWord(section.mData, caret);
	int namespaceBegin = (int)section.mData.find("{", caret) + 1;
	string block = trim(ReadBlock(section.mData, caret), " \r\t\n");

	SyntaxNamespace* newNamespace = new SyntaxNamespace();
	newNamespace->mBegin = begin;
	newNamespace->mLength = caret - begin;
	newNamespace->mLine = GetLineNumber(section.mData, caret);
	newNamespace->mData = trim(block.substr(1, block.length() - 1), " \r\t\n");
	newNamespace->mName = namespaceName;
	newNamespace->mFullName = section.mFullName.empty() ? namespaceName : section.mFullName + "::" + namespaceName;
	newNamespace->mFile = section.mFile;
	newNamespace->mParentSection = &section;
	section.mSections.push_back(newNamespace);

	ParseSyntaxSection(*newNamespace, newNamespace->mData, *section.mFile, SyntaxProtectionSection::Public);
}

void CppSyntaxParser::ParseComment(SyntaxSection& section, int& caret,
								   SyntaxProtectionSection& protectionSection)
{
	int begin = caret;
	caret += (int)strlen("//");

	SyntaxComment* comment = new SyntaxComment();

	int dataLength = (int)section.mData.length();
	string skipSymbols = " \r\t;";
	int tmpCaret = caret;
	bool first = true;
	int line = 0;
	do
	{
		caret = tmpCaret;

		if (!first)
		{
			caret += 2;
			comment->mData += '\n';
		}

		string buff = trim(ReadWord(section.mData, caret, "\n", ""), " \r");
		comment->mData += buff;

		line = GetLineNumber(section.mData, caret);

		tmpCaret = caret + 1;
		for (; tmpCaret < dataLength; tmpCaret++)
		{
			if (skipSymbols.find(section.mData[tmpCaret]) == string::npos)
				break;
		}

		if (first)
			first = false;
	}
	while (tmpCaret < dataLength - 2 && section.mData[tmpCaret] == '/' && section.mData[tmpCaret + 1] == '/');

	caret = tmpCaret;

	comment->mBegin  = begin;
	comment->mLength = caret - begin;
	comment->mFile   = section.mFile;
	comment->mLine   = line;

	section.mComments.push_back(comment);
}

void CppSyntaxParser::ParseMultilineComment(SyntaxSection& section, int& caret,
											SyntaxProtectionSection& protectionSection)
{
	int begin = caret;
	caret += (int)strlen("/*");
	int end = (int)section.mData.find("*/", caret);
	caret = end;

	SyntaxComment* comment = new SyntaxComment();
	comment->mData = trim(section.mData.substr(begin + 2, end - 4 - begin), " \r\t\n");
	comment->mBegin = begin;
	comment->mLength = caret - begin;
	comment->mFile = section.mFile;

	section.mComments.push_back(comment);
}

void CppSyntaxParser::ParsePragma(SyntaxSection& section, int& caret,
								  SyntaxProtectionSection& protectionSection)
{
	int begin = caret;
	caret += (int)strlen("#pragma");
	ReadWord(section.mData, caret);
}

void CppSyntaxParser::ParseInclude(SyntaxSection& section, int& caret,
								   SyntaxProtectionSection& protectionSection)
{
	int begin = caret;
	caret += (int)strlen("#include");
	ReadWord(section.mData, caret, "\n");
}

void CppSyntaxParser::ParseDefine(SyntaxSection& section, int& caret,
								  SyntaxProtectionSection& protectionSection)
{
	int begin = caret;
	caret += (int)strlen("#define");

	ReadWord(section.mData, caret);
	ReadWord(section.mData, caret, "\n");
}

void CppSyntaxParser::ParseIfMacros(SyntaxSection& section, int& caret,
									SyntaxProtectionSection& protectionSection)
{
	int begin = caret;
	caret += (int)strlen("#if");

	string endWord = "#endif";
	int endIfPos = (int)section.mData.find(endWord, caret);

	if (endIfPos == section.mData.npos)
		caret = (int)section.mData.length();
	else
		caret = endIfPos + (int)endWord.length();
}

void CppSyntaxParser::ParseMetaClass(SyntaxSection& section, int& caret,
									 SyntaxProtectionSection& protectionSection)
{
	ParseClassOrStruct(section, caret, protectionSection, true, true, "");
}

void CppSyntaxParser::ParseClass(SyntaxSection& section, int& caret,
								 SyntaxProtectionSection& protectionSection)
{
	ParseClassOrStruct(section, caret, protectionSection, true, false, "");
}

void CppSyntaxParser::ParseStruct(SyntaxSection& section, int& caret,
								  SyntaxProtectionSection& protectionSection)
{
	ParseClassOrStruct(section, caret, protectionSection, false, false, "");
}

void CppSyntaxParser::ParseClassOrStruct(SyntaxSection& section, int& caret,
										 SyntaxProtectionSection& protectionSection,
										 bool isClass, bool isMeta, const string& templates)
{
	int begin = caret;

	if (isMeta)
		caret += (int)strlen("meta");

	if (isClass) caret += (int)strlen("class");
	else         caret += (int)strlen("struct");

	string className = ReadWord(section.mData, caret, " \n\t\r:;/");
	string afterName = trim(ReadWord(section.mData, caret, ";{/"), " :\r\n\t");

	string shortClassName = className;
	className = section.mFullName.empty() ? className : section.mFullName + "::" + className;

	SyntaxClass* newClass = new SyntaxClass();

	newClass->mBegin              = begin;
	newClass->mLength             = caret - begin;
	newClass->mLine               = GetLineNumber(section.mData, caret);
	newClass->mData               = section.mData.substr(begin, caret - begin);
	newClass->mName               = shortClassName;
	newClass->mFullName           = className;
	newClass->mFile               = section.mFile;
	newClass->mParentSection      = &section;
	newClass->mClassSection       = protectionSection;
	newClass->mTemplateParameters = templates;

	if (!afterName.empty())
	{
		auto baseClasses = Split(afterName, ',');

		for (auto& baseClass : baseClasses)
		{
			trim(baseClass);

			int spacePos = (int)baseClass.find(' ');
			if (spacePos == baseClass.npos)
				newClass->mBaseClasses.push_back(SyntaxClassInheritance(baseClass, SyntaxProtectionSection::Private));
			else
			{
				if (startsWith(baseClass, "virtual"))
				{
					baseClass.erase(0, strlen("virtual") + 1);
					spacePos = (int)baseClass.find(' ');
				}

				string sectionTypeName = baseClass.substr(0, spacePos);
				string baseClassName = baseClass.substr(spacePos + 1);

				if (startsWith(baseClassName, "virtual"))
					baseClassName.erase(0, strlen("virtual") + 1);

				SyntaxProtectionSection sectionType = SyntaxProtectionSection::Private;

				if (sectionTypeName == "public")
					sectionType = SyntaxProtectionSection::Public;
				else if (sectionTypeName == "protected")
					sectionType = SyntaxProtectionSection::Protected;

				newClass->mBaseClasses.push_back(SyntaxClassInheritance(baseClassName, sectionType));
			}
		}
	}

	int dataLength = (int)section.mData.length();

	if (caret < dataLength && section.mData[caret] == '/')
	{
		string comment = ReadWord(section.mData, caret, "\n");
		ReadWord(section.mData, caret, ";{/");
	}

	if (caret < dataLength && section.mData[caret] == '{')
	{
		int sectionBegin = caret;
		newClass->mData = trim(ReadBlock(section.mData, caret), "{} \n\r\t");

		section.mSections.push_back(newClass);

		ParseSyntaxSection(*newClass, newClass->mData, *newClass->mFile, protectionSection);
	}
}

void CppSyntaxParser::ParseTemplate(SyntaxSection& section, int& caret,
									SyntaxProtectionSection& protectionSection)
{
	caret += (int)strlen("template");

	int dataLen = (int)section.mData.length();

	for (; caret < dataLen; caret++)
		if (section.mData[caret] == '<')
			break;

	caret++;
	int braces = 1;
	int begin = caret;

	for (; caret < dataLen; caret++)
	{
		if (section.mData[caret] == '<')
			braces++;

		if (section.mData[caret] == '>')
		{
			braces--;

			if (braces == 0)
				break;
		}
	}

	string tempInside = section.mData.substr(begin, caret - begin);

	int tmpCaret = caret + 1;
	string block = trim(ReadBlock(section.mData, tmpCaret), " \n\r\t");

	if (startsWith(block, "class"))
	{
		caret = (int)section.mData.find("class", caret + 1);
		ParseClassOrStruct(section, caret, protectionSection, true, false, tempInside);
	}
	else if (startsWith(block, "struct"))
	{
		caret = (int)section.mData.find("struct", caret + 1);
		ParseClassOrStruct(section, caret, protectionSection, false, false, tempInside);
	}
	else if (startsWith(block, "metaclass"))
	{
		caret = (int)section.mData.find("metaclass", caret + 1);
		ParseClassOrStruct(section, caret, protectionSection, true, true, tempInside);
	}
	else if (startsWith(block, "friend"))
	{
		caret = (int)section.mData.find("friend", caret + 1);
		ParseFriend(section, caret, protectionSection);
	}
	else if (IsFunction(block))
	{
		auto func = ParseFunction(block, protectionSection, 0, (int)block.length());
		func->mTemplates = tempInside;
		func->mFile = section.mFile;
		func->mLine = GetLineNumber(section.mData, caret);
		section.mFunctions.push_back(func);

		caret = tmpCaret;
	}
	else caret = tmpCaret;
}

void CppSyntaxParser::ParseTypedef(SyntaxSection& section, int& caret,
								   SyntaxProtectionSection& protectionSection)
{
	int begin = caret;
	caret += (int)strlen("typedef");
	string temp = trim(ReadWord(section.mData, caret, ";"), " \r\n\t");
	int lastSpace = (int)temp.rfind(' ');
	string value = trim(temp.substr(0, lastSpace), " \r\t\n");
	string name = trim(temp.substr(lastSpace + 1), " \r\t\n;");

	if (startsWith(value, "typename"))
		value.erase(0, strlen("typename "));

	SyntaxTypedef* synTypedef = new SyntaxTypedef();
	synTypedef->mBegin      = begin;
	synTypedef->mLength     = caret - begin;
	synTypedef->mLine       = GetLineNumber(section.mData, caret);
	synTypedef->mData       = section.mData.substr(begin, caret - begin);
	synTypedef->mWhatName   = value;
	synTypedef->mNewDefName = name;

	section.mTypedefs.push_back(synTypedef);
}

void CppSyntaxParser::ParseEnum(SyntaxSection& section, int& caret,
								SyntaxProtectionSection& protectionSection)
{
	int begin = caret;
	caret += (int)strlen("enum");

	string name = ReadWord(section.mData, caret);

	if (name == "class")
		name = ReadWord(section.mData, caret);

	string block = trim(ReadBlock(section.mData, caret), " {}\r\t\n");
	RemoveComments(block);
	auto content = Split(block, ',');

	SyntaxEnum* newEnum = new SyntaxEnum();
	newEnum->mBegin  = begin;
	newEnum->mLength = caret - begin;
	newEnum->mLine   = GetLineNumber(section.mData, caret);
	newEnum->mData   = section.mData.substr(begin, caret - begin);
	newEnum->mName   = name;
	newEnum->mFile   = section.mFile;

	for (auto& x : content)
	{
		trim(x, " \n\t\r");

		string name, value;
		int valuePos = (int)x.find('=');

		if (valuePos != x.npos)
		{
			name = trim(x.substr(0, valuePos), " \n\t\r");
			value = trim(x.substr(valuePos + 1), " \n\t\r");
		}
		else name = x;

		newEnum->mEntries[name] = value;
	}

	section.mEnums.push_back(newEnum);
}

void CppSyntaxParser::ParseUsing(SyntaxSection& section, int& caret,
								 SyntaxProtectionSection& protectionSection)
{
	int begin = caret;
	caret += (int)strlen("using");

	ReadWord(section.mData, caret);
	string name = trim(ReadWord(section.mData, caret), " \r\n;");

	SyntaxUsingNamespace* newUsing = new SyntaxUsingNamespace();

	newUsing->mBegin              = begin;
	newUsing->mLength             = caret - begin;
	newUsing->mLine               = GetLineNumber(section.mData, caret);
	newUsing->mData               = section.mData.substr(begin, caret - caret);
	newUsing->mUsingNamespaceName = name;

	section.mUsingNamespaces.push_back(newUsing);
}

void CppSyntaxParser::ParsePublicSection(SyntaxSection& section, int& caret,
										 SyntaxProtectionSection& protectionSection)
{
	caret += (int)strlen("public:");
	protectionSection = SyntaxProtectionSection::Public;
}

void CppSyntaxParser::ParsePrivateSection(SyntaxSection& section, int& caret,
										  SyntaxProtectionSection& protectionSection)
{
	caret += (int)strlen("private:");
	protectionSection = SyntaxProtectionSection::Private;
}

void CppSyntaxParser::ParseProtectedSection(SyntaxSection& section, int& caret,
											SyntaxProtectionSection& protectionSection)
{
	caret += (int)strlen("protected:");
	protectionSection = SyntaxProtectionSection::Protected;
}

void CppSyntaxParser::ParseFriend(SyntaxSection& section, int& caret,
								  SyntaxProtectionSection& protectionSection)
{
	int begin = caret;
	caret += (int)strlen("friend");
	ReadWord(section.mData, caret, " \n\r\t");
	ReadWord(section.mData, caret, " \n\r\t");
}

void CppSyntaxParser::ParseAttributeCommentDef(SyntaxSection& section, int& caret, 
											   SyntaxProtectionSection& protectionSection)
{
	SyntaxClass* classSection = dynamic_cast<SyntaxClass*>(&section);

	caret += (int)strlen("ATTRIBUTE_COMMENT_DEFINITION");
	caret = (int)section.mData.find('"', caret) + 1;
	int begin = caret;
	caret = (int)section.mData.find('"', caret);

	classSection->mAttributeCommentDef = section.mData.substr(begin, caret - begin);

	caret = (int)section.mData.find(';', caret) + 1;
}

void CppSyntaxParser::ParseAttributeShortDef(SyntaxSection& section, int& caret, 
											 SyntaxProtectionSection& protectionSection)
{
	SyntaxClass* classSection = dynamic_cast<SyntaxClass*>(&section);

	caret += (int)strlen("ATTRIBUTE_SHORT_DEFINITION");
	caret = (int)section.mData.find('"', caret) + 1;
	int begin = caret;
	caret = (int)section.mData.find('"', caret);

	classSection->mAttributeShortDef = section.mData.substr(begin, caret - begin);

	caret = (int)section.mData.find(';', caret) + 1;
}

void CppSyntaxParser::ParseAttributes(SyntaxSection& section, int& caret, SyntaxProtectionSection& protectionSection)
{
	int begin = caret;

	caret += (int)strlen("ATTRIBUTES");
	caret = (int)section.mData.find('(', caret) + 1;
	string braces = trim(ReadBraces(section.mData, caret), " \n\r\t()");
	caret = (int)section.mData.find(';', caret);

	vector<string> separated = Split(braces, ',');
	for (auto& x : separated)
		trim(x, " \n\r\t,");

	SyntaxAttributes* attributes = new SyntaxAttributes();
	attributes->mBegin  = begin;
	attributes->mLength = caret - begin;
	attributes->mLine   = GetLineNumber(section.mData, caret);
	attributes->mData   = section.mData.substr(begin, caret - caret);
	attributes->mAttributesList = separated;

	section.mAttributes.push_back(attributes);
}

string CppSyntaxParser::ReadWord(const string& data, int& caret,
								 const char* breakSymbols /*= " \n\r(){}.,;+-* /=@!|&*:~\\"*/,
								 const char* skipSymbols /*= " \n\r"*/)
{
	string res = "";
	int braces = 0, sqBraces = 0, trBraces = 0, fgBraces = 0;
	int dataLen = (int)data.length();

	for (; caret < dataLen; caret++)
	{
		int i = 0;
		bool stop = true;
		char s = data[caret];

		while (skipSymbols[i] != '\0')
		{
			if (skipSymbols[i] == s)
			{
				stop = false;
				break;
			}

			i++;
		}

		if (stop)
			break;
	}

	for (; caret < dataLen; caret++)
	{
		int i = 0;
		bool stop = false;
		char s = data[caret];

		if (fgBraces == 0 && braces == 0 && trBraces == 0)
		{
			while (breakSymbols[i] != '\0')
			{
				if (breakSymbols[i] == s)
				{
					stop = true;
					break;
				}

				i++;
			}

			if (stop)
				break;
		}

		switch (s)
		{
		case '{': fgBraces++; break;
		case '}': fgBraces--; break;

		case ')': braces--; break;
		case '(': braces++; break;

		case '[': sqBraces++; break;
		case ']': sqBraces--; break;

		case '<': trBraces++; break;
		case '>': trBraces--; break;
		}

		res += s;
	}

	return res;
}

string CppSyntaxParser::ReadBlock(const string& data, int& caret)
{
	int begin = caret;
	int braces = 0, fgBraces = 0, sqBraces = 0, trBraces = 0;
	bool isInstring = false;
	int dataLen = (int)data.length();

	for (; caret < dataLen; caret++)
	{
		if (data[caret] == '{')
			break;

		if (data[caret] == ';')
			return data.substr(begin, caret - begin);
	}

	caret++;
	fgBraces++;

	bool complete = false;
	for (; caret < dataLen && !complete; caret++)
	{
		if (isInstring)
		{
			if (data[caret] == '"' && data[caret - 1] != '\\')
				isInstring = false;

			continue;
		}

		switch (data[caret])
		{
		case '{': fgBraces++; break;
		case '}':
		fgBraces--;
		if (fgBraces == 0 && braces == 0 && sqBraces == 0)
			complete = true;
		break;

		case ')': braces--; break;
		case '(': braces++; break;

		case '[': sqBraces++; break;
		case ']': sqBraces--; break;

		case '<': trBraces++; break;
		case '>': trBraces--; break;

		case '"':
		isInstring = true;
		break;
		}
	}

	return data.substr(begin, min(caret, dataLen - 1) - begin);
}

string CppSyntaxParser::ReadBraces(const string& data, int& caret)
{
	int begin = caret;
	int braces = 0, fgBraces = 0, sqBraces = 0, trBraces = 0;
	bool isInstring = false;
	int dataLen = (int)data.length();

	for (; caret < dataLen; caret++)
	{
		if (data[caret] == '(')
			break;

		if (data[caret] == ';')
			return data.substr(begin, caret - begin);
	}

	caret++;
	braces++;

	bool complete = false;
	for (; caret < dataLen && !complete; caret++)
	{
		if (isInstring)
		{
			if (data[caret] == '"' && data[caret - 1] != '\\')
				isInstring = false;

			continue;
		}

		switch (data[caret])
		{
		case '}': fgBraces--; break;
		case '{': fgBraces++; break;

		case '[': sqBraces++; break;
		case ']': sqBraces--; break;

		case '<': trBraces++; break;
		case '>': trBraces--; break;

		case '(': braces++; break;
		case ')':
		braces--;
		if (fgBraces == 0 && braces == 0 && sqBraces == 0 && trBraces == 0)
			complete = true;
		break;

		case '"':
		isInstring = true;
		break;
		}
	}

	string res = data.substr(begin, min(caret, dataLen) - begin);
	int resLen = (int)res.length();

	if (!res.empty() && res[resLen - 1] == ')')
		res.erase(resLen - 1, 1);

	if (!res.empty() && res[0] == '(')
		res.erase(0, 1);

	return res;
}

char CppSyntaxParser::GetNextSymbol(const string& data, int begin, const char* skipSymbols /*= " \n\r\t()[]{}"*/)
{
	int dataLen = (int)data.length();
	for (; begin < dataLen; begin++)
	{
		int i = 0;
		bool stop = true;
		char s = data[begin];

		while (skipSymbols[i] != '\0')
		{
			if (skipSymbols[i] == s)
			{
				stop = false;
				break;
			}

			i++;
		}

		if (stop)
			return s;
	}

	return '\0';
}

vector<string> CppSyntaxParser::Split(const string& data, char splitSymbol)
{
	vector<string> res;
	int braces = 0, sqBraces = 0, trBraces = 0, fgBraces = 0;
	int dataLen = (int)data.length();

	int lastSplit = 0;
	for (int i = 0; i < dataLen; i++)
	{
		switch (data[i])
		{
		case '{': fgBraces++; break;
		case '}': fgBraces--; break;

		case '(': braces++; break;
		case ')': braces--; break;

		case '<': trBraces++; break;
		case '>': trBraces--; break;

		case '[': sqBraces++; break;
		case ']': sqBraces--; break;
		}

		if (braces == 0 && sqBraces == 0 && trBraces == 0 && fgBraces == 0 && data[i] == splitSymbol)
		{
			res.push_back(data.substr(lastSplit, i - lastSplit));
			lastSplit = i + 1;
		}
	}

	res.push_back(data.substr(lastSplit));

	return res;
}

void CppSyntaxParser::RemoveComments(string& input)
{
	int len = (int)input.length();
	int offs = 0;
	for (int i = 0; i < len; i++)
	{
		if (input[i] == '/' && i < len - 1 && input[i + 1] == '/')
		{
			for (; i < len; i++, offs++)
			{
				if (input[i] == '\n')
					break;
			}
		}

		if (input[i] == '/' && i < len - 1 && input[i + 1] == '*')
		{
			for (; i < len; i++, offs++)
			{
				if (input[i - 1] == '*' && input[i] == '/')
					break;
			}
		}

		input[i - offs] = input[i];
	}
}

CppSyntaxParser::ExpressionParser::ExpressionParser(const char* keyWord, ParserDelegate parser,
													bool isPossibleInClass /*= true*/,
													bool isPossibleInNamespace /*= true*/):
	keyWord(keyWord), parser(parser), isPossibleInClass(isPossibleInClass), isPossibleInNamespace(isPossibleInNamespace)
{}