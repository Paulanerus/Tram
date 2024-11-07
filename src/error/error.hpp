#pragma once

#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <system_error>

namespace tram {
enum class ErrorCode {
    None,
    System,
    MissingConfigFile,
    DirAlreadyExists,
    UnableToCreateFile,
    UnableToCreateMakefile,
};

class TramError {
public:
    TramError(ErrorCode&& code, std::string&& error_message)
        : m_ErrorCode(std::move(code))
        , m_ErrorMessage(std::move(error_message))
    {
    }

    ErrorCode code() const noexcept
    {
        return m_ErrorCode;
    }

    std::string_view message() const noexcept
    {
        return m_ErrorMessage;
    }

    bool is(ErrorCode code) const noexcept
    {
        return this->m_ErrorCode == code;
    }

    bool is(const std::initializer_list<ErrorCode>& codes) const noexcept
    {
        for (auto& c : codes) {
            if (this->is(c))
                return true;
        }

        return false;
    }

    operator bool() const noexcept
    {
        return !is_none();
    }

    bool is_none() const noexcept
    {
        return is(ErrorCode::None);
    }

    void report() const noexcept
    {
        if (is_none())
            return;

        std::cout << "[" << code_to_str() << "] " << m_ErrorMessage << std::endl;
    }

private:
    ErrorCode m_ErrorCode;

    std::string m_ErrorMessage;

    std::string code_to_str() const noexcept
    {
        switch (m_ErrorCode) {
        case ErrorCode::System:
            return "System";
        case ErrorCode::MissingConfigFile:
            return "MissingConfigFile";
        case ErrorCode::UnableToCreateFile:
            return "UnableToCreateFile";
        case ErrorCode::DirAlreadyExists:
            return "DirAlreadyExists";
        case ErrorCode::UnableToCreateMakefile:
            return "UnableToCreateMakefile";
        default:
            return "None";
        }
    }
};

inline TramError make_error(ErrorCode&& code, std::string&& message = "") noexcept
{
    return TramError { std::move(code), std::move(message) };
}

inline TramError make_systemerror(const std::error_code& ec) noexcept
{
    return make_error(ErrorCode::System, ec.message());
}

inline const TramError NO_ERROR = make_error(ErrorCode::None);
}
