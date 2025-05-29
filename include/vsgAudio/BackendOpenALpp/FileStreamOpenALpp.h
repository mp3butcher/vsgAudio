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

#ifndef vsgAudio_FILESTREAMOPENALPP_H_H
#define vsgAudio_FILESTREAMOPENALPP_H_H 1

#include <vsgAudio/Export.h>
#include <vsgAudio/Error.h>
#include <vsgAudio/Stream.h>

#include <openalpp/FileStream.h>

namespace vsgAudio
{
/// A FileStream is an audio stream resident in a (typically compressed) file like MP3 or OGG.
/*!
    This class offers a basic abstract API that adapts to various audio backends.
    It is based on the Adapter Design pattern.
    */

class VSGAUDIO_EXPORT FileStream : public vsg::Inherit<vsgAudio::Stream, FileStream> {
public:
    /**
        * Constructor. A FileError will be thrown if the file can't be opened or
        * isn't recognized.
        * @param filename is the name of the file to try to open.
        * @param buffersize is an optional parameter specifying how large the
        * buffer should be (in samples per second).
        */
    FileStream(const std::string& filename,const int buffersize=4096)
        noexcept(false);

    /**
        * Copy constructor.
        */
    FileStream(const FileStream &stream);


    FileStream &operator=(const FileStream &stream);

    /**
        * Turn on/off looping.
        * @param loop is true if the stream should loop, false otherwise.
        */
    void setLooping(bool loop = true);

    std::string getFilename() const;

protected:
    /**
        * Destructor.
        */
    virtual ~FileStream();

    // we have no FileStream object here, because it is a derived class of Stream, so we store it there
}; // FileStream


} // namespace vsgAudio

#endif /* vsgAudio_FILESTREAMOPENALPP_H_H */
