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

#ifndef vsgAudio_SAMPLEFMOD_H
#define vsgAudio_SAMPLEFMOD_H 1


#include <vsgAudio/Error.h>
#include <vsgAudio/Export.h>
#include <vsgAudio/BackendFMOD/SoundFMOD.h>

#include <fmod.hpp>

namespace vsgAudio
{
/// A Sample is an audio waveform, typically a static file of some format, usually linear PCM like WAV.
/*!
    This class offers a basic abstract API that adapts to various audio backends.
    It is based on the Adapter Design pattern.
    */

class VSGAUDIO_EXPORT Sample : public vsgAudio::Sound {
public:
    /**
        * Constructor.
        * @param filename is name of file to load.
        */
    Sample(const std::string& filename ) noexcept(false);

    /**
        * Copy constructor.
        */
    Sample(const Sample &sample);

    /**
        * Constructor.
        * @param format to use to create sample from data.
        * @param data use to create sample.
        * @param size of data.
        * @param freq of data.
        */
    // <<<>>> TODO: Create a portable way of specifying the format
    //Sample(ALenum format,ALvoid* data,ALsizei size,ALsizei freq) throw (FileError);

    /**
        * Get file name of loaded file.
        * @return file name.
        */
    std::string getFilename() const;

    /**
        * Assignment operator.
        */
    Sample &operator=(const Sample &sample);

    /**
        * Interface inherited from vsgAudio::Sound
        * Get internal FMOD object (used only by vsgAudio FMOD layer)
        * @return FMOD::Sound object wrapped by vsgAudio.
        */
    virtual FMOD::Sound *getInternalSound(void);
    virtual const FMOD::Sound *getInternalSound(void) const;

protected:
    /**
        * Destructor
        */
    virtual ~Sample();

private:
    // the actual FMOD datatype
    FMOD::Sound *_FMODSound;
    std::string _internalFullPath;

    // actual create implementation
    void createSampleFromFilename(const std::string& filename ) noexcept(false);

};

} // namespace vsgAudio

#endif /* vsgAudio_SAMPLEFMOD_H */
