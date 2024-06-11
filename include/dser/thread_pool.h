#ifndef __DSER_THREAD_POOL_H__
#define __DSER_THREAD_POOL_H__

#include <functional>
#include <queue>

namespace dser::threads
{

    class thread_pool
    {
        public:
            using task_t = std::function<void()>;

        public:
            void submit(task_t task);
    
        private:
            std::queue<task_t> _task_queue;
    };

}

#endif // __DSER_THREAD_POOL_H__

