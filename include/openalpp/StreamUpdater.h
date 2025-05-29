/* -*-c++-*- */
/**
 * vsgAudio - VulkanSceneGraph Audio Library
 * (C) Copyright 2009-2012 by Kenneth Mark Bryden
 * Copyright 2025 Julien Valentin
 * based on a fork of:
 * Osg AL - VulkanSceneGraph Audio Library
 * Copyright (C) 2004 VRlab, Umeå University
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * Please see COPYING file for special static-link exemption to LGPL.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef OPENALPP_STREAMUPDATER_H
#define OPENALPP_STREAMUPDATER_H 1

#ifndef BARRIER_HPP
#define BARRIER_HPP

#include <condition_variable>
#include <mutex>
#include <stdexcept>
#include <thread>

class Barrier {
public:
    // Construct barrier for use with num threads.
    Barrier(std::size_t num)
        : num_threads(num),
        wait_count(0),
        instance(0),
        mut(),
        cv()
    {
        if (num == 0) {
            throw std::invalid_argument("Barrier thread count cannot be 0");
        }
    }

    // disable copying of barrier
    Barrier(const Barrier&) = delete;
    Barrier& operator =(const Barrier&) = delete;

    // This function blocks the calling thread until
    // all threads (specified by num_threads) have
    // called it. Blocking is achieved using a
    // call to condition_variable.wait().
    void wait() {
        std::unique_lock<std::mutex> lock(mut); // acquire lock
        std::size_t inst = instance; // store current instance for comparison
            // in predicate

        if (++wait_count == num_threads) { // all threads reached barrier
            wait_count = 0; // reset wait_count
            instance++; // increment instance for next use of barrier and to
                // pass condition variable predicate
            cv.notify_all();
        } else { // not all threads have reached barrier
            cv.wait(lock, [this, &inst]() { return instance != inst; });
            // NOTE: The predicate lambda here protects against spurious
            //       wakeups of the thread. As long as this->instance is
            //       equal to inst, the thread will not wake.
            //       this->instance will only increment when all threads
            //       have reached the barrier and are ready to be unblocked.
        }
    }
private:
    std::size_t num_threads; // number of threads using barrier
    std::size_t wait_count; // counter to keep track of waiting threads
    std::size_t instance; // counter to keep track of barrier use count
    std::mutex mut; // mutex used to protect resources
    std::condition_variable cv; // condition variable used to block threads
};

#endif

#include <vector>

#ifdef WIN32
// Ignore the dll interface warning using std::vector members
#pragma warning(disable : 4251)
#endif

//extern "C" {
#include <al.h>
//}

#include <vsg/core/Inherit.h>
#include <vsg/core/Object.h>
#include <openalpp/Export.h>


#include <openalpp/AudioBase.h>
#include <openalpp/Error.h>
#include <openalpp/windowsstuff.h>
#include <thread>
#include <mutex>
//#include <reentrantMutex>
//#include <block>

namespace openalpp {

/**
    * Base class for (threaded) updating of stream buffers.
    */
class OPENALPP_API StreamUpdater : public  vsg::Inherit<vsg::Object, StreamUpdater>,   public std::recursive_mutex{ //public OpenThreads::ReentrantMutex, public osg::Referenced {

public:std::thread *_delegate;

    void start();
    virtual void run()=0;
    /**
        * Constructor.
        * @param buffer1 and...
        * @param buffer2 are the buffers used for double-buffered streaming.
        * @param format is the (OpenAL) format of the sound.
        * @param frequency is the frequency of the sound.
        */
    StreamUpdater(ALuint buffer1,ALuint buffer2,
                  ALenum format,unsigned int frequency);


    /**
        * Add a source to the stream.
        * @param sourcename is the OpenAL name of the source.
        */
    void addSource(ALuint sourcename);

    /**
        * Remove a source from the stream.
        * @param sourcename is the OpenAL name of the source.
        */
    void removeSource(ALuint sourcename);

    /**
        * Seeks to specified time
        */
    virtual void seek(float time_s) {};


    /**
        * Tell this StreamUpdater thread to wait until some thread call its release
        */
    void hold() ;

    /**
        * Release this thread.
        */
    void release();

    /**
        * Update the stream.
        * I.e. add new data to play.
        * @param buffer is a pointer to sound data.
        * @param length is the length of the sound data (in bytes).
        * @return done flag. I.e. stoprunning_.
        */
    bool update(void *buffer,unsigned int length);

    /**
        Tell the thread to stop executing, also release it if it is waiting to play.
        */
    virtual void stop() { stoprunning_ = true; release(); }

    /**
        @return true if the stop method has been called

        */
    bool shouldStop() const { return stoprunning_; }

    /**
        * Inherited from Thread.
        * Is called after run() finishes, and deletes this.
        */
    void cancelCleanup();

    /**
        */
    void setSleepTime(int mseconds) { sleepTime_ = mseconds; }

protected:

    /**
        * Remove any sources that the user has scheduled for removal.
        */
    void processRemovedSources();

    /**
        * Add any sources that the user has scheduled for adding.
        */
    void processAddedSources();


    /**
        *  Wait for someone to call release. This indicates that we should start playing a stream
        */
    void waitForPlay() ;

    /**
        * Names of the buffers to update.
        */
    ALuint buffers_[2];

    /**
        * OpenAL format of the sound data.
        */
    ALenum format_;

    /**
        * Frequency of the sound data.
        */
    unsigned int frequency_;

    /**
        * Source to update.
        */

    std::vector<ALuint> sources_,newsources_,removesources_;
    /**
        * Flag for when Run should stop running.
        */
    bool stoprunning_;


    /**
        * Mutex for stoprunning_.
        */
    //OpenThreads::ReentrantMutex runmutex_;
    std::recursive_mutex runmutex_;

    /**
        * Time to sleep in run method, i.e. sleeptime to reduce CPU usage
        */
    int sleepTime_;
    void sleep() {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(std::chrono::microseconds(sleepTime_)); }


    /** Event signaling that source is playing
        *
        */
    std::recursive_mutex threadMutex;
   // Barrier m_playEvent;

    std::mutex playEventMutex;
    std::condition_variable cv;
    bool wait4playevent;
protected:

    /**
        * Destructor.
        */
     ~StreamUpdater()  override ;

};

}

#endif /* OPENALPP_STREAMUPDATER_H */
