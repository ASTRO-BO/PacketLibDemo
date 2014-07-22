#############################################################################
# Template: exe lib version 2013-10-08
# Use make variable_name=' options ' to override the variables or make -e to
# override the file variables with the environment variables
# 		make CFLAGS='-g'
#		make prefix='/usr'
#		make CC=gcc-4.8
# External environment variable. 
#	Build: CPLUS_INCLUDE_PATH LIBRARY_PATH
#	Execution: LD_LIBRARY_PATH DYLD_LIBRARY_PATH LOCAL  
# Instructions:
# - modify the section 1)
# - in section 10), modify the following action:
#		* all: and or remove exe and lib prerequisite
#		* lib: and or remove staticlib and dynamiclib prerequisite
#		* clean: add or remove the files and directories that should be cleaned
#		* install: add or remove the files and directories that should be installed
#		* uninstall: add or remove the files and directories that should be uninstalled
#############################################################################

PROJECT= PacketLibDemo
SHELL = /bin/sh

####### 1) Project names and system

SYSTEM= $(shell gcc -dumpmachine)
#ice, ctarta, mpi, cfitsio
LINKERENV= ctarta
EXE_NAME0 = packetlibdemo
EXE_NAME1 = exe1
EXE_NAME2 = exe2
EXE_NAME3 = exe3
LIB_NAME = 
VER_FILE_NAME = version.h
#the name of the directory where the conf file are copied (into $(datadir))
CONF_DEST_DIR = packetlibdemo
#the name of the icon for the installation
ICON_NAME=

####### 2) Directories for the installation

# Prefix for each installed program. Only ABSOLUTE PATH
prefix=$(LOCAL)
exec_prefix=$(prefix)
# The directory to install the binary files in.
bindir=$(exec_prefix)/bin
# The directory to install the local configuration file.
datadir=$(exec_prefix)/share
# The directory to install the libraries in.
libdir=$(exec_prefix)/lib
# The directory to install the info files in.
infodir=$(exec_prefix)/info
# The directory to install the include files in.
includedir=$(exec_prefix)/include
# The directory to install the icon
icondir=$(HOME)/.local/share/applications/

####### 3) Directories for the compiler

OBJECTS_DIR = obj
SOURCE_DIR = code
INCLUDE_DIR = code
DOC_DIR = ref
DOXY_SOURCE_DIR = code_filtered
EXE_DESTDIR  = .
LIB_DESTDIR = lib
CONF_DIR= conf
ICON_DIR = ui

####### 4) Compiler, tools and options


#Set INCPATH to add the inclusion paths
INCPATH = -I $(INCLUDE_DIR) 
LIBS = -lstdc++
#Insert the optional parameter to the compiler. The CFLAGS could be changed externally by the user
CFLAGS   = -g
#Insert the implicit parameter to the compiler:
ALL_CFLAGS = -m64 -fexceptions -Wall $(CFLAGS) $(INCPATH)
#Use CPPFLAGS for the preprocessor
CPPFLAGS =

ifneq (, $(findstring ctarta, $(LINKERENV)))
	LIBS += -lpacket 
endif

ifneq (, $(findstring linux, $(SYSTEM)))
        LIBS += -lrt
endif

#Set addition parameters that depends by operating system


LINK     = $CC
#for link
LFLAGS = -shared -Wl,-soname,$(TARGET1) -Wl,-rpath,$(DESTDIR)
AR       = ar cqs
TAR      = tar -cf
GZIP     = gzip -9f
COPY     = cp -f -r
COPY_FILE= $(COPY) -p
COPY_DIR = $(COPY) -pR
DEL_FILE = rm -f
SYMLINK  = ln -sf
DEL_DIR  = rm -rf
MOVE     = mv -f
CHK_DIR_EXISTS= test -d
MKDIR    = mkdir -p

####### 5) VPATH

VPATH=$(SOURCE_DIR):$(INCLUDE_DIR):
vpath %.o $(OBJECTS_DIR)

####### 6) Files of the project
	
INCLUDE=$(foreach dir,$(INCLUDE_DIR), $(wildcard $(dir)/*.h))
SOURCE=$(foreach dir,$(SOURCE_DIR), $(wildcard $(dir)/*.cpp))
SOURCE+=$(foreach dir,$(SOURCE_DIR), $(wildcard $(dir)/*.c))
#Objects to build
OBJECTS=$(addsuffix .o, $(basename $(notdir $(SOURCE))))
#only for documentation generation
DOC_INCLUDE= $(addprefix $(DOXY_SOURCE_DIR)/, $(notdir $(INCLUDE)))
DOC_SOURCE= $(addprefix $(DOXY_SOURCE_DIR)/, $(notdir $(SOURCE)))

####### 7) Only for library generation

TARGET  = $(LIB_NAME).so.$(shell cat version)
TARGETA	= $(LIB_NAME).a
TARGETD	= $(LIB_NAME).so.$(shell cat version)
TARGET0	= $(LIB_NAME).so
TARGET1	= $(LIB_NAME).so.$(shell cut version -f 1 -d '.')
TARGET2	= $(LIB_NAME).so.$(shell cut version -f 1 -d '.').$(shell cut version -f 2 -d '.')

####### 8) Preliminar operations

$(shell  cut $(INCLUDE_DIR)/$(VER_FILE_NAME) -f 3 > version)
#WARNING: use -d ' ' if in the version.h the separator is a space

####### 9) Pattern rules

%.o : %.cpp
	$(CC) $(CPPFLAGS) $(ALL_CFLAGS) -c $< -o $(OBJECTS_DIR)/$@

%.o : %.c
	$(CC) $(CPPFLAGS) $(ALL_CFLAGS) -c $< -o $(OBJECTS_DIR)/$@

#only for documentation generation
$(DOXY_SOURCE_DIR)/%.h : %.h
	cp  $<  $@

$(DOXY_SOURCE_DIR)/%.cpp : %.cpp
	cp  $<  $@
	
####### 10) Build rules

#all: compile the entire program.
all: exe
		#only if conf directory is present:
		#$(SYMLINK) $(CONF_DIR) $(CONF_DEST_DIR)

lib: staticlib 
	
exe: makeobjdir $(OBJECTS)
		test -d $(EXE_DESTDIR) || mkdir -p $(EXE_DESTDIR)
		$(CC) $(CPPFLAGS) $(ALL_CFLAGS) -o $(EXE_DESTDIR)/$(EXE_NAME0) $(OBJECTS_DIR)/mac_clock_gettime.o  $(OBJECTS_DIR)/main.o  $(OBJECTS_DIR)/packetlibop.o $(LIBS)
		$(CC) $(CPPFLAGS) $(ALL_CFLAGS) -o $(EXE_DESTDIR)/$(EXE_NAME1) $(OBJECTS_DIR)/exe1.o $(LIBS)
		$(CC) $(CPPFLAGS) $(ALL_CFLAGS) -o $(EXE_DESTDIR)/$(EXE_NAME2) $(OBJECTS_DIR)/exe2.o $(LIBS)
		$(CC) $(CPPFLAGS) $(ALL_CFLAGS) -o $(EXE_DESTDIR)/$(EXE_NAME3) $(OBJECTS_DIR)/exe3.o $(LIBS)

staticlib: makelibdir makeobjdir $(OBJECTS)	
		test -d $(LIB_DESTDIR) || mkdir -p $(LIB_DESTDIR)	
		$(DEL_FILE) $(LIB_DESTDIR)/$(TARGETA) 	
		$(AR) $(LIB_DESTDIR)/$(TARGETA) $(OBJECTS_DIR)/*.o
	
dynamiclib: makelibdir makeobjdir $(OBJECTS)	
		$(DEL_FILE) $(TARGET) $(TARGET0) $(TARGET1) $(TARGET2)
		$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS_DIR)/*.o $(LIBS)
		$(SYMLINK) $(TARGET) $(TARGET0)
		$(SYMLINK) $(TARGET) $(TARGET1)
		$(SYMLINK) $(TARGET) $(TARGET2)
		test $(LIB_DESTDIR) = . || $(DEL_FILE) $(LIB_DESTDIR)/$(TARGET)
		test $(LIB_DESTDIR) = . || $(DEL_FILE) $(LIB_DESTDIR)/$(TARGET0)
		test $(LIB_DESTDIR) = . || $(DEL_FILE) $(LIB_DESTDIR)/$(TARGET1)
		test $(LIB_DESTDIR) = . || $(DEL_FILE) $(LIB_DESTDIR)/$(TARGET2)
		test $(LIB_DESTDIR) = . || $(MOVE) $(TARGET) $(TARGET0) $(TARGET1) $(TARGET2) $(LIB_DESTDIR)
	
makeobjdir:
	test -d $(OBJECTS_DIR) || mkdir -p $(OBJECTS_DIR)
	
makelibdir:
	test -d $(LIB_DESTDIR) || mkdir -p $(LIB_DESTDIR)

#clean: delete all files from the current directory that are normally created by building the program. 
clean:
	$(DEL_FILE) $(OBJECTS_DIR)/*.o
	$(DEL_FILE) *~ core *.core
	$(DEL_FILE) $(LIB_DESTDIR)/*.a
	$(DEL_FILE) $(LIB_DESTDIR)/*.so*
	$(DEL_FILE) $(EXE_DESTDIR)/$(EXE_NAME0)
	$(DEL_FILE) $(EXE_DESTDIR)/$(EXE_NAME1)
	$(DEL_FILE) $(EXE_DESTDIR)/$(EXE_NAME2)
	$(DEL_FILE) $(EXE_DESTDIR)/$(EXE_NAME3)
	$(DEL_FILE) version
	$(DEL_FILE) prefix
	$(DEL_FILE) $(PROJECT).dvi
	$(DEL_FILE) $(PROJECT).pdf
	$(DEL_FILE) $(PROJECT).ps
	test $(OBJECTS_DIR) = . || $(DEL_DIR) $(OBJECTS_DIR)
	test $(EXE_DESTDIR) = . || $(DEL_DIR) $(EXE_DESTDIR)
	test $(LIB_DESTDIR) = . || $(DEL_DIR) $(LIB_DESTDIR)
	test $(DOXY_SOURCE_DIR) = . || $(DEL_DIR) $(DOXY_SOURCE_DIR)
	test $(DOC_DIR) = . || $(DEL_DIR) $(DOC_DIR)
	
	
#Delete all files from the current directory that are created by configuring or building the program. 
distclean: clean

#install: compile the program and copy the executables, libraries, 
#and so on to the file names where they should reside for actual use. 
install: all
	$(shell echo $(prefix) > prefix)
	#test -d $(infodir) || mkdir -p $(infodir)	

	# For library installation
	#test -d $(libdir) || mkdir -p $(libdir)
	#test -d $(includedir) || mkdir -p $(includedir)	
	#$(COPY_FILE) $(LIB_DESTDIR)/$(TARGETA) $(libdir)
	#$(COPY_FILE) $(LIB_DESTDIR)/$(TARGET0) $(libdir)
	#$(COPY_FILE) $(LIB_DESTDIR)/$(TARGET1) $(libdir)
	#$(COPY_FILE) $(LIB_DESTDIR)/$(TARGET2) $(libdir)
	#$(COPY_FILE) $(LIB_DESTDIR)/$(TARGETD) $(libdir)
	#$(COPY_FILE) $(INCLUDE) $(includedir)
	
	# For exe installation
	test -d $(bindir) || mkdir -p $(bindir)	
	$(COPY_FILE) $(EXE_DESTDIR)/$(EXE_NAME1) $(bindir)
	
	#copy icon
	#test -d $(icondir) || mkdir -p $(icondir)
	#$(COPY_FILE) $(ICON_DIR)/$(ICON_NAME) $(icondir)

	# For conf files installation
	test -d $(datadir)/$(CONF_DEST_DIR) || mkdir -p $(datadir)/$(CONF_DEST_DIR)
	$(COPY_FILE) $(CONF_DIR)/* $(datadir)/$(CONF_DEST_DIR)


#uninstall: delete all the installed files--the copies that the `install' target creates. 
uninstall:
	#For library uninstall
	$(DEL_FILE) $(libdir)/$(TARGETA)	
	$(DEL_FILE) $(libdir)/$(TARGETD)
	$(DEL_FILE) $(libdir)/$(TARGET0)
	$(DEL_FILE) $(libdir)/$(TARGET1)
	$(DEL_FILE) $(libdir)/$(TARGET2)
	$(DEL_FILE) $(addprefix $(includedir)/, $(notdir $(INCLUDE)))
	
	# For exe uninstall
	$(DEL_FILE) $(bindir)/$(EXE_NAME1)
	#$(DEL_FILE) $(icondir)/$(ICON_NAME)
	
#dist: create a distribution tar file for this program
dist: all

# dvi, pdf, ps, for documentation generation	
dvi: info
	cd $(DOC_DIR)/latex && $(MAKE)
	$(SYMLINK) $(DOC_DIR)/latex/refman.dvi $(PROJECT).dvi
	
pdf: info
	cd $(DOC_DIR)/latex && $(MAKE) pdf
	$(SYMLINK) $(DOC_DIR)/latex/refman.pdf $(PROJECT).pdf

ps: info
	cd $(DOC_DIR)/latex && $(MAKE) ps
	$(SYMLINK) $(DOC_DIR)/latex/refman.ps $(PROJECT).ps
	
#info: generate any Info files needed.
info:	makedoxdir $(DOC_INCLUDE) $(DOC_SOURCE)
	test -d $(DOC_DIR) || mkdir -p $(DOC_DIR)
	doxygen Doxyfile
	
makedoxdir:
	test -d $(DOXY_SOURCE_DIR) || mkdir -p $(DOXY_SOURCE_DIR)
