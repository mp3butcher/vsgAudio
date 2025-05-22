/* -*-c++-*- */
/**
 * vsgAudio - VulkanSceneGraph Audio Library
 * (C) Copyright 2009-2012 byKenneth Mark Bryden
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


#include <vsgAudio/Version.h>
#include <string>
#include <sstream>


unsigned int
vsgAudio::getVersionNumber()
{
    return( VSGAUDIO_VERSION );
}


static std::string s_vsgaudio_version( "" );

std::string
vsgAudio::getVersion()
{
    if( s_vsgaudio_version.empty() )
    {
        std::ostringstream oStr;
        oStr << std::string( "vsgAudio version " ) <<
            VSGAUDIO_MAJOR_VERSION << "." <<
            VSGAUDIO_MINOR_VERSION << "." <<
            VSGAUDIO_SUB_VERSION << " (" <<
            getVersionNumber() << ").";
        s_vsgaudio_version = oStr.str();
    }
    return( s_vsgaudio_version );
}


std::string
vsgAudio::getLibraryName()
{
    return "VulkanSceneGraph Audio Library (vsgAudio)";
}
