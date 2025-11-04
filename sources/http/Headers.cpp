#include <sstream>

#include "http/Headers.hpp"

fion::http::Headers::Headers(void) {}

fion::http::Headers::~Headers(void) {}

void fion::http::Headers::set(const std::string &key, const std::string &value)
{
    _headers[key] = value;
}

std::string fion::http::Headers::get(const std::string &key) const
{
    if (_headers.find(key) != _headers.end())
        return _headers.at(key);
    throw std::invalid_argument("Header not found: " + key);
}

std::string fion::http::Headers::get(const std::string &key, const std::string &defaultValue) const
{
    if (_headers.find(key) != _headers.end())
        return _headers.at(key);
    return defaultValue;
}

std::optional<std::string> fion::http::Headers::getOptional(const std::string &key) const
{
    if (_headers.find(key) != _headers.end())
        return _headers.at(key);
    return std::nullopt;
}

bool fion::http::Headers::has(const std::string &key) const
{
    return _headers.find(key) != _headers.end();
}

bool fion::http::Headers::remove(const std::string &key)
{
    return _headers.erase(key) > 0;
}

void fion::http::Headers::clear(void)
{
    _headers.clear();
}

std::vector<std::string> fion::http::Headers::getKeys(void) const
{
    std::vector<std::string> keys;
    keys.reserve(_headers.size());
    for (const auto &header : _headers)
    {
        keys.push_back(header.first);
    }
    return keys;
}

size_t fion::http::Headers::size(void) const
{
    return _headers.size();
}

bool fion::http::Headers::empty(void) const
{
    return _headers.empty();
}

const std::map<std::string, std::string> &fion::http::Headers::getAll(void) const
{
    return _headers;
}

void fion::http::Headers::parseFromRaw(const std::string &rawHeaders)
{
    std::istringstream stream(rawHeaders);
    std::string line;
    while (std::getline(stream, line) && line != "\r")
    {
        auto delimiterPos = line.find(": ");
        if (delimiterPos != std::string::npos)
        {
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 2);
            
            // Remove trailing \r if present
            if (!value.empty() && value.back() == '\r')
                value.pop_back();
            
            _headers[key] = value;
        }
    }
}

std::string fion::http::Headers::toRawString(void) const
{
    std::ostringstream rawHeaders;
    for (const auto &header : _headers)
    {
        rawHeaders << header.first << ": " << header.second << "\r\n";
    }
    return rawHeaders.str();
}
