# CurveAttachmentTool
	This Tool is used to attach a set of transforms to the closest point on a mesh and keep its
	relative orientation to mesh face for natural looking transform movement while the mesh deforms.
	Note that this is 3 seperate visual studio project because each plugin can be used in its own in
	different contexts.


# Installation Instructions

###Note that these instruction apply to ALL of the visual studio projects

Go into the CONFIGURATION PROPERTIES of the Visual Studio Project
and make sure you have these settings with correct path to your
Autodesk maya installation:


General:

	Target Extension:
		.mll


	Configuration Type:
		Dynamic Library .dll


C++ Properties:

	Include Directories (NOTE: replace Maya2017 with your version of Maya):
		C:\Program Files\Autodesk\Maya2017\include

	Preprocessor (NOTE: Paste this at END of the Preprocessor string):
		;WIN32;NDEBUG;_WINDOWS;NT_PLUGIN;REQUIRE_IOSTREAM

		NOTE, if it is a release build use these preprocessor defintions instead: 
			WIN32;NT_PLUGIN;REQUIRE_IOSTREAM;

	Code Generation --> Runtime Library:
		Multi-threaded Debug DLL /MDd



Linker Properties:

	Library Directories (NOTE: replace Maya2017 with your version of maya):
		C:\Program Files\Autodesk\Maya2017\lib


	Input (Paste at the END of the Input string):
		;Foundation.lib;OpenMaya.lib;OpenMayaUI.lib;OpenMayaanim.lib;OpenMayaFX.lib;OpenMayaRender.lib;Image.lib;opengl32.lib;glu32.lib;


	Command Line:
		/export:initializePlugin /export:uninitializePlugin
