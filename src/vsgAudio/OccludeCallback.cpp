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
#include <vsgAudio/OccludeCallback.h>
#include <vsgAudio/SoundState.h>
#include <vsgAudio/Math.h>
#include <vsg/nodes/Node.h>
#include <vsg/utils/LineSegmentIntersector.h>

using namespace vsgAudio;


OccludeCallback::OccludeCallback(vsg::ref_ptr<vsg::Node>root) :
    vsg::Inherit<vsg::Object, OccludeCallback>(),
    m_root(root), m_sound_state(0), m_ear_distance(0.2), m_near_threshold(0.1f),
    m_was_occluded(false), m_delay(10)
{
}

/// Here we set an empty node. This constructor is called by osg when reading a file,
/// and later will the real node will be set.
OccludeCallback::OccludeCallback() :
    vsg::Inherit<vsg::Object, OccludeCallback>(),
    m_root(vsg::Node::create()), m_sound_state(0), m_ear_distance(0.2), m_near_threshold(0.1f),
    m_was_occluded(false), m_delay(10)
{
}

void OccludeCallback::operator()(double /*distance*/, vsg::Node * /*occluder*/, bool left_occluded, bool /*right_occluded*/)
{
    vsgAudio::SoundState *sound_state = m_sound_state;

    // Sound node is occluded by something
    if (left_occluded) {

        // Was it not occluded last frame?
        if (!m_was_occluded)
            m_start_tick = vsg::clock::now(); // Then start timer
        ;
        // Linearly interpolate occlusion from 0 to max
        double dt = m_delay*std::chrono::duration_cast<std::chrono::milliseconds>(vsg::clock::now()-m_start_tick).count() * 0.001f;
        float scale = vsgAudio::mix(1.0f, 0.0f, dt);
        sound_state->setOccludeScale(scale);
        m_sound_state->setOccluded(true);
    }
    else { // Is not occluded anymore

        // If occlusion is already shut of, do no more
        if (!sound_state->getOccluded())
            return;

        if (m_was_occluded) // Was it occluded last frame, then start timer
            m_start_tick = vsg::clock::now();

        // Interpolate from max to 0 damping
        double dt = m_delay*std::chrono::duration_cast<std::chrono::milliseconds>(vsg::clock::now()-m_start_tick).count() * 0.001f;
        float scale = vsgAudio::mix(0.0f, 0.99f, dt);
        sound_state->setOccludeScale(scale);

        // When enough time have passed, disable occlusion
        if (dt > 1/m_delay)
            m_sound_state->setOccluded(false);
    }
}

void OccludeCallback::apply(const vsg::mat4& listener_matrix, const vsg::vec3& sound_pos, vsgAudio::SoundState* sound_state)
{
    m_sound_state = sound_state;
    double distance = 0;
    vsg::mat4 m = vsg::inverse(listener_matrix);
    vsg::dvec3 start(m[3][0],m[3][1],m[3][2]), end(sound_pos);
    vsg::dvec3 dir = end-start;
    distance = vsg::length(dir);
    dir /= distance;
    vsg::Node *occluder=0;
    dir*=distance;
    dir+=start;
    auto intersector = vsg::LineSegmentIntersector::create(start,dir);
    m_root->accept(*intersector);


    bool occluded = false;
    if (!intersector->intersections.empty()) {

        // sort the intersections front to back
        std::sort(intersector->intersections.begin(), intersector->intersections.end(), [](auto& lhs, auto& rhs) { return lhs->ratio < rhs->ratio; });

        auto np=intersector->intersections[0]->nodePath;
        double d = vsg::length(intersector->intersections[0]->worldIntersection - start);
        double diff = fabs(d - distance);

        if ( diff > m_near_threshold) {
            occluded = true;

            if (np.size())
                occluder = const_cast<vsg::Node*>(*np.begin());

            this->operator ()(d, occluder, true, true);
        }
    }

    // If it is not occluded this frame but it was the previous, restore the state
    if (!occluded)
        this->operator ()(0, NULL, false, false);

    // save the state of the occlusion for next frame
    m_was_occluded = occluded;

}

