/* -*-c++-*- */
/**
 * vsgAudio - OpenSceneGraph Audio Library
 * (C) Copyright 2009-2012 byKenneth Mark Bryden
 * Copyright 2025 Julien Valentin
 * based on a fork of:
 * Osg AL - OpenSceneGraph Audio Library
 * Copyright (C) 2004 VRlab, Ume� University
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

#ifndef vsgAudio_STREAMFMOD_H
#define vsgAudio_STREAMFMOD_H 1


#include <vsgAudio/Export.h>
#include <vsgAudio/Error.h>
#include <vsgAudio/BackendFMOD/SoundFMOD.h>

#include <fmod.hpp>

namespace vsgAudio
{

/// A Stream is a dynamic audio input, typically not a static file but a net stream or live feed.
/*!
    This class offers a basic abstract API that adapts to various audio backends.
    It is based on the Adapter Design pattern.
    */

class VSGAUDIO_EXPORT Stream : public vsgAudio::Sound {
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


    void prepareForDeletion() {};

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
        * Interface inherited from vsgAudio::Sound
        * Get internal FMOD object (used only by vsgAudio FMOD layer)
        * @return FMOD::Sound object wrapped by vsgAudio.
        */
    virtual FMOD::Sound *getInternalSound(void);
    virtual const FMOD::Sound *getInternalSound(void) const;

    const std::string getFilename(void) const {return(_filename);}


protected:
    /**
        * Destructor.
        */
    virtual ~Stream();

    /**
        * NULL constructor, only called by derived classes to prevent base class from
        * instantiating an openalpp::Stream in _openalppStream
        */
    Stream(unsigned long int) {};

    // the actual FMOD datatype
    FMOD::Sound *_FMODSound;

    // actual create implementation
    void createStreamFromFilename(const std::string& filename ) throw (FileError,NameError);

    std::string _filename;


}; // Stream

} // namespace vsgAudio

#endif /* vsgAudio_STREAMFMOD_H */
