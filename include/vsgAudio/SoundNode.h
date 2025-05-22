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
#ifndef vsgAudio_SOUNDNODE_H
#define vsgAudio_SOUNDNODE_H 1


#include <vsgAudio/Export.h>

#include <vsg/nodes/Node.h>

#include <vsg/core/observer_ptr.h>

#include <vsgAudio/SoundState.h>
#include <vsgAudio/OccludeCallback.h>



namespace vsgAudio
{
class SoundManager;

/// A placeholder for a soundstate. The transformation is updated automatically during update traversal.
/**
    WARNING: This class is potentially not thread safe. It is deprecated and has been
    replaced by SoundUpdateCB. Please use the new class. Avoid SoundNode. You have been warned.

    This class implements a scenegraph node has a SoundState associated to it.
    During cull traversal the orientation and position in world coordinates is calculated for this node and
    fed to the SoundSystem. This ensures that the node follows any transformation above this node.
    */
class VSGAUDIO_EXPORT SoundNode: public vsg::Inherit<vsg::Node, SoundNode> {
public:

    /// Default constructor
    SoundNode();

    /*!
        Constructor that also associates a sound state to this node.
        */
    SoundNode(SoundState *sound_state);

    /*!
        Constructor that also associates a sound state and sound manager to this node.
        Useful on Windows when working with multiple apps so that multiple instances are
        not created with the singleton use of SoundManager.
        */
    SoundNode(SoundState *sound_state, SoundManager *sound_manager);

    /// Copy constructor
    SoundNode(const SoundNode &copy, const vsg::CopyOp &copyop = {});

    /// Associates a soundstate with this SoundNode.
    void setSoundState(SoundState *sound_state) { m_sound_state = sound_state; }

    /// Returns a reference to to the Soundstate associated with this SoundNode
    SoundState *getSoundState() { return vsg::ref_ptr<SoundState>(m_sound_state).get(); }

    /// Returns a const reference to to the Soundstate associated with this SoundNode
    const SoundState *getSoundState() const { return vsg::ref_ptr<const SoundState>(m_sound_state).get(); }

    /// Updates the transformation of the SoundState during Cull traversal.
    //void traverse(osg::NodeVisitor &nv);


    void setOccludeCallback(OccludeCallback *cb) { m_occlude_callback = cb; }
    OccludeCallback *getOccludeCallback() { return m_occlude_callback.get(); }
    const OccludeCallback *getOccludeCallback() const { return m_occlude_callback.get(); }
protected:

    /// Destructor
    virtual ~SoundNode() {}

    /// Assignment operator
    SoundNode &operator=(const SoundNode &node);

    /*!@Todo: The SoundState should be referenced with a ref_ptr,
        This currently causes problem, as the soundstate, and therefore any referenced soundsources
        are held, until this node is deleted. Which can be by OpenSceneGraph model cache. The time of this event can
        be late, outside the scope of main. This causes problems with OpenAL, which for some reason reports an invalid
        context during the deletion of that source. Therefore, just an ordinary pointer. Memory is properly
        deallocated by the SoundManager, so its not really a problem.
        */
    vsg::observer_ptr<SoundState> m_sound_state;
    //osg::ref_ptr<SoundState> m_sound_state;        // Sukender: I'm testing it with OpenAL-Soft and OSG 2.7+...

private:
    SoundManager *m_sound_manager;

    vsg::ref_ptr<OccludeCallback> m_occlude_callback;
    double m_last_time;
    bool m_first_run;
    vsg::vec3 m_last_pos;
    int m_last_traversal_number;
};

// INLINE FUNCTIONS


} // namespace vsgAudio


#endif // vsgAudio_SOUNDNODE_H
