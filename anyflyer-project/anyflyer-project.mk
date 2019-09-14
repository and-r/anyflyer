##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=anyflyer-project
ConfigurationName      :=Release
WorkspaceConfiguration := $(ConfigurationName)
WorkspacePath          :=/media/ja/BA72B4A872B46B31/Users/Laptop/Documents/Projekty/anyflyer
ProjectPath            :=/media/ja/BA72B4A872B46B31/Users/Laptop/Documents/Projekty/anyflyer/anyflyer-project
IntermediateDirectory  :=../build-$(ConfigurationName)/anyflyer-project
OutDir                 :=../build-$(ConfigurationName)/anyflyer-project
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Ja
Date                   :=14/09/19
CodeLitePath           :=/home/ja/.codelite
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=../build-$(ConfigurationName)/bin/anyflyer
Preprocessors          :=$(PreprocessorSwitch)NDEBUG 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :=$(IntermediateDirectory)/ObjectsList.txt
PCHCompileFlags        :=
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)/home/ja/Dokumenty/irrlicht-1.8.4/include 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)Irrlicht 
ArLibs                 :=  "Irrlicht" 
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS :=  -O2 -Wall $(Preprocessors)
CFLAGS   :=  -O2 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=../build-$(ConfigurationName)/anyflyer-project/anyflyer_main.cpp$(ObjectSuffix) ../build-$(ConfigurationName)/anyflyer-project/camera.cpp$(ObjectSuffix) ../build-$(ConfigurationName)/anyflyer-project/dictionary.cpp$(ObjectSuffix) ../build-$(ConfigurationName)/anyflyer-project/flightplan.cpp$(ObjectSuffix) ../build-$(ConfigurationName)/anyflyer-project/simulation_manager.cpp$(ObjectSuffix) ../build-$(ConfigurationName)/anyflyer-project/uavnode.cpp$(ObjectSuffix) ../build-$(ConfigurationName)/anyflyer-project/simeventreceiver.cpp$(ObjectSuffix) ../build-$(ConfigurationName)/anyflyer-project/settingsmgr.cpp$(ObjectSuffix) ../build-$(ConfigurationName)/anyflyer-project/uavarray.cpp$(ObjectSuffix) ../build-$(ConfigurationName)/anyflyer-project/menu.cpp$(ObjectSuffix) \
	../build-$(ConfigurationName)/anyflyer-project/menulist.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: MakeIntermediateDirs $(OutputFile)

$(OutputFile): ../build-$(ConfigurationName)/anyflyer-project/.d $(Objects) 
	@mkdir -p "../build-$(ConfigurationName)/anyflyer-project"
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

PostBuild:
	@echo Executing Post Build commands ...
	mv /media/ja/BA72B4A872B46B31/Users/Laptop/Documents/Projekty/anyflyer/build-Release/bin/anyflyer /media/ja/BA72B4A872B46B31/Users/Laptop/Documents/Projekty/anyflyer/workdir/anyflyer
	@echo Done

MakeIntermediateDirs:
	@mkdir -p "../build-$(ConfigurationName)/anyflyer-project"
	@mkdir -p ""../build-$(ConfigurationName)/bin""

../build-$(ConfigurationName)/anyflyer-project/.d:
	@mkdir -p "../build-$(ConfigurationName)/anyflyer-project"

PreBuild:


##
## Objects
##
../build-$(ConfigurationName)/anyflyer-project/anyflyer_main.cpp$(ObjectSuffix): anyflyer_main.cpp ../build-$(ConfigurationName)/anyflyer-project/anyflyer_main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/media/ja/BA72B4A872B46B31/Users/Laptop/Documents/Projekty/anyflyer/anyflyer-project/anyflyer_main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/anyflyer_main.cpp$(ObjectSuffix) $(IncludePath)
../build-$(ConfigurationName)/anyflyer-project/anyflyer_main.cpp$(DependSuffix): anyflyer_main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT../build-$(ConfigurationName)/anyflyer-project/anyflyer_main.cpp$(ObjectSuffix) -MF../build-$(ConfigurationName)/anyflyer-project/anyflyer_main.cpp$(DependSuffix) -MM anyflyer_main.cpp

../build-$(ConfigurationName)/anyflyer-project/anyflyer_main.cpp$(PreprocessSuffix): anyflyer_main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) ../build-$(ConfigurationName)/anyflyer-project/anyflyer_main.cpp$(PreprocessSuffix) anyflyer_main.cpp

../build-$(ConfigurationName)/anyflyer-project/camera.cpp$(ObjectSuffix): camera.cpp ../build-$(ConfigurationName)/anyflyer-project/camera.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/media/ja/BA72B4A872B46B31/Users/Laptop/Documents/Projekty/anyflyer/anyflyer-project/camera.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/camera.cpp$(ObjectSuffix) $(IncludePath)
../build-$(ConfigurationName)/anyflyer-project/camera.cpp$(DependSuffix): camera.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT../build-$(ConfigurationName)/anyflyer-project/camera.cpp$(ObjectSuffix) -MF../build-$(ConfigurationName)/anyflyer-project/camera.cpp$(DependSuffix) -MM camera.cpp

../build-$(ConfigurationName)/anyflyer-project/camera.cpp$(PreprocessSuffix): camera.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) ../build-$(ConfigurationName)/anyflyer-project/camera.cpp$(PreprocessSuffix) camera.cpp

../build-$(ConfigurationName)/anyflyer-project/dictionary.cpp$(ObjectSuffix): dictionary.cpp ../build-$(ConfigurationName)/anyflyer-project/dictionary.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/media/ja/BA72B4A872B46B31/Users/Laptop/Documents/Projekty/anyflyer/anyflyer-project/dictionary.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/dictionary.cpp$(ObjectSuffix) $(IncludePath)
../build-$(ConfigurationName)/anyflyer-project/dictionary.cpp$(DependSuffix): dictionary.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT../build-$(ConfigurationName)/anyflyer-project/dictionary.cpp$(ObjectSuffix) -MF../build-$(ConfigurationName)/anyflyer-project/dictionary.cpp$(DependSuffix) -MM dictionary.cpp

../build-$(ConfigurationName)/anyflyer-project/dictionary.cpp$(PreprocessSuffix): dictionary.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) ../build-$(ConfigurationName)/anyflyer-project/dictionary.cpp$(PreprocessSuffix) dictionary.cpp

../build-$(ConfigurationName)/anyflyer-project/flightplan.cpp$(ObjectSuffix): flightplan.cpp ../build-$(ConfigurationName)/anyflyer-project/flightplan.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/media/ja/BA72B4A872B46B31/Users/Laptop/Documents/Projekty/anyflyer/anyflyer-project/flightplan.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/flightplan.cpp$(ObjectSuffix) $(IncludePath)
../build-$(ConfigurationName)/anyflyer-project/flightplan.cpp$(DependSuffix): flightplan.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT../build-$(ConfigurationName)/anyflyer-project/flightplan.cpp$(ObjectSuffix) -MF../build-$(ConfigurationName)/anyflyer-project/flightplan.cpp$(DependSuffix) -MM flightplan.cpp

../build-$(ConfigurationName)/anyflyer-project/flightplan.cpp$(PreprocessSuffix): flightplan.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) ../build-$(ConfigurationName)/anyflyer-project/flightplan.cpp$(PreprocessSuffix) flightplan.cpp

../build-$(ConfigurationName)/anyflyer-project/simulation_manager.cpp$(ObjectSuffix): simulation_manager.cpp ../build-$(ConfigurationName)/anyflyer-project/simulation_manager.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/media/ja/BA72B4A872B46B31/Users/Laptop/Documents/Projekty/anyflyer/anyflyer-project/simulation_manager.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/simulation_manager.cpp$(ObjectSuffix) $(IncludePath)
../build-$(ConfigurationName)/anyflyer-project/simulation_manager.cpp$(DependSuffix): simulation_manager.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT../build-$(ConfigurationName)/anyflyer-project/simulation_manager.cpp$(ObjectSuffix) -MF../build-$(ConfigurationName)/anyflyer-project/simulation_manager.cpp$(DependSuffix) -MM simulation_manager.cpp

../build-$(ConfigurationName)/anyflyer-project/simulation_manager.cpp$(PreprocessSuffix): simulation_manager.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) ../build-$(ConfigurationName)/anyflyer-project/simulation_manager.cpp$(PreprocessSuffix) simulation_manager.cpp

../build-$(ConfigurationName)/anyflyer-project/uavnode.cpp$(ObjectSuffix): uavnode.cpp ../build-$(ConfigurationName)/anyflyer-project/uavnode.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/media/ja/BA72B4A872B46B31/Users/Laptop/Documents/Projekty/anyflyer/anyflyer-project/uavnode.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/uavnode.cpp$(ObjectSuffix) $(IncludePath)
../build-$(ConfigurationName)/anyflyer-project/uavnode.cpp$(DependSuffix): uavnode.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT../build-$(ConfigurationName)/anyflyer-project/uavnode.cpp$(ObjectSuffix) -MF../build-$(ConfigurationName)/anyflyer-project/uavnode.cpp$(DependSuffix) -MM uavnode.cpp

../build-$(ConfigurationName)/anyflyer-project/uavnode.cpp$(PreprocessSuffix): uavnode.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) ../build-$(ConfigurationName)/anyflyer-project/uavnode.cpp$(PreprocessSuffix) uavnode.cpp

../build-$(ConfigurationName)/anyflyer-project/simeventreceiver.cpp$(ObjectSuffix): simeventreceiver.cpp ../build-$(ConfigurationName)/anyflyer-project/simeventreceiver.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/media/ja/BA72B4A872B46B31/Users/Laptop/Documents/Projekty/anyflyer/anyflyer-project/simeventreceiver.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/simeventreceiver.cpp$(ObjectSuffix) $(IncludePath)
../build-$(ConfigurationName)/anyflyer-project/simeventreceiver.cpp$(DependSuffix): simeventreceiver.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT../build-$(ConfigurationName)/anyflyer-project/simeventreceiver.cpp$(ObjectSuffix) -MF../build-$(ConfigurationName)/anyflyer-project/simeventreceiver.cpp$(DependSuffix) -MM simeventreceiver.cpp

../build-$(ConfigurationName)/anyflyer-project/simeventreceiver.cpp$(PreprocessSuffix): simeventreceiver.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) ../build-$(ConfigurationName)/anyflyer-project/simeventreceiver.cpp$(PreprocessSuffix) simeventreceiver.cpp

../build-$(ConfigurationName)/anyflyer-project/settingsmgr.cpp$(ObjectSuffix): settingsmgr.cpp ../build-$(ConfigurationName)/anyflyer-project/settingsmgr.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/media/ja/BA72B4A872B46B31/Users/Laptop/Documents/Projekty/anyflyer/anyflyer-project/settingsmgr.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/settingsmgr.cpp$(ObjectSuffix) $(IncludePath)
../build-$(ConfigurationName)/anyflyer-project/settingsmgr.cpp$(DependSuffix): settingsmgr.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT../build-$(ConfigurationName)/anyflyer-project/settingsmgr.cpp$(ObjectSuffix) -MF../build-$(ConfigurationName)/anyflyer-project/settingsmgr.cpp$(DependSuffix) -MM settingsmgr.cpp

../build-$(ConfigurationName)/anyflyer-project/settingsmgr.cpp$(PreprocessSuffix): settingsmgr.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) ../build-$(ConfigurationName)/anyflyer-project/settingsmgr.cpp$(PreprocessSuffix) settingsmgr.cpp

../build-$(ConfigurationName)/anyflyer-project/uavarray.cpp$(ObjectSuffix): uavarray.cpp ../build-$(ConfigurationName)/anyflyer-project/uavarray.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/media/ja/BA72B4A872B46B31/Users/Laptop/Documents/Projekty/anyflyer/anyflyer-project/uavarray.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/uavarray.cpp$(ObjectSuffix) $(IncludePath)
../build-$(ConfigurationName)/anyflyer-project/uavarray.cpp$(DependSuffix): uavarray.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT../build-$(ConfigurationName)/anyflyer-project/uavarray.cpp$(ObjectSuffix) -MF../build-$(ConfigurationName)/anyflyer-project/uavarray.cpp$(DependSuffix) -MM uavarray.cpp

../build-$(ConfigurationName)/anyflyer-project/uavarray.cpp$(PreprocessSuffix): uavarray.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) ../build-$(ConfigurationName)/anyflyer-project/uavarray.cpp$(PreprocessSuffix) uavarray.cpp

../build-$(ConfigurationName)/anyflyer-project/menu.cpp$(ObjectSuffix): menu.cpp ../build-$(ConfigurationName)/anyflyer-project/menu.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/media/ja/BA72B4A872B46B31/Users/Laptop/Documents/Projekty/anyflyer/anyflyer-project/menu.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/menu.cpp$(ObjectSuffix) $(IncludePath)
../build-$(ConfigurationName)/anyflyer-project/menu.cpp$(DependSuffix): menu.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT../build-$(ConfigurationName)/anyflyer-project/menu.cpp$(ObjectSuffix) -MF../build-$(ConfigurationName)/anyflyer-project/menu.cpp$(DependSuffix) -MM menu.cpp

../build-$(ConfigurationName)/anyflyer-project/menu.cpp$(PreprocessSuffix): menu.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) ../build-$(ConfigurationName)/anyflyer-project/menu.cpp$(PreprocessSuffix) menu.cpp

../build-$(ConfigurationName)/anyflyer-project/menulist.cpp$(ObjectSuffix): menulist.cpp ../build-$(ConfigurationName)/anyflyer-project/menulist.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/media/ja/BA72B4A872B46B31/Users/Laptop/Documents/Projekty/anyflyer/anyflyer-project/menulist.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/menulist.cpp$(ObjectSuffix) $(IncludePath)
../build-$(ConfigurationName)/anyflyer-project/menulist.cpp$(DependSuffix): menulist.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT../build-$(ConfigurationName)/anyflyer-project/menulist.cpp$(ObjectSuffix) -MF../build-$(ConfigurationName)/anyflyer-project/menulist.cpp$(DependSuffix) -MM menulist.cpp

../build-$(ConfigurationName)/anyflyer-project/menulist.cpp$(PreprocessSuffix): menulist.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) ../build-$(ConfigurationName)/anyflyer-project/menulist.cpp$(PreprocessSuffix) menulist.cpp


-include ../build-$(ConfigurationName)/anyflyer-project//*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r $(IntermediateDirectory)


