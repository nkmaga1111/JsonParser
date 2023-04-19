#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <stdexcept>

constexpr uint32_t IndentSpace = 2;

class JsonValue;
using JsonValuePtr = std::shared_ptr<JsonValue>;

class JsonValue {
public:
    virtual ~JsonValue() = default;

    virtual bool IsString() { return false; }
    virtual bool IsNumber() { return false; }
    virtual bool IsBoolean() { return false; }
    virtual bool IsNull() { return false; }
    virtual bool IsObject() { return false; }
    virtual bool IsArray() { return false; }

    virtual std::string GetString() { throw std::logic_error("not a string"); }
    virtual int64_t GetInteger() { throw std::logic_error("not a string"); }
    virtual double GetDouble() { throw std::logic_error("not a string"); }
    virtual bool GetBoolean() { throw std::logic_error("not a string"); }

    virtual std::string Dump(int indent) = 0;
};

class JsonString : public JsonValue {
public:
    JsonString(std::string value) : value_(value) {}
    virtual ~JsonString() = default;

    virtual bool IsString() { return true; }

    virtual std::string GetString() {
        return value_;
    }

    virtual void SetString(std::string value) {
        value_ = value;
    }

    virtual std::string Dump(int indent) {
        return '"' + value_ + '"';
    }

private:
    std::string value_;
};

class JsonInteger : public JsonValue {
public:
    JsonInteger(std::string raw, int64_t value) : raw_(raw), value_(value) {}
    virtual ~JsonInteger() = default;

    virtual bool IsNumber() { return true; }

    virtual int64_t GetInteger() {
        return value_;
    }

    virtual void SetInteger(int64_t value) {
        value_ = value;
    }

    virtual std::string Dump(int indent) {
        return raw_;
    }

private:
    int64_t value_;
    std::string raw_;
};

class JsonDouble : public JsonValue {
public:
    JsonDouble(std::string raw, double value) : raw_(raw), value_(value) {}
    virtual ~JsonDouble() = default;

    virtual bool IsNumber() { return true; }

    virtual double GetDouble() {
        return value_;
    }

    virtual void SetDouble(double value) {
        value_ = value;
    }

    virtual std::string Dump(int indent) {
        return raw_;
    }

private:
    double value_;
    std::string raw_;
};

class JsonBoolean : public JsonValue {
public:
    JsonBoolean(bool value) : value_(value) {}
    virtual ~JsonBoolean() = default;

    virtual bool IsBoolean() { return true; }

    virtual bool GetBoolean() {
        return value_;
    }
    virtual void SetBoolean(bool value) {
        value_ = value;
    }

    virtual std::string Dump(int indent) {
        return value_ ? "true" : "false";
    }

private:
    bool value_;
};

class JsonNull : public JsonValue {
public:
    virtual ~JsonNull() = default;

    virtual bool IsNull() { return true; }

    virtual std::string Dump(int indent) {
        return "null";
    }
};

class JsonArray : public JsonValue {
public:
    virtual ~JsonArray() = default;

    virtual bool IsArray() { return true; }

    JsonValuePtr& operator[](std::size_t index) {
        if (index >= values_.size())
            throw std::logic_error("subscript overflow!");

        return values_[index];
    }

    void AddItem(JsonValuePtr value) {
        values_.push_back(value);
    }

    virtual std::string Dump(int indent) {
        std::string prefix((indent + 1) * IndentSpace, ' ');
        std::string str = "[\n";
        for (auto i = 0ull; i < values_.size(); i++) {
            str += prefix;
            str += values_[i]->Dump(indent + 1);
            if (i == values_.size() - 1)
                str += "\n";
            else
                str += ",\n";
        }
        str += std::string(indent * IndentSpace, ' ') + ']';
        return str;
    }

private:
    std::vector<JsonValuePtr> values_;
};

class JsonObject : public JsonValue {
public:
    virtual ~JsonObject() = default;

    virtual bool IsObject() { return true; }

    JsonValuePtr& operator[](const std::string& key) {
        auto iter = std::find_if(values_.begin(), values_.end(), [&key](auto& pair) {
            return pair.first == key;
                                 });
        if (iter == values_.end())
            throw std::logic_error("key not exists!");

        return iter->second;
    }

    void AddKeyValue(std::string key, JsonValuePtr value) {
        values_.emplace_back(key, value);
    }

    virtual std::string Dump(int indent) {
        std::string prefix((indent + 1) * IndentSpace, ' ');
        std::string str = "{\n";
        for (auto i = 0ull; i < values_.size(); i++) {
            str += prefix;
            str += '"' + values_[i].first + '"';
            str += ": ";
            str += values_[i].second->Dump(indent + 1);
            if (i == values_.size() - 1)
                str += "\n";
            else
                str += ",\n";
        }
        str += std::string(indent * IndentSpace, ' ') + '}';
        return str;
    }

private:
    std::vector<std::pair<std::string, JsonValuePtr>> values_;
};