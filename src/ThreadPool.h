// ThreadPool.h
//
// Copyright (c) 2016
// Prof. Dr. Rainer Schnell
// Universitaet Duisburg-Essen
// Campus Duisburg
// Institut fuer Soziologie
// Lotharstr. 65
// 47057 Duisburg
//
// This file is part of the command line application "mbtSearch".
//
// "mbtSearch" is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// "mbtSearch" is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with "mbtSearch". If not, see <http://www.gnu.org/licenses/>.

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <assert.h>

// Instances of template class ThreadPool hold a set threads that can
// concurrently perform the same kind of task. ThreadPool dispatches
// a new task to the next free worker thread and returns. If all
// worker threads are busy, ThreadPool waits until the task can be
// dispatched.
template <class T>
class ThreadPool {
	public:

	// Instances of threadDataType store task information
	// for a running thread. The main thread dispatches tasks
	// by filling in the task information into the thread's slot
	// and signaling the condition to start.
	typedef struct threadDataStruct {
		int slot;			// the thread's slot
		ThreadPool<T> *pool;		// pointer on responsible ThreadPool
		int task;			// task
		pthread_mutex_t mutex;		// mutex for signal handling
		pthread_cond_t condition;	// condition for signal handling
	} threadDataType;

	private:

	pthread_t *mPool;			// array of threads
	threadDataType *mThreadData;		// array of task information per thread
	T *mTasks;				// array of task specific information

	pthread_mutex_t mSlotStackMutex;	// mutex for signal handling
	pthread_cond_t mSlotStackCondition;	// condition for signal handling

	int mPoolSize;				// number of managed threads
	int *mSlotStack;			// stack of free threads
	int mSlotStackPosition;			// stack position

	int getSlot();				// lock next free thread
	void startSlot(int task, int slot);	// start locked thread to perform task
	void releaseSlot(int slot);		// unlock thread after completing a task

	public:

	// constructor
	ThreadPool(int size);			// create a new ThreadPool with size threads

	// destructor
	~ThreadPool();				// stop all threads and discard allocated resources

	void worker(int slot);			// thread main loop for retrieving and performing tasks

	// dispatch a task to a thread
	void dispatchTask(T& task);

	// wait until all threads have completed
	void wait();
};

// thread wrapper that is compatible to pthread-API and calls the
// worker member function of ThreadPool after thread creation
template <class T>
void *threadWrapper(void *p) {
	typename ThreadPool<T>::threadDataType *threadData;

	threadData = ((typename ThreadPool<T>::threadDataType*)p);
	threadData->pool->worker(threadData->slot);

	return NULL;
}

// thread main loop for retrieving and executing tasks
template <class T>
void ThreadPool<T>::worker(int slot) {
	int running = true;
	int *task = &(mThreadData[slot].task);

	pthread_mutex_t *mutex = &(mThreadData[slot].mutex);
	pthread_cond_t *condition = &(mThreadData[slot].condition);

	while (running) {
		// wait for task

		pthread_mutex_lock(mutex);

		while (*task == 0) {
			pthread_cond_wait(condition, mutex);
		}

		pthread_mutex_unlock(mutex);

		if (*task == 1) {
			// run task
			mTasks[slot].run();
		} else {
			// stop thread
			running = false;
		}

		// signal the thread to be ready for a new task
		releaseSlot(slot);
	}
}

// constructor
// create a new ThreadPool with size threads
template <class T>
ThreadPool<T>::ThreadPool(int size) {
	pthread_attr_t pthreadAttr;

	// allocate data space
	mPool = new pthread_t[size];
	mThreadData = new threadDataType[size];
	mSlotStack = new int[size];
	mTasks = new T[size];

	mPoolSize = size;

	pthread_attr_init(&pthreadAttr);
	pthread_attr_setdetachstate(&pthreadAttr, PTHREAD_CREATE_DETACHED);

	for (int i = 0; i < size; i++) {
		// initialize thread data structure
		mThreadData[i].slot = i;
		mThreadData[i].pool = this;
		mThreadData[i].task = 0;
		pthread_mutex_init(&(mThreadData[i].mutex), NULL);
		pthread_cond_init(&(mThreadData[i].condition), NULL);

		// start thread
		int rc = pthread_create(&mPool[i], &pthreadAttr, threadWrapper<T>, (void*) (&(mThreadData[i])));
		//rc = rc;
		assert(rc == 0);

		// put all threads on thread stack
		mSlotStack[i] = i;
	}

	mSlotStackPosition = 0;

	pthread_mutex_init(&mSlotStackMutex, NULL);
	pthread_cond_init(&mSlotStackCondition, NULL);
}

// destructor
// stop all threads and discard allocated resources
template <class T>
ThreadPool<T>::~ThreadPool() {
	// wait for locked threads
	wait();

	// lock all threads
	for (int i = 0; i < mPoolSize; i++) {
		getSlot();
	}

	// send task "stop" = 2 to each thread
	for (int i = 0; i < mPoolSize; i++) {
		startSlot(2, i);
	}

	// wait for all threads to complete
	wait();

	// release resources
	delete[] mPool;
	delete[] mSlotStack;
	delete[] mTasks;
}

// lock next free thread by getting its corresponding
// slot from the slot stack
template <class T>
int ThreadPool<T>::getSlot() {
	int slot;

	// wait for a free slot
	pthread_mutex_lock(&mSlotStackMutex);

	while (mSlotStackPosition >= mPoolSize) {
		pthread_cond_wait(&mSlotStackCondition, &mSlotStackMutex);
	}

	// get next free slot from the stack
	slot = mSlotStack[mSlotStackPosition];
	mSlotStackPosition++;

	pthread_mutex_unlock(&mSlotStackMutex);

	return slot;
}

// unlock thread after completing a task by putting
// its corresponding slot back on the slot stack
template <class T>
void ThreadPool<T>::releaseSlot(int slot) {
	pthread_mutex_lock(&mSlotStackMutex);

	mSlotStackPosition--;
	assert(mSlotStackPosition >= 0);
	mSlotStack[mSlotStackPosition] = slot;
	mThreadData[slot].task = 0;	// set task to "wait" = 0

	// signal main thread that "mSlotStackPosition" has changed
	pthread_cond_signal(&mSlotStackCondition);

	pthread_mutex_unlock(&mSlotStackMutex);
}

// start locked thread to perform task
template <class T>
void ThreadPool<T>::startSlot(int task, int slot) {
	pthread_mutex_lock(&(mThreadData[slot].mutex));

	// set task
	mThreadData[slot].task = task;

	// signal thread that "task" has changed
	pthread_cond_signal(&(mThreadData[slot].condition));
	pthread_mutex_unlock(&(mThreadData[slot].mutex));
}

// dispatch a task
template <class T>
void ThreadPool<T>::dispatchTask(T& task) {
	int slot;

	// lock thread
	slot = getSlot();

	// set attributes by standard copy assignment
	mTasks[slot] = task;

	// start thread with task = 1
	startSlot(1, slot);
}

// wait until all threads have completed
template <class T>
void ThreadPool<T>::wait() {
	pthread_mutex_lock(&mSlotStackMutex);

	while (mSlotStackPosition > 0) {
		pthread_cond_wait(&mSlotStackCondition, &mSlotStackMutex);
	}

	pthread_mutex_unlock(&mSlotStackMutex);
}
#endif
