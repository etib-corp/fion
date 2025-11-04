#pragma once

#include <string>
#include <cstdint>
#include <map>

namespace fion
{

    class URL
    {
    private:
        std::string scheme;
        std::string domainName;
        std::uint16_t port;
        std::string pathToResource;
        std::map<std::string, std::string> queryParameters;
        std::string anchor;

    public:
        URL(void);
        URL(const std::string &rawURL);

        std::string getScheme(void) const;
        std::string getDomainName(void) const;
        std::uint16_t getPort(void) const;
    };

} // namespace fion