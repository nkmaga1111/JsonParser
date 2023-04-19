#pragma once
#include "JsonValue.h"

class JsonParser {
public:
    void Parse(std::string data);

    std::string Dump();

    JsonValuePtr GetRoot() { return root_; }

private:
    JsonValuePtr ParseObject(std::string::const_iterator& it, const std::string::const_iterator& end);
    JsonValuePtr ParseArray(std::string::const_iterator& it, const std::string::const_iterator& end);
    JsonValuePtr ParseValue(std::string::const_iterator& it, const std::string::const_iterator& end);

    std::string ParseString(std::string::const_iterator& it, const std::string::const_iterator& end);
    JsonValuePtr ParseNumber(std::string::const_iterator& it, const std::string::const_iterator& end);
    JsonValuePtr ParseBoolean(std::string::const_iterator& it, const std::string::const_iterator& end);
    JsonValuePtr ParseNull(std::string::const_iterator& it, const std::string::const_iterator& end);

    void SkipWhiteSpace(std::string::const_iterator& it, const std::string::const_iterator& end);

private:
    JsonValuePtr root_;
};

