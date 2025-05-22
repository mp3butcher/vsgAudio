/* -*-c++-*- */
/**
 * VSGAUDIO - VulkanSceneGraph Audio Library
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

#ifndef VSGAUDIO_EXPORT_H
#define VSGAUDIO_EXPORT_H 1

#if defined(WIN32) && !(defined(__CYGWIN__) || defined(__MINGW32__))
#pragma warning( disable : 4244 )
#pragma warning( disable : 4251 )
#pragma warning( disable : 4275 )
#pragma warning( disable : 4786 )
#pragma warning( disable : 4290 )
#pragma warning( disable : 4305 )
#endif


#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#  ifdef VSGAUDIO_LIBRARY
#    define VSGAUDIO_EXPORT   __declspec(dllexport)
#  else
#    define VSGAUDIO_EXPORT   __declspec(dllimport)
#  endif /* VSGAUDIO_LIBRARY */
#else
#define VSGAUDIO_EXPORT
#endif



#endif //VSGAUDIO_EXPORT_H
