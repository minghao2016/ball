### list all filenames of the directory here ###
SET(SOURCES_LIST
	glDisplayList.C
	glRenderer.C
	renderer.C
	renderSetup.C
	POVRenderer.C
	STLRenderer.C
	tilingRenderer.C
	VRMLRenderer.C
	vertexBuffer.C
	glRenderWindow.C
	glOffscreenTarget.C
	renderWindow.C
	pixelFormat.C
)

### the list of all files requiring a moc run ###
SET(MOC_SOURCES_LIST "")

#INCLUDE(source/VIEW/RENDERING/RENDERERS/sources.cmake)

ADD_VIEW_SOURCES("VIEW/RENDERING" "${SOURCES_LIST}")

ADD_BALL_MOCFILES("VIEW/RENDERING" "include/BALL/VIEW/RENDERING" "${MOC_SOURCES_LIST}")