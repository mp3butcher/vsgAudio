IF(0_BUILD_EXAMPLES_VSGAUDIO)
	FOREACH( myexamplefolder 
			vsgaudio
                        vsgaudiomultiple
                #	vsgaudioocclude
                #	vsgaudioviewer
	    )
	    ADD_SUBDIRECTORY(${myexamplefolder})
	ENDFOREACH( myexamplefolder )
ENDIF(0_BUILD_EXAMPLES_VSGAUDIO)

IF(0_BUILD_EXAMPLES_VSGAUDIO_LOWLEVEL)
        ADD_SUBDIRECTORY(vsgaudio-lowlevel)
ENDIF(0_BUILD_EXAMPLES_VSGAUDIO_LOWLEVEL)

IF(0_BUILD_EXAMPLES_OALPP)
        ADD_SUBDIRECTORY(openalpp)
ENDIF(0_BUILD_EXAMPLES_OALPP)
