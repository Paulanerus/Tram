#pragma once

#include "../../config/config.hpp"
#include "../../error/error.hpp"

#include <fstream>
#include <string_view>
#include <vector>

namespace tram {
namespace gen {
    TramError create_make_file();

    namespace internal {
        void make_variable(std::ofstream& file_out, std::string_view var_name, std::string value, bool indent = false) noexcept;

        void make_config(std::ofstream& file_out, const std::vector<tram::internal::build_filter_conf>& configs) noexcept;

        TramError write_filetime() noexcept;

        std::string validate_lang(std::string lang);

        std::string translate_warning_to_flags(std::string_view warning);
    }
}
}
