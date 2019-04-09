# CurveAttachmentTool


# Installation Instructions


Go into the CONFIGURATION PROPERTIES of the Visual Studio Project
and make sure you have these settings with correct path to your
Autodesk maya installation:


General:

	Target Extension:
		.mll


	Configuration Type:
		Dynamic Library .dll


C++ Properties:

	Include Directories (Replace: Maya2017, with your version of Maya):
		C:\Program Files\Autodesk\Maya2017\include

	Preprocessor:
		WIN32;NDEBUG;_WINDOWS;NT_PLUGIN;REQUIRE_IOSTREAM;

	(if it is a release build use these preprocessor defintions instead): WIN32;NT_PLUGIN;REQUIRE_IOSTREAM;

	C/C++---Runtime Library---Code Generation:
		Multi-threaded Debug DLL /MDd



Linker Properties:


	Library Directories:
		C:\Program Files\Autodesk\Maya2017\lib


	Input (Paste at the END of the Input string):
		;Foundation.lib;OpenMaya.lib;OpenMayaUI.lib;OpenMayaanim.lib;OpenMayaFX.lib;OpenMayaRender.lib;Image.lib;opengl32.lib;glu32.lib;


	Command Line:
		/export:initializePlugin /export:uninitializePlugin
