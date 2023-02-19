#pragma once

#include "FileState.h"

#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <string_view>

#include <rapidjson/document.h>

struct RapidJsonDomNode
{
};

class RapidJsonDomFile : public JsonFile
{
public:

public:
	rapidjson::Document m_Doc;
	std::vector<char> m_Buffer;
};
