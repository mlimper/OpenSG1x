
#########################################################################
# Build Rules
#########################################################################

.SUFFIXES:	.src $(OBJ_SUFFIX) .cpp .c .hpp .l .y .tab.cpp .tab.h .s 	\
			$(SO_SUFFIX) _moc.o _moc.cpp _qt.hpp .inl _as.s _ias.s

cnvUnix2Win = "$(shell cygpath -w $(1))"

ifneq ($(IN_TEST_DIR),1)
STRIP_CURRENT_LIB_DEF=$(strip $(CURRENT_LIB_DEF))

ifneq ($(STRIP_CURRENT_LIB_DEF),)
	LIB_CDEF = -D$(STRIP_CURRENT_LIB_DEF)
else
	LIB_CDEF = 
endif
else
LIB_CDEF = 
endif

ifeq ($(OS_BASE), cygwin)
ifeq ($(OS_CMPLR),g++)
BD := $(OSGPOOL)
else
BD := $(shell cygpath -w $(OSGPOOL))
endif
else
BD := $(OSGPOOL)
endif

export BD

ifeq ($(OS_BASE), cygwin)
ifeq ($(OS_CMPLR),bcc)
$(OBJDIR)/%$(OBJ_SUFFIX): %.cpp
	@echo using BD := $$BD
	$(CC) $(CCFLAGS) $(CCLOCALFLAGS) $(COMPONLY_OPTION) $(INCL) \
	$(LIB_CDEF) $(INC_OPTION)"$(OBJDIR)" $(INC_OPTION)"."		\
	$(OBJ_OPTION)$(call cnvUnix2Win,$@) $(call cnvUnix2Win,$<)
$(OBJDIR)/%$(OBJ_SUFFIX): %.c
	@echo using BD := $$BD
	$(CC) $(CCFLAGS) $(CCLOCALFLAGS) $(COMPONLY_OPTION) $(INCL) \
	$(LIB_CDEF) $(INC_OPTION)"$(OBJDIR)" $(INC_OPTION)"."		\
	 $(OBJ_OPTION) $(call cnvUnix2Win,$@) $(call cnvUnix2Win,$<)
else
ifeq ($(OS_CMPLR),g++)
$(OBJDIR)/%$(OBJ_SUFFIX): %.cpp
	@echo using BD := $$BD
	$(CC) $(CCFLAGS) $(CCLOCALFLAGS) $(COMPONLY_OPTION) $(INCL) \
	$(LIB_CDEF) $(INC_OPTION)$(OBJDIR) $(INC_OPTION).			\
	 $(OBJ_OPTION) $@ $< $($(PROJ)SODEF)
$(OBJDIR)/%$(OBJ_SUFFIX): %.c
	@echo using BD := $$BD
	$(CC) $(CCFLAGS) $(CCLOCALFLAGS) $(COMPONLY_OPTION) $(INCL) \
	$(LIB_CDEF) $(INC_OPTION)$(OBJDIR) $(INC_OPTION).			\
	$(OBJ_OPTION) $@ $< $($(PROJ)SODEF)
else
$(OBJDIR)/%$(OBJ_SUFFIX): %.cpp
	@echo using BD := $$BD
	$(CC) $(CCFLAGS) $(CCLOCALFLAGS) $(COMPONLY_OPTION) $(INCL) \
	$(LIB_CDEF) $(INC_OPTION)"$(OBJDIR)" $(INC_OPTION)"."		\
	$(OBJ_OPTION)"$(OBJDIR)\\" $(call cnvUnix2Win,$<)
$(OBJDIR)/%$(OBJ_SUFFIX): %.c
	@echo using BD := $$BD
	$(CC) $(CCFLAGS) $(CCLOCALFLAGS) $(COMPONLY_OPTION) $(INCL) \
	$(LIB_CDEF) $(INC_OPTION)"$(OBJDIR)" $(INC_OPTION)"."		\
	 $(OBJ_OPTION)"$(OBJDIR)\\" $(call cnvUnix2Win,$<)
endif
endif
else
$(OBJDIR)/%$(OBJ_SUFFIX): %.cpp
	@echo using BD = $$BD
	$(CC) $(CCFLAGS) $(CCLOCALFLAGS) $(COMPONLY_OPTION) $(INCL) \
	$(INC_OPTION)$(OBJDIR) $(INC_OPTION).						\
	 $(OBJ_OPTION) $@ $< $($(PROJ)SODEF)
$(OBJDIR)/%$(OBJ_SUFFIX): %.c
	@echo using BD := $$BD
	$(CC) $(CCFLAGS) $(CCLOCALFLAGS) $(COMPONLY_OPTION) $(INCL) \
	$(INC_OPTION)$(OBJDIR) $(INC_OPTION).						\
	$(OBJ_OPTION) $@ $< $($(PROJ)SODEF)
endif

ifeq ($(OS_BASE), cygwin)
ifeq ($(OS_CMPLR),g++)
$(OBJDIR)/%$(OBJ_SUFFIX): $(OBJDIR)/%.cpp
	@echo using BD := $$BD
	$(CC) $(CCFLAGS) $(CCLOCALFLAGS) $(COMPONLY_OPTION) $(INCL) \
	$(LIB_CDEF) $(INC_OPTION)$(OBJDIR) $(INC_OPTION).			\
	$(OBJ_OPTION) $@ $< $($(PROJ)SODEF)
else
$(OBJDIR)/%$(OBJ_SUFFIX): $(OBJDIR)/%.cpp
	@echo using BD := $$BD
	$(CC) $(CCFLAGS) $(CCLOCALFLAGS) $(COMPONLY_OPTION) $(INCL) \
	$(LIB_CDEF) $(INC_OPTION)"$(OBJDIR)" $(INC_OPTION)"."		\
	$(OBJ_OPTION)"$(OBJDIR)\\" $<
endif
else
$(OBJDIR)/%$(OBJ_SUFFIX): $(OBJDIR)/%.cpp
	@echo using BD := $$BD
	$(CC) $(CCFLAGS) $(CCLOCALFLAGS) $(COMPONLY_OPTION) $(INCL) \
	$(INC_OPTION)$(OBJDIR) $(INC_OPTION).						\
	$(OBJ_OPTION) $@ $< $($(PROJ)SODEF)

$(OBJDIR)/%_ias.s: %.cpp
	@echo using BD := $$BD
	$(CC) $(CCFLAGS) $(CCLOCALFLAGS) $(ASONLY_OPTION) $(INCL) \
	$(INC_OPTION)$(OBJDIR) $(INC_OPTION).						\
	$(OBJ_OPTION) $@ $< $($(PROJ)SODEF)

$(OBJDIR)/%_as.s: $(OBJDIR)/%_ias.s
	cat $< > $@
#	cat $< |														\
#	$(SED) -e 's/private_extern _getClassType/globl _getClassType/' \
#		   -e 's/private_extern _getType__CQ/globl _getType__CQ/' 	\
#	> $@ 

$(OBJDIR)/%$(OBJ_SUFFIX): $(OBJDIR)/%.s
	@echo using BD := $$BD
	$(CC) $(CCFLAGS) $(CCLOCALFLAGS) $(COMPONLY_OPTION) $(INCL) \
	$(INC_OPTION)$(OBJDIR) $(INC_OPTION).						\
	$(OBJ_OPTION) $@ $< $($(PROJ)SODEF)
endif

ifeq ($(OS_BASE), cygwin)
$(EXEDIR)/%.exe: $(OBJDIR)/%.obj
	$(LD) $< $(LD_FLAGS) $(LDLOCALFLAGS) \
		     $(LIBPATHS) $(LIBS) /out:$@ 
else
$(EXEDIR)/%: $(OBJDIR)/%.o 
	$(LD) $(LD_OUTOPT)$(LD_OUTSPACE)$@ $(LD_FLAGS) $(LDLOCALFLAGS) \
		$(call cnvSubDirsUnix2Win,$<) $(LIBPATHS) $(LIBS) $(DEBUG_LIBS)
endif


define win_make_depend 	
	@echo "# Building dependency $(@F) from $(<F)" 				
	@-rm -f $@;													
	@echo '# Module dependencies' > $@							
	@$(CC) $(DEPEND_OPTION) $(call cnvUnix2Win,$<) $(CCFLAGS) $(INC_OPTION)"."\
	 $(CCLOCALFLAGS) $(COMPONLYFLAG) $(INCL) $(INC_OPTION)"$(OBJDIR)" |	\
	 $(SED) -e '/:.*\\Microsoft /d' 				\
		 	-e '/:.*\\Intel\\/d' 					\
		 	-e '/:.*\\stdlib/d' 					\
		 	-e 's/\([A-Z]\):/\\\1/' 				\
		 	-e 's/\\/\//g' 							\
		 	-e 's/ /\\ /g' 							\
		 	-e 's/:\\ /: /1' 						\
		 	-e 's///' 								\
			-e '/:.*\.\.\/Base\//d'					\
		 	-e 's/^\([^\.]*\)$(OBJ_SUFFIX):/$(OBJDIR)\/\1$(DEP_SUFFIX) $(OBJDIR)\/\1$(OBJ_SUFFIX):/1'		\
	 		>> $@
endef

define linux_make_depend
	@echo "# Building dependency $(@F) from $(<F)"
	@-rm -f $@
	@echo '# Module dependencies' > $@
	@$(CC) $(DEPEND_OPTION) $< $(CCFLAGS) $(CCLOCALFLAGS) $(INCL) 	\
	 $(INC_OPTION)$(OBJDIR) $(INC_OPTION). 							\
	 | $(SED) -e 's/^\([^:]*:\)/$(OBJDIR)\/\1/1' 					\
	 		  -e 's/\/usr\/include\/[^ ]*//g'						\
	 		  -e 's/\/usr\/Software\/[^ ]*//g'						\
	 		  -e 's/\/igd\/a4\/software\/[^ ]*//g'					\
			  -e 's/.*\.\.\/Base\/[^ ]*//g'							\
			  -e 's/^\([^\.]*\)$(OBJ_SUFFIX):/\1$(DEP_SUFFIX) \1$(OBJ_SUFFIX):/1' \
			>> $@ 
endef

define hpux_make_depend
	@echo "# Building dependency $(@F) from $(<F)"
	@-rm -f $@
	@echo '# Module dependencies' > $@
	@$(CC) $(DEPEND_OPTION) $< $(CCFLAGS) $(CCLOCALFLAGS) $(INCL) 	\
	 $(INC_OPTION)$(OBJDIR) $(INC_OPTION). 							\
	 | $(SED) -e 's/^\([^:]*:\)/$(OBJDIR)\/\1/1' 					\
	 		  -e 's/\/usr\/include\/[^ ]*//g'						\
	 		  -e 's/\/usr\/Software\/[^ ]*//g'						\
	 		  -e 's/\/opt\/[^ ]*//g'								\
			  -e 's/.*\.\.\/Base\/[^ ]*//g'							\
			  -e 's/^\([^\.]*\)$(OBJ_SUFFIX):/\1$(DEP_SUFFIX) \1$(OBJ_SUFFIX):/1' \
			>> $@ 
endef

define irix_make_depend
	@echo "# Building dependency $(@F) from $(<F) "
	@-rm -f $@
	@echo '# Module dependencies' > $@
	@$(CC) $(DEPEND_OPTION) $< $(CCFLAGS) $(CCLOCALFLAGS) $(INCL) 	\
	 $(INC_OPTION)$(OBJDIR) $(INC_OPTION).							\
	 | $(SED)	-e 's/^\([^:]*:\)/$(OBJDIR)\/\1/1' 					\
	 		 	-e '/:[     ]*\/usr\/include\//d'					\
				-e '/:.*\/stdlib\//d' 								\
				-e '/:.*\.\.\/Base\//d'								\
				-e 's/^\([^\.]*\)$(OBJ_SUFFIX):/\1$(DEP_SUFFIX) \1$(OBJ_SUFFIX):/1' \
			>> $@
endef

define darwin_make_depend
	@echo "# Building dependency $(@F) from $(<F)"
	@-rm -f $@
	@echo '# Module dependencies' > $@
	@$(CC) $(DEPEND_OPTION) $< $(CCFLAGS) $(CCLOCALFLAGS) $(INCL) 	\
	 $(INC_OPTION)$(OBJDIR) $(INC_OPTION). 							\
	 | $(SED) -e 's/^\([^:]*:\)/$(OBJDIR)\/\1/1' 					\
	 		  -e 's/\/usr\/include\/[^ ]*//g'						\
			  -e 's/.*\.\.\/Base\/[^ ]*//g'							\
			  -e 's/^\([^\.]*\)$(OBJ_SUFFIX):/\1$(DEP_SUFFIX) \1$(OBJ_SUFFIX):/1' \
			  -e 's/^[ \t]*\\/\\/g' \
	 >> $@ 
endef

ifeq ($(OS_BASE), cygwin)
$(OBJDIR)/%$(DEP_SUFFIX): %.cpp 
ifneq ($(OSGNODEPSREBUILD),1)
	$(win_make_depend)
else
	@echo "# Skipping dependency $(@F) from $(<F) "
endif

$(OBJDIR)/%$(DEP_SUFFIX): $(OBJDIR)/%.cpp 
ifneq ($(OSGNODEPSREBUILD),1)
	$(win_make_depend)
else
	@echo "# Skipping dependency $(@F) from $(<F) "
endif

$(OBJDIR)/%$(DEP_SUFFIX): %.c 
ifneq ($(OSGNODEPSREBUILD),1)
	$(win_make_depend)
else
	@echo "# Skipping dependency $(@F) from $(<F) "
endif
endif

ifeq ($(OS_BASE), irix6.5)
$(OBJDIR)/%$(DEP_SUFFIX): %.cpp
ifneq ($(OSGNODEPSREBUILD),1)
	$(irix_make_depend)
else
	@echo "# Skipping dependency $(@F) from $(<F) "
endif

$(OBJDIR)/%$(DEP_SUFFIX): $(OBJDIR)/%.cpp
ifneq ($(OSGNODEPSREBUILD),1)
	$(irix_make_depend)
else
	@echo "# Skipping dependency $(@F) from $(<F) "
endif

$(OBJDIR)/%$(DEP_SUFFIX): %.c
ifneq ($(OSGNODEPSREBUILD),1)
	$(irix_make_depend)
else
	@echo "# Skipping dependency $(@F) from $(<F) "
endif
endif

ifeq ($(OS_BASE), linux-gnu)
$(OBJDIR)/%$(DEP_SUFFIX): %.cpp
ifneq ($(OSGNODEPSREBUILD),1)
	$(linux_make_depend)
else
	@echo "# Skipping dependency $(@F) from $(<F) "
endif

$(OBJDIR)/%$(DEP_SUFFIX): $(OBJDIR)/%.cpp
ifneq ($(OSGNODEPSREBUILD),1)
	$(linux_make_depend)
else
	@echo "# Skipping dependency $(@F) from $(<F) "
endif

$(OBJDIR)/%$(DEP_SUFFIX): %.c
ifneq ($(OSGNODEPSREBUILD),1)
	$(linux_make_depend)
else
	@echo "# Skipping dependency $(@F) from $(<F) "
endif
endif

ifeq ($(OS_BASE), hpux11.00)
$(OBJDIR)/%$(DEP_SUFFIX): %.cpp
ifneq ($(OSGNODEPSREBUILD),1)
	$(hpux_make_depend)
else
	@echo "# Skipping dependency $(@F) from $(<F) "
endif

$(OBJDIR)/%$(DEP_SUFFIX): $(OBJDIR)/%.cpp
ifneq ($(OSGNODEPSREBUILD),1)
	$(hpux_make_depend)
else
	@echo "# Skipping dependency $(@F) from $(<F) "
endif

$(OBJDIR)/%$(DEP_SUFFIX): %.c
ifneq ($(OSGNODEPSREBUILD),1)
	$(hpux_make_depend)
else
	@echo "# Skipping dependency $(@F) from $(<F) "
endif
endif

ifeq ($(OS_BASE), darwin)
$(OBJDIR)/%$(DEP_SUFFIX): %.cpp
ifneq ($(OSGNODEPSREBUILD),1)
	$(darwin_make_depend)
else
	@echo "# Skipping dependency $(@F) from $(<F) "
endif

$(OBJDIR)/%$(DEP_SUFFIX): $(OBJDIR)/%.cpp
ifneq ($(OSGNODEPSREBUILD),1)
	$(darwin_make_depend)
else
	@echo "# Skipping dependency $(@F) from $(<F) "
endif

$(OBJDIR)/%$(DEP_SUFFIX): %.c
ifneq ($(OSGNODEPSREBUILD),1)
	$(darwin_make_depend)
else
	@echo "# Skipping dependency $(@F) from $(<F) "
endif
endif

ifeq ($(OS_BASE), cygwin)
$(OBJDIR)/%_qt_moc.cpp: %_qt.h
	$(MOC) $(call cnvUnix2Win,$<) -i -o $@

$(OBJDIR)/%_qt_moc.cpp: %_qt.cpp
	$(MOC) $(call cnvUnix2Win,$<) -i -o $@
else
$(OBJDIR)/%_qt_moc.cpp: %_qt.h
	$(MOC) $< -i -o $@

$(OBJDIR)/%_qt_moc.cpp: %_qt.cpp
	$(MOC) $< -i -o $@
endif

#########################################################################
# Automatic Targets Lib Toplevel
#########################################################################

ifeq ($(OS_BASE), cygwin)
SO_INIT_FLAGS =
else
ifeq ($(SO_NEEDS_INIT),1)
SO_INIT_FLAGS = #$(LINKER_INIT_FLAG) -Wl,osgInitSharedObject$(PACKAGE_NAME)
else
SO_INIT_FLAGS =
endif
endif

$(PROJ)SUBPRELINKPAR :=

ifeq ($(OS_BASE), irix6.5)
ifneq ($($(PROJ)SUBPARJOBS),)
ifneq ($($(PROJ)SUBPARJOBS),0)
$(PROJ)SUBPRELINKPAR = -J $($(PROJ)SUBPARJOBS)
endif
endif
endif

ifeq ($(OS_BASE), cygwin)
ifneq ($(SUB_SO_DEF),)
SO_DEF_FLAGS=-Def:$(SUB_SO_DEF)
else
SO_DEF_FLAGS=
endif
else
SO_DEF_FLAG=
endif

ifneq ($(SUB_SO),)

SubLib: $(LIBS_DEP) $(SUB_SO) 
	@echo "LASTDBG=$(DBG)" > .lastdbg

$(SUB_SO): $(LIBS_DEP) $(LIB_QTTARGET_CPP) $(LIB_OBJECTS) $(SUB_SO_DEF) 
	@echo $(LIB_OBJECTS) $(AR_FLAGS) $(SUB_SO)
	$(LD_SHARED) $($(PROJ)SUBPRELINKPAR) $(LD_OUTOPT)$(LD_OUTSPACE)$(SUB_SO) \
		$(LIBPATHS) $(call cnvSubDirsUnix2Win,$(LIB_OBJECTS)) $(LIBS) 		 \
		$(SO_INIT_FLAGS) $(LD_FLAGS) $(SO_DEF_FLAGS)

$(LIB_QT_TARGET)
$(LIB_QTTARGET_DEPS): $(LIB_QTTARGET_CPP)

endif

ifneq ($(SUB_LNK_LIB),)
SubLnkLib: $(SUB_LNK_LIB)
	@echo "Done SubLib (LIB|$(SUB_LNK_LIB)"

$(SUB_LNK_LIB): $(LIB_QTTARGET_CPP) $(LIB_OBJECTS) 
	@-rm -f $(SUB_LNK_LIB)											
	$(LD_LNK) $($(PROJ)SUBPRELINKPAR) 								\
		$(LNK_LD_OUTOPT)$(LD_OUTSPACE)$(SUB_LNK_LIB)				\
		$(call cnvSubDirsUnix2Win,$(LIB_OBJECTS)) 
ifneq ($(RANLIB),)
	$(RANLIB) $(SUB_LNK_LIB)
endif
endif

ifeq ($(OS_BASE),darwin)
$(MACHACK_I_TARGETS)
$(MACHACK_TARGETS)
endif


#########################################################################
# Automatic Targets Test Toplevel
#########################################################################

Test: $(TEST_TARGETS_IN)
	@echo Tests done  

TestLnk: $(TEST_TARGETS_IN)
	@echo Tests done  

$(TEST_TARGETS_IN):  $(LIB_TESTQTTARGET_CPP) $(TEST_TARGETS) 
	@for file in $@; do                \
		echo $$file;                   \
		rm -f $$file;                  \
		ln -s $(EXEDIR)/$$file $$file; \
	done

$(TEST_TARGETS): $(TEST_OBJS) $(LIBS_DEP) $(TEST_DEPS)  

$(LIB_TESTQT_TARGET)

ifeq ($(IN_TEST_DIR),1)
$(LIB_TESTQTTARGET_DEPS): $(LIB_TESTQTTARGET_CPP)

list: 
	@echo 
	@echo "	available test cases :"
	@echo "	======================\n"
	@(for file in $(TEST_TARGETS_LIST); do	\
		echo "		$(MAKE) $$file";         	\
	done) | sort
	@echo 
	@echo 
endif

#########################################################################
# Automatic Targets Flex
#########################################################################

ifneq ($(LIB_FLEXTARGET_CPP),)
$(OBJDIR)/%.lex.cpp: %.l
	$(FLEX) -l -P$(call flex_int,$<) $<
	@cat lex.$(call flex_int,$<).c | 								\
		sed -e 's/\(yy\)\(text_ptr\)/$(call flex_int,$<)\2/g'		\
		> $(OBJDIR)/$(call flex_ext,$<).lex.cpp
	@-rm lex.$(call flex_int,$<).c

$(LIB_FLEXTARGET_CPP) : $(LIB_FLEXSOURCES)

ifneq ($(OSGNODEPS),1) 
$(LIB_FLEXTARGET_DEPS): $(LIB_FLEXTARGET_CPP)
endif

endif

#########################################################################
# Automatic Targets Bison
#########################################################################

ifneq ($(LIB_BISONTARGET_CPP),)

$(OBJDIR)/%.tab.cpp: %.y
	$(BISON) -d -v -p$(call bison_int,$<) -b$(call bison_int,$<) $<
	mv $(call bison_int,$<).tab.c  $(OBJDIR)/$(call bison_ext,$<).tab.cpp
	mv $(call bison_int,$<).tab.h            $(call bison_ext,$<).tab.h
	mv $(call bison_int,$<).output $(OBJDIR)/$(call bison_ext,$<).output

$(LIB_BISONTARGET_CPP) : $(LIB_BISONSOURCES)

ifneq ($(OSGNODEPS),1) 
$(LIB_BISONTARGET_DEPS): $(LIB_BISONTARGET_CPP)
endif

endif


#########################################################################
# Normal lib Targets
#########################################################################

ifneq ($(IN_TEST_DIR),1)
dbg: DBG := dbg
dbg: SubLib
	@echo "LASTDBG=dbg" > .lastdbg

dbgLnk: DBG := dbglnk
dbgLnk: SubLnkLib
	@echo "Done dbg (SubLibLnk)"

opt: DBG := opt
opt: SubLib
	@echo "LASTDBG=dbg" > .lastdbg

optLnk: DBG := optlnk
optLnk: SubLnkLib
	@echo "Done opt (SubLibLnk)"

else
dbg: Test
	@echo "LASTDBG=dbg" > .lastdbg
opt: Test
	@echo "LASTDBG=dbg" > .lastdbg

dbgLnk: TestLnk
	@echo "LASTDBG=dbg" > .lastdbg

optLnk: TestLnk
	@echo "LASTDBG=dbg" > .lastdbg

endif

#########################################################################
# lib.def
#########################################################################

ifneq ($(LIB_DEF_SRC),)

ifneq ($(LIB_HEADER_SRC),)

lib.$(DBG).def: $(LIB_DEF_SRC) $(LIB_HEADER_SRC)
	cp  $(LIB_HEADER_SRC) ./lib.$(DBG).def
	cat $(LIB_DEF_SRC) >> ./lib.$(DBG).def
else
lib.$(DBG).def:
endif

else

lib.$(DBG).def:

endif

#########################################################################
# depend
#########################################################################

ifeq ($(IN_TEST_DIR),1)

depend_i: $(LIB_TESTQTTARGET_DEPS) $(TEST_DEPS)

else

depend_i: $(LIB_BISONTARGET_DEPS) $(LIB_FLEXTARGET_DEPS) $(LIB_QTTARGET_DEPS) $(LIB_DEPS)

endif

depend: DepClean depend_i

#########################################################################
# clean
#########################################################################

commonclean:
	-rm -f core                       2>/dev/null
	-rm -f *.pure                     2>/dev/null
	-rm -f *.idb                      2>/dev/null
	-rm -f *.pdb                      2>/dev/null
	-rm -f *.core                     2>/dev/null
	-rm -f *.stackdump                2>/dev/null
	-rm -f *.h                        2>/dev/null
	-rm -f .lastdbg                   2>/dev/null
	-rm -f $(OBJDIR)/*$(OBJ_SUFFIX)   2>/dev/null
	-rm -f $(OBJDIR)/*.s			  2>/dev/null
	-rm -f $(OBJDIR)/*.I			  2>/dev/null
	-rm -f $(OBJDIR)/*_moc.cpp        2>/dev/null
	-rm -f $(OBJDIR)/*.lex.cpp        2>/dev/null
	-rm -f $(OBJDIR)/*.tab.cpp        2>/dev/null
	-rm -f $(OBJDIR)/*.tab.h          2>/dev/null
	-rm -f $(OBJDIR)/*.output         2>/dev/null
ifneq ($(II_FILESDIR),)
	-rm -rf  $(OBJDIR)/$(II_FILESDIR)  2>/dev/null
endif

dbgclean: DBG := dbg
dbgclean: commonclean

optclean: DBG := opt
optclean: commonclean

dbgcleanLnk: DBG := dbglnk
dbgcleanLnk: commonclean

optcleanLnk: DBG := optlnk
optcleanLnk: commonclean

clean:    commonclean
cleanLnk: commonclean

#########################################################################
# Clean
#########################################################################

.PHONY: commonClean

commonClean : commonclean DepClean
	-rm -f $(LIBDIR)/*$(SO_SUFFIX)  					2>/dev/null
	-rm -f $(LIBDIR)/*$(LIB_SUFFIX) 					2>/dev/null
	-rm -f $(LIBDIR)/*.exp          					2>/dev/null
	-rm -f $(LIBDIR)/*.ilk          					2>/dev/null
	-rm -f $(LIBDIR)/*.map          					2>/dev/null
	-rm -f $(LIBDIR)/*.pdb          					2>/dev/null
	-rm -f so_locations             					2>/dev/null
	-rm -f $(EXEDIR)/*             						2>/dev/null
	-find . -type l -name 'test*' -print -exec rm {} \; 2>/dev/null

dbgClean: DBG := dbg
dbgClean: OBJDIR := $(OBJDIR_BASE)-$(DBG)
dbgClean: LIBDIR := $(LIBDIR_BASE)-$(DBG)
dbgClean: commonClean
	@echo "Done dbgClean"

optClean: DBG := opt
optClean: OBJDIR := $(OBJDIR_BASE)-$(DBG)
optClean: LIBDIR := $(LIBDIR_BASE)-$(DBG)
optClean: commonClean
	@echo "Done optClean"

Clean:    commonClean
CleanLnk: commonClean

distclean: 
	@$(SUB_MAKE) dbgClean 
	@$(SUB_MAKE) optClean

#########################################################################
# DepClean
#########################################################################

commonDepClean:
	-rm -f $(OBJDIR)/*.d 2>/dev/null

dbgDepClean: DBG := dbg
dbgDepClean: commonDepClean

optDepClean: DBG := opt
optDepClean: commonDepClean

DepClean: commonDepClean

#########################################################################
# LibClean
#########################################################################

LibClean:
ifeq ($(OS_BASE), cygwin)
	@-rm -f $(LIBDIR)/*.exp 			2>/dev/null
	@-rm -f $(LIBDIR)/*.pdb 			2>/dev/null
	@-rm -f $(LIBDIR)/*$(LIB_SUFFIX) 	2>/dev/null
endif
	@-rm -f $(LIBDIR)/*$(SO_SUFFIX)	 	2>/dev/null

ExeClean:
	@-rm -f $(EXEDIR)/*                 2>/dev/null

#########################################################################
# DSP
#########################################################################

ifeq ($(MAKECMDGOALS),dsp)

.PHONY: dsp

DSP_SUBPACKS := $(LIB_SOURCEPACKAGES)
DSP_SUBDIRS  := $(LIB_ABSSOURCEDIRS)

dsp_src = $(sort $(call getProjFiles,$(1)))
dsp_def = $(wildcard lib.$(DBG).def)
dsp_out = $(shell echo $(PACKAGE_NAME) $(1) $(OSGPOOL) $(call dsp_src,$(2)) > tmp_$(1) ) tmp_$(1)

dsp_getPack = $(strip $(subst :, ,$(subst /,,$(subst $($(PROJ)POOL)/,,\
	$(subst .:,,$(1))))))

dsp_procPack = $(call dsp_out,$(call dsp_getPack,$(1)),$(1))

DSP_PACKS  := $(sort $(foreach dir,$(DSP_SUBDIRS),$(call dsp_procPack,$(dir))))

INCL_EXP_$(OS_BASE) := -I.. -I. $(INCL_EXP_$(OS_BASE))

DSP_DEFS   := $(call dsp_def)


dsp:
	@echo "Building $(PACKAGE_NAME)Lib dsp file for $(OS_CMPLR)"
	@cat $(OSGPOOL)/$(OSGCOMMONCONF)/OSGLib.$(OS_CMPLR).dsp.in |		\
	$(SED)  -e 's|@OSGPACK@|$(PACKAGE_NAME)|g' 							\
			-e 's|@OSG_CCFLAGS@|$(CCFLAGS_EXT)|g'						\
			-e 's|@OSG_CCFLAGS_DBG@|$(CCFLAGS_EXT_DBG)|g'				\
			-e 's|@OSG_CURR_LIB_DEF@|-D$(CURRENT_LIB_DEF)|g'			\
			-e 's|@OSG_CONF_PACKS@|$(CONFIGURED_PACKAGE_DEFS)|g'		\
			-e 's|@OSG_QT_DEFS@|$(QT_PLATTFORMDEF_EXT)|g'				\
			-e 's|@OSG_INCL@|$(INCL_EXP_$(OS_BASE))|g'					\
			-e 's|@OSG_LIB_EXT@|$(OSG_LIB_EXT)|g'						\
		> $(PACKAGE_NAME)Lib.dsp
	@$(OSGPOOL)/$(OSGCOMMON)/createDSPSourcePart.pl $(DSP_PACKS) $(DSP_DEFS) >> $(PACKAGE_NAME)Lib.dsp
	@rm -f $(DSP_PACKS)
	@mv $(PACKAGE_NAME)Lib.dsp $(OSGPOOL)/VSBuild/VS6/$(PACKAGE_NAME)Lib
ifneq ($(DSP_DEFS),)
	@cp lib.$(DBG).def $(OSGPOOL)/VSBuild/VS6/$(PACKAGE_NAME)Lib
endif
endif

ifeq ($(IN_TEST_DIR),0)
ifeq ($(SUB_JOB), build)
-include $(LIB_DEPS) $(LIB_QTTARGET_DEPS) $(LIB_FLEXTARGET_DEPS) $(LIB_BISONTARGET_DEPS)
endif
else
ifeq ($(SUB_JOB), build)
-include $(TEST_DEPS) $(LIB_TESTQTTARGET_DEPS)
endif
endif
