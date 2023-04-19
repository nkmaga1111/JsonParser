#include "JsonParser.h"

void JsonParser::Parse(std::string data) {
    auto it = data.begin();

    SkipWhiteSpace(it, data.end());

    if (*it == '{') {
        root_ = ParseObject(++it, data.end());
    } else if (*it == '[') {
        root_ = ParseArray(++it, data.end());
    } else {
        root_ = ParseValue(it, data.end());
    }
}

std::string JsonParser::Dump() {
    return root_->Dump(0);
}

JsonValuePtr JsonParser::ParseObject(std::string::const_iterator& it, const std::string::const_iterator& end) {
    SkipWhiteSpace(it, end);

    auto obj = std::make_shared<JsonObject>();

    while (it != end) {
        if (*it == '}') {
            ++it;
            break;
        }

        SkipWhiteSpace(it, end);

        if (*it == ',') {
            ++it;
            SkipWhiteSpace(it, end);
        }

        auto key = ParseString(++it, end);

        SkipWhiteSpace(it, end);

        if (*it != ':')
            throw std::logic_error("json object missing : ");

        ++it;
        SkipWhiteSpace(it, end);

        auto value = ParseValue(it, end);

        SkipWhiteSpace(it, end);

        if (*it != ',' && *it != '}')
            throw std::logic_error("wrong object format!");

        obj->AddKeyValue(key, value);
    }

    return obj;
}

JsonValuePtr JsonParser::ParseArray(std::string::const_iterator& it, const std::string::const_iterator& end) {
    SkipWhiteSpace(it, end);

    auto arr = std::make_shared<JsonArray>();

    while (it != end) {
        if (*it == ']') {
            ++it;
            break;
        }

        SkipWhiteSpace(it, end);

        if (*it == ',') {
            ++it;
            SkipWhiteSpace(it, end);
        }

        auto value = ParseValue(it, end);

        SkipWhiteSpace(it, end);

        if (*it != ',' && *it != ']')
            throw std::logic_error("wrong object format!");

        arr->AddItem(value);
    }

    return arr;
}

JsonValuePtr JsonParser::ParseValue(std::string::const_iterator& it, const std::string::const_iterator& end) {
    if (*it == '"') {
        return std::make_shared<JsonString>(ParseString(++it, end));
    } else if (*it == '-' || std::isdigit(*it)) {
        return ParseNumber(it, end);
    } else if (*it == 'f' || *it == 't') {
        return ParseBoolean(it, end);
    } else if (*it == 'n') {
        return ParseNull(it, end);
    } else if (*it == '{') {
            return ParseObject(++it, end);
     } else if (*it == '[') {
         return ParseArray(++it, end);
    } else {
        throw std::logic_error("parse error!");
    }
}

std::string JsonParser::ParseString(std::string::const_iterator& it, const std::string::const_iterator& end) {
    auto start = it;
    while (it != end) {
        if (*it == '"') {
            ++it;
            break;
        }

        if (*it == '\\') {
            ++it;
            if (it == end)
                throw std::logic_error("wrong escape prefix");

            if (*it == '"' || *it == '\\' || *it == '/' || *it == 'b' ||
                *it == 'f' || *it == 'n' || *it == 'r' || *it == 't') {
                ++it; 
            } else if (*it == 'u') {
                ++it;
                if (std::distance(it, end) < 4)
                    throw std::logic_error("wrong unicode codepoint");

                for (auto i = 0; i < 4; i++) {
                    if (!std::isxdigit(*it))
                        throw std::logic_error("wrong unicode codepoint number");
                    ++it;
                }
            } else {
                throw std::logic_error("wrong escape character");
            }
        } else {
            ++it;
        }
    }

    return std::string(start, it - 1);
}

JsonValuePtr JsonParser::ParseNumber(std::string::const_iterator& it, const std::string::const_iterator& end) {
    auto start = it;

    bool is_double = false;

    ++it;

    while (it != end && std::isdigit(*it)) {
        ++it;
    }

    if (*it == '.') {
        ++it;
        while (it != end && std::isdigit(*it)) {
            ++it;
        }
        is_double = true;
    }

    if (*it == 'E' || *it == 'e') {
        ++it;
        if (it == end)
            throw std::logic_error("wrong scientific notation");
        ++it;
        if (*it == '-' || *it == '+')
            ++it;

        is_double = true;
    }

    while (it != end && std::isdigit(*it)) {
        ++it;
    }

    std::string number(start, it);

    if (is_double) {
        double value = std::strtod(number.data(), nullptr);
        return std::make_shared<JsonDouble>(number, value);
    } else {
        int64_t value = std::strtoll(number.data(), nullptr, 10);
        return std::make_shared<JsonInteger>(number, value);
    }
}

JsonValuePtr JsonParser::ParseBoolean(std::string::const_iterator& it, const std::string::const_iterator& end) {
    bool result = false;

    if (*it == 'f') {
        if (std::distance(it, end) < 5)
            throw std::logic_error("wrong boolean false");

        std::string value(it, it + 5);
        if (value.compare("false") != 0)
            throw std::logic_error("wrong boolean value");

        std::advance(it, 5);
    } else {
        if (std::distance(it, end) < 4)
            throw std::logic_error("wrong boolean true");

        std::string value(it, it + 4);
        if (value.compare("true") != 0)
            throw std::logic_error("wrong boolean value");

        result = true;
        std::advance(it, 4);
    }

    return std::make_shared<JsonBoolean>(result);
}

JsonValuePtr JsonParser::ParseNull(std::string::const_iterator& it, const std::string::const_iterator& end) {
    if (std::distance(it, end) < 4)
        throw std::logic_error("wrong null value");

    std::string value(it, it + 4);
    if (value.compare("null") != 0)
        throw std::logic_error("wrong boolean value");

    std::advance(it, 4);
    return std::make_shared<JsonNull>();
}

void JsonParser::SkipWhiteSpace(std::string::const_iterator& it, const std::string::const_iterator& end) {
    while (it != end) {
        if (!std::isspace(*it)) {
            break;
        }
        ++it;
    }
}
