./ut-event_loop_test.d ./ut-event_loop_test.o: event_loop_test.cc ../../../ldd/net/event_loop.h \
  ../../../ldd/util/atomic.h ../../../ldd/util/memory_order.h \
  ../../../ldd/util/detail/atomic/platform.h \
  ../../../ldd/util/detail/atomic/gcc_x86.h \
  ../../../ldd/util/detail/atomic/base.h \
  ../../../ldd/util/detail/atomic/lockpool.h \
  ../../../ldd/util/detail/atomic/classification.h \
  ../../../ldd/util/thread.h ../../../ldd/net/event_loop_thread_pool.h
