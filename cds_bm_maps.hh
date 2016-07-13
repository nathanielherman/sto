#pragma once

#include <cds/container/michael_list_hp.h>
#include <cds/container/michael_set.h>
#include <cds/container/split_list_map.h>
#include <cds/container/cuckoo_map.h>
#include <cds/container/michael_map.h>
#include <cds/container/michael_kvlist_hp.h>
#include <cds/container/feldman_hashmap_hp.h>
#include <cds/container/skip_list_map_hp.h>
#include <cds/container/striped_map.h>
#include <cds/container/striped_map/std_map.h>
#include <cds/details/binary_functor_wrapper.h>
#include <cds/opt/hash.h> 
#include <cds/algo/atomic.h> 
#include <map> 
#include <vector> 
#include <functional> 

#include "Hashtable.hh" 
#include "CuckooHashMap.hh"
#include "CuckooHashMapNT.hh"
#include "city_hasher.hh"
#include "cds_benchmarks.hh"

// types of map_operations available
enum map_op {insert, erase, find};
std::array<map_op, 3> map_ops_array = {insert, erase, find};

template <typename DS> struct CDSMapHarness { 
    typedef typename DS::mapped_type mapped_type;
    typedef typename DS::key_type key_type;
public:
    CDSMapHarness() {};
    bool insert(key_type k, mapped_type v) { return v_.insert(k, v); }
    bool erase(key_type k) { return v_.erase(k); }
    bool find(key_type k) { return v_.contains(k); }
    void init_insert(key_type k, mapped_type v) { v_.insert(k, v); }
    bool cleanup_erase() { v_.clear(); return false; }
private:
    DS v_;
};

template <typename DS> struct STOMapHarness { 
    typedef typename DS::mapped_type mapped_type;
    typedef typename DS::key_type key_type;
public:
    STOMapHarness() {};
    bool insert(key_type k, mapped_type v) { return v_.transInsert(k, v); }
    bool erase(key_type k) { return v_.transDelete(k); }
    bool find(key_type k) { mapped_type ret; return v_.transGet(k, ret); }
    void init_insert(key_type k, mapped_type v) { v_.nontrans_insert(k, v); }
    bool cleanup_erase() { 
        v_.nontrans_clear();
        return false; 
    }
private:
    DS v_;
};

/* 
 * Hashtable Templates
 */
template <typename K, typename T> struct DatatypeHarness<Hashtable<K,T,true,1000000>> : 
    public STOMapHarness<Hashtable<K,T,true,1000000>>{};
template <typename K, typename T> struct DatatypeHarness<Hashtable<K,T,false,1000000>> : 
    public STOMapHarness<Hashtable<K,T,false,1000000>>{};
template <typename K, typename T> struct DatatypeHarness<Hashtable<K,T,true,125000>> : 
    public STOMapHarness<Hashtable<K,T,false,125000>>{};
template <typename K, typename T> struct DatatypeHarness<Hashtable<K,T,false,125000>> : 
    public STOMapHarness<Hashtable<K,T,false,125000>>{};

template <typename K, typename T> struct DatatypeHarness<CuckooHashMap<K, T, CityHasher<K>,std::equal_to<K>,125000>> {
    typedef CuckooHashMap<K,T,CityHasher<K>,std::equal_to<K>,125000> DS;
    typedef typename DS::mapped_type mapped_type;
    typedef typename DS::key_type key_type;
public:
    DatatypeHarness() { v_ = new DS(); };
    bool insert(key_type k, mapped_type v) { return v_->insert(k, v); }
    bool erase(key_type k) { return v_->erase(k); }
    bool find(key_type k) { mapped_type v; return v_->find(k,v); }
    void init_insert(key_type k, mapped_type v) { 
        Sto::start_transaction();
        v_->insert(k, v); 
        assert(Sto::try_commit());
    }
    bool cleanup_erase() { delete v_; v_ = new DS(); return false; }
private:
    DS* v_;
};
template <typename K, typename T> struct DatatypeHarness<CuckooHashMap<K, T, CityHasher<K>,std::equal_to<K>,1000000>> {
    typedef CuckooHashMap<K,T,CityHasher<K>,std::equal_to<K>,1000000> DS;
    typedef typename DS::mapped_type mapped_type;
    typedef typename DS::key_type key_type;
public:
    DatatypeHarness() { v_ = new DS(); };
    bool insert(key_type k, mapped_type v) { return v_->insert(k, v); }
    bool erase(key_type k) { return v_->erase(k); }
    bool find(key_type k) { mapped_type v; return v_->find(k,v); }
    void init_insert(key_type k, mapped_type v) { 
        Sto::start_transaction();
        v_->insert(k, v); 
        assert(Sto::try_commit());
    }
    bool cleanup_erase() { delete v_; v_ = new DS(); return false; }
private:
    DS* v_;
};
template <typename K, typename T> struct DatatypeHarness<CuckooHashMapNT<K,T,CityHasher<K>,std::equal_to<K>,125000>> {
    typedef CuckooHashMapNT<K,T,CityHasher<K>,std::equal_to<K>,125000> DS;
    typedef typename DS::mapped_type mapped_type;
    typedef typename DS::key_type key_type;
public:
    DatatypeHarness() { v_ = new DS(); };
    bool insert(key_type k, mapped_type v) { return v_->insert(k, v); }
    bool erase(key_type k) { return v_->erase(k); }
    bool find(key_type k) { mapped_type v; return v_->find(k,v); }
    void init_insert(key_type k, mapped_type v) { v_->insert(k, v); }
    bool cleanup_erase() { delete v_; v_ = new DS(); return false; }
private:
    DS* v_;
};
template <typename K, typename T> struct DatatypeHarness<CuckooHashMapNT<K,T,CityHasher<K>,std::equal_to<K>,1000000>> {
    typedef CuckooHashMapNT<K,T,CityHasher<K>,std::equal_to<K>,1000000> DS;
    typedef typename DS::mapped_type mapped_type;
    typedef typename DS::key_type key_type;
public:
    DatatypeHarness() { v_ = new DS(); };
    bool insert(key_type k, mapped_type v) { return v_->insert(k, v); }
    bool erase(key_type k) { return v_->erase(k); }
    bool find(key_type k) { mapped_type v; return v_->find(k,v); }
    void init_insert(key_type k, mapped_type v) { v_->insert(k, v); }
    bool cleanup_erase() { delete v_; v_ = new DS(); return false; }
private:
    DS* v_;
};

template <typename K>
struct hash1 { size_t operator()(K const& k) const { return std::hash<K>()( k ); } };
template <typename K>
struct hash2: private hash1<K> {
    size_t operator()(K const& k) const
    {
        size_t h = ~( hash1<K>::operator()(k));
        return ~h + 0x9e3779b9 + (h << 6) + (h >> 2);
    }
};
template <typename K>
struct cuckoo_traits: public cds::container::cuckoo::traits
{
    typedef std::equal_to< K > equal_to;
    typedef cds::opt::hash_tuple< hash1<K>, hash2<K> >  hash;
    static bool const store_hash = true;
};
template <typename K, typename T> struct DatatypeHarness<cds::container::CuckooMap<K,T,cuckoo_traits<K>>> :
    public CDSMapHarness<cds::container::CuckooMap<K,T,cuckoo_traits<K>>>{};

template <typename K, typename T> struct DatatypeHarness<cds::container::FeldmanHashMap<cds::gc::HP,K,T>> :
    public CDSMapHarness<cds::container::FeldmanHashMap<cds::gc::HP,K,T>>{};

#define MICHAELMAP(K,T) \
    cds::container::MichaelHashMap< \
        cds::gc::HP,  \
        cds::container::MichaelKVList<  \
            cds::gc::HP, K, T, \
            typename cds::container::michael_list::make_traits<cds::container::opt::less<std::less<K>>>::type \
        >, \
        typename cds::container::michael_map::make_traits<\
            cds::opt::hash<cds::opt::v::hash<K>>, \
            cds::opt::item_counter<cds::atomicity::item_counter>, \
            cds::opt::allocator<CDS_DEFAULT_ALLOCATOR>\
        >::type\
    >
template <typename K, typename T> struct DatatypeHarness<MICHAELMAP(K,T)> {
    typedef MICHAELMAP(K,T) DS;
    typedef typename DS::mapped_type mapped_type;
    typedef typename DS::key_type key_type;
public:
    DatatypeHarness() : v_(125000, 5) {};
    bool insert(key_type k, mapped_type v) { return v_.insert(k, v); }
    bool erase(key_type k) { return v_.erase(k); }
    bool find(key_type k) { return v_.contains(k); }
    void init_insert(key_type k, mapped_type v) { v_.insert(k, v); }
    bool cleanup_erase() { v_.clear(); return false; }
private:
    DS v_;
};
template <typename K, typename T> struct DatatypeHarness<cds::container::SkipListMap<cds::gc::HP,K,T>> :
    public CDSMapHarness<cds::container::SkipListMap<cds::gc::HP,K,T>>{};
template <typename K, typename T> struct DatatypeHarness<cds::container::StripedMap<std::map<K,T>>> :
    public CDSMapHarness<cds::container::StripedMap<std::map<K,T>>>{};

template <typename K>
struct map_traits: public cds::container::split_list::traits
{
    typedef cds::container::michael_list_tag   ordered_list    ;   // what type of ordered list we want to use
    typedef std::hash<K>         hash            ;   // hash functor for the key stored in split-list map
    // Type traits for our MichaelList class
    struct ordered_list_traits: public cds::container::michael_list::traits
    {
    typedef std::less<K> less   ;   // use our std::less predicate as comparator to order list nodes
    };
};
template <typename K, typename T> struct DatatypeHarness<
        cds::container::SplitListMap<cds::gc::HP,K,T,map_traits<K>>> :
    public CDSMapHarness<cds::container::SplitListMap<cds::gc::HP,K,T,map_traits<K>>>{};

/*
 * Test interfaces and templates.
 */
template <typename DH> class DHMapTest : public GenericTest {
    typedef typename DH::mapped_type mapped_type;
    typedef typename DH::key_type key_type;
public:
    DHMapTest(int p_insert, int p_erase) : p_insert_(p_insert), p_erase_(p_erase), opdist_(0,99) {};
    void initialize(size_t init_sz) {
        keydist_ = std::uniform_int_distribution<long>(0,init_sz*2-1);
        Rand transgen(0);
        for (unsigned i = 0; i < init_sz; ++i) {
            v_.init_insert(keydist_(transgen),i); 
        }
    }

    void cleanup() {
        while (v_.cleanup_erase()){/*keep popping*/}
    }
    
    inline void do_map_op(int me, Rand& transgen) {
        map_op my_op;
        int p = opdist_(transgen);
        if (p >= p_insert_ + p_erase_) my_op = erase;
        else if (p < p_insert_) my_op = insert;
        else my_op = find;

        int key = keydist_(transgen); 
        switch (my_op) {
            case erase:
                if (v_.erase(key)) global_thread_ctrs[me].ke_erase++;
                global_thread_ctrs[me].erase++;
                break;
            case insert:
                if (v_.insert(key,me)) global_thread_ctrs[me].ke_insert++;
                global_thread_ctrs[me].insert++;
                break;
            case find:
                if (v_.find(key)) global_thread_ctrs[me].ke_find++;
                global_thread_ctrs[me].find++;
                break;
        }
    }

protected:
    DH v_;
    int p_insert_;
    int p_erase_;
    std::uniform_int_distribution<long> keydist_;
    std::uniform_int_distribution<long> opdist_;
};

template <typename DH> class RandomSingleOpTest : public DHMapTest<DH> {
public:
    RandomSingleOpTest(int ds_type, int p_insert, int p_erase) : DHMapTest<DH>(p_insert, p_erase), ds_type_(ds_type) {};
    void run(int me) {
        Rand transgen(initial_seeds[2*me], initial_seeds[2*me + 1]);
        if (ds_type_ == STO) {
            for (int i = NTRANS; i > 0; --i) {
                Rand transgen_snap = transgen;
                while (1) {
                    Sto::start_transaction();
                    try {
                        this->do_map_op(me, transgen);
                        if (Sto::try_commit()) break;
                    } catch (Transaction::Abort e) {transgen = transgen_snap;}
                }
            }
        } else {
            for (int i = NTRANS; i > 0; --i) {
                this->do_map_op(me, transgen);
            }
        }
    }
private:
    int ds_type_;
};

#define MAKE_MAP_TESTS(desc, test, key, val, size,...) \
    {desc, "STO Opaque Hashtable", new test<DatatypeHarness<Hashtable<key,val,true,size>>>(STO, ## __VA_ARGS__)},                                  \
    {desc, "STO Nonopaque Hashtable", new test<DatatypeHarness<Hashtable<key,val,false,size>>>(STO, ## __VA_ARGS__)},                                  \
    {desc, "STO MGCuckooMap", new test<DatatypeHarness<CuckooHashMap<key, val, CityHasher<key>,std::equal_to<key>,size>>>(STO, ## __VA_ARGS__)},                 \
    {desc, "MGCuckooMapNT", new test<DatatypeHarness<CuckooHashMapNT<key, val, CityHasher<key>,std::equal_to<key>,size>>>(CDS, ## __VA_ARGS__)},                 \
    {desc, "MichaelHashMap", new test<DatatypeHarness<MICHAELMAP(key,val)>>(CDS, ## __VA_ARGS__)},                 \
    {desc, "SplitListMap", new test<DatatypeHarness<cds::container::SplitListMap<cds::gc::HP,key,val,map_traits<key>>>>(CDS, ## __VA_ARGS__)},\
    //{desc, "FeldmanHashMap", new test<DatatypeHarness<cds::container::FeldmanHashMap<cds::gc::HP,key,val>>>(CDS, ## __VA_ARGS__)},                    
    //{desc, "StripedMap", new test<DatatypeHarness<cds::container::StripedMap<std::map<key,val>>>>(CDS, ## __VA_ARGS__)}, 
    //{desc, "SkipListMap", new test<DatatypeHarness<cds::container::SkipListMap<cds::gc::HP,key,val>>>(CDS, ## __VA_ARGS__)}, 
    //{desc, "CuckooMap", new test<DatatypeHarness<cds::container::CuckooMap<key,val,cuckoo_traits<key>>>>(CDS, ## __VA_ARGS__)},                

std::vector<Test> make_map_tests() {
    return {
        //MAKE_MAP_TESTS("HM1M:F34,I33,E33", RandomSingleOpTest, int, int, 1000000, 33, 33)
        //MAKE_MAP_TESTS("HM1M:F90,I5,E5", RandomSingleOpTest, int, int, 1000000, 5, 5)
        MAKE_MAP_TESTS("HM125K:F34,I33,E33", RandomSingleOpTest, int, int, 125000, 33, 33)
        MAKE_MAP_TESTS("HM125K:F90,I5,E5", RandomSingleOpTest, int, int, 125000, 5, 5)
        //MAKE_MAP_TESTS("HM:RandSingleOps(F10,I88,E2)", RandomSingleOpTest, int, int, 88, 2)
        //MAKE_MAP_TESTS("HM:RandSingleOps(F88,I10,E2)", RandomSingleOpTest, int, int, 10, 2)
    };
}
int num_maps = 6;
