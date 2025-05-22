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

#ifndef vsgAudio_SOUNDFMOD_H
#define vsgAudio_SOUNDFMOD_H 1

#include <vsgAudio/Export.h>
#include <vsgAudio/Error.h>
#include <vsg/core/Object.h>
#include <vsg/core/Inherit.h>
#include <fmod.hpp>


namespace vsgAudio
{

/// A Sound is a generic interface to a Sample or Stream
/*!
    Mostly this is just used to store a ref-counted pointer to another object.
    */

class VSGAUDIO_EXPORT Sound : public vsg::Inherit<vsg::Object, Sound>  {
public:
    /**
        * Get internal FMOD object (used only by vsgAudio FMOD layer)
        * @return FMOD::Sound object wrapped by vsgAudio.
        * Must be over-ridden in subclasses such as Sample, FileStream
        */
    virtual FMOD::Sound *getInternalSound(void) {return(NULL);};
    virtual const FMOD::Sound *getInternalSound(void) const {return(NULL);};

protected:
    virtual ~Sound() {};
}; // Sound

} // namespace vsgAudio

#endif /* vsgAudio_SOUNDFMOD_H */
