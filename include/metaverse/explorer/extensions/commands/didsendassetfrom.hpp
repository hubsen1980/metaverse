/**
 * Copyright (c) 2016-2018 mvs developers
 *
 * This file is part of metaverse-explorer.
 *
 * metaverse-explorer is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#pragma once
#include <metaverse/explorer/define.hpp>
#include <metaverse/explorer/extensions/command_extension.hpp>
#include <metaverse/explorer/extensions/command_extension_func.hpp>
#include <metaverse/explorer/extensions/command_assistant.hpp>

namespace libbitcoin {
namespace explorer {
namespace commands {


/************************ didsendassetfrom *************************/

class didsendassetfrom: public command_extension
{
public:
    static const char* symbol(){ return "didsendassetfrom";}
    const char* name() override { return symbol();}
    bool category(int bs) override { return (ex_online & bs ) == bs; }
    const char* description() override { return "didsendassetfrom "; }

    arguments_metadata& load_arguments() override
    {
        return get_argument_metadata()
            .add("ACCOUNTNAME", 1)
            .add("ACCOUNTAUTH", 1)
            .add("FROM_", 1)
            .add("TO_", 1)
            .add("SYMBOL", 1)
            .add("AMOUNT", 1);
    }

    void load_fallbacks (std::istream& input,
        po::variables_map& variables) override
    {
        const auto raw = requires_raw_input();
        load_input(auth_.name, "ACCOUNTNAME", variables, input, raw);
        load_input(auth_.auth, "ACCOUNTAUTH", variables, input, raw);
        load_input(argument_.fromdid, "FROM_", variables, input, raw);
        load_input(argument_.todid, "TO_", variables, input, raw);
        load_input(argument_.symbol, "SYMBOL", variables, input, raw);
        load_input(argument_.amount, "AMOUNT", variables, input, raw);
    }

    options_metadata& load_options() override
    {
        using namespace po;
        options_description& options = get_option_metadata();
        options.add_options()
        (
            BX_HELP_VARIABLE ",h",
            value<bool>()->zero_tokens(),
            "Get a description and instructions for this command."
        )
        (
            "ACCOUNTNAME",
            value<std::string>(&auth_.name)->required(),
            BX_ACCOUNT_NAME
        )
        (
            "ACCOUNTAUTH",
            value<std::string>(&auth_.auth)->required(),
            BX_ACCOUNT_AUTH
        )
        (
            "FROM_",
            value<std::string>(&argument_.fromdid)->required(),
            "From did/address"
        )
        (
            "TO_",
            value<std::string>(&argument_.todid)->required(),
            "Target did/address"
        )
        (
            "SYMBOL",
            value<std::string>(&argument_.symbol)->required(),
            "Asset symbol"
        )
        (
            "AMOUNT",
            value<uint64_t>(&argument_.amount)->required(),
            "Asset integer bits. see asset <decimal_number>."
        )
        (
            "change,c",
            value<std::string>(&option_.change_address),
            "Change to this did/address"
        )
        (
            "model,m",
            value<std::string>(&option_.attenuation_model_param),
            BX_MST_OFFERING_CURVE
        )
        (
            "fee,f",
            value<uint64_t>(&argument_.fee)->default_value(10000),
            "Transaction fee. defaults to 10000 ETP bits"
        )
        (
            "message,i",
            value<std::string>(&option_.message),
            "Information attached to this transaction"
        );

        return options;
    }

    void set_defaults_from_config (po::variables_map& variables) override
    {
    }

    console_result invoke (Json::Value& jv_output,
         libbitcoin::server::server_node& node) override;

    struct argument
    {
        std::string fromdid;
        std::string todid;
        std::string symbol;
        uint64_t amount;
        uint64_t fee;
    } argument_;

    struct option
    {
        std::string attenuation_model_param;
        std::string message;
        std::string change_address;
    } option_;

};

} // namespace commands
} // namespace explorer
} // namespace libbitcoin

