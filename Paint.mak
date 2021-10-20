# Microsoft Developer Studio Generated NMAKE File, Based on Paint.dsp
!IF "$(CFG)" == ""
CFG=Paint - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Paint - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Paint - Win32 Release" && "$(CFG)" != "Paint - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Paint.mak" CFG="Paint - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Paint - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Paint - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Paint - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\Paint.exe"


CLEAN :
	-@erase "$(INTDIR)\Credits.obj"
	-@erase "$(INTDIR)\Paint.obj"
	-@erase "$(INTDIR)\Paint.res"
	-@erase "$(INTDIR)\splash.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Paint.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\Paint.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Paint.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Paint.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\Paint.pdb" /machine:I386 /out:"$(OUTDIR)\Paint.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Credits.obj" \
	"$(INTDIR)\Paint.obj" \
	"$(INTDIR)\splash.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Paint.res"

"$(OUTDIR)\Paint.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Paint - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\Paint.exe" "$(OUTDIR)\Paint.bsc"


CLEAN :
	-@erase "$(INTDIR)\Credits.obj"
	-@erase "$(INTDIR)\Credits.sbr"
	-@erase "$(INTDIR)\Paint.obj"
	-@erase "$(INTDIR)\Paint.res"
	-@erase "$(INTDIR)\Paint.sbr"
	-@erase "$(INTDIR)\splash.obj"
	-@erase "$(INTDIR)\splash.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Paint.bsc"
	-@erase "$(OUTDIR)\Paint.exe"
	-@erase "$(OUTDIR)\Paint.ilk"
	-@erase "$(OUTDIR)\Paint.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Paint.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Paint.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Paint.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\Credits.sbr" \
	"$(INTDIR)\Paint.sbr" \
	"$(INTDIR)\splash.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\Paint.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\Paint.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Paint.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\Credits.obj" \
	"$(INTDIR)\Paint.obj" \
	"$(INTDIR)\splash.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Paint.res"

"$(OUTDIR)\Paint.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Paint.dep")
!INCLUDE "Paint.dep"
!ELSE 
!MESSAGE Warning: cannot find "Paint.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Paint - Win32 Release" || "$(CFG)" == "Paint - Win32 Debug"
SOURCE=.\Credits.cpp

!IF  "$(CFG)" == "Paint - Win32 Release"


"$(INTDIR)\Credits.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Paint - Win32 Debug"


"$(INTDIR)\Credits.obj"	"$(INTDIR)\Credits.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Paint.cpp

!IF  "$(CFG)" == "Paint - Win32 Release"


"$(INTDIR)\Paint.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Paint - Win32 Debug"


"$(INTDIR)\Paint.obj"	"$(INTDIR)\Paint.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Paint.rc

"$(INTDIR)\Paint.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\splash.cpp

!IF  "$(CFG)" == "Paint - Win32 Release"


"$(INTDIR)\splash.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Paint - Win32 Debug"


"$(INTDIR)\splash.obj"	"$(INTDIR)\splash.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "Paint - Win32 Release"


"$(INTDIR)\StdAfx.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Paint - Win32 Debug"


"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 

