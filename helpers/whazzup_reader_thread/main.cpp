#include <unistd.h>
#include <pthread.h>
#include "../../simulators/xplane/xenon/ivao/whazzup_reader_thread.h"

using namespace std;

int main() {
    pthread_t whazzup_thread = -1;
    pthread_create(&whazzup_thread, nullptr, & xenon::whazzup_reader_thread, nullptr);
    for (;;) {
        sleep(1);
    }
}
