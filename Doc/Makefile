#!make

include ../Common/common.mk

#########################################################################
# Documenation Settings
#########################################################################

DOCBASEDIR ?= .
DOCCODEDIR := Code
DOCDIR     := $(DOCBASEDIR)/$(DOCCODEDIR)/$(DOC_LEVEL_NAME)

DOC_PROJECT_NAME = OpenSG
DOC_PROJECT_NUMBER = $(shell cat ../VERSION)

DOCS_BASE  := ../Source/Base/Base 				\
			  ../Source/Base/Field 				\
			  ../Source/Base/Functors			\
			  ../Source/Base/Network/Base 		\
			  ../Source/Base/Network/Socket		\
			  ../Source/Base/StringConversion

DOCS_SYS   := ../Source/System/Action  										\
              ../Source/System/Action/DrawAction                            \
              ../Source/System/Action/IntersectAction                       \
              ../Source/System/Action/RenderAction                          \
              ../Source/System/Cluster/Base                                 \
              ../Source/System/Cluster/Server                               \
              ../Source/System/Cluster/Window                               \
              ../Source/System/Cluster/Window/Base                          \
              ../Source/System/Cluster/Window/MultiDisplay                  \
              ../Source/System/Cluster/Window/SortFirst                     \
              ../Source/System/FieldContainer                               \
              ../Source/System/FieldContainer/Impl                          \
              ../Source/System/FileIO                                       \
              ../Source/System/FileIO/Base                                  \
              ../Source/System/FileIO/BIN                                   \
              ../Source/System/FileIO/OBJ                                   \
              ../Source/System/FileIO/OFF                                   \
              ../Source/System/FileIO/OSG                                   \
              ../Source/System/FileIO/RAW                                   \
              ../Source/System/FileIO/WRL                                   \
              ../Source/System/FileIO/ScanParseSkel                         \
              ../Source/System/GraphOp                                      \
              ../Source/System/Image                                        \
              ../Source/System/Material                                     \
              ../Source/System/NodeCores/Drawables/Base                     \
              ../Source/System/NodeCores/Drawables/Geometry                 \
              ../Source/System/NodeCores/Drawables/Misc                     \
              ../Source/System/NodeCores/Drawables/Particles                \
              ../Source/System/NodeCores/Drawables/VolRen                   \
              ../Source/System/NodeCores/Drawables/Nurbs                    \
              ../Source/System/NodeCores/Groups/Base                        \
              ../Source/System/NodeCores/Groups/Light                       \
              ../Source/System/NodeCores/Groups/Misc                        \
              ../Source/System/RenderingBackend                             \
              ../Source/System/State                                        \
              ../Source/System/Statistics		    	            \
              ../Source/System/Text                                         \
              ../Source/System/Window

DOCS_EXP   := ../Source/Experimental/NewAction \
              ../Source/Experimental/SHL 
              

DOCS_X     := ../Source/WindowSystem/X
DOCS_W32   := ../Source/WindowSystem/WIN32
DOCS_GLUT  := ../Source/WindowSystem/GLUT
DOCS_QT    := ../Source/WindowSystem/QT

DOC_LIBS   ?= $(DOCS_BASE) $(DOCS_SYS) $(DOCS_EXP)							\
			  $(DOCS_X) $(DOCS_W32) $(DOCS_GLUT) $(DOCS_QT) $(DOCS_PAGES)

# defines for starter guide generation
DOC_LATEX ?= "NO"
DOC_DOT   ?= "NO"
# DOC_DOT   ?= "YES"
DOC_INTRO ?= "tutorial/main.dox"

# The separate documentation pages

DOC_TUTORIAL_PAGES  := 	        tutorial/main.dox \
				tutorial/introduction.dox \
				tutorial/firstapp.dox \
				tutorial/basics.dox \
				tutorial/nodecores.dox \
				tutorial/geometry.dox \
				tutorial/light.dox \
				tutorial/windows.dox \
				tutorial/traversal.dox \
				tutorial/multithreading.dox \
				tutorial/clustering.dox \
				tutorial/modelling.dox \
				tutorial/text.dox \
				tutorial/solutions.dox \
				tutorial/qt.dox \
				tutorial/doxypages.dox \
				Code/acknowledgements.dox \
				Code/faq.dox

DOC_OLD_PAGES  := 	        Code/mainpage.dox \
				$(DOC_INTRO) \
				Code/scenegraph.dox \
				../Source/Base/Base/Base.dox \
				../Source/System/System.dox \
				../Source/Base/Field/Field.dox \
				../Source/Base/Functors/Functors.dox \
				../Source/Base/Network/Socket.dox \
				../Source/Base/Network/Connection.dox \
				../Source/Base/StringConversion/StringConversion.dox \
				../Source/System/FieldContainer/FieldContainer.dox \
				../Source/System/Image/Image.dox \
				../Source/System/NodeCores/NodeCores.dox \
				../Source/System/NodeCores/Groups/Groups.dox \
				../Source/System/NodeCores/Groups/Base/GroupsBase.dox \
				../Source/System/NodeCores/Groups/Light/GroupsLight.dox \
				../Source/System/NodeCores/Groups/Misc/GroupsMisc.dox \
				../Source/System/NodeCores/Drawables/Drawables.dox \
				../Source/System/NodeCores/Drawables/Base/DrawablesBase.dox \
				../Source/System/NodeCores/Drawables/Geometry/DrawablesGeometry.dox \
				../Source/System/NodeCores/Drawables/Misc/DrawablesMisc.dox \
				../Source/System/NodeCores/Drawables/Particles/DrawablesParticles.dox \
				../Source/System/State/State.dox \
				../Source/System/Material/Material.dox \
				../Source/System/Action/Action.dox \
				../Source/System/Window/Window.dox \
				../Source/System/Window/OGLExt.dox \
				../Source/WindowSystem/WindowSystem.dox \
				../Source/WindowSystem/GLUT/WindowGlut.dox \
				../Source/WindowSystem/QT/WindowQt.dox \
				../Source/WindowSystem/X/WindowX.dox \
				../Source/WindowSystem/WIN32/WindowWin32.dox \
				../Source/System/FileIO/FileIO.dox \
				../Source/System/System.dox  \
				../Source/System/Cluster/Cluster.dox  \
				../Source/System/RenderingBackend/RenderingBackend.dox \
				../Source/System/Statistics/Statistics.dox

DOC_PAGES :=                    ../Common/dummyClasses.dox\
                                $(DOC_TUTORIAL_PAGES)\
                                $(DOC_OLD_PAGES)

DOC_IMAGE_PATHES := Images

# All the files that are merged to generate the whole documentation
DOC_FILES := $(DOC_LIBS) $(DOC_PAGES)

# Files for testing. Generate test doc via "make TESTDOC=1 <target>"
TESTDOC_FILES := $(DOC_PAGES)


# Trivial, but useful...
ifneq ($TESTDOC),)
ifeq ($TESTDOC),1)
DOC_FILES := $(TESTDOC_FILES)
endif
endif

# level-dependent defines
DOC_LEVEL ?= 1
DOC_LEVEL_PREDEF ?= ""
DOC_LEVEL_ENABLED ?= ""


DOC_ENV := DOC_PROJECT_NAME="$(DOC_PROJECT_NAME)" DOCDIR=$(DOCDIR)
DOC_ENV += DOC_PROJECT_NUMBER=$(DOC_PROJECT_NUMBER) DOC_FILES="$(DOC_FILES)"
DOC_ENV += OSGPOOL="$(OSGPOOL)"  DOC_LATEX="$(DOC_LATEX)" DOC_DOT="$(DOC_DOT)"
DOC_ENV += OSG_USE_DOC_LEVEL="$(DOC_LEVEL)" DOC_LEVEL_PREDEF="$(DOC_LEVEL_PREDEF)"
DOC_ENV += DOC_LEVEL_ENABLED="$(DOC_LEVEL_ENABLED)"
DOC_ENV += DOC_IMAGE_PATHES="$(DOC_IMAGE_PATHES)"

DOXYGEN := doxygen
#DOXYGEN := /home/gerrit/SoftwareInstall/doxygen_cvs/bin/doxygen
#DOXYGEN := /net/pc1107/data/dirk/software/doxygen-1.2.18/bin/doxygen
#DOXYGEN := /net/pc1107/data/dirk/software/doxygen-1.3-rc2/bin/doxygen
#DOXYGEN := /data/dirk/software/doxygen_cvs_send/bin/doxygen
#DOXYGEN := /home/gerrit/SoftwareInstall/doxygen_cvs/bin/doxygen
#DOXYGEN := /home/gerrit/SoftwareInstall/doxygen_cvs_org/bin/doxygen
#DOXYGEN := /home/gerrit/SoftwareTest/doxygen-1.3-rc1/bin/doxygen
#DOXYGEN := /Users/gandalf/SoftwareBuilds/doxygen_cvs/bin/doxygen

default: help

doc: userdoc

qdoc:
	$(MAKE) userdoc | egrep -v '^Preprocessing' | egrep -v '^Parsing' | \
			egrep -v '^Generating'

alldocs: userstarter devdoc

# Doc targets

.PHONY: dodoc dostarter
dodoc:
#   find all headers and create dummy classes
	@rm -f ../Common/dummyClasses.list
	@touch ../Common/dummyClasses.list

	@for i in $(DOC_LIBS) ; do \
		find $$i -name '*.h' -print  >> ../Common/dummyClasses.list; \
	done

	@cat ../Common/dummyClasses.list | xargs perl ../Common/makeDummyClasses  \
				> ../Common/dummyClasses.dox

#   @rm -f ../Common/dummyClasses.list

#   find all images and convert them (if necessary)
	$(MAKE) DOC_IMAGE_TYPE=html images2

#   do doxygen
	$(DOC_ENV) $(DOXYGEN) ../Common/Doxygen_$(BUILD_ENV).cfg -d

#	@rm -f ../Common/dummyClasses.dox

userdoc:
	$(MAKE) DOC_LEVEL=1 DOC_LEVEL_NAME=user DOC_LEVEL_NAME=user \
	    DOC_LEVEL_PREDEF= \
		DOC_LEVEL_ENABLED= dodoc

extdoc:
	$(MAKE) DOC_LEVEL=2 DOC_LEVEL_NAME=ext DOC_LEVEL_NAME=ext \
	    DOC_LEVEL_PREDEF="OSG_DOC_EXT" \
		DOC_LEVEL_ENABLED="ext" dodoc

devdoc:
	$(MAKE) DOC_LEVEL=3 DOC_LEVEL_NAME=dev DOC_LEVEL_NAME=dev \
	    DOC_LEVEL_PREDEF="OSG_DOC_EXT OSG_DOC_DEV" \
	    DOC_LEVEL_ENABLED="ext dev"  dodoc

devtrdoc:
	$(MAKE) DOC_LEVEL=4 DOC_LEVEL_NAME=dev_traits DOC_LEVEL_NAME=dev \
	    DOC_LEVEL_PREDEF="OSG_DOC_EXT OSG_DOC_DEV OSG_DOC_DEV_TRAITS" \
	    DOC_LEVEL_ENABLED="ext dev dev_traits"  dodoc

# Starter targets

dostarter:
	$(MAKE) DOC_LATEX="YES" DOC_DOT="NO" DOC_INTRO="starter.dox" $(DOC_STARTER)doc
	# link the images
	cd $(DOCDIR)/$(DOC_STARTER)/latex/ ; \
        for i in *.eps; do echo $$i ; epstopdf $$i; done; \
	../../../../Common/refmanToStarter refman.tex StarterGuide.tex ;  \
	latex StarterGuide && latex StarterGuide

userstarter:
	$(MAKE) DOC_IMAGE_TYPE=latex images2
	$(MAKE) DOC_STARTER="user" DOC_FILES="$(DOC_TUTORIAL_PAGES)" dostarter

extstarter:
	$(MAKE) DOC_IMAGE_TYPE=latex images2
	$(MAKE) DOC_STARTER="ext" DOC_FILES="$(DOC_TUTORIAL_PAGES)" dostarter

devstarter:
	$(MAKE) DOC_IMAGE_TYPE=latex images2
	$(MAKE) DOC_STARTER="dev" DOC_FILES="$(DOC_TUTORIAL_PAGES)" dostarter


# Image conversion rules

# amz I renamed this rule to images2. On windows filenames a case insensitive
# so it is not possible to distinguish between Images (the directory) and images the rule.
images2:
#   find all used images and convert them (if necessary)
	rm -f imagelist
	-for i in $(DOC_PAGES) ; do \
		fgrep "\image $(DOC_IMAGE_TYPE)" $$i  | cut -d' ' -f3-3 | \
		sed -e 's|^|Images/|' >> imagelist; 	\
	done

	$(MAKE) `cat imagelist`


Images/%.eps: Images/%.gif
	convert -compress none $< PS:$@

Images/%.eps: Images/%.tif
	convert -compress none $< PS:$@

Images/%.eps: Images/%.jpg
	convert -compress none $< PS:$@
#  jpeg2ps gives better results, but is not usually available
#  jpeg2ps -q $< > $@

Images/%.eps: Images/%.fig
	(cd Images && fig2dev -L eps $(notdir $<) $(notdir $@) )

Images/%.eps: Images/%.gpl
	gnuplot $<

Images/%.eps: Images/%.ps
	cp $< $@
	touch $@

Images/%.eps: Images/%.eps.gz
	gzip -dc $< > $@
	touch $@

Images/%.eps: Images/%.png
	convert -compress none $< PS:$@

Images/%.png: Images/%.fig
	(cd Images && fig2dev -L png $(notdir $<) $(notdir $@) )


Images/%.png: Images/%.jpg
	convert  $< $@

Images/%.png: Images/%.tif
	convert  $< $@

Images/%.png: Images/%.eps.gz
	gzip -dc $< | convert - $@



help:
	@echo "Targets supported on this level:"
	@echo "================================"
	@echo
	@echo "userdoc      create the HMTL version of the user-level docs"
	@echo "userstarter  create the PS/PDF version of the user-level docs"
	@echo "devdoc       create the HMTL version of the developer-level docs"
	@echo "devstarter   create the PS/PDF version of the developer-level docs"
	@echo "alldocs      all of the above"

