#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/print.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

CONTRACT pinkbankroll : public contract {
  public:
    using contract::contract;
    pinkbankroll(name receiver, name code, datastream<const char*> ds):contract(receiver, code, ds),
    rollsTable(receiver, receiver.value),
    investorsTable(receiver, receiver.value),
    statsTable(receiver, receiver.value)
    {}
    
    ACTION init();
    ACTION announceroll(name creator, uint64_t creator_id, uint32_t upper_range_limit, name rake_recipient);
    ACTION announcebet(name creator, uint64_t creator_id, name bettor, asset amount, uint32_t lower_bound, uint32_t upper_bound, uint32_t muliplier);
    ACTION withdraw(name from, asset amount);
    
    ACTION receiverand(uint64_t assoc_id, checksum256 random_value);
    [[eosio::on_notify("eosio.token::transfer")]] void receivetransfer(name from, name to, asset quantity, std::string memo);
  
    ACTION logannounce(uint64_t roll_id, name creator, uint64_t creator_id, uint32_t upper_range_limit, name rake_recipient);
    ACTION logbet(uint64_t roll_id, uint64_t bet_id, name creator, uint64_t creator_id, name bettor, asset amount, uint32_t lower_bound, uint32_t upper_bound, uint32_t muliplier);
  
    
    TABLE rollStruct {
      uint64_t roll_id;
      name creator;
      uint64_t creator_id;
      uint32_t upper_range_limit;
      name rake_recipient;
      asset total_amount;
      bool paid;
      
      uint64_t primary_key() const { return roll_id; }
      uint128_t get_creator_and_id() const { return uint128_t{creator.value} << 64 | creator_id; }
    };
    typedef multi_index<
    "rolls"_n,
    rollStruct,
    indexed_by<"creatorandid"_n, const_mem_fun<rollStruct, uint128_t, &rollStruct::get_creator_and_id>>>
    rolls_t;

    
    TABLE betStruct {
      uint64_t bet_id;
      name bettor;
      asset amount;
      uint32_t lower_bound;
      uint32_t upper_bound;
      uint32_t muliplier;
      
      uint64_t primary_key() const { return bet_id; }
    };
    typedef multi_index<"rollbets"_n, betStruct> rollBets_t;
    
    
    TABLE investorStruct {
      name investor;
      asset bankroll_weight;
      
      uint64_t primary_key() const { return investor.value; }
    };
    typedef multi_index<"investors"_n, investorStruct> investors_t;
    
    
    TABLE statsStruct {
      asset bankroll = asset(0, symbol("WAX", 8));
      uint64_t total_bankroll_weight = 0;
    };
    typedef singleton<"stats"_n, statsStruct> stats_t;
    // https://github.com/EOSIO/eosio.cdt/issues/280
    typedef multi_index<"stats"_n, statsStruct> stats_t_for_abi;
    
    
    rolls_t rollsTable;
    investors_t investorsTable;
    stats_t statsTable;
    
};