/*
 * ----------------------------------------------------------------------------
 * Copyright (c) 2014-2015 BigObject Inc.
 * All Rights Reserved.
 *
 * Use of, copying, modifications to, and distribution of this software
 * and its documentation without BigObject's written permission can
 * result in the violation of U.S., Taiwan and China Copyright and Patent laws.
 * Violators will be prosecuted to the highest extent of the applicable laws.
 *
 * BIGOBJECT MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF
 * THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
 * ----------------------------------------------------------------------------
 */

#ifndef _BIGOBJECT_PARALLEL_
#define _BIGOBJECT_PARALLEL_

#include <queue>

#include <boost/atomic.hpp>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/locks.hpp>

#include <boost/tuple/tuple.hpp>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <boost/noncopyable.hpp>

#include <boost/mpl/empty_base.hpp>

#include <boost/shared_ptr.hpp>

namespace bigobject
{

// http://www.justsoftwaresolutions.co.uk/threading/
// implementing-a-thread-safe-queue-using-condition-variables.html
template<typename T, class Impl = std::queue<T> >
class ConcurrentQueue
{
private:

 /// Implemation queue.
 Impl queue;
 
 /// Mutex to protect queue.
 mutable boost::mutex mutex;
 
 /// Condition variable to wait queue not empty.
 boost::condition_variable condition;
 
public:
 
 void Push(T const& data)
 {
  boost::mutex::scoped_lock lock(mutex);  
  queue.push(data);
  lock.unlock();
  
  condition.notify_one();
 }

 bool Empty() const
 {
  boost::mutex::scoped_lock lock(mutex);
  return queue.empty();
 }

 bool TryPop(T& value)
 {
  boost::mutex::scoped_lock lock(mutex);
  
  if(queue.empty())
   return false;
        
  value = queue.front();
  queue.pop();
  
  return true;
 }

 void WaitPop(T& value)
 {
  boost::mutex::scoped_lock lock(mutex);
  while(queue.empty())
   condition.wait(lock);
        
  value = queue.front();
  queue.pop();
 }

};

// http://stackoverflow.com/questions/3928853/
// how-can-i-achieve-something-similar-to-a-semaphore-using-boost-in-c
class Semaphore : boost::noncopyable
{
private:

 /// The current semaphore count.
 unsigned int count;

 //Any code that reads or writes the count data must hold a lock on 
 // the mutex. 
 /// Variable mutex protects count.
 mutable boost::mutex mutex;

 /// Code that increments count must notify the condition variable.
 boost::condition_variable condition;

public:

 explicit Semaphore(unsigned int initial) : count(initial)
 {
 }

 void Signal()
 {
  boost::unique_lock<boost::mutex> lock(mutex);

  ++count;

  // Wake up any waiting threads. 
  // Always do this, even if count_ wasn't 0 on entry. 
  // Otherwise, we might not wake up enough waiting threads if we 
  // get a number of signal() calls in a row.
  condition.notify_one(); 
 }

 void Wait()
 {
  boost::unique_lock<boost::mutex> lock(mutex);
  
  while(count == 0)
   condition.wait(lock);
   
  --count;
 }

 unsigned int Count() const
 {
  return count;
 }

};

typedef boost::shared_ptr<Semaphore> SemaphorePtr;

class CountDownLatch : boost::noncopyable
{
private:

 /// The current count.
 unsigned int count;

 /// Variable mutex protects count.
 mutable boost::mutex mutex;

 /// Condition variable to wait until count is 0.
 boost::condition_variable condition;

public:

 explicit CountDownLatch(unsigned int initial) : count(initial)
 {
 }

 void Wait()
 {
  boost::unique_lock<boost::mutex> lock(mutex);
   
  while(count > 0)
   condition.wait(lock);
 }

 void Signal()
 {
  boost::unique_lock<boost::mutex> lock(mutex);
  
  --count;
  
  if(count == 0)
   condition.notify_all();
 }

 unsigned int Count() const
 {
  return count;
 }

};

typedef boost::shared_ptr<CountDownLatch> CountDownLatchPtr;

/**
Generic queue to dispatch tasks to threads.
*/
template<typename T>
class DispatchQueue
{
public:

 typedef T TaskActionParameter;

 /// Task synchronization and preprocess functor.
 typedef boost::function<bool (T)> TaskAction;

 /// Parameters which will be passed to ConsumeTask.
 typedef boost::tuple<TaskAction, T> ConsumerParameterTuple;

 /// Concurrent queue.
 typedef ConcurrentQueue<ConsumerParameterTuple> Queue;

public:

 /**
 Constructor.
 */
 DispatchQueue() : producerDone(false)
 {
 }

 /**
 Enqueue tasks.

 @param enqueueTasksFunc Custom functor to enqueue tasks.
 @param args Arguments for enqueueTasksFunc.
 @param threads Number of threads you want to dispatch.
 */
 template<typename F, typename A>
 void EnqueueTasks(F enqueueTasksFunc, A& args,
                   std::size_t threads = boost::thread::hardware_concurrency())
 { 
  CreateThreads(threads);
  producerDone = enqueueTasksFunc(*this, args);
 }

 /**
 Enqueue tasks.

 @param iter Task arguments iterator begin.
 @param iterEnd Task arguments iterator end.
 @param action Task action functor.
 @param threads Number of threads you want to dispatch.
 */
 template<typename I>
 void EnqueueTasks(I iter, I iterEnd, TaskAction action,
                   std::size_t threads = boost::thread::hardware_concurrency())
 { 
  CreateThreads(threads);

  for(; iter != iterEnd; ++iter)
   Enqueue(action, *iter);

  producerDone = true;
 }

 /**
 Enqueue one task.

 @param action Task action functor.
 @param args Arguments.
 */
 void Enqueue(TaskAction action, const T& args)
 {
  queue.Push(ConsumerParameterTuple(action, args));
 }

 /**
 Join threads to wait jobs done.
 */
 void Join()
 {
  threadGroup.join_all();
 }

protected:

 /**
 Create required threads.

 @param threads Number of threads.
 */
 void CreateThreads(std::size_t threads)
 {
  for(std::size_t i = 0; i != threads; ++i)
  {
   threadGroup.create_thread(boost::bind(
    &DispatchQueue::ConsumeTask, this));
  }
 }

 /**
 Thread function to do task.
 */
 void ConsumeTask(void) 
 {  
  struct _Local
  {   
   static void TryConsume(Queue& queue)
   {
    ConsumerParameterTuple p;
       
    while(queue.TryPop(p))
    {
     TaskAction& action = boost::get<0>(p);
     T& params = boost::get<1>(p);

     if(!action(params))
      queue.Push(p); // Re-queue.
    }
   }
  };
 
  while(!producerDone) 
   _Local::TryConsume(queue);

  // Last one.
  _Local::TryConsume(queue);
 }

protected:

 /// Thread group.
 boost::thread_group threadGroup;

 /// Are all tasks pushed into queue?
 boost::atomic<bool> producerDone;
 
 /// Queue to store unprocessed skus.
 Queue queue;

};

// Threading policies, 
// Implementation of the ThreadingModel policy used by various classes,
// Reference:
// http://loki-lib.sourceforge.net/index.php?n=MT.LockingBaseClasses

/**
Implements a single-threaded model; no synchronization.
*/
template<
 typename H, 
 typename M = boost::mpl::empty_base,
 typename L = boost::mpl::empty_base
>
class NullLockable
{
public:

 typedef NullLockable<H, M, L> ThreadPolicyType;

 /// Volatile type.
 typedef H VolatileType;

 /// Mutext type.
 typedef M MutexType;

 /// Lock type.
 typedef L LockType;

public:
 
 /// Dummy Lock class
 struct Lock
 {
  Lock(MutexType& m) 
  {
  }
 };
        
 template<typename T>
 struct Atomic
 {
  typedef T Type;

  static Type& Add(Type& lval, T val)
  {
   return lval += val; 
  }

  static Type& Sub(Type& lval, T val)
  {
   return lval -= val; 
  }

  static Type& Increment(Type& lval)
  { 
   return ++lval; 
  }

  static Type& Decrement(Type& lval)
  { 
   return --lval; 
  }

  static Type& Assign(Type& lval, T val)
  { 
   return lval = val; 
  }
 };

protected:

 /// Dummy mutex.
 mutable M _mutex; 

};

#define __PARALLEL_THREADS_ATOMIC                \
 template<typename T>                            \
 struct Atomic                                   \
 {                                               \
  typedef boost::atomic<T> Type;                 \
                                                 \
  static Type& Add(Type& lval, T val)            \
  {                                              \
   return lval.fetch_add(val);                   \
  }                                              \
                                                 \
  static Type& Sub(Type& lval, T val)            \
  {                                              \
   return lval.fetch_sub(val);                   \
  }                                              \
                                                 \
  static Type& Increment(Type& lval)             \
  {                                              \
   return ++lval;                                \
  }                                              \
                                                 \
  static Type& Decrement(Type& lval)             \
  {                                              \
   return --lval;                                \
  }                                              \
                                                 \
  static Type& Assign(Type& lval, T val)         \
  {                                              \
   return lval.store(val);                       \
  }                                              \
 };

/**  
Implements a object-level locking scheme.
*/
template<
 typename H, 
 typename M = boost::mutex, 
 typename L = boost::unique_lock<M> 
>
class ObjectLevelLockable
{
public:

 typedef ObjectLevelLockable<H, M, L> ThreadPolicyType;

 /// Volatile type.
 typedef volatile H VolatileType;

 /// Mutext type.
 typedef M MutexType;

 /// Lock type.
 typedef L LockType;

public:

 /// Lock.
 typedef LockType Lock;

 __PARALLEL_THREADS_ATOMIC

protected:

 /// Mutex.
 mutable M _mutex;
        
};

/// Handy macro to do object level lock.
#define OBJECT_LEVEL_LOCK \
 typename ThreadPolicyType::Lock _lock(ThreadPolicyType::_mutex);

/**  
Implements a class-level locking scheme.
*/
template<
 typename H, 
 typename M = boost::mutex, 
 typename L = boost::unique_lock<M> 
>
class ClassLevelLockable
{
public:

 typedef ClassLevelLockable<H, M, L> ThreadPolicyType;

 /// Volatile type.
 typedef volatile H VolatileType;

 /// Mutext type.
 typedef M MutexType;

 /// Lock type.
 typedef L LockType;

public:

 /// Lock.
 typedef LockType Lock;

 __PARALLEL_THREADS_ATOMIC

protected:

 /// Mutex.
 static M _mutex;
        
};

/// Handy macro to do object level lock.
#define CLASS_LEVEL_LOCK \
 typename ThreadPolicyType::Lock _lock(ThreadPolicyType::_mutex);

// Handy type defines for C++98 lack of supporting default template value for 
// template template parameter.
// Use C++11 might solve this problem by using or variadic templates.
// Reference: http://stackoverflow.com/questions/5301706/
// default-values-in-templates-with-template-arguments-c

template<typename H>
struct DefaultNullLockable : public NullLockable<H>
{
};

template<typename H>
struct DefaultObjectLevelLockable : public ObjectLevelLockable<H>
{
};

template<typename H>
struct DefaultClassLevelLockable : public ClassLevelLockable<H>
{
};

}

#endif
