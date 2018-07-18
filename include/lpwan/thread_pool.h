/* -*- c++ -*- */
/*
 * Copyright 2018 <+YOU OR YOUR COMPANY+>.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


#ifndef INCLUDED_LPWAN_THREAD_POOL_H
#define INCLUDED_LPWAN_THREAD_POOL_H

// taken from https://stackoverflow.com/questions/23896421/efficiently-waiting-for-all-tasks-in-a-threadpool-to-finish


#include <queue>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <deque>
#include <functional>
#include <thread>
#include <condition_variable>
#include <mutex>


namespace gr {
  namespace lpwan {

    //thread pool
    class ThreadPool
    {
    public:
      ThreadPool(unsigned int n = std::thread::hardware_concurrency());

      template<class F> void enqueue(F&& f);
      void waitFinished();
      ~ThreadPool();

      unsigned int getProcessed() const { return processed; }

    private:
      std::vector< std::thread > workers;
      std::deque< std::function<void()> > tasks;
      std::mutex queue_mutex;
      std::condition_variable cv_task;
      std::condition_variable cv_finished;
      std::atomic_uint processed;
      unsigned int busy;
      bool stop;

      void thread_proc();
    };

    ThreadPool::ThreadPool(unsigned int n)
        : busy()
        , processed()
        , stop()
    {
      for (unsigned int i=0; i<n; ++i)
        workers.emplace_back(std::bind(&ThreadPool::thread_proc, this));
    }

    ThreadPool::~ThreadPool()
    {
      // set stop-condition
      std::unique_lock<std::mutex> latch(queue_mutex);
      stop = true;
      cv_task.notify_all();
      latch.unlock();

      // all threads terminate, then we're done.
      for (auto& t : workers)
        t.join();
    }

    void ThreadPool::thread_proc()
    {
      while (true)
      {
        std::unique_lock<std::mutex> latch(queue_mutex);
        cv_task.wait(latch, [this](){ return stop || !tasks.empty(); });
        if (!tasks.empty())
        {
          // got work. set busy.
          ++busy;

          // pull from queue
          auto fn = tasks.front();
          tasks.pop_front();

          // release lock. run async
          latch.unlock();

          // run function outside context
          fn();
          ++processed;

          latch.lock();
          --busy;
          cv_finished.notify_one();
        }
        else if (stop)
        {
          break;
        }
      }
    }

    // generic function push
    template<class F>
    void ThreadPool::enqueue(F&& f)
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      tasks.emplace_back(std::forward<F>(f));
      cv_task.notify_one();
    }

    // waits until the queue is empty.
    void ThreadPool::waitFinished()
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      cv_finished.wait(lock, [this](){ return tasks.empty() && (busy == 0); });
    }
  }
}
#endif /* INCLUDED_LPWAN_THREAD_POOL_H */