#pragma once

#include <string>
#include <map>
#include <optional>
#include <vector>

namespace fion::http
{
    /**
     * @brief A class to manage HTTP headers for both requests and responses
     *
     * This class provides a clean interface for managing HTTP headers,
     * including adding, retrieving, checking existence, and removing headers.
     */
    class Headers
    {
    private:
        std::map<std::string, std::string> _headers;

    public:
        Headers(void);
        ~Headers(void);

        /**
         * @brief Set a header key-value pair
         *
         * @param key The header name
         * @param value The header value
         */
        void set(const std::string &key, const std::string &value);

        /**
         * @brief Get a header value by key
         *
         * @param key The header name
         * @return The header value if found
         * @throws std::invalid_argument if header is not found
         */
        std::string get(const std::string &key) const;

        /**
         * @brief Get a header value by key, with optional default
         *
         * @param key The header name
         * @param defaultValue The default value to return if header is not found
         * @return The header value if found, otherwise defaultValue
         */
        std::string get(const std::string &key, const std::string &defaultValue) const;

        /**
         * @brief Get a header value by key, returning std::optional
         *
         * @param key The header name
         * @return std::optional containing the value if found, std::nullopt otherwise
         */
        std::optional<std::string> getOptional(const std::string &key) const;

        /**
         * @brief Check if a header exists
         *
         * @param key The header name
         * @return true if the header exists, false otherwise
         */
        bool has(const std::string &key) const;

        /**
         * @brief Remove a header by key
         *
         * @param key The header name
         * @return true if the header was removed, false if it didn't exist
         */
        bool remove(const std::string &key);

        /**
         * @brief Clear all headers
         */
        void clear(void);

        /**
         * @brief Get all header keys
         *
         * @return A vector containing all header keys
         */
        std::vector<std::string> getKeys(void) const;

        /**
         * @brief Get the number of headers
         *
         * @return The number of headers
         */
        size_t size(void) const;

        /**
         * @brief Check if there are no headers
         *
         * @return true if there are no headers, false otherwise
         */
        bool empty(void) const;

        /**
         * @brief Get the underlying map of headers
         *
         * @return A const reference to the headers map
         */
        const std::map<std::string, std::string> &getAll(void) const;

        /**
         * @brief Parse headers from a raw string
         *
         * @param rawHeaders The raw headers string
         */
        void parseFromRaw(const std::string &rawHeaders);

        /**
         * @brief Convert headers to raw string format
         *
         * @return The raw headers string
         */
        std::string toRawString(void) const;
    };

} // namespace fion::http
