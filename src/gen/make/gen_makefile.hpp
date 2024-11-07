#pragma once

#include "../../config/config.hpp"
#include "../../error/error.hpp"

#include <fstream>
#include <vector>
#include <string_view>

namespace tram {
namespace gen {
    TramError create_make_file();

    namespace internal {
        void make_variable(std::ofstream& file_out, std::string_view var_name, std::string value, bool indent = false) noexcept;

        void make_config(std::ofstream& file_out, const std::vector<tram::internal::build_filter_conf>& configs) noexcept;
    }
}
}
