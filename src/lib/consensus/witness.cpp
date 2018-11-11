/**
 * Copyright (c) 2016-2018 metaverse core developers (see MVS-AUTHORS)
 *
 * This file is part of metaverse.
 *
 * metaverse is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <metaverse/consensus/witness.hpp>
#include <metaverse/node/p2p_node.hpp>
#include <metaverse/blockchain/settings.hpp>


#define LOG_HEADER "witness"

namespace libbitcoin {
namespace consensus {

witness::witness(p2p_node& node)
    : node_(node)
    , setting_(node_.chain_impl().chain_settings())
{
}

witness::~witness()
{
}

bool witness::exists(const list& l, const witness_id& id)
{
    auto cmp = [&id](const witness_id& item){return id == item;};
    return std::find_if(std::begin(l), std::end(l), cmp) != l.end();
}

witness::list witness::get_witness_list() const
{
    shared_lock lock(mutex_);
    return witness_list_;
}

witness::list witness::get_candidate_list() const
{
    shared_lock lock(mutex_);
    return candidate_list_;
}

bool witness::is_witness(const witness_id& id) const
{
    shared_lock lock(mutex_);
    return exists(witness_list_, id);
}

bool witness::register_witness(const witness_id& id)
{
    upgrade_lock lock(mutex_);

    if (exists(witness_list_, id) || exists(candidate_list_, id)) {
        log::debug(LOG_HEADER) << "In register_witness, " << id << " is already registered.";
        return false;
    }

    upgrade_to_unique_lock ulock(lock);
    candidate_list_.push_back(id);
    return true;
}

bool witness::unregister_witness(const witness_id& id)
{
    upgrade_lock lock(mutex_);

    if (! (exists(witness_list_, id) || exists(candidate_list_, id))) {
        log::debug(LOG_HEADER) << "In unregister_witness, " << id << " is not registered.";
        return false;
    }

    upgrade_to_unique_lock ulock(lock);
    witness_list_.remove(id);
    candidate_list_.remove(id);
    return true;
}

// DPOS_TODO: add algorithm to generate the witness list of each epoch
bool witness::update_witness_list(uint64_t height)
{
    return true;
}

bool witness::sign(endorsement& out, const ec_secret& secret, const header& h)
{
    const auto sighash = h.hash();

    ec_signature signature;
    if (!bc::sign(signature, secret, sighash) || !bc::encode_signature(out, signature)) {
        return false;
    }

    out.push_back(h.number & 0xff);
    return true;
}

bool witness::verify_sign(const endorsement& out, const data_chunk& public_key, const header& h)
{
    if (public_key.empty()) {
        return false;
    }

    if (out.back() != (h.number & 0xff)) {
        return false;
    }

    auto distinguished = out;
    distinguished.pop_back();

    ec_signature signature;

    if (!parse_signature(signature, distinguished, true)) {
        return false;
    }

    const auto sighash = h.hash();

    return bc::verify_signature(public_key, sighash, signature);
}

} // consensus
} // libbitcoin