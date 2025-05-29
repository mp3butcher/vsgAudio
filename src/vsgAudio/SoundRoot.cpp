// $Id$
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


#include <vsgAudio/SoundManager.h>
#include <vsgAudio/SoundRoot.h>
#include <vsgAudio/SoundUpdateCB.h>
#include <vsg/io/Logger.h>

using namespace vsg;
using namespace vsgAudio;

/*
class SoundRootUpdate : public vsg::NodeCallback
{
public:
    SoundRootUpdate( SoundRoot& sr )
        : _sr( sr )
    {
        vsg::debug(  "SoundRootUpdate: Constructor");
    }
    ~SoundRootUpdate()
    {}

    virtual void operator()( vsg::Node* node, vsg::NodeVisitor* nv )
    {
        _sr.update( nv );

        traverse( node, nv );
    }

protected:
    SoundRoot& _sr;
};

*/

SoundRoot::SoundRoot()
    : //_camera( nullptr ),
    m_last_time(0),
    m_update_enabled(true)
{

    //setCullingActive(false);
}


SoundRoot & SoundRoot::operator=(const SoundRoot &node)
{
    if (this == &node) return *this;

   // _camera = node._camera;
    m_last_time = node.m_last_time;
    m_update_enabled = node.m_update_enabled;

    return *this;
}

SoundRoot::SoundRoot(const SoundRoot &copy, const vsg::CopyOp &copyop)
    : vsg::Inherit<vsg::Node, SoundRoot>( copy, copyop )
{
    *this = copy;
}

void SoundRoot::setCamera( vsg::Camera* cam )
{
   // _camera = cam;
}

const vsg::Camera* SoundRoot::getCamera() const
{
    return(nullptr);// _camera.get() );
}
/// Deffered operations
void opDeferredDeleteAudio::run()
{
    if(_frameDelay <= 0)
    {
        _node = nullptr;
        vsg::ref_ptr<vsg::Viewer> ref_viewer = _viewer;

        if (ref_viewer) ref_viewer->updateOperations->remove(vsg::ref_ptr<opDeferredDeleteAudio>(this));
    }
    _frameDelay--;
}

void opMergeAudio::run()
{
    vsg::ref_ptr<vsg::Viewer> ref_viewer = _viewer;

    if (ref_viewer)
    {
        updateViewer(*ref_viewer, _compileResult);
    }

    _attachmentPoint->addChild(_node);
}

void opDetachAudio::run()
{
    if (_detachmentPoint)
    {
        auto it = std::find(_detachmentPoint->children.begin(), _detachmentPoint->children.end(), _node);
        if(it != _detachmentPoint->children.end())
        {
            _detachmentPoint->children.erase(it);
            vsg::ref_ptr<vsg::Viewer> ref_viewer = _viewer;

            if (ref_viewer)
                ref_viewer->addUpdateOperation(opDeferredDeleteAudio::create(_viewer, _node), vsg::UpdateOperations::ALL_FRAMES);
        }
    }
}

class  FindAudioNodes : public vsg::Inherit<vsg::Visitor, FindAudioNodes>
{
    std::vector<const vsg::Transform *> _localNodePath;
public:
    std::vector< std::pair< vsg::ref_ptr<SoundTransform>,vsg::dmat4> > _foundaudios;
    void apply(vsg::Transform& node ) override
    {
             _localNodePath.push_back(& node); node.traverse(*this);
             _localNodePath.pop_back();
    }
    void apply(vsg::MatrixTransform& node) override
    {
        _localNodePath.push_back(& node);
        if(auto w = node.cast<SoundTransform>()){
            vsg::dmat4 m;
            for(auto loc:_localNodePath) loc->transform(m);
            _foundaudios.push_back(std::make_pair(vsg::ref_ptr< SoundTransform>(w), m));
        }
        node.traverse(*this);
        _localNodePath.pop_back();
    }
    void apply(vsg::Group& node) override{
        node.traverse(*this);
    }
    void apply(vsg::Node& node) override{
        if(auto w = node.cast<SoundTransform>()){
            vsg::dmat4 m;
            for(auto loc:_localNodePath) loc->transform(m);
            _foundaudios.push_back(std::make_pair(vsg::ref_ptr< SoundTransform>(w), m));
        }
        node.traverse(*this);
    }
};

void AudioUpdateOperation::run() {
{
    vsg::debug(  "AudioUpdateOperation::run(). ");


        FindAudioNodes fan;

        _scene->accept(fan);
         fan._foundaudios;



    auto curr_time = vsg::clock::now();
   // const double curr_time( nv->getFrameStamp()->getReferenceTime() );
    auto soundmanager=vsgAudio::SoundManager::instance();
    float time=std::chrono::duration_cast<std::chrono::milliseconds>(curr_time-_last_time).count()*0.001f;

    for(auto audio: fan._foundaudios){

        if( !audio.first->getSoundState() )
        {
            // Early exit.
            vsg::debug("SoundTransform::operator()() No SoundState attached, or invalid FrameStamp." );

            return;
        }


        if(time >= soundmanager->getUpdateFrequency())
        {

            audio.first->updateAudio(audio.second, time);

        }

    }
    bool time_to_update( //( _last_time == 0.0 ) ||
                        ( time >= SoundManager::instance()->getUpdateFrequency() ) );

    if (time_to_update)// && m_update_enabled)
    {
        _last_time = curr_time;

        if( soundmanager->initialized())
        {
            // Update the soundmanager (process queued sound states)
            soundmanager->update();

            // Set the position/orientation of the listener.
            // This is only done if there's a Camera; otherwise, the
            // app must set the matrix explicitly.

               soundmanager->setListenerMatrix( *_worldtrans.get() );

        }
    }
}
}

