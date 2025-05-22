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

#ifndef vsgAudio_STREAMOPENALPP_H
#define vsgAudio_STREAMOPENALPP_H 1

#include <vsgAudio/Export.h>
#include <vsg/core/Object.h>
#include <vsg/core/Inherit.h>
#include <vsgAudio/Error.h>

#include <openalpp/Stream.h>

namespace vsgAudio
{

/// A Stream is a dynamic audio input, typically not a static file but a net stream or live feed.
/*!
    This class offers a basic abstract API that adapts to various audio backends.
    It is based on the Adapter Design pattern.
    */

class VSGAUDIO_EXPORT Stream : public vsg::Inherit<vsg::Object, Stream>{
public:
    /**
        * Default constructor.
        */
    Stream() noexcept(false);

    /**
        * Copy constructor.
        */
    Stream(const Stream &stream);

    /**
        * Assignment operator.
        */
    Stream &operator=(const Stream &stream);


    void prepareForDeletion();

    /**
        * Start recording.
        * I.e. start copying data to buffers.
        * @param sourcename is the (OpenAL) name of the source.
        */
    // <<<>>> Can we implement this portably?
    //void record(ALuint sourcename);

    /**
        * Seeks to specified time
        */
    void seek(float time_s);

    /**
        * Stop recording.
        * @param sourcename is the (OpenAL) name of the source.
        */
    // <<<>>> Can we implement this portably?
    //void stop(ALuint sourcename);

    /**
        * Get internal openalpp Stream object (used only by vsgAudio openalpp Adapter layer)
        * @return openalp::Stream object wrapped by vsgAudio.
        */
    openalpp::Stream *getInternalStream(void);
    const openalpp::Stream *getInternalStream(void) const;

protected:
    /**
        * Destructor.
        */
    virtual ~Stream();

    /**
        * NULL constructor, only called by derived classes to prevent base class from
        * instantiating an openalpp::Stream in _openalppStream
        */
    Stream(unsigned long int DummyValue) {};


    // can also be used to store pointer to a derived class like FileStream
    vsg::ref_ptr<openalpp::Stream> _openalppStream;
}; // Stream

} // namespace vsgAudio

#endif /* vsgAudio_STREAMOPENALPP_H */
