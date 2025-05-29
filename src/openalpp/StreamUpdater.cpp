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

// TODO: Need to update this so that frequency and format can be changed after
// the class has been instantiated (the ogg vorbis format allows this)

#include <openalpp/StreamUpdater.h>

#define ENTER_CRITICAL lock();

#define LEAVE_CRITICAL unlock();

#if 0        // #ifdef _DEBUG
// for memory leak reporting
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif



using namespace openalpp;

StreamUpdater::StreamUpdater(ALuint buffer1,ALuint buffer2,
                             ALenum format,unsigned int frequency)
    : vsg::Inherit<vsg::Object, StreamUpdater>(),format_(format), frequency_(frequency), stoprunning_(false), sleepTime_(10)//,m_playEvent(4)
{
    buffers_[0]=buffer1;
    buffers_[1]=buffer2;

    // Start by not letting this thread stream data
    //m_playEvent.reset();

}
void StreamUpdater::release() {
    std::lock_guard g(playEventMutex);
    wait4playevent=false;
    cv.notify_all();

    //    m_playEvent.release();
}
void StreamUpdater::start(){
    _delegate = new std::thread( [&](){run();});
    sleep();

}
void StreamUpdater::hold() { //m_playEvent.reset();
    std::unique_lock g(playEventMutex);
    if(!wait4playevent)return;
    wait4playevent=true;
    cv.wait(g,[&]{
        return !wait4playevent;});
}
void StreamUpdater::waitForPlay() {
    std::unique_lock g(playEventMutex);
    if(!wait4playevent)return;
    wait4playevent=true;
    cv.wait(g,[&]{ return !wait4playevent;});
}
StreamUpdater::~StreamUpdater() {
    runmutex_.lock();
    //stop();
    runmutex_.unlock();

    release();

    //cancel();

    /* remove all sources, there may be a better way
    ** but otherwise sources are not removed for streams that reach EOF
    */
    for (unsigned int i=0; i<sources_.size(); i++)
    {
        alSourceStop(sources_[i]);
        ALuint temp[2];
        ALint nqueued;
        alGetSourceiv(sources_[i], AL_BUFFERS_QUEUED, &nqueued);
        if (nqueued)
        {
            alSourceUnqueueBuffers(sources_[i], nqueued, temp);
        }
    }
    sources_.clear();
    removesources_.clear();
    newsources_.clear();

}

void StreamUpdater::addSource(ALuint sourcename) {
    std::lock_guard<std::recursive_mutex> scope_lock(*this);

    alSourceStop(sourcename);
    newsources_.push_back(sourcename);
    if(!sources_.size()) {
        if (!_delegate){//isRunning()) {
            start(); // Now when we have added a source, start the thread

            //
        }
    }
   release();
}

void StreamUpdater::removeSource(ALuint sourcename) {
    {
        std::lock_guard<std::recursive_mutex> scope_lock(*this);
        removesources_.push_back(sourcename);
    }

    // If there are no more sources, tell the thread to wait
    if (!(sources_.size()-1))
        hold();
}

void StreamUpdater::processRemovedSources()
{
    // remove sources that are queued for removal
   std::lock_guard<std::recursive_mutex> scope_lock(*this);
    while (sources_.size() && removesources_.size())
    {
        for (unsigned int i=0;i<sources_.size();i++)
            if (removesources_.back()==sources_[i])
            {
                alSourceStop(removesources_.back());
                ALuint dump[2];
                ALint nqueued;
                alGetSourceiv(removesources_.back(),AL_BUFFERS_QUEUED,&nqueued);
                if(nqueued)
                    alSourceUnqueueBuffers(removesources_.back(),nqueued,dump);
                alGetError();
                while((i+1)<sources_.size())
                    sources_[i]=sources_[i+1];
                break;
            }
        sources_.pop_back();
        removesources_.pop_back();
    }
    removesources_.clear();
}


void StreamUpdater::processAddedSources()
{
    /* Add sources queued to be added, waiting until at least
    ** one source is added.
    ** Do not add duplicates */
    std::lock_guard<std::recursive_mutex> scope_lock(*this);

    while (newsources_.size())
    {
        if (newsources_.size())
        {
            while(newsources_.size())
            {
                bool duplicateSource = false;
                for (size_t n=0; n<sources_.size(); ++n)
                {
                    if (sources_[n] == newsources_.back()) {
                        duplicateSource = true;
                        newsources_.pop_back();
                        if (!newsources_.size())
                            break;
                    }
                }
                if (!duplicateSource && newsources_.size())
                {
                    sources_.push_back(newsources_.back());
                    alSourceStop(newsources_.back());
                    newsources_.pop_back();
                }
            }
        }
    }


}


/**
* If playing, wait for a buffer to be processed,
* unqueue, load it with the new data, and re-queue.
* If stopped, queue two new buffers and play.
* Needs refactoring, this is difficult to work with.
*/
bool StreamUpdater::update(void *buffer, unsigned int length)
{
    if(!(length && buffer))     // Zero length or NULL pointer => return
    {
        return false;
    }

    ALint processed,state;
    ALuint albuffer=0;

    // Wait here until someone calls release, which indicates that there are sources to be processed
    waitForPlay();
    processRemovedSources();
    processAddedSources();

    ENTER_CRITICAL;
    processed = 0;

    while (sources_.size() && !processed)
    {

        state=AL_PLAYING;
        for(unsigned int i=0;i<sources_.size();i++)
        {
            alGetSourceiv(sources_[i],AL_SOURCE_STATE,&state);
            if(state!=AL_PLAYING)
                break;
        }

        ///TODO: Pause is really not something that is very well implemented right now.
        // It only uses state of first source to make out if we are in pause state or not.

        /* a hack to get the paused mode to work, wait for non paused state,
        ** using only the first source */
        if (state == AL_PAUSED)
        {
            while (state == AL_PAUSED)
            {
                LEAVE_CRITICAL;
                //YieldCurrentThread();
                std::this_thread::yield();
                ENTER_CRITICAL;

                std::this_thread::sleep_for(std::chrono::microseconds(1));
                alGetSourceiv(sources_[0], AL_SOURCE_STATE, &state);
            }
        }

        /* if there are buffers queued, play the source (i.e. restart it),
        ** otherwise set state to AL_INITIAL to start up two new buffers
        */
        if (state == AL_STOPPED)
        {
            for(unsigned int i=0;i<sources_.size();i++)
            {
                alSourceStop(sources_[i]);
                ALint nqueued;
                alGetSourceiv(sources_[i], AL_BUFFERS_QUEUED, &nqueued);
                ALCHECKERROR();
                if (nqueued)
                {
                    alSourcePlay(sources_[i]);
                    ALCHECKERROR();
                }
                else
                {
                    state = AL_INITIAL;
                }
            }
        }
        ALCHECKERROR();

        // start up two new buffers
        if (state == AL_INITIAL)
        {
            ALuint dump[2];
            for(unsigned int i=0;i<sources_.size();i++) {
                // Yeah, this sucks, but how else could the sources be kept
                // synchronized (they have to be unless we start using different
                // buffers for each source)...
                alSourceStop(sources_[i]);
                ALCHECKERROR();
                ALint nqueued;
                alGetSourceiv(sources_[i],AL_BUFFERS_QUEUED,&nqueued);
                ALCHECKERROR();
                if(nqueued)
                    alSourceUnqueueBuffers(sources_[i],nqueued,dump);
                ALCHECKERROR();
            }
            unsigned int l = length*0.5;
            alBufferData(buffers_[0],format_,buffer,l,frequency_);
            ALCHECKERROR();
            alBufferData(buffers_[1],format_,
                         (char *)buffer+l,l,frequency_);
            ALCHECKERROR();
            for(unsigned int i=0;i<sources_.size();i++)
            {
                alSourceQueueBuffers(sources_[i],2,buffers_);
                ALCHECKERROR();
                alSourcePlay(sources_[i]);  // TODO: This would be better handled by
                ALCHECKERROR();
                // alSourcePlayv((ALuint *)&sources_[0]..)...;
            }
            processed=1;

        }
        else // AL_PLAYING
        {
            processed=1;
            for(unsigned int i=0;i<sources_.size();i++) {
                alGetSourceiv(sources_[i],AL_BUFFERS_PROCESSED,&processed);
                ALCHECKERROR();
                if(!processed)
                    break;
            }
            ALCHECKERROR();

            if(processed) {
                for(unsigned int i=0;i<sources_.size();i++) {
                    alSourceUnqueueBuffers(sources_[i],1,&albuffer);
                    ALCHECKERROR();
                }
                alBufferData(albuffer,format_,buffer,length,frequency_);
                ALCHECKERROR();
                for(unsigned int i=0;i<sources_.size();i++)
                    alSourceQueueBuffers(sources_[i],1,&albuffer);
                std::this_thread::yield();
               // YieldCurrentThread();
            } else {
                LEAVE_CRITICAL;
                std::this_thread::yield();
               // YieldCurrentThread();
                std::this_thread::sleep_for(std::chrono::microseconds(1000*10));
                ENTER_CRITICAL;
                // Not sure if this is necessary, but just in case...
                if(removesources_.size()) {
                    LEAVE_CRITICAL;
                    return update(buffer,length);
                }
            } // if(processed) else
            } // if(state!=AL_PLAYING) else

        } // while(!processed)
    ALCHECKERROR();

    LEAVE_CRITICAL;
    bool ret;
    runmutex_.lock();
    ret=shouldStop();
    runmutex_.unlock();

    return ret;
}

void StreamUpdater::cancelCleanup() {
    std::cerr << "StreamUpdater::cancelCleanup: Should probably not delete this" << std::endl;
    if(_delegate)
       delete _delegate;
}
