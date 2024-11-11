#include "gen_makefile.hpp"

#include <arg_parser.hpp>

#include "../../config/config.hpp"
#include "../../utility/utility.hpp"

#include <filesystem>
#include <format>
#include <fstream>
#include <ios>

namespace tram {
namespace gen {

    TramError create_make_file()
    {
        const auto& conf = tram::config();

        const auto& settings = conf.settings();
        const auto& build_conf = conf.build();

        if (auto err = tram::fs::create_dir_if_notexists(tram::fs::TRAM_TEMP); err && !err.is(ErrorCode::DirAlreadyExists))
            return err;

        std::filesystem::path temp_dir { tram::fs::TRAM_TEMP };

        std::ofstream file_out { temp_dir / "Makefile", std::ios::trunc };

        if (!file_out.is_open())
            return make_error(ErrorCode::UnableToCreateMakefile);

        file_out << ".SILENT:\n";
        file_out << "config ?= debug\n\n";

        internal::make_variable(file_out, "NAME", settings.name);
        internal::make_variable(file_out, "VERSION", settings.version);

        file_out << "\n";

        internal::make_variable(file_out, "CXX", "g++");
        internal::make_variable(file_out, "LANG_STD", std::format("-std={}", internal::validate_lang(build_conf.lang)));
        internal::make_variable(file_out, "ARCH", std::format("-m{}", build_conf.arch));

        file_out << "\n";

        file_out << "SRC_FILES := ";

        auto src_files = tram::fs::collect_src_files(build_conf.src_files);

        for (auto& file : src_files)
            file_out << file.string() << " ";

        file_out << "\n";

        internal::make_variable(file_out, "OBJ_DIR", std::format("{}/$(NAME)/$(config)/obj", tram::fs::TRAM_BUILD));
        internal::make_variable(file_out, "OBJ_FILES", "$(foreach src,$(SRC_FILES),$(OBJ_DIR)/$(notdir $(src:.cpp=.o)))");

        file_out << "INCLUDE_PATHS := ";

        for (auto& include : build_conf.include_files)
            file_out << std::format("-I{} ", include);

        file_out << "\n\n";

        internal::make_variable(file_out, "WARN_FLAGS", internal::translate_warning_to_flags(build_conf.warning));

        file_out << "\n";

        internal::make_config(file_out, conf.build_configurations());

        internal::make_variable(file_out, "LDFLAGS", std::format("-L/usr/lib{}", build_conf.arch));
        internal::make_variable(file_out, "LDLIBS", "");

        file_out << "\n";

        internal::make_variable(file_out, "OUT_DIR", "bin/$(config)");
        internal::make_variable(file_out, "OUT_BIN", std::format("$(OUT_DIR)/{}", build_conf.filename));

        file_out << "\n";

        file_out << "$(OBJ_DIR)/%.o: */%.cpp\n";
        file_out << "\t@echo \"\033[0;35m Compiling\033[0m $<\"\n";
        file_out << "\t@mkdir -p $(OBJ_DIR)\n";
        file_out << "\t@$(CXX) $(LANG_STD) $(ARCH) $(DEFINES) $(SYMBOLS) $(OPT_FLAGS) $(WARN_FLAGS) $(INCLUDE_PATHS) -c $< -o $@\n";

        file_out << "\n";

        file_out << "$(OUT_BIN): $(OBJ_FILES)\n";
        file_out << "\t@echo \"\033[0;35m   Linking\033[0m $(NAME)\"\n";
        file_out << "\t@mkdir -p $(OUT_DIR)\n";
        file_out << "\t@$(CXX) $(OBJ_FILES) -o $@ $(LDFLAGS) $(LDLIBS)\n\n";

        file_out << "all: $(OUT_BIN)\n";

        file_out << ".PHONY: all";

        return internal::write_filetime();
    }

    namespace internal {

        void make_variable(std::ofstream& file_out, std::string_view var_name, std::string value, bool indent) noexcept
        {
            file_out << std::format("{}{} := {}\n", indent ? "\t" : "", var_name, value);
        }

        void make_config(std::ofstream& file_out, const std::vector<tram::internal::build_filter_conf>& configs) noexcept
        {
            if (configs.empty())
                return;

            for (std::size_t i {}; i < configs.size(); i++) {
                const auto& conf = configs[i];

                file_out << std::format("{}ifeq ($(config), {})\n", i == 0 ? "" : "else ", conf.name);

                file_out << "\tDEFINES := ";

                for (auto& define : conf.defines)
                    file_out << std::format("-D{} ", define);

                file_out << "\n";

                internal::make_variable(file_out, "SYMBOLS", conf.symbols ? "-g" : "", true);
                internal::make_variable(file_out, "OPT_FLAGS", conf.optimize ? "-O2" : "", true);
            }

            file_out << "endif\n\n";
        }

        TramError write_filetime() noexcept
        {
            std::ofstream file_out { fs::TRAM_LAST_MODIFIED_FILE.data(), std::ios::trunc };

            if (!file_out.is_open())
                return make_error(ErrorCode::UnableToCreateFile, "test file could not be created.");

            file_out << fs::last_modified_time(fs::TRAM_PROJECT_FILE);

            return NO_ERROR;
        }

        std::string validate_lang(std::string lang)
        {
            psap::string::convert_str_to_lower(lang);

            if (!lang.starts_with("c"))
                return "c++11";

            return lang;
        }

        std::string translate_warning_to_flags(std::string_view warning)
        {
            if (warning == "off" || warning == "Off")
                return "-w";

            if (warning == "extra" || warning == "Extra")
                return "-Wall -Wextra";

            return "";
        }
    }
}
}
