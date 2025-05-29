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

#include <vsgAudio/FileStream.h>
#include <openalpp/FileStream.h>


using namespace vsgAudio;

FileStream::FileStream(const std::string& filename,const int buffersize)
    noexcept(false)
    : vsg::Inherit<vsgAudio::Stream, FileStream>(0) {
    try {
        _openalppStream = new openalpp::FileStream (filename, buffersize);
    }
    catch(openalpp::NameError error) { throw error; }
    catch(openalpp::InitError error) { throw error; }
    catch(openalpp::FileError error) { throw error; }
}

FileStream::FileStream(const FileStream &stream)
    : vsg::Inherit<vsgAudio::Stream, FileStream>(0) {
    // static cast to our derived subclass.
    // Cast could only fail if everything were amiss,
    // so dynamic_cast isn't an advantage here.
    _openalppStream = new openalpp::FileStream(*static_cast<openalpp::FileStream *>(stream._openalppStream.get()));
}

FileStream::~FileStream()
{
}

FileStream &FileStream::operator=(const FileStream &stream) {
    if(&stream!=this)
    {
        *_openalppStream = *(stream._openalppStream.get());
    }
    return *this;
}

void FileStream::setLooping(bool loop) {
    // static cast to our derived subclass.
    // Cast could only fail if everything were amiss,
    // so dynamic_cast isn't an advantage here.
    (static_cast<openalpp::FileStream *>(_openalppStream.get()))->setLooping(loop);
}

std::string FileStream::getFilename() const {
    // static cast to our derived subclass.
    // Cast could only fail if everything were amiss,
    // so dynamic_cast isn't an advantage here.
    return (static_cast<openalpp::FileStream *>(_openalppStream.get()))->getFileName();
}
