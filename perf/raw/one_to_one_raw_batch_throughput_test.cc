/**
 * UniCast a series of items between 1 publisher and 1 event processor.
 *
 * +----+    +-----+
 * | P1 |--->| EP1 |
 * +----+    +-----+
 *
 *
 * Queue Based:
 * ============
 *
 *        put      take
 * +----+    +====+    +-----+
 * | P1 |--->| Q1 |<---| EP1 |
 * +----+    +====+    +-----+
 *
 * P1  - Publisher 1
 * Q1  - Queue 1
 * EP1 - EventProcessor 1
 *
 *
 * Disruptor:
 * ==========
 *              track to prevent wrap
 *              +------------------+
 *              |                  |
 *              |                  v
 * +----+    +====+    +====+   +-----+
 * | P1 |--->| RB |<---| SB |   | EP1 |
 * +----+    +====+    +====+   +-----+
 *      claim      get    ^        |
 *                        |        |
 *                        +--------+
 *                          waitFor
 *
 * P1  - Publisher 1
 * RB  - RingBuffer
 * SB  - SequenceBarrier
 * EP1 - EventProcessor 1
 *
 */

#include "one_to_one_raw_batch_throughput_test.h"
#include "single_producer_sequencer.h"
#include "yielding_wait_strategy.h"

static const int BUFFER_SIZE = 1024 * 64;
static const int64_t ITERATIONS = 1000 * 1000 * 200;

OneToOneRawBatchThroughputTest::OneToOneRawBatchThroughputTest() {
  wait_strategy_ =  new magic_bean::YieldingWaitStrategy;
  sequencer_ = new magic_bean::SingleProducerSequencer(BUFFER_SIZE, wait_strategy_);
}

OneToOneRawBatchThroughputTest::~OneToOneRawBatchThroughputTest() {
  delete sequencer_;
  delete wait_strategy_;
}

int64_t OneToOneRawBatchThroughputTest::RunDisruptorPass() {
  int batch_size = 10;
  return 0;
}
