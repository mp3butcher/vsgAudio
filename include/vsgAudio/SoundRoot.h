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
#include "vsg/app/Viewer.h"
#ifndef vsgAudio_SOUNDROOT_H
#define vsgAudio_SOUNDROOT_H 1


#include "SoundUpdateCB.h"

#include <vsgAudio/Export.h>

#include <vsg/nodes/Node.h>
#include <vsg/threading/OperationThreads.h>
#include <vsg/ui/UIEvent.h>
#include <vsg/app/ViewMatrix.h>

#include <vsg/threading/OperationQueue.h>
#include <vsg/core/Inherit.h>
#include <vsg/core/observer_ptr.h>
#include <vsg/app/CompileManager.h>
namespace vsgAudio
{
class VSGAUDIO_EXPORT  AudioUpdateOperation : public vsg::Inherit<vsg::Operation, AudioUpdateOperation> {
public:
    AudioUpdateOperation(vsg::ref_ptr<vsg::LookAt> &p, vsg::ref_ptr<vsg::Node> &scene): _worldtrans(p),_scene(scene){};

    vsg::clock::time_point _last_time;
    void run() override;
    std::vector<vsg::ref_ptr<SoundTransform> > _managedaudionodes;
protected:
    vsg::ref_ptr<vsg::LookAt>  _worldtrans;
    vsg::ref_ptr<vsg::Node>  _scene;
};

/// A node that updates the transformation of the listener. Should be traversed before any SoundNode nodes
/**
    This class updates the internal state of the vsgAudio::SoundManager during cull traversal from the current
    modelView matrix.
    The cull traversal for this node should be called after any changes are made to the SoundManager (added
    SoundState events or such).
    */
class VSGAUDIO_EXPORT SoundRoot: public vsg::Node {
public:
    /// Default constructor
    SoundRoot();


    /*!
        The Camera's view matrix is used to position the Listener.
        SoundRoot::update() queries the camera during update to
        obtain the view and update the Listener position.
        If camera is NULL, SoundRoot sets the Listener position to 0,0,0.

        If you have multiple views, specify the Camera for the view you'd
        like to use to represent the Listener's audio experience.

        If you want to specify the Listener position explicitly from your
        application, then there is no need to set a Camera here. However, you
        will need to explicitly set the Listener position after the update
        traversal and before any cull traversals, like so:
            vsgAudio::SoundManager::instance()->setListenerMatrix( m );
        */
    void setCamera( vsg::Camera* cam );
    const vsg::Camera* getCamera() const;

    void setUpdateEnable(bool flag) { m_update_enabled = flag; }
    bool getUpdateEnable()  const { return m_update_enabled; }

    /*!
        Called by the update callback during the update traversal.
        Sets the orientation/position and velocity of the Listener.
        */
    //void update( osg::NodeVisitor* nv );


protected:
    /// Destructor
    virtual ~SoundRoot() {}

    /// Copy constructor
    SoundRoot(const SoundRoot &copy, const vsg::CopyOp &copyop = {});

    /// Assignment operator
    SoundRoot &operator=(const SoundRoot &node);


    //vsg::ref_ptr< vsg::Camera > _camera;

    double m_last_time;
    bool m_update_enabled;
};


/**  vsg::Viewer utilities **/
struct opDeferredDeleteAudio : public vsg::Inherit<vsg::Operation, opDeferredDeleteAudio>
{
    opDeferredDeleteAudio(vsg::observer_ptr<vsg::Viewer> viewer,vsg::ref_ptr<SoundTransform> node, int frameDelay = 4):
        _node(node),
        _viewer(viewer),
        _frameDelay(frameDelay)
    {}

    vsg::ref_ptr<SoundTransform> _node;
    vsg::observer_ptr<vsg::Viewer>  _viewer;
    int _frameDelay;

    void run() override;
};

struct opMergeAudio : public vsg::Inherit<vsg::Operation, opMergeAudio>
{
    opMergeAudio(vsg::observer_ptr<vsg::Viewer> viewer, vsg::observer_ptr<AudioUpdateOperation> audiooperation, vsg::ref_ptr<vsg::Group> attachmentPoint, vsg::ref_ptr<SoundTransform> node, const vsg::CompileResult& compileResult):
        _viewer(viewer),
        _audiooperation(audiooperation),
        _attachmentPoint(attachmentPoint),
        _node(node),
        _compileResult(compileResult)
    {}

    vsg::observer_ptr<vsg::Viewer> _viewer;
    vsg::observer_ptr<AudioUpdateOperation> _audiooperation;
    vsg::ref_ptr<vsg::Group> _attachmentPoint;
    vsg::ref_ptr<SoundTransform> _node;
    vsg::CompileResult _compileResult;

    void run() override;
};

struct opDetachAudio : public vsg::Inherit<vsg::Operation, opDetachAudio>
{
    opDetachAudio(vsg::observer_ptr<vsg::Viewer> viewer, vsg::observer_ptr<AudioUpdateOperation> audiooperation, vsg::ref_ptr<vsg::Group> detachmentPoint, vsg::ref_ptr<SoundTransform> node):
        _viewer(viewer),
        _audiooperation(audiooperation),
        _detachmentPoint(detachmentPoint),
        _node(node)
    {}

    vsg::observer_ptr<vsg::Viewer> _viewer;
    vsg::observer_ptr<AudioUpdateOperation> _audiooperation;
    vsg::ref_ptr<vsg::Group> _detachmentPoint;
    vsg::ref_ptr<SoundTransform> _node;

    void run() override;
};

} // namespace vsgAudio


#endif //vsgAudio_SOUNDROOT_H
