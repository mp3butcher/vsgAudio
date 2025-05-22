/* -*-c++-*- */
/**
 * vsgAudio - VulkanSceneGraph Audio Library
 * Copyright 2025 Julien Valentin
 * (C) Copyright 2009-2012 byKenneth Mark Bryden
 * based on a fork of:
 * Osg AL - OpenSceneGraph Audio Library
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

#include <vsgAudio/SoundUpdateCB.h>
#include <vsgAudio/SoundManager.h>
#include <vsg/io/Logger.h>

using namespace vsg;
using namespace vsgAudio;


SoundTransform::SoundTransform()
    :
    vsg::Inherit<vsg::MatrixTransform, SoundTransform>(),
    m_sound_manager(SoundManager::instance()),
    m_last_time(0),
    m_first_run(true),
    m_last_traversal_number(0)
{
}

SoundTransform::SoundTransform(SoundState *sound_state)
    :
    vsg::Inherit<vsg::MatrixTransform, SoundTransform>(),
    m_sound_state(sound_state),
    m_sound_manager(SoundManager::instance()),
    m_last_time(0),
    m_first_run(true)
{
}

SoundTransform::SoundTransform(SoundState *sound_state, SoundManager *sound_manager)
    :vsg::Inherit<vsg::MatrixTransform, SoundTransform>(),
    m_sound_state(sound_state),
    m_sound_manager(sound_manager),
    m_last_time(0),
    m_first_run(true)
{
}

SoundTransform::SoundTransform(const SoundTransform &copy, const vsg::CopyOp &copyop)
    :vsg::Inherit<vsg::MatrixTransform, SoundTransform>(copy, copyop)
{
    *this = copy;
}

SoundTransform & SoundTransform::operator=(const SoundTransform &node)
{
    if (this == &node)
        return( *this );

    m_sound_state = node.m_sound_state;
    m_sound_manager = node.m_sound_manager;
    m_last_time = node.m_last_time;
    m_first_run = node.m_first_run;
    return( *this );
}

void SoundTransform::updateAudio(const vsg::dmat4& m, float deltatime)
{
    vsg::vec3 newpos(m[3][0], m[3][1], m[3][2]);
    if(!m_sound_state.valid())
    {
        // Early exit.
        vsg::warn("SoundTransform::operator()() No SoundState attached, or invalid FrameStamp.");
        return;
    }

    if(deltatime >= m_sound_manager->getUpdateFrequency())
    {
        m_sound_state->setPosition(newpos);

        //Calculate velocity
        vsg::vec3 velocity(0,0,0);
        if (m_first_run)
        {
            m_first_run = false;
            m_last_pos = newpos;
        }
        else
        {
            velocity = newpos - m_last_pos;
            m_last_pos = newpos;
            velocity /= deltatime;
        }

        if(m_sound_manager->getClampVelocity())
        {
            float max_vel = m_sound_manager->getMaxVelocity();
            float len = length(velocity);
            if ( len > max_vel)
            {
                velocity=normalize(velocity);
                velocity *= max_vel;
            }
        }
        m_sound_state->setVelocity(velocity);

        //Get new direction
        vsg::dvec3 dir = vsg::dvec3( 0., 1., 0. ) * m;
        dir = normalize(dir);
        m_sound_state->setDirection(vsg::vec3(dir));

        // Only do occlusion calculations if the sound is playing
        if (m_sound_state->getPlay() && m_occlude_callback.valid())
            m_occlude_callback->apply(m_sound_manager->getListenerMatrix(), newpos, m_sound_state.get());
    }
}
