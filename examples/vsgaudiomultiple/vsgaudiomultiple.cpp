/* -*-c++-*- $Id: vsgAudiomultiple.cpp */
/**
 * vsgAudio - OpenSceneGraph Audio Library
 * (C) Copyright 2009-2012 byKenneth Mark Bryden
 * (programming by Chris 'Xenon' Hanson, AlphaPixel, LLC xenon at alphapixel.com)
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

#include <vsg/io/read.h>
#include <vsg/utils/Builder.h>
#include <vsg/utils/CommandLine.h>
#include <vsg/utils/ComputeBounds.h>
#include <vsg/app/Trackball.h>
#include <vsg/app/CloseHandler.h>
#include <vsg/nodes/StateGroup.h>

#define USE_SOUNDNODE 1
#ifdef USE_SOUNDNODE
#  include <vsgAudio/SoundNode.h>
#else
#  include <vsgAudio/SoundUpdateCB.h>
#endif
#include <vsgAudio/SoundUpdateCB.h>
#include <vsgAudio/SoundRoot.h>
#include <vsgAudio/SoundManager.h>
#include <vsgAudio/SoundState.h>
#include <vsgAudio/Version.h>


/*
vsg::AnimationPath* createAnimationPath(const vsg::vec3& center,float radius,double looptime)
{
    // set up the animation path
    vsg::AnimationPath* animationPath = new vsg::AnimationPath;
    animationPath->setLoopMode(vsg::AnimationPath::LOOP);

    int numSamples = 40;
    float yaw = 0.0f;
    float yaw_delta = 2.0f*vsg::PI/((float)numSamples-1.0f);
    float roll = vsg::inDegrees(30.0f);

    double time=0.0f;
    double time_delta = looptime/(double)numSamples;
    for(int i=0;i<numSamples;++i)
    {
        vsg::vec3 position(center+vsg::vec3(sinf(yaw)*radius,cosf(yaw)*radius,0.0f));
        vsg::Quat rotation(vsg::Quat(roll,vsg::vec3(0.0,1.0,0.0))*vsg::Quat(-(yaw+vsg::inDegrees(90.0f)),vsg::vec3(0.0,0.0,1.0)));

        animationPath->insert(time,vsg::AnimationPath::ControlPoint(position,rotation));

        yaw += yaw_delta;
        time += time_delta;
    }
    return animationPath;
}
*/
vsg::ref_ptr<vsg::Node> createBase(const vsg::vec3& center,float radius)
{
    auto geode = vsg::Group::create();
    int numTilesX = 10;
    int numTilesY = 10;

    float width = 2*radius;
    float height = 2*radius;

    vsg::vec3 v000(center - vsg::vec3(width*0.5f,height*0.5f,0.0f));
    vsg::vec3 dx(vsg::vec3(width/((float)numTilesX),0.0,0.0f));
    vsg::vec3 dy(vsg::vec3(0.0f,height/((float)numTilesY),0.0f));

    // fill in vertices for grid, note numTilesX+1 * numTilesY+1...
    vsg::vec3Array* coords = new vsg::vec3Array(numTilesY*numTilesX);
    int iy;
    vsg::Builder builder;
    vsg::GeometryInfo geomInfo;
    vsg::StateInfo stateInfo;
    for(iy=0;iy<=numTilesY;++iy)
    {
        for(int ix=0;ix<=numTilesX;++ix)
        {


            geomInfo.color = vsg::vec4{1, 1, 1, 1};
            geomInfo.position= v000+dx*(float)ix+dy*(float)iy;
            geomInfo.dx = dx;
            geomInfo.dy = dy;

            auto node = builder.createQuad(geomInfo, stateInfo);
            auto stateGroup = builder.createStateGroup(stateInfo);
            geode->addChild(node);
            //coords->at(ix*numTilesX+ix)=v000+dx*(float)ix+dy*(float)iy;
        }
    }

    //Just two colours - black and white.
    /*   vsg::vec4Array* colors = new vsg::vec4Array(2);
    colors->at(0)=vsg::vec4(1.0f,1.0f,1.0f,1.0f); // white
    colors->at(1)=vsg::vec4(0.0f,0.0f,0.0f,1.0f); // black
    int numColors=colors->size();


    int numIndicesPerRow=numTilesX+1;
    vsg::UByteArray* coordIndices = new vsg::UByteArray; // assumes we are using less than 256 points...
    for(iy=0;iy<numTilesY;++iy)
    {
        for(int ix=0;ix<numTilesX;++ix)
        {
            // four vertices per quad.
            coordIndices->push_back(ix    +(iy+1)*numIndicesPerRow);
            coordIndices->push_back(ix    +iy*numIndicesPerRow);
            coordIndices->push_back((ix+1)+iy*numIndicesPerRow);
            coordIndices->push_back((ix+1)+(iy+1)*numIndicesPerRow);

            // one color per quad
            colorIndices->push_back((ix+iy)%numColors);
        }
    }

   */

    return geode;
}

vsgAudio::SoundState* createSoundState(const std::string& file)
{
    // Create a sample, load a .wav file.
    vsg::ref_ptr<vsgAudio::Sample> sample =
        vsgAudio::SoundManager::instance()->getSample(file.c_str(), false);
    // Create a named sound state.
    vsg::ref_ptr<vsgAudio::SoundState> sound_state =  vsgAudio::SoundState::create( file );
    // Allocate a hardware soundsource to this soundstate (priority 10)
    sound_state->allocateSource(10, false);
    // Let the soundstate use the sample we just created
    sound_state->setSample(sample);

    // Set its gain (volume) to 0.9
    sound_state->setGain(0.9f);

    sound_state->setReferenceDistance(70);

    // Set its pitch to 1 (normal speed)
    sound_state->setPitch(1);

    // Make it play
    sound_state->setPlay(true);

    // The sound should loop over and over again
    sound_state->setLooping(true);

    // Add the soundstate to the sound manager, so we can find it later on if we want to
    vsgAudio::SoundManager::instance()->addSoundState(sound_state);

    return sound_state;
}

vsg::ref_ptr<vsg::Node> createMovingModel(const vsg::vec3& center, float radius)
{
    float animationLength = 10.0f;

    // vsg::AnimationPath* animationPath = createAnimationPath(center,radius,animationLength);

    auto model = vsg::Group::create();

    const std::string fileName( "Duck.vsgt" );
    auto options = vsg::Options::create();
    options->sharedObjects = vsg::SharedObjects::create();
    options->fileCache = vsg::getEnv("VSG_FILE_CACHE");
    options->paths = vsg::getEnvPaths("VSG_FILE_PATH");
    vsg::ref_ptr<vsg::Node>glider =vsg::read_cast<vsg::Node>(fileName,options);

    //vsg::Node* glider = osgDB::readNodeFile("glider.osg");
    if (glider)
    {
        vsg::ComputeBounds computeBounds;
        glider->accept(computeBounds);
        vsg::dvec3 centre = (computeBounds.bounds.min + computeBounds.bounds.max) * 0.5;
        vsg::dvec3 size(computeBounds.bounds.max-computeBounds.bounds.min);

        // vsg::BoundingSphere::value_type size = radius/bs.radius()*0.3f;
        auto positioned =  vsg::MatrixTransform::create();
        positioned->matrix=(  vsg::translate(-centre)*
                              vsg::scale(size*0.3)*
                              vsg::rotate(vsg::PI/180.*(-90.0),0.0,0.0,1.0));

        positioned->addChild(glider);
        model->addChild(positioned);
        // Create a sound node
        vsg::ref_ptr<vsgAudio::SoundNode> sound_node = vsgAudio::SoundNode::create(createSoundState("bee.wav"));

        // Add the sound node
        positioned->addChild(sound_node);

        /*     vsg::PositionAttitudeTransform* xform = new vsg::PositionAttitudeTransform;
        xform->setUpdateCallback(new vsg::AnimationPathCallback(animationPath,0.0,1.0));
        xform->addChild(positioned);

        model->addChild(xform);*/
    }

    vsg::Node* cessna = vsg::read_cast<vsg::Node>("cessna.vsgt");
    if (cessna)
    {
        /*  const vsg::BoundingSphere& bs = cessna->getBound();

        vsg::BoundingSphere::value_type size = radius/bs.radius()*0.3f;
        vsg::MatrixTransform* positioned = new vsg::MatrixTransform;
        positioned->setDataVariance(vsg::Object::STATIC);
        positioned->setMatrix(vsg::Matrix::translate(-bs.center())*
                              vsg::Matrix::scale(size,size,size)*
                              vsg::Matrix::rotate(vsg::inDegrees(180.0f),0.0f,0.0f,1.0f));

        positioned->addChild(cessna);

        vsg::MatrixTransform* xform = new vsg::MatrixTransform;
        xform->setUpdateCallback(new vsg::AnimationPathCallback(animationPath,0.0f,2.0));
        xform->addChild(positioned);

        model->addChild(xform);*/
    }

    return model;
}


vsg::ref_ptr<vsg::Node> createModel()
{
    vsg::vec3 center(0.0f,0.0f,0.0f);
    float radius = 100.0f;

    auto root = vsg::Group::create();

    root->addChild(createMovingModel(center,radius*0.8f));

    root->addChild(createBase(center-vsg::vec3(0.0f,0.0f,radius*0.5),radius));

    return root;
}


int main( int argc, char **argv )
{
    vsg::warn( "\n\n" , vsgAudio::getLibraryName() , " demo" );
    vsg::warn( "Version: " , vsgAudio::getVersion() , "\n\n" );

    vsg::warn( "Demonstrates how to create and destroy soundsources on the fly" );


    try {
        // set up defaults and read command line arguments to override them
        vsg::CommandLine arguments(&argc, argv);

        // if we want to redirect std::cout and std::cerr to the vsg::Logger call vsg::Logger::redirect_stdout()
        if (arguments.read({"--redirect-std", "-r"})) vsg::Logger::instance()->redirect_std();

        // set up vsg::Options to pass in filepaths, ReaderWriters and other IO related options to use when reading and writing files.
        auto options = vsg::Options::create();
        options->sharedObjects = vsg::SharedObjects::create();
        options->fileCache = vsg::getEnv("VSG_FILE_CACHE");
        options->paths = vsg::getEnvPaths("VSG_FILE_PATH");

#ifdef vsgXchange_all
        // add vsgXchange's support for reading and writing 3rd party file formats
        options->add(vsgXchange::all::create());
#endif

        arguments.read(options);

        if (uint32_t numOperationThreads = 0; arguments.read("--ot", numOperationThreads)) options->operationThreads = vsg::OperationThreads::create(numOperationThreads);

        auto windowTraits = vsg::WindowTraits::create();
        windowTraits->windowTitle = "vsgviewer";
        windowTraits->debugLayer = arguments.read({"--debug", "-d"});
        windowTraits->apiDumpLayer = arguments.read({"--api", "-a"});
        windowTraits->synchronizationLayer = arguments.read("--sync");
        bool reportAverageFrameRate = arguments.read("--fps");
        if (arguments.read("--double-buffer")) windowTraits->swapchainPreferences.imageCount = 2;
        if (arguments.read("--triple-buffer")) windowTraits->swapchainPreferences.imageCount = 3; // default
        if (arguments.read("--IMMEDIATE")) { windowTraits->swapchainPreferences.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR; }
        if (arguments.read("--FIFO")) windowTraits->swapchainPreferences.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        if (arguments.read("--FIFO_RELAXED")) windowTraits->swapchainPreferences.presentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
        if (arguments.read("--MAILBOX")) windowTraits->swapchainPreferences.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
        if (arguments.read({"-t", "--test"}))
        {
            windowTraits->swapchainPreferences.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
            windowTraits->fullscreen = true;
            reportAverageFrameRate = true;
        }
        if (arguments.read({"--st", "--small-test"}))
        {
            windowTraits->swapchainPreferences.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
            windowTraits->width = 192, windowTraits->height = 108;
            windowTraits->decoration = false;
            reportAverageFrameRate = true;
        }

        bool multiThreading = arguments.read("--mt");
        if (arguments.read({"--fullscreen", "--fs"})) windowTraits->fullscreen = true;
        if (arguments.read({"--window", "-w"}, windowTraits->width, windowTraits->height)) { windowTraits->fullscreen = false; }
        if (arguments.read({"--no-frame", "--nf"})) windowTraits->decoration = false;
        if (arguments.read("--or")) windowTraits->overrideRedirect = true;
        auto maxTime = arguments.value(std::numeric_limits<double>::max(), "--max-time");

        if (arguments.read("--d32")) windowTraits->depthFormat = VK_FORMAT_D32_SFLOAT;
        if (arguments.read("--sRGB")) windowTraits->swapchainPreferences.surfaceFormat = {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
        if (arguments.read("--RGB")) windowTraits->swapchainPreferences.surfaceFormat = {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};

        arguments.read("--screen", windowTraits->screenNum);
        arguments.read("--display", windowTraits->display);
        arguments.read("--samples", windowTraits->samples);
        if (int log_level = 0; arguments.read("--log-level", log_level)) vsg::Logger::instance()->level = vsg::Logger::Level(log_level);
        auto numFrames = arguments.value(-1, "-f");
        auto pathFilename = arguments.value<vsg::Path>("", "-p");
        auto loadLevels = arguments.value(0, "--load-levels");
        auto maxPagedLOD = arguments.value(0, "--maxPagedLOD");
        auto horizonMountainHeight = arguments.value(0.0, "--hmh");
        auto nearFarRatio = arguments.value<double>(0.001, "--nfr");
        if (arguments.read("--rgb")) options->mapRGBtoRGBAHint = false;

        bool depthClamp = arguments.read({"--dc", "--depthClamp"});
        if (depthClamp)
        {
            vsg::info("Enabled depth clamp." );
            auto deviceFeatures = windowTraits->deviceFeatures = vsg::DeviceFeatures::create();
            deviceFeatures->get().samplerAnisotropy = VK_TRUE;
            deviceFeatures->get().depthClamp = VK_TRUE;
        }

        // create the viewer and assign window(s) to it
        auto viewer = vsg::Viewer::create();
        auto window = vsg::Window::create(windowTraits);
        if (!window)
        {
            vsg::info( "Could not create window." );
            return 1;
        }

        viewer->addWindow(window);

        int num_hw_soundsources = 10;
        vsgAudio::SoundManager::instance()->init(num_hw_soundsources);
        vsgAudio::SoundManager::instance()->getEnvironment()->setDistanceModel(vsgAudio::InverseDistance);
        vsgAudio::SoundManager::instance()->getEnvironment()->setDopplerFactor(1);

        // load the nodes from the commandline arguments.
        auto model = createModel();
        if (!model)
        {
            return 1;
        }

        // tilt the scene so the default eye position is looking down on the model.
        vsg::ref_ptr<vsg::MatrixTransform> rootnode = vsg::MatrixTransform::create();
        rootnode->matrix= vsg::rotate(vsg::PIf*30.0f/180.f,1.0f,0.0f,0.0f);
        rootnode->addChild(model);


        // Make use of a few sound samples
        std::vector<std::string> wave_vector;
        wave_vector.push_back("a.wav");
        wave_vector.push_back("high-e.wav");
        wave_vector.push_back("low-e.wav");


        vsg::ref_ptr<vsg::MatrixTransform> sound_transform = vsg::MatrixTransform::create();
        sound_transform->matrix = vsg::translate(vsg::vec3(0,-100,50));
        rootnode->addChild(sound_transform);
        /*
        // Create a transformation node onto we will attach a soundnode
        vsg::ref_ptr<vsg::PositionAttitudeTransform> sound_transform = new vsg::PositionAttitudeTransform;
        rootnode->addChild(sound_transform.get());
        sound_transform->setPosition(vsg::vec3(0,-100,50));
*/
        // Create a sphere so we can "see" the sound
        vsg::Builder builder;
        vsg::GeometryInfo geomInfo;
        vsg::StateInfo stateInfo;

        geomInfo.color = vsg::vec4{1, 1, 1, 1};


        auto node = builder.createSphere(geomInfo, stateInfo);

        sound_transform->addChild(node);

#ifdef USE_SOUNDNODE
        vsg::ref_ptr<vsgAudio::SoundNode> sound_node = vsgAudio::SoundNode::create();
        sound_transform->addChild(sound_node);
#else
        vsg::ref_ptr< vsgAudio::SoundUpdateCB > soundCB = new vsgAudio::SoundUpdateCB;
        geode->setUpdateCallback( soundCB);
#endif




        // compute the bounds of the scene graph to help position camera
        vsg::ComputeBounds computeBounds;
        rootnode->accept(computeBounds);
        vsg::dvec3 centre = (computeBounds.bounds.min + computeBounds.bounds.max);
        centre *= 0.5;
        double radius = vsg::length(computeBounds.bounds.max - computeBounds.bounds.min);// * 0.6;

        // set up the camera
        auto lookAt = vsg::LookAt::create(centre + vsg::dvec3(0.0, -radius * 3.5, 0.0), centre, vsg::dvec3(0.0, 0.0, 1.0));

        vsg::ref_ptr<vsg::ProjectionMatrix> perspective;
        auto ellipsoidModel = model->getRefObject<vsg::EllipsoidModel>("EllipsoidModel");
        if (ellipsoidModel)
        {
            perspective = vsg::EllipsoidPerspective::create(lookAt, ellipsoidModel, 30.0, static_cast<double>(window->extent2D().width) / static_cast<double>(window->extent2D().height), nearFarRatio, horizonMountainHeight);
        }
        else
        {
            perspective = vsg::Perspective::create(30.0, static_cast<double>(window->extent2D().width) / static_cast<double>(window->extent2D().height), nearFarRatio * radius, radius * 100);
        }

        auto camera = vsg::Camera::create(perspective, lookAt, vsg::ViewportState::create(window->extent2D()));
        // Create ONE (only one, otherwise the transformation of the listener and update for SoundManager will be
        // called several times, which is not catastrophic, but unnecessary)
        // SoundRoot that will make sure the listener is updated and
        // to keep the internal state of the SoundManager updated
        // This could also be done manually, this is just a handy way of doing it.
        vsg::ref_ptr<vsgAudio::SoundRoot> sound_root = vsgAudio::SoundRoot::create();
        sound_root->setCamera( camera);

        // The position in the scenegraph of this node is not important.
        // Just as long as the cull traversal should be called after any changes to the SoundManager are made.
        rootnode->addChild(sound_root);

        // add close handler to respond to the close window button and pressing escape
        viewer->addEventHandler(vsg::CloseHandler::create(viewer));
        viewer->addEventHandler(vsg::Trackball::create( camera, ellipsoidModel));

        auto commandGraph = vsg::createCommandGraphForView(window, camera, rootnode);
        viewer->assignRecordAndSubmitTaskAndPresentation({commandGraph});
        viewer->compile();
        viewer->addUpdateOperation(vsgAudio::AudioUpdateOperation::create(lookAt, rootnode), vsg::UpdateOperations::ALL_FRAMES);
        viewer->start_point() = vsg::clock::now();
        vsg::clock::time_point curr,start;
        double interval = 2; // 2 seconds interval
        unsigned int n = 0;
        start = vsg::clock::now();
        // rendering main loop
        while (viewer->advanceToNextFrame() && (numFrames < 0 || (numFrames--) > 0) && (viewer->getFrameStamp()->simulationTime < maxTime))
        {
            curr = vsg::clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(curr - start).count() * 0.001f > interval)
            {
                start = curr;

                std::string file = wave_vector[n%wave_vector.size()];

                vsgAudio::SoundManager::instance()->removeSoundState(file);

                n++;
                // Create a sample, load a .wav file.
                file = wave_vector[n%wave_vector.size()];
                bool add_to_cache = true;
                vsg::ref_ptr<vsgAudio::Sample> sample = vsgAudio::SoundManager::instance()->getSample(file.c_str(), add_to_cache);
                vsg::warn("Loading sample: " , file);

                // Create a new soundstate, give it the name of the file we loaded.
                vsg::ref_ptr<vsgAudio::SoundState> sound_state = vsgAudio::SoundState::create(file);
                sound_state->setSample(sample.get());
                sound_state->setGain(2.0f);
                sound_state->setReferenceDistance(60);
                //sound_state->setRolloffFactor(3); // FMOD backend doesn't currently support non-realistic rolloff, so this is omitted
                sound_state->setPlay(true);
                sound_state->setLooping(true);

                // Allocate a hardware soundsource to this soundstate (priority 10)
                sound_state->allocateSource(10, false);

                // Add the soundstate to the sound manager, so we can find it later on if we want to
                vsgAudio::SoundManager::instance()->addSoundState(sound_state);

#ifdef USE_SOUNDNODE
                sound_node->setSoundState(sound_state);
#else
                soundCB->setSoundState( sound_state );
#endif
            }

            // pass any events into EventHandlers assigned to the Viewer
            viewer->handleEvents();

            viewer->update();

            viewer->recordAndSubmit();

            viewer->present();
        }
    }
    catch (std::exception& e) {
        vsg::warn( "Caught: " , e.what() );
    }
    // Very important to call this before end of main.
    // Otherwise OpenAL will do all sorts of strange things after end of main
    // in the destructor of soundmanager.
    /* if (vsg::Referenced::getDeleteHandler()) {
        vsg::Referenced::getDeleteHandler()->setNumFramesToRetainObjects(0);
        vsg::Referenced::getDeleteHandler()->flushAll();
    }*/

    vsgAudio::SoundManager::instance()->shutdown();
    return 0;
}
