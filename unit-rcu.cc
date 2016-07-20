#undef NDEBUG
#include "Transaction.hh"
#include "TWrapped.hh"
#include "clp.h"
#include <stdlib.h>
#include <inttypes.h>

uint64_t nallocated;
uint64_t nfreed;
double delay;
bool stop;

class Tracker {
public:
    Tracker() {
        __sync_fetch_and_add(&nallocated, 1);
    }
    ~Tracker() {
        __sync_fetch_and_add(&nfreed, 1);
    }
};


void* tracker_run(void* argument) {
    int tid = reinterpret_cast<uintptr_t>(argument);
    TThread::set_id(tid);

    while (!stop) {
        double this_delay = delay + (drand48() - 0.5) * delay / 3;
        usleep(useconds_t(this_delay * 0.7e6));
        TRANSACTION {
            Transaction::rcu_delete(new Tracker);
            usleep(useconds_t(this_delay * 0.3e6));
        } RETRY(false);
    }
    return nullptr;
}


class movable {
public:
    movable()
        : origin_(0) {
    }
    movable(movable&& x)
        : origin_(1) {
        x.origin_ = -1;
    }
    movable(const movable&)
        : origin_(2) {
    }

    int origin() const {
        return origin_;
    }

private:
    int origin_;
};


static const Clp_Option options[] = {
    { "delay", 'd', 'd', Clp_ValDouble, Clp_Negate },
    { "nthreads", 'j', 'j', Clp_ValInt, 0 },
    { "nepochs", 'e', 'e', Clp_ValInt, 0 }
};

int main(int argc, char* argv[]) {
    unsigned nthreads = 4;
    TRcuSet::epoch_type nepochs = 10;
    delay = 0.000001;

    // check perfect forwarding works as we expect
    {
        movable m1;
        assert(m1.origin() == 0);
        movable m2(m1);
        assert(m1.origin() == 0);
        assert(m2.origin() == 2);
        movable m3(std::move(m1));
        assert(m1.origin() == -1);
        assert(m2.origin() == 2);
        assert(m3.origin() == 1);

        TWrapped<movable> w1;
        assert(w1.access().origin() == 0);
        movable m4;
        TWrapped<movable> w2(m4);
        assert(m4.origin() == 0);
        assert(w2.access().origin() == 2);
        TWrapped<movable> w3(std::move(m4));
        assert(m4.origin() == -1);
        assert(w3.access().origin() == 1);
    }

    Clp_Parser *clp = Clp_NewParser(argc, argv, arraysize(options), options);
    int opt;
    while ((opt = Clp_Next(clp)) != Clp_Done) {
        switch (opt) {
        case 'd':
            delay = clp->val.d;
            break;
        case 'j':
            nthreads = clp->val.i;
            break;
        case 'e':
            nepochs = clp->val.i;
            break;
        default:
            abort();
        }
    }
    Clp_DeleteParser(clp);

    if (nthreads > MAX_THREADS) {
        printf("Asked for %d threads but MAX_THREADS is %d\n", nthreads, MAX_THREADS);
        exit(1);
    }

    pthread_t tids[nthreads];
    for (uintptr_t i = 0; i < nthreads; ++i)
        pthread_create(&tids[i], NULL, tracker_run, reinterpret_cast<void*>(i));
    pthread_t advancer;
    pthread_create(&advancer, NULL, Transaction::epoch_advancer, NULL);
    pthread_detach(advancer);

    while (Transaction::global_epochs.global_epoch < nepochs + 1)
        usleep(useconds_t(delay * 1e6));
    stop = true;

    for (unsigned i = 0; i < nthreads; ++i)
        pthread_join(tids[i], NULL);

    auto nfreed_before = nfreed;
    for (unsigned i = 0; i < nthreads; ++i)
        Transaction::tinfo[i].rcu_set.~TRcuSet();

    printf("created %" PRIu64 ", deleted %" PRIu64 ", finally deleted %" PRIu64 "\n", nallocated, nfreed_before, nfreed);
}
