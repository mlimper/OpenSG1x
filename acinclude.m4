AC_DEFUN(AC_GDZ_PROG_CXX_WORKS,
[AC_BEFORE([$0], [AC_PROG_CXXCPP])dnl

if test $enable_compiler_path = yes; then
    if test $2"set" = set; then
        AC_PATH_PROGS(CXX, $1, unknown_compiler, $PATH)
    else
        AC_PATH_PROGS(CXX, $1, unknown_compiler, $PATH:$2)
    fi
else
    if test $2"set" = set; then
        AC_CHECK_PROGS(CXX, $1, unknown_compiler, $PATH)
    else
        AC_CHECK_PROGS(CXX, $1, unknown_compiler, $PATH:$2)
    fi
fi

AC_PROG_CXX_WORKS
AC_PROG_CXX_GNU

dnl Check whether -g works, even if CXXFLAGS is set, in case the package
dnl plays around with CXXFLAGS (such as to build both debugging and
dnl normal versions of a library), tasteless as that idea is.
ac_test_CXXFLAGS="${CXXFLAGS+set}"
ac_save_CXXFLAGS="$CXXFLAGS"
CXXFLAGS=
AC_PROG_CXX_G
if test "$ac_test_CXXFLAGS" = set; then
  CXXFLAGS="$ac_save_CXXFLAGS"
elif test $ac_cv_prog_cxx_g = yes; then
  if test "$GXX" = yes; then
    CXXFLAGS="-g -O2"
  else
    CXXFLAGS="-g"
  fi
else
  if test "$GXX" = yes; then
    CXXFLAGS="-O2"
  else
    CXXFLAGS=
  fi
fi
])
AC_DEFUN(AC_GDZ_PROG_CXX_AVAILABLE,
[AC_BEFORE([$0], [AC_PROG_CXXCPP])dnl

if test "$2set" = set; then
    AC_PATH_PROGS(CXX, $1, unknown_compiler, $PATH)
else
    AC_PATH_PROGS(CXX, $1, unknown_compiler, $PATH:$2)
fi

dnl AC_PROG_CXX_GNU

dnl Check whether -g works, even if CXXFLAGS is set, in case the package
dnl plays around with CXXFLAGS (such as to build both debugging and
dnl normal versions of a library), tasteless as that idea is.
dnl ac_test_CXXFLAGS="${CXXFLAGS+set}"
dnl ac_save_CXXFLAGS="$CXXFLAGS"
dnl CXXFLAGS=
dnl AC_PROG_CXX_G
dnl if test "$ac_test_CXXFLAGS" = set; then
dnl   CXXFLAGS="$ac_save_CXXFLAGS"
dnl elif test $ac_cv_prog_cxx_g = yes; then
dnl   if test "$GXX" = yes; then
dnl    CXXFLAGS="-g -O2"
dnl  else
dnl    CXXFLAGS="-g"
dnl  fi
dnl else 
dnl if test "$GXX" = yes; then
dnl    CXXFLAGS="-O2"
dnl  else
dnl    CXXFLAGS=
dnl  fi
dnl fi
])

AC_DEFUN(AC_GDZ_PROG_CC_WORKS,
[AC_MSG_CHECKING([whether the C compiler ($CC $CFLAGS $LDFLAGS) works])
AC_LANG_SAVE
AC_LANG_C

if test "set"$2 = set; then
    AC_PATH_PROGS(CC, $1, unknown_compiler, $PATH)
else
    AC_PATH_PROGS(CC, $1, unknown_compiler, $PATH:$2)
fi

AC_TRY_COMPILER([main(){return(0);}], ac_cv_prog_cc_works, ac_cv_prog_cc_cross)
AC_LANG_RESTORE
AC_MSG_RESULT($ac_cv_prog_cc_works)
if test $ac_cv_prog_cc_works = no; then
  AC_MSG_ERROR([installation or configuration problem: C compiler cannot create executables.])
fi
AC_MSG_CHECKING([whether the C compiler ($CC $CFLAGS $LDFLAGS) is a cross-compiler])
AC_MSG_RESULT($ac_cv_prog_cc_cross)
cross_compiling=$ac_cv_prog_cc_cross
])

AC_DEFUN(AC_GDZ_PROG_CC_AVAILABLE,
[AC_MSG_CHECKING([whether the C compiler ($CC $CFLAGS $LDFLAGS) is available])

AC_LANG_SAVE
AC_LANG_C

if test "xset$2" = xset; then
    AC_PATH_PROGS(CC, $1, unknown_compiler, $PATH)
else
    AC_PATH_PROGS(CC, $1, unknown_compiler, $PATH:$2)
fi


AC_LANG_RESTORE

AC_MSG_RESULT($ac_cv_prog_cc_works)

cross_compiling=$ac_cv_prog_cc_cross
])

AC_DEFUN(AC_GDZ_FIND_STUDIO_DIR,
[
    ac_gdz_find_prog_dir_result=""

    for drive in c d e f g; do
     for progdir in "Program Files" "Programme"; do
      if test -d "/cygdrive/$drive/$progdir/Microsoft Visual Studio"; then
       ac_gdz_find_prog_dir_result="/cygdrive/$drive/$progdir/Microsoft Visual Studio"
       break 2
      fi
     done
    done
])

AC_DEFUN(AC_GDZ_FIND_STUDIONET_DIR,
[
    ac_gdz_find_prog_dir_result=""

    for drive in c d e f g; do
     for progdir in "Program Files" "Programme"; do
      for vsnet in "Microsoft Visual Studio.NET" "Microsoft Visual Studio .NET"; do
       if test -d "/cygdrive/$drive/$progdir/$vsnet/"; then
        ac_gdz_find_prog_dir_result="/cygdrive/$drive/$progdir/$vsnet"
        break 2
       fi
      done
     done
    done
])

AC_DEFUN(AC_GDZ_FIND_PROG_DIR,
[
    ac_gdz_find_prog_dir_result=""

    test_path=$1

    for drive in c d e f g; do
        for progdir in "Program Files/" "Programme/" ""; do
            if test -d "/cygdrive/$drive/${progdir}${test_path}"; then
                ac_gdz_find_prog_dir_result="/cygdrive/$drive/${progdir}${test_path}"
                break 2
            fi
        done
    done
])

AC_DEFUN(AC_GDZ_SETUP_INTEL,
[
    changequote(<<, >>)dnl

    ac_gdz_compiler_version=`echo $ac_gdz_compiler | sed 's/^\(icl\)\([0-9]*\)$/\2/'`
    ac_gdz_compiler_base=icl

    changequote([, ])dnl

    if test $ac_gdz_compiler_version"set" = set; then
        ac_gdz_compiler_version=50
    fi

    case $ac_gdz_compiler_version in

        60)
        ac_gdz_intelsearchdir=Intel/compiler$ac_gdz_compiler_version/ia32
        ;;
        50)
        ac_gdz_intelsearchdir=Intel/compiler$ac_gdz_compiler_version/ia32
        ;;
        45)
        ac_gdz_intelsearchdir=Intel/compiler$ac_gdz_compiler_version
        ;;
         *)
        ac_gdz_intelsearchdir=Intel/compiler$ac_gdz_compiler_version
        ;;
    esac    

    AC_GDZ_FIND_PROG_DIR($ac_gdz_intelsearchdir)

    ac_gdz_compiler_dir=$ac_gdz_find_prog_dir_result
    ac_gdz_compiler_path=$ac_gdz_compiler_dir/bin
    ac_gdz_compiler_incl=$ac_gdz_compiler_dir/Include
    ac_gdz_compiler_lib=$ac_gdz_compiler_dir/Lib
    ac_gdz_compiler_exe=icl.exe
    ac_gdz_linker_exe=xilink.exe
    ac_gdz_check_compiler_available=yes

    if ! test -d "$ac_gdz_compiler_dir"; then
        echo
        echo "ERROR could not determine compiler dir, tried :"

        for drive in c d e f g; do
            for progdir in "Program Files" "Programme"; do
                echo "    /cygdrive/$drive/$progdir/$ac_gdz_intelsearchdir"
            done
        done

        exit
    fi
])

AC_DEFUN(AC_GDZ_SETUP_MSVC,
[
    AC_GDZ_FIND_STUDIO_DIR()

    ac_gdz_compiler_dir=$ac_gdz_find_prog_dir_result/VC98
    ac_gdz_compiler_path=$ac_gdz_compiler_dir/bin
    ac_gdz_compiler_incl=$ac_gdz_compiler_dir/Include
    ac_gdz_compiler_lib=$ac_gdz_compiler_dir/Lib
    ac_gdz_compiler_exe=cl.exe
    ac_gdz_linker_exe=link.exe
    ac_gdz_check_compiler_available=yes
])

AC_DEFUN(AC_GDZ_SETUP_MSVCNET,
[
    AC_GDZ_FIND_STUDIONET_DIR()

    ac_gdz_compiler_dir=$ac_gdz_find_prog_dir_result/Vc7
    ac_gdz_compiler_path=$ac_gdz_compiler_dir/bin
    ac_gdz_compiler_incl=$ac_gdz_compiler_dir/include
    ac_gdz_compiler_lib=$ac_gdz_compiler_dir/lib
    ac_gdz_compiler_exe=cl.exe
    ac_gdz_linker_exe=link.exe
    ac_gdz_check_compiler_available=yes

    if ! test -d "$ac_gdz_compiler_path"; then
        echo
        echo "ERROR could not determine compiler dir, tried :"

        for drive in c d e f g; do
            for progdir in "Program Files" "Programme"; do
                for vsnet in "Microsoft Visual Studio.NET" "Microsoft Visual Studio .NET"; do
                    echo "    /cygdrive/$drive/$progdir/$vsnet"
                done
            done
        done

        exit
    fi
])

AC_DEFUN(AC_GDZ_SETUP_BORLAND,
[
    changequote(<<, >>)dnl

    ac_gdz_compiler_version=`echo $ac_gdz_compiler | sed 's/^\(bcc\)\([0-9]*\)$/\2/'`
    ac_gdz_compiler_base=bcc

    changequote([, ])dnl

    if test $ac_gdz_compiler_version"set" = set; then
        ac_gdz_compiler_version=55
    fi

    case $ac_gdz_compiler_version in

        55)
        ac_gdz_borlandsearchdir=Borland/BCC$ac_gdz_compiler_version
        ;;
        *)
        ac_gdz_borlandsearchdir=Borland/BCC$ac_gdz_compiler_version
        ;;
    esac    

    AC_GDZ_FIND_PROG_DIR($ac_gdz_borlandsearchdir)

    ac_gdz_compiler_dir=$ac_gdz_find_prog_dir_result
    ac_gdz_compiler_path=$ac_gdz_compiler_dir/bin
    ac_gdz_compiler_incl=$ac_gdz_compiler_dir/Include
    ac_gdz_compiler_lib=$ac_gdz_compiler_dir/Lib
    ac_gdz_compiler_exe=bcc32.exe
    ac_gdz_linker_exe=bcc32.exe
    ac_gdz_check_compiler_available=yes
])

AC_DEFUN(AC_GDZ_GET_MSVS_PATH,
[
])

AC_DEFUN(AC_GDZ_GUESS_COMPILER_DIR_AND_EXE,
[
    AC_MSG_CHECKING("/ guessing compiler dir for $build-$ac_gdz_compiler")

    case "$build_os" in
       irix*)
        ac_gdz_compiler_exe=$ac_gdz_compiler
        ac_gdz_compiler_path=/usr/bin
        ;;
        *-pc-linux-gnu)
        ac_gdz_compiler_exe=$ac_gdz_compiler
        ac_gdz_compiler_path=/usr/bin
        ;;
       cygwin*)

        case "$ac_gdz_compiler" in
            icl*)
            AC_GDZ_SETUP_INTEL()
            ;;
            cl.net*)
            AC_GDZ_SETUP_MSVCNET()
            ;;
            cl*)
            AC_GDZ_SETUP_MSVC()
            ;;
            bcc*)
            AC_GDZ_SETUP_BORLAND()
            ;;
        esac

        ;;
        *)
        ;;
    esac

    AC_MSG_RESULT($ac_gdz_compiler_path)
])

AC_DEFUN(AC_GDZ_GUESS_COMPILER_DEFAULTS,
[
    # guess compiler if not set
    if test "x$ac_gdz_compiler" = "x"
    then
        case "$build_os" in
        cygwin*)    ac_gdz_compiler=icl 
                    ;;
        linux*)     ac_gdz_compiler=g++
                    ;;
        irix*)      ac_gdz_compiler=CC
                    ;;
        *)          echo "System $build_os unknown, use --with-compiler!"
                    exit 1
                    ;;
        esac
        echo "selected compiler $ac_gdz_compiler for system $build_os"
    fi
])

AC_DEFUN(AC_GDZ_SET_COMPILER_DEFAULTS,
[
    ac_gdz_check_compiler_available=no

    AC_MSG_CHECKING("site config conf.$build-$ac_gdz_compiler.in")

    if test -r "$ac_gdz_commonconf_dir/conf.$build-$ac_gdz_compiler.in"; then
        AC_MSG_RESULT("yes")
        . "$ac_gdz_commonconf_dir/conf.$build-$ac_gdz_compiler.in"
    else
        AC_MSG_RESULT("no")
    fi

    if test $ac_gdz_compiler_dir"set" = set; then 
        AC_GDZ_GUESS_COMPILER_DIR_AND_EXE()
    fi

    if test $ac_gdz_check_compiler_available = yes; then
        AC_GDZ_PROG_CC_AVAILABLE($ac_gdz_c_compiler_exe, $ac_gdz_compiler_path)
        AC_GDZ_PROG_CXX_AVAILABLE($ac_gdz_compiler_exe, $ac_gdz_compiler_path)
    else
        AC_GDZ_PROG_CC_WORKS($ac_gdz_c_compiler_exe, $ac_gdz_compiler_path)
        AC_GDZ_PROG_CXX_WORKS($ac_gdz_compiler_exe, $ac_gdz_compiler_path)
        ac_gdz_compiler_exe=$CXX
    fi
])

AC_DEFUN(AC_GDZ_EXEEXT, 
[
    case "$build_os" in

       cygwin*)
        AC_MSG_CHECKING(executable suffix)
        EXEEXT=.exe
        ac_gdz_exe_suffix=.exe
        AC_MSG_RESULT($EXEEXT)
        ;;

        *)
        AC_EXEEXT()
        ac_gdz_exe_suffix=.$EXEEXT
        ;;

    esac

    if test $ac_gdz_exe_suffix = "."; then
        ac_gdz_exe_suffix=
    fi
])

AC_DEFUN(AC_GDZ_OBJEXT,
[
    case "$build_os" in

       cygwin*)
        if test $ac_gdz_compiler_base = g++; then
        	AC_MSG_CHECKING(obj suffix)
        	ac_gdz_obj_suffix=.o
        	AC_MSG_RESULT($ac_gdz_obj_suffix)
        else
        	AC_MSG_CHECKING(obj suffix)
        	ac_gdz_obj_suffix=.obj
        	AC_MSG_RESULT($ac_gdz_obj_suffix)
        fi
        ;;

        *)
        AC_OBJEXT()
        ac_gdz_obj_suffix=.$OBJEXT
        ;;

    esac
])

AC_DEFUN(AC_GDZ_LIBEXT,
[
    AC_MSG_CHECKING(lib suffix)

    case "$build_os" in

       cygwin*)
        if test $ac_gdz_compiler_base = g++; then
	        ac_gdz_lib_suffix=.a
        else
	        ac_gdz_lib_suffix=.lib
	    fi
        ;;

        *)
        ac_gdz_lib_suffix=.a
        ;;

    esac

    AC_MSG_RESULT($ac_gdz_lib_suffix)
])

AC_DEFUN(AC_GDZ_SOEXT,
[
    AC_MSG_CHECKING(shared object suffix)

    case "$build_os" in

        cygwin*)
        ac_gdz_so_suffix=.dll
        ;;

        darwin*)
        ac_gdz_so_suffix=.dylib
        ;;

        hpux*)
        ac_gdz_so_suffix=.sl
        ;;
        *)
        ac_gdz_so_suffix=.so
        ;;

    esac

    AC_MSG_RESULT($ac_gdz_so_suffix)
])

AC_DEFUN(AC_GDZ_SOPRAEFIX,
[
     AC_MSG_CHECKING(shared object praefix)

     case "$build_os" in

       cygwin*)
        ac_gdz_so_praefix=
        ;;

        *)
        ac_gdz_so_praefix=lib
        ;;

    esac

    AC_MSG_RESULT($ac_gdz_so_praefix)
])

AC_DEFUN(AC_GDZ_FIND_STUDIO_DIR,
[
    ac_gdz_find_prog_dir_result=""

    for drive in c d e f g; do
     for progdir in "Program Files" "Programme"; do
      if test -d "/cygdrive/$drive/$progdir/Microsoft Visual Studio"; then
       ac_gdz_find_prog_dir_result="/cygdrive/$drive/$progdir/Microsoft Visual Studio"
       break 2
      fi
     done
    done
])

AC_DEFUN(AC_GDZ_SET_SYSTEM_DIRS,
[
    AC_MSG_CHECKING(system dir)

    case "$build_os" in
        cygwin*)
            case "$ac_gdz_compiler" in
                icl*)
                    AC_GDZ_FIND_STUDIO_DIR()

                    ac_gdz_studio_dir=$ac_gdz_find_prog_dir_result

                    ac_gdz_system_incl_dir=$ac_gdz_studio_dir/VC98/Include
                    ac_gdz_system_lib_dir=$ac_gdz_studio_dir/VC98/Lib

                    if ! test -d "$ac_gdz_studio_dir"; then
                        echo
                        echo "ERROR could not determine compiler dir, tried :"

                        for drive in c d e f g; do
                            for progdir in "Program Files" "Programme"; do
                                echo "    /cygdrive/$drive/$progdir/Microsoft Visual Studio"
                            done
                        done
                    
                        exit
                    fi
                ;;
                *)
                    ac_gdz_system_incl_dir=$ac_gdz_compiler_incl
                    ac_gdz_system_lib_dir=$ac_gdz_compiler_lib
                ;;
            esac
        ;;

        *)
        ac_gdz_so_praefix=lib
        ;;
    esac

    AC_MSG_RESULT($ac_gdz_studio_dir)
])

AC_DEFUN(AC_GDZ_SCAN_PACKET_DESC,
[
    changequote(<<, >>)dnl

    for ac_gdz_package_name in ${ac_gdz_packages} ; do

        eval inc_dep=\${ac_gdz_package_inc_dep_${ac_gdz_package_name}}
        eval link_dep=\${ac_gdz_package_link_dep_${ac_gdz_package_name}}
    
        if test ${build_os} = "cygwin"; then
            inc_dep=` echo $inc_dep  | sed 's/@WINDOWSYSTEM@/WIN32/g'`
            link_dep=`echo $link_dep | sed 's/@WINDOWSYSTEM@/WIN32/g'`
        else
            inc_dep=` echo $inc_dep  | sed 's/@WINDOWSYSTEM@/X/g'`
            link_dep=`echo $link_dep | sed 's/@WINDOWSYSTEM@/X/g'`
        fi

        eval ac_gdz_package_inc_dep_${ac_gdz_package_name}=\$inc_dep;
        eval ac_gdz_package_link_dep_${ac_gdz_package_name}=\$link_dep;
    done

    changequote([, ])dnl
])


AC_DEFUN(AC_GDZ_WRITE_PACKET_MAKE,
[

dnl e2

    changequote(<<, >>)dnl

    for ac_gdz_package_name in ${ac_gdz_packages} ; do

        echo resolving $ac_gdz_package_name

        eval ac_gdz_package_inc_cnv_in=\${ac_gdz_package_inc_dep_${ac_gdz_package_name}}

        ac_gdz_package_inc_cnv_out=

        for dir in ${ac_gdz_package_inc_cnv_in}; do

            p1=`echo ${dir} | sed 's/@\([^@]*\)@/\1/'`
            p2=`echo ${dir} | sed 's/@\([^@]*\)@/XXX/'`

            if test $p2 = "XXX"; then

                eval ac_gdz_package_inc_cnv_tmp=\${ac_gdz_package_dirs_${p1}}

                ac_gdz_package_inc_cnv_tmp=`echo ${ac_gdz_package_inc_cnv_tmp} | sed 's/@ / /g' | sed 's/@$'//g`

                ac_gdz_package_inc_cnv_out="$ac_gdz_package_inc_cnv_out $ac_gdz_package_inc_cnv_tmp"
            else
                ac_gdz_package_inc_cnv_out="$ac_gdz_package_inc_cnv_out $p1" 
            fi
                    
        done

        echo "    from :${ac_gdz_package_inc_cnv_in}"
        echo "        to : ${ac_gdz_package_inc_cnv_out}"

        eval ac_gdz_package_inc_dep_${ac_gdz_package_name}=\$ac_gdz_package_inc_cnv_out

    done

    for ac_gdz_package_name in ${ac_gdz_packages} ; do

        echo configuring package ${ac_gdz_package_name}

        if test ${ac_gdz_package_name} = "WindowX"; then
            case ${build_os} in
    
                cygwin*)
                    echo Skipping ${ac_gdz_package_name}
                    continue
                ;;
                darwin*)
                    echo Skipping ${ac_gdz_package_name}
                    continue
                ;;
                *)
                    ac_gdz_package_order="${ac_gdz_package_order} WindowXLib"
                    ac_gdz_package_order_test="${ac_gdz_package_order_test} WindowXTest"
                ;;
            esac
        fi
        if test ${ac_gdz_package_name} = "WindowWIN32"; then
            if test ${build_os} != "cygwin"; then
                echo Skipping ${ac_gdz_package_name}
                continue
            else
                ac_gdz_package_order="${ac_gdz_package_order} WindowWIN32Lib"
                ac_gdz_package_order_test="${ac_gdz_package_order_test} WindowWIN32Test"
            fi
        fi
        if test ${ac_gdz_package_name} = "WindowQT"; then
            if test ${ac_gdz_conf_package_qt} = 0; then
                echo Skipping ${ac_gdz_package_name}
                continue
            else
                ac_gdz_package_order="${ac_gdz_package_order} WindowQTLib"
                ac_gdz_package_order_test="${ac_gdz_package_order_test} WindowQTTest"
            fi
        fi
        if test ${ac_gdz_package_name} = "WindowGLUT"; then
            if test ${ac_gdz_conf_package_glut} = 0; then
                echo Skipping ${ac_gdz_package_name}
                continue
            else
                ac_gdz_package_order="${ac_gdz_package_order} WindowGLUTLib"
                ac_gdz_package_order_test="${ac_gdz_package_order_test} WindowGLUTTest"
            fi
        fi

        eval ac_gdz_package_def_out=\${ac_gdz_package_def_${ac_gdz_package_name}}

        ac_gdz_package_name_out=${ac_gdz_package_name}

        eval ac_gdz_package_inc_dep=\${ac_gdz_package_inc_dep_${ac_gdz_package_name}}

        ac_gdz_package_inc_dep_out=  

        for dir in ${ac_gdz_package_inc_dep}; do

            p1=`echo ${dir} | sed 's/\([^@]*\)@\(.*\)/\1/'`
            p2=`echo ${dir} | sed 's/\([^@]*\)@\(.*\)/\2/'`

            if test $p1 = $p2; then
                dir=$p1 
            else
                if test $build_os = $p2; then
                    dir=$p1
                else
                    continue
                fi
            fi

            ac_gdz_package_inc_dep_out="$ac_gdz_package_inc_dep_out $dir"
        done

        ac_gdz_package_inc_dep_out_files= 

        eval ac_gdz_package_link_dep_out=\${ac_gdz_package_link_dep_${ac_gdz_package_name}}

        for dir in ${ac_gdz_package_inc_dep}; do

            p1=`echo ${dir} | sed 's/\([^@]*\)@\(.*\)/\1/'`
            p2=`echo ${dir} | sed 's/\([^@]*\)@\(.*\)/\2/'`
            
            if test $p1 = $p2; then
                dir=$p1 
            else
                if test $build_os = $p2; then
                    dir=$p1
                else
                    continue
                fi
            fi

            ac_gdz_package_check_dir_e2=$ac_gdz_src_dir/$dir            

            if test -d $ac_gdz_package_check_dir_e2; then
                ac_gdz_package_inc_dep_out_files=$ac_gdz_package_inc_dep_out_files' $('${ac_gdz_project_praefix}'POOL)'/$dir/common.mk
            else
                ac_gdz_package_check_file_e2=$ac_gdz_commonpackage_dir/common$dir.mk

                if test -r $ac_gdz_package_check_file_e2; then
                    ac_gdz_package_inc_dep_out_files="$ac_gdz_package_inc_dep_out_files $ac_gdz_commonpackage_dir/common$dir.mk"
    
                    if test $build_os = cygwin || test $build_os = darwin; then
                        ac_gdz_package_link_dep_out="$ac_gdz_package_link_dep_out $dir"
                    fi
                else
                    ac_gdz_package_inc_dep_out_files=$ac_gdz_package_inc_dep_out_files' $('${ac_gdz_project_praefix}'POOL)'/$dir/common.mk
                fi
            fi
        done

        ac_gdz_package_link_dep_out_files=

        eval ac_gdz_package_link_dep=\${ac_gdz_package_link_dep_${ac_gdz_package_name}}

        for dir in ${ac_gdz_package_link_dep}; do

            ac_gdz_package_check_file_e2=$ac_gdz_commonpackage_dir/common$dir.mk
            
            if test -r $ac_gdz_package_check_file_e2; then
                ac_gdz_package_link_dep_out_files="$ac_gdz_package_link_dep_out_files $ac_gdz_commonpackage_dir/common$dir.mk"
            else
                ac_gdz_package_link_dep_out_files=$ac_gdz_package_link_dep_out_files' $(BUILD_BASE)'/$dir/common.mk
            fi
        done

        eval ac_gdz_package_testinc_dep_out=\"\${ac_gdz_package_name_out}Lib  \${ac_gdz_package_testinc_dep_${ac_gdz_package_name}}\"
        ac_gdz_package_testinc_dep_out_files='$(BUILD_BASE)'/${ac_gdz_package_name_out}Lib/common.mk

        eval ac_gdz_package_testinc_dep=\${ac_gdz_package_testinc_dep_${ac_gdz_package_name}}

        for dir in ${ac_gdz_package_testinc_dep}; do
            ac_gdz_package_check_file_e2=$ac_gdz_src_dir/$dir/common.mk
            
            if test -r $ac_gdz_package_check_file_e2; then
                ac_gdz_package_testinc_dep_out_files="$ac_gdz_package_testinc_dep_out_files $ac_gdz_src_dir/$dir/common.mk"
            else
                ac_gdz_package_check_file_e2=$ac_gdz_commonpackage_dir/common$dir.mk
            
                if test -r $ac_gdz_package_check_file_e2; then
                    ac_gdz_package_testinc_dep_out_files="$ac_gdz_package_testinc_dep_out_files $ac_gdz_commonpackage_dir/common$dir.mk"
                 else
                    ac_gdz_package_testinc_dep_out_files=$ac_gdz_package_testinc_dep_out_files' $(BUILD_BASE)'/$dir/common.mk
                fi
            fi
        done


        eval ac_gdz_package_testlink_dep_out=\"\${ac_gdz_package_name_out}Lib' '\${ac_gdz_package_testlink_dep_${ac_gdz_package_name}}\"
        ac_gdz_package_testlink_dep_out_files='$(BUILD_BASE)'/${ac_gdz_package_name_out}Lib/common.mk

        eval ac_gdz_package_testlink_dep=\${ac_gdz_package_testlink_dep_${ac_gdz_package_name}}

        for dir in ${ac_gdz_package_testlink_dep}; do
            ac_gdz_package_check_file_e2=$ac_gdz_commonpackage_dir/common$dir.mk
            
            if test -r $ac_gdz_package_check_file_e2; then
                ac_gdz_package_testlink_dep_out_files="$ac_gdz_package_testlink_dep_out_files $ac_gdz_commonpackage_dir/common$dir.mk"
            else
                ac_gdz_package_testlink_dep_out_files=$ac_gdz_package_testlink_dep_out_files' $(BUILD_BASE)'/$dir/common.mk
            fi
        done


        ac_gdz_package_dir_base=${ac_gdz_package_sub_dir_out}/${ac_gdz_package_name}
        ac_gdz_package_dir=${ac_gdz_package_dir_base}Lib
        ac_gdz_package_test_dir=${ac_gdz_package_dir_base}Test

        ac_gdz_common_packet_make=${ac_gdz_package_dir}/Makefile
        ac_gdz_common_packet_testmake=${ac_gdz_package_test_dir}/Makefile

        eval ac_gdz_package_fact_init=\${ac_gdz_package_fact_init_${ac_gdz_package_name}}

        if test ${ac_gdz_package_fact_init} = "1"; then

            case $build_os in

                cygwin*)
                    ac_gdz_common_init_code_in_e2=${ac_gdz_commonconf_dir}/OSGDllInit.cpp.in
                    ac_gdz_common_init_code_e2=${ac_gdz_package_dir}/OSGDllInit.cpp
                    ac_gdz_common_init_code_files_e2="$ac_gdz_common_init_code_e2:$ac_gdz_common_init_code_in_e2"
                    ac_gdz_package_so_needs_init_e2=1
                ;;
                linux-gnu*)
                    ac_gdz_common_init_code_in_e2=${ac_gdz_commonconf_dir}/OSGSoInit.cpp.in
                    ac_gdz_common_init_code_e2=${ac_gdz_package_dir}/OSGSoInit.cpp
                    ac_gdz_common_init_code_files_e2="$ac_gdz_common_init_code_e2:$ac_gdz_common_init_code_in_e2"
                    ac_gdz_package_so_needs_init_e2=0
                ;;
                darwin*)
                    ac_gdz_common_init_code_in_e2=${ac_gdz_commonconf_dir}/OSGSoInit.cpp.in
                    ac_gdz_common_init_code_e2=${ac_gdz_package_dir}/OSGSoInit.cpp
                    ac_gdz_common_init_code_files_e2="$ac_gdz_common_init_code_e2:$ac_gdz_common_init_code_in_e2"
                    ac_gdz_package_so_needs_init_e2=0
                ;;
                solaris*)
                    ac_gdz_common_init_code_i_e2=${ac_gdz_commonconf_dir}/OSGSoInit.cpp.in
                    ac_gdz_common_init_code_e2=${ac_gdz_package_dir}/OSGSoInit.cpp
                    ac_gdz_common_init_code_files_e2="$ac_gdz_common_init_code_e2:$ac_gdz_common_init_code_in_e2"
                    ac_gdz_package_so_needs_init_e2=0
                ;;
                *)
                    ac_gdz_common_init_code_in_e2=${ac_gdz_commonconf_dir}/OSGSoInit.cpp.in
                    ac_gdz_common_init_code_e2=${ac_gdz_package_dir}/OSGSoInit.cpp
                    ac_gdz_common_init_code_files_e2="$ac_gdz_common_init_code_e2:$ac_gdz_common_init_code_in_e2"
                    ac_gdz_package_so_needs_init_e2=1
                ;;
            esac
        else
            ac_gdz_common_init_code_files_e2=
            ac_gdz_package_so_needs_init_e2=0
        fi

        ac_gdz_common_mk_in_e2=${ac_gdz_commonconf_dir}/common.mk.in
        ac_gdz_common_mk_out_e2=${ac_gdz_package_dir}/common.mk
        ac_gdz_common_mk_files_e2="$ac_gdz_common_mk_out_e2:$ac_gdz_common_mk_in_e2"

        ac_gdz_vpath_out=.

        iFirstTime="yes"
        ac_gdz_package_def_e2=

        eval ac_gdz_package_dirs=\${ac_gdz_package_dirs_${ac_gdz_package_name}}

        for dir in ${ac_gdz_package_dirs}; do

            p1=`echo ${dir} | sed 's/\([^@]*\)@\(.*\)/\1/'`
            p2=`echo ${dir} | sed 's/\([^@]*\)@\(.*\)/\2/'`
            
        	FK_GDZ_CREATE_PACKAGE_DIRS ${ac_gdz_package_dir_base}
        
            if test -n "$p2"; then
                if test $build_os = $p2; then
                    ac_gdz_vpath_out=$ac_gdz_vpath_out:'$('${ac_gdz_project_praefix}'POOL)'/$p1
                fi
            else
                ac_gdz_vpath_out=$ac_gdz_vpath_out:'$('${ac_gdz_project_praefix}'POOL)'/$p1
            fi

        done

        changequote([, ])dnl

        ac_gdz_win_pool_e2=
        ac_gdz_win_build_base_e2=
        ac_gdz_build_dir_e2=$ac_gdz_build_dir        

        ac_gdz_package_test_debug_libs_e2=$ac_gdz_test_debug_libs

        if test $build_os = cygwin; then
            ac_gdz_src_dir_win_e2=`cygpath -w $ac_gdz_src_dir`
            ac_gdz_win_pool_e2=${ac_gdz_project_praefix}POOL_WIN"     ?= "$ac_gdz_src_dir_win_e2

            ac_gdz_package_sub_dir_win_e2=`cygpath -w $ac_gdz_build_dir_e2/$ac_gdz_package_sub_dir_out`
            ac_gdz_win_build_base_e2="BUILD_BASE_WIN := "$ac_gdz_package_sub_dir_win_e2
        fi

        AC_SUBST(ac_gdz_src_dir)
        AC_SUBST(ac_gdz_build_dir_e2)
        AC_SUBST(ac_gdz_win_pool_e2)
        AC_SUBST(ac_gdz_win_build_base_e2)
        AC_SUBST(ac_gdz_package_def_out)
        AC_SUBST(ac_gdz_package_name_out)
        AC_SUBST(ac_gdz_package_inc_dep_out)
        AC_SUBST(ac_gdz_package_inc_dep_out_files)
        AC_SUBST(ac_gdz_package_link_dep_out)
        AC_SUBST(ac_gdz_package_link_dep_out_files)
        AC_SUBST(ac_gdz_package_testinc_dep_out)
        AC_SUBST(ac_gdz_package_testinc_dep_out_files)
        AC_SUBST(ac_gdz_package_testlink_dep_out)
        AC_SUBST(ac_gdz_package_testlink_dep_out_files)

        AC_SUBST(ac_gdz_package_sub_dir_out)        AC_SUBST(ac_gdz_package_sys_common_out)
        
        AC_SUBST(ac_gdz_vpath_out)
        AC_SUBST(ac_gdz_project_praefix)

        AC_SUBST(ac_gdz_package_so_needs_init_e2)

        AC_SUBST(ac_gdz_package_def_e2)

        AC_SUBST(ac_gdz_package_test_debug_libs_e2)        

        touch confdefs.h

        AC_OUTPUT([$ac_gdz_common_packet_make:$ac_gdz_common_packetmake_in
                   $ac_gdz_common_packet_testmake:$ac_gdz_common_testmake_in
                   $ac_gdz_common_init_code_files_e2 
                   $ac_gdz_common_mk_files_e2])

    done

])


AC_DEFUN(AC_GDZ_WRITE_BUILDS_MAKE,
[
dnl e3

    ac_gdz_common_builds_make_e3=$ac_gdz_package_sub_dir_out/Makefile
    ac_gdz_install_prefix_e3=$ac_gdz_install_prefix

    AC_SUBST(ac_gdz_lib_src_dir)
    AC_SUBST(ac_gdz_package_order)
    AC_SUBST(ac_gdz_package_order_test)
    AC_SUBST(ac_gdz_lib_package_sys_common_out)
    AC_SUBST(ac_gdz_lib_project_praefix)
    AC_SUBST(ac_gdz_install_prefix_e3)

    touch confdefs.h

    AC_OUTPUT($ac_gdz_common_builds_make_e3:$ac_gdz_common_buildsmake_in)
])



AC_DEFUN(AC_GDZ_WRITE_COMMON_STL,
[
dnl e4

    ac_gdz_stl_lib_e4=
    ac_gdz_stl_dir_e4=

    if test -n "$ac_gdz_stl_dir"; then
        ac_gdz_stl_lib_e4='-lCio'
        ac_gdz_stl_dir_e4=$ac_gdz_stl_dir
    else
        if test $build_os = irix6.5; then
            ac_gdz_stl_lib_e4='-lCio'
        fi
    fi

    ac_gdz_common_stl_in_e4=$ac_gdz_commonconf_dir/commonSTL.in
    ac_gdz_common_stl_e4=$ac_gdz_commonpackage_dir/commonSTL.mk

    AC_SUBST(ac_gdz_stl_dir_e4)
    AC_SUBST(ac_gdz_stl_lib_e4)
   
    touch confdefs.h

    AC_OUTPUT($ac_gdz_common_stl_e4:$ac_gdz_common_stl_in_e4)
])


AC_DEFUN(AC_GDZ_WRITE_COMMON_GLUT,
[
dnl e5

    ac_gdz_glut_lib_e5=
    ac_gdz_glut_incdir_e5=
    ac_gdz_glut_libdir_e5=

    if test "$enable_glut" = yes; then

        case $build_os in
            cygwin*)
                ac_gdz_glut_lib_e5='glut32.lib'
            ;;
            darwin*)
                ac_gdz_glut_lib_e5='-framework Cocoa -framework GLUT'
            ;;
            *)
                ac_gdz_glut_lib_e5='-lglut'
            ;;
        esac

        if test -n "$ac_gdz_glut_incdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_glut_incdir_e5='"'`cygpath -w $ac_gdz_glut_incdir`'"'
                ;;
                *)
                    ac_gdz_glut_incdir_e5=$ac_gdz_glut_incdir
                ;;
            esac
        else
            case $build_os in
                darwin*)
                    ac_gdz_glut_incdir_e5=/System/Library/Frameworks/GLUT.framework/Headers
                ;;
            esac
        fi

        if test -n "$ac_gdz_glut_libdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_glut_libdir_e5='"'`cygpath -w $ac_gdz_glut_libdir`'"'
                ;;
                *)
                    ac_gdz_glut_libdir_e5=$ac_gdz_glut_libdir
                ;;

            esac
        fi

    fi

    ac_gdz_common_glut_in_e5=$ac_gdz_commonconf_dir/commonGLUT.in
    ac_gdz_common_glut_e5=$ac_gdz_commonpackage_dir/commonGLUT.mk

    AC_SUBST(ac_gdz_glut_incdir_e5)
    AC_SUBST(ac_gdz_glut_libdir_e5)
    AC_SUBST(ac_gdz_glut_lib_e5)
   
    touch confdefs.h

    AC_OUTPUT($ac_gdz_common_glut_e5:$ac_gdz_common_glut_in_e5)
])


AC_DEFUN(AC_GDZ_WRITE_COMMON_QT,
[
dnl e5

    changequote(<<, >>)dnl

    ac_gdz_qt_lib_e6=
    ac_gdz_qt_incdir_e6=
    ac_gdz_qt_libdir_e6=
    ac_gdz_qt_moc_e6=moc
    ac_gdz_qt_uic_e6=uic

    if test "$enable_qt" = yes; then

        case $build_os in
            cygwin*)
                ac_gdz_qt_lib_e6=`cd $ac_gdz_qt_libdir; ls qt-mt[0-9]*.lib 2> /dev/null`
                            
                if test "x"$ac_gdz_qt_lib_e6 = "x"; then
                    ac_gdz_qt_lib_e6=`cd $ac_gdz_qt_libdir; ls qt[0-9]*.lib 2> /dev/null`
                fi
            ;;
            *)
                ac_gdz_qt_lib_e6=`cd $ac_gdz_qt_libdir; ls libqt-mt.so 2> /dev/null`

                if test "x"$ac_gdz_qt_lib_e6 = "x"; then
                    ac_gdz_qt_lib_e6=`cd $ac_gdz_qt_libdir; ls libqt.so 2> /dev/null`

                    if test "x"$ac_gdz_qt_lib_e6 != "x"; then
                        ac_gdz_qt_lib_e6='-lqt'
                    fi
        
                else
                    ac_gdz_qt_lib_e6='-lqt-mt'
                fi
            ;;
        esac

        if test -n "$ac_gdz_qt_incdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_qt_incdir_e6='"'`cygpath -w $ac_gdz_qt_incdir`'"'
                ;;
                *)
                   ac_gdz_qt_incdir_e6=$ac_gdz_qt_incdir
                ;;
            esac
        fi

        if test -n "$ac_gdz_qt_libdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_qt_libdir_e6='"'`cygpath -w $ac_gdz_qt_libdir`'"'
                ;;
                *)
                   ac_gdz_qt_libdir_e6=$ac_gdz_qt_libdir
                ;;
            esac
        fi

        if test -n "$ac_gdz_qt_bindir"; then
            ac_gdz_qt_moc_e6=$ac_gdz_qt_bindir/moc
            ac_gdz_qt_uic_e6=$ac_gdz_qt_bindir/uic
        fi

    fi

    changequote([, ])dnl

    ac_gdz_common_qt_in_e6=$ac_gdz_commonconf_dir/commonQT.in
    ac_gdz_common_qt_e6=$ac_gdz_commonpackage_dir/commonQT.mk

    AC_SUBST(ac_gdz_qt_incdir_e6)
    AC_SUBST(ac_gdz_qt_libdir_e6)
    AC_SUBST(ac_gdz_qt_lib_e6)
    AC_SUBST(ac_gdz_qt_moc_e6)
    AC_SUBST(ac_gdz_qt_uic_e6)
   
    touch confdefs.h

    AC_OUTPUT($ac_gdz_common_qt_e6:$ac_gdz_common_qt_in_e6)
])



AC_DEFUN(AC_GDZ_WRITE_SYSTEM_COMMON,
[
dnl e7

    ac_gdz_common_sys_mk_os_base_e7=$build_os
    ac_gdz_common_sys_mk_build_env_e7=$build

    ac_gdz_common_sys_mk_in_e7=$ac_gdz_commonconf_dir/commonSys.mk.in
    ac_gdz_common_sys_mk_e7=$ac_gdz_common_dir/common.mk

    AC_SUBST(ac_gdz_common_sys_mk_os_base_e7)
    AC_SUBST(ac_gdz_common_sys_mk_build_env_e7)

    touch confdefs.h

    AC_OUTPUT($ac_gdz_common_sys_mk_e7:$ac_gdz_common_sys_mk_in_e7)
])


AC_DEFUN(AC_GDZ_WRITE_COMMON_GL,
[
dnl e8

    ac_gdz_gl_lib_e8=
    ac_gdz_gl_incdir_e8=
    ac_gdz_gl_libdir_e8=

    if test "$with_gl" = yes; then

        case $build_os in
            cygwin*)
                ac_gdz_gl_lib_e8=''
            ;;
            darwin*)
                ac_gdz_gl_lib_e8=''
                ac_gdz_gl_incdir_e8=/System/Library/Frameworks/OpenGL.framework/Headers
            ;;
            *)
                ac_gdz_gl_lib_e8=''
            ;;
        esac

    elif test -n "$ac_gdz_gl_dir"; then
        if test $build_os = cygwin; then
           ac_gdz_gl_incdir_e8='"'`cygpath -w $ac_gdz_gl_dir/include`'"'
           ac_gdz_gl_libdir_e8='"'`cygpath -w $ac_gdz_gl_dir/lib`'"'
        else
           ac_gdz_gl_incdir_e8=$ac_gdz_gl_dir/include
           ac_gdz_gl_libdir_e8=$ac_gdz_gl_dir/lib
        fi

        case $build_os in
            cygwin*)
                ac_gdz_gl_lib_e8=''
            ;;
            darwin*)
                ac_gdz_gl_lib_e8=''
            ;;
            *)
                ac_gdz_gl_lib_e8=''
            ;;
        esac
    fi

    ac_gdz_common_gl_in_e8=$ac_gdz_commonconf_dir/commonGL.in
    ac_gdz_common_gl_e8=$ac_gdz_commonpackage_dir/commonGL.mk

    AC_SUBST(ac_gdz_gl_incdir_e8)
    AC_SUBST(ac_gdz_gl_libdir_e8)
    AC_SUBST(ac_gdz_gl_lib_e8)
   
    touch confdefs.h

    AC_OUTPUT($ac_gdz_common_gl_e8:$ac_gdz_common_gl_in_e8)
])

AC_DEFUN(AC_GDZ_WRITE_COMMON_TIF,
[
dnl e9

    ac_gdz_tif_lib_e9=
    ac_gdz_tif_incdir_e9=
    ac_gdz_tif_libdir_e9=

    if test "$enable_tif" = yes; then

        case $build_os in
            cygwin*)
                ac_gdz_tif_lib_e9='tif32.lib'
            ;;
            *)
                ac_gdz_tif_lib_e9='-ltiff'
            ;;
        esac

        if test -n "$ac_gdz_tif_incdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_tif_incdir_e9='"'`cygpath -w $ac_gdz_tif_incdir`'"'
                ;;
                *)
                    ac_gdz_tif_incdir_e9=$ac_gdz_tif_incdir
                ;;
            esac
        fi

        if test -n "$ac_gdz_tif_libdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_tif_libdir_e9='"'`cygpath -w $ac_gdz_tif_libdir`'"'
                ;;
                *)
                    ac_gdz_tif_libdir_e9=$ac_gdz_tif_libdir
                ;;
            esac
        fi

    fi

    ac_gdz_common_tif_in_e9=$ac_gdz_commonconf_dir/commonTIF.in
    ac_gdz_common_tif_e9=$ac_gdz_commonpackage_dir/commonTIF.mk

    AC_SUBST(ac_gdz_tif_incdir_e9)
    AC_SUBST(ac_gdz_tif_libdir_e9)
    AC_SUBST(ac_gdz_tif_lib_e9)
   
    touch confdefs.h

    AC_OUTPUT($ac_gdz_common_tif_e9:$ac_gdz_common_tif_in_e9)
])

AC_DEFUN(AC_GDZ_WRITE_COMMON_JPG,
[
dnl e10

    ac_gdz_jpg_lib_e10=
    ac_gdz_jpg_incdir_e10=
    ac_gdz_jpg_libdir_e10=

    if test "$enable_jpg" = yes; then

        case $build_os in
            cygwin*)
                ac_gdz_jpg_lib_e10='libjpeg.lib'
            ;;
            *)
                ac_gdz_jpg_lib_e10='-ljpeg'
            ;;
        esac

        if test -n "$ac_gdz_jpg_incdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_jpg_incdir_e10='"'`cygpath -w $ac_gdz_jpg_incdir`'"'
                ;;
                *)
                    ac_gdz_jpg_incdir_e10=$ac_gdz_jpg_incdir
                ;;
            esac
        fi

        if test -n "$ac_gdz_jpg_libdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_jpg_libdir_e10='"'`cygpath -w $ac_gdz_jpg_libdir`'"'
                ;;
                *)
                    ac_gdz_jpg_libdir_e10=$ac_gdz_jpg_libdir
                ;;
            esac
        fi

    fi

    ac_gdz_common_jpg_in_e10=$ac_gdz_commonconf_dir/commonJPG.in
    ac_gdz_common_jpg_e10=$ac_gdz_commonpackage_dir/commonJPG.mk

    AC_SUBST(ac_gdz_jpg_incdir_e10)
    AC_SUBST(ac_gdz_jpg_libdir_e10)
    AC_SUBST(ac_gdz_jpg_lib_e10)
   
    touch confdefs.h

    AC_OUTPUT($ac_gdz_common_jpg_e10:$ac_gdz_common_jpg_in_e10)
])

AC_DEFUN(AC_GDZ_WRITE_COMMON_PNG,
[
dnl e11

    ac_gdz_png_lib_e11=
    ac_gdz_png_incdir_e11=
    ac_gdz_png_libdir_e11=

    if test "$enable_png" = yes; then

        case $build_os in
            cygwin*)
                ac_gdz_png_lib_e11='png32.lib'
            ;;
            *)
                ac_gdz_png_lib_e11='-lpng -lz'
            ;;
        esac

        if test -n "$ac_gdz_png_incdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_png_incdir_e11='"'`cygpath -w $ac_gdz_png_incdir`'"'
                ;;
                *)
                    ac_gdz_png_incdir_e11=$ac_gdz_png_incdir
                ;;
            esac
        fi

        if test -n "$ac_gdz_png_libdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_png_libdir_e11='"'`cygpath -w $ac_gdz_png_libdir`'"'
                ;;
                *)
                    ac_gdz_png_libdir_e11=$ac_gdz_png_libdir
                ;;
            esac
        fi

    fi

    ac_gdz_common_png_in_e11=$ac_gdz_commonconf_dir/commonPNG.in
    ac_gdz_common_png_e11=$ac_gdz_commonpackage_dir/commonPNG.mk

    AC_SUBST(ac_gdz_png_incdir_e11)
    AC_SUBST(ac_gdz_png_libdir_e11)
    AC_SUBST(ac_gdz_png_lib_e11)
   
    touch confdefs.h

    AC_OUTPUT($ac_gdz_common_png_e11:$ac_gdz_common_png_in_e11)
])

AC_DEFUN(AC_GDZ_WRITE_COMMON_MNG,
[
dnl e12

    ac_gdz_mng_lib_e12=
    ac_gdz_mng_incdir_e12=
    ac_gdz_mng_libdir_e12=

    if test "$enable_mng" = yes; then

        case $build_os in
            cygwin*)
                ac_gdz_mng_lib_e12='mng32.lib'
            ;;
            *)
                ac_gdz_mng_lib_e12='-lmng'
            ;;
        esac

        if test -n "$ac_gdz_mng_incdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_mng_incdir_e12='"'`cygpath -w $ac_gdz_mng_incdir`'"'
                ;;
                *)
                    ac_gdz_mng_incdir_e12=$ac_gdz_mng_incdir
                ;;
            esac
        fi

        if test -n "$ac_gdz_mng_libdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_mng_libdir_e12='"'`cygpath -w $ac_gdz_mng_libdir`'"'
                ;;
                *)
                    ac_gdz_mng_libdir_e12=$ac_gdz_mng_libdir
                ;;
            esac
        fi

    fi

    ac_gdz_common_mng_in_e12=$ac_gdz_commonconf_dir/commonMNG.in
    ac_gdz_common_mng_e12=$ac_gdz_commonpackage_dir/commonMNG.mk

    AC_SUBST(ac_gdz_mng_incdir_e12)
    AC_SUBST(ac_gdz_mng_libdir_e12)
    AC_SUBST(ac_gdz_mng_lib_e12)
   
    touch confdefs.h

    AC_OUTPUT($ac_gdz_common_mng_e12:$ac_gdz_common_mng_in_e12)
])

AC_DEFUN(AC_GDZ_WRITE_COMMON_GIF,
[
dnl e13

    ac_gdz_gif_lib_e13=
    ac_gdz_gif_incdir_e13=
    ac_gdz_gif_libdir_e13=

    ac_gdz_common_gif_in_e13=$ac_gdz_commonconf_dir/commonGIF.in
    ac_gdz_common_gif_e13=$ac_gdz_commonpackage_dir/commonGIF.mk

    AC_SUBST(ac_gdz_gif_incdir_e13)
    AC_SUBST(ac_gdz_gif_libdir_e13)
    AC_SUBST(ac_gdz_gif_lib_e13)
   
    touch confdefs.h

    AC_OUTPUT($ac_gdz_common_gif_e13:$ac_gdz_common_gif_in_e13)
])


AC_DEFUN(AC_GDZ_WRITE_COMMON_FREETYPE1,
[
dnl e14

    ac_gdz_freetype1_lib_e14=
    ac_gdz_freetype1_incdir_e14=
    ac_gdz_freetype1_libdir_e14=

    if test "$enable_freetype1" = yes; then

        case $build_os in
            cygwin*)
                ac_gdz_freetype1_lib_e14='ttf.lib'
            ;;
            *)
                ac_gdz_freetype1_lib_e14='-lttf'
            ;;
        esac

        if test -n "$ac_gdz_freetype1_incdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_freetype1_incdir_e14='"'`cygpath -w $ac_gdz_freetype1_incdir`'"'
                ;;
                *)
                    ac_gdz_freetype1_incdir_e14=$ac_gdz_freetype1_incdir
                ;;
            esac
        fi

        if test -n "$ac_gdz_freetype1_libdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_freetype1_libdir_e14='"'`cygpath -w $ac_gdz_freetype1_libdir`'"'
                ;;
                *)
                    ac_gdz_freetype1_libdir_e14=$ac_gdz_freetype1_libdir
                ;;
            esac
        fi

    fi

    ac_gdz_common_freetype1_in_e14=$ac_gdz_commonconf_dir/commonFREETYPE1.in
    ac_gdz_common_freetype1_e14=$ac_gdz_commonpackage_dir/commonFREETYPE1.mk

    AC_SUBST(ac_gdz_freetype1_incdir_e14)
    AC_SUBST(ac_gdz_freetype1_libdir_e14)
    AC_SUBST(ac_gdz_freetype1_lib_e14)
   
    touch confdefs.h

    AC_OUTPUT($ac_gdz_common_freetype1_e14:$ac_gdz_common_freetype1_in_e14)
])


AC_DEFUN(AC_GDZ_WRITE_OSG_CONFIG,
[
dnl e15

    ac_gdz_glut_lib_e15=
    ac_gdz_glut_incdir_e15=
    ac_gdz_glut_libdir_e15=

    if test "$enable_glut" = yes; then

        case $build_os in
            cygwin*)
                ac_gdz_glut_lib_e15='glut32.lib'
            ;;
            darwin*)
                ac_gdz_glut_lib_e15='-framework Cocoa -framework GLUT'
            ;;
            *)
                ac_gdz_glut_lib_e15='-lglut'
            ;;
        esac

        if test -n "$ac_gdz_glut_incdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_glut_incdir_e15='"'`cygpath -w $ac_gdz_glut_incdir`'"'
                ;;
                *)
                    ac_gdz_glut_incdir_e15=$ac_gdz_glut_incdir
                ;;
            esac
        else
            case $build_os in
                darwin*)
                    ac_gdz_glut_incdir_e15=/System/Library/Frameworks/GLUT.framework/Headers
                ;;
            esac
        fi

        if test -n "$ac_gdz_glut_libdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_glut_libdir_e15='"'`cygpath -w $ac_gdz_glut_libdir`'"'
                ;;
                *)
                    ac_gdz_glut_libdir_e15=$ac_gdz_glut_libdir
                ;;

            esac
        fi
        ac_gdz_have_glut_e15=yes
    else
        ac_gdz_have_glut_e15=no
    fi



    changequote(<<, >>)dnl

    ac_gdz_qt_lib_e15=
    ac_gdz_qt_incdir_e15=
    ac_gdz_qt_libdir_e15=
    ac_gdz_qt_moc_e15=moc
    ac_gdz_qt_uic_e15=uic

    if test "$enable_qt" = yes; then

        case $build_os in
            cygwin*)
                ac_gdz_qt_lib_e15=`cd $ac_gdz_qt_libdir; ls qt-mt[0-9]*.lib 2> /dev/null`
                            
                if test "x"$ac_gdz_qt_lib_e15 = "x"; then
                    ac_gdz_qt_lib_e15=`cd $ac_gdz_qt_libdir; ls qt[0-9]*.lib 2> /dev/null`
                fi
            ;;

            *)
                ac_gdz_qt_lib_e15=`cd $ac_gdz_qt_libdir; ls libqt-mt.so 2> /dev/null`

                if test "x"$ac_gdz_qt_lib_e15 = "x"; then
                    ac_gdz_qt_lib_e15=`cd $ac_gdz_qt_libdir; ls libqt.so 2> /dev/null`

                    if test "x"$ac_gdz_qt_lib_e15 != "x"; then
                        ac_gdz_qt_lib_e15='-lqt'
                    fi
        
                else
                    ac_gdz_qt_lib_e15='-lqt-mt'
                fi
            ;;

        esac

        if test -n "$ac_gdz_qt_incdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_qt_incdir_e15='"'`cygpath -w $ac_gdz_qt_incdir`'"'
                ;;
                *)
                   ac_gdz_qt_incdir_e15=$ac_gdz_qt_incdir
                ;;
            esac
        fi

        if test -n "$ac_gdz_qt_libdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_qt_libdir_e15='"'`cygpath -w $ac_gdz_qt_libdir`'"'
                ;;
                *)
                   ac_gdz_qt_libdir_e15=$ac_gdz_qt_libdir
                ;;
            esac
        fi

        if test -n "$ac_gdz_qt_bindir"; then
            ac_gdz_qt_moc_e15=$ac_gdz_qt_bindir/moc
            ac_gdz_qt_uic_e15=$ac_gdz_qt_bindir/uic
        fi

        ac_gdz_have_qt_e15=yes
    else
        ac_gdz_have_qt_e15=no
    fi

    changequote([, ])dnl



    ac_gdz_tif_lib_e15=
    ac_gdz_tif_incdir_e15=
    ac_gdz_tif_libdir_e15=

    if test "$enable_tif" = yes; then

        case $build_os in
            cygwin*)
                ac_gdz_tif_lib_e15='tif32.lib'
            ;;
            *)
                ac_gdz_tif_lib_e15='-ltiff'
            ;;
        esac

        if test -n "$ac_gdz_tif_incdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_tif_incdir_e15='"'`cygpath -w $ac_gdz_tif_incdir`'"'
                ;;
                *)
                    ac_gdz_tif_incdir_e15=$ac_gdz_tif_incdir
                ;;
            esac
        fi

        if test -n "$ac_gdz_tif_libdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_tif_libdir_e15='"'`cygpath -w $ac_gdz_tif_libdir`'"'
                ;;
                *)
                    ac_gdz_tif_libdir_e15=$ac_gdz_tif_libdir
                ;;
            esac
        fi
        ac_gdz_have_tif_e15=yes
    else
        ac_gdz_have_tif_e15=no
    fi



    ac_gdz_jpg_lib_e15=
    ac_gdz_jpg_incdir_e15=
    ac_gdz_jpg_libdir_e15=

    if test "$enable_jpg" = yes; then

        case $build_os in
            cygwin*)
                ac_gdz_jpg_lib_e15='libjpeg.lib'
            ;;
            *)
                ac_gdz_jpg_lib_e15='-ljpeg'
            ;;
        esac

        if test -n "$ac_gdz_jpg_incdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_jpg_incdir_e15='"'`cygpath -w $ac_gdz_jpg_incdir`'"'
                ;;
                *)
                    ac_gdz_jpg_incdir_e15=$ac_gdz_jpg_incdir
                ;;
            esac
        fi

        if test -n "$ac_gdz_jpg_libdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_jpg_libdir_e15='"'`cygpath -w $ac_gdz_jpg_libdir`'"'
                ;;
                *)
                    ac_gdz_jpg_libdir_e15=$ac_gdz_jpg_libdir
                ;;
            esac
        fi
        ac_gdz_have_jpg_e15=yes
    else    
        ac_gdz_have_jpg_e15=no
    fi



    ac_gdz_png_lib_e15=
    ac_gdz_png_incdir_e15=
    ac_gdz_png_libdir_e15=

    if test "$enable_png" = yes; then

        case $build_os in
            cygwin*)
                ac_gdz_png_lib_e15='png32.lib'
            ;;
            *)
                ac_gdz_png_lib_e15='"-lpng -lz"'
            ;;
        esac

        if test -n "$ac_gdz_png_incdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_png_incdir_e15='"'`cygpath -w $ac_gdz_png_incdir`'"'
                ;;
                *)
                    ac_gdz_png_incdir_e15=$ac_gdz_png_incdir
                ;;
            esac
        fi

        if test -n "$ac_gdz_png_libdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_png_libdir_e15='"'`cygpath -w $ac_gdz_png_libdir`'"'
                ;;
                *)
                    ac_gdz_png_libdir_e15=$ac_gdz_png_libdir
                ;;
            esac
        fi
        ac_gdz_have_png_e15=yes
    else
        ac_gdz_have_png_e15=no
    fi



    ac_gdz_mng_lib_e15=
    ac_gdz_mng_incdir_e15=
    ac_gdz_mng_libdir_e15=

    if test "$enable_mng" = yes; then

        case $build_os in
            cygwin*)
                ac_gdz_mng_lib_e15='mng32.lib'
            ;;
            *)
                ac_gdz_mng_lib_e15='-lmng'
            ;;
        esac

        if test -n "$ac_gdz_mng_incdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_mng_incdir_e15='"'`cygpath -w $ac_gdz_mng_incdir`'"'
                ;;
                *)
                    ac_gdz_mng_incdir_e15=$ac_gdz_mng_incdir
                ;;
            esac
        fi

        if test -n "$ac_gdz_mng_libdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_mng_libdir_e15='"'`cygpath -w $ac_gdz_mng_libdir`'"'
                ;;
                *)
                    ac_gdz_mng_libdir_e15=$ac_gdz_mng_libdir
                ;;
            esac
        fi
        ac_gdz_have_mng_e15=yes
    else
        ac_gdz_have_mng_e15=no
    fi


    AC_SUBST(ac_gdz_mng_incdir_e15)
    AC_SUBST(ac_gdz_mng_libdir_e15)
    AC_SUBST(ac_gdz_mng_lib_e15)
    AC_SUBST(ac_gdz_have_mng_e15)


    ac_gdz_freetype1_lib_e15=
    ac_gdz_freetype1_incdir_e15=
    ac_gdz_freetype1_libdir_e15=

    if test "$enable_freetype1" = yes; then

        case $build_os in
            cygwin*)
                ac_gdz_freetype1_lib_e15='ttf.lib'
            ;;
            *)
                ac_gdz_freetype1_lib_e15='-lttf'
            ;;
        esac

        if test -n "$ac_gdz_freetype1_incdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_freetype1_incdir_e15='"'`cygpath -w $ac_gdz_freetype1_incdir`'"'
                ;;
                *)
                    ac_gdz_freetype1_incdir_e15=$ac_gdz_freetype1_incdir
                ;;
            esac
        fi

        if test -n "$ac_gdz_freetype1_libdir"; then
            case $build_os in
                cygwin*)
                    ac_gdz_freetype1_libdir_e15='"'`cygpath -w $ac_gdz_freetype1_libdir`'"'
                ;;
                *)
                    ac_gdz_freetype1_libdir_e15=$ac_gdz_freetype1_libdir
                ;;
            esac
        fi

        ac_gdz_have_freetype1_e15=yes
    else
        ac_gdz_have_freetype1_e15=no
    fi


    ac_gdz_gif_lib_e15=
    ac_gdz_gif_incdir_e15=
    ac_gdz_gif_libdir_e15=

    if test "$enable_gif" = yes; then
        ac_gdz_have_gif_e15=yes
    else
        ac_gdz_have_gif_e15=no
    fi


    if test $build_os = cygwin; then 
        ac_gdz_system_win_e15=yes
        ac_gdz_system_unix_e15=no
    else
        ac_gdz_system_win_e15=no
        ac_gdz_system_unix_e15=yes
    fi


    if test $enable_win_exclude_default_path = yes; then
        ac_gdz_compiler_lib_e15=
        ac_gdz_system_lib_dir_e15=
        ac_gdz_system_incl_dir_e15=
    else
        ac_gdz_compiler_lib_e15=$ac_gdz_compiler_lib 

        if test $build_os = cygwin; then 
            ac_gdz_system_lib_dir_e15=`cygpath -w "$ac_gdz_system_lib_dir"`
            ac_gdz_system_incl_dir_e15=`cygpath -w "$ac_gdz_system_incl_dir"`
        else
            ac_gdz_system_lib_dir_e15="$ac_gdz_system_lib_dir"
            ac_gdz_system_incl_dir_e15="$ac_gdz_system_incl_dir"
        fi
    fi

    ac_gdz_base_system_libs_e15='"'$ac_gdz_base_system_libs'"'

    ac_gdz_osg_config_in_e15=$ac_gdz_commonconf_dir/osg-config.in
    ac_gdz_osg_config_e15=$ac_gdz_commonpackage_dir/osg-config

    touch confdefs.h

    AC_SUBST(ac_gdz_glut_lib_e15)
    AC_SUBST(ac_gdz_glut_incdir_e15)
    AC_SUBST(ac_gdz_glut_libdir_e15)
    AC_SUBST(ac_gdz_have_glut_e15)

    AC_SUBST(ac_gdz_qt_incdir_e15)
    AC_SUBST(ac_gdz_qt_libdir_e15)
    AC_SUBST(ac_gdz_qt_lib_e15)
    AC_SUBST(ac_gdz_qt_moc_e15)
    AC_SUBST(ac_gdz_qt_uic_e15)
    AC_SUBST(ac_gdz_have_qt_e15)

    AC_SUBST(ac_gdz_tif_incdir_e15)
    AC_SUBST(ac_gdz_tif_libdir_e15)
    AC_SUBST(ac_gdz_tif_lib_e15)
    AC_SUBST(ac_gdz_have_tif_e15)

    AC_SUBST(ac_gdz_jpg_incdir_e15)
    AC_SUBST(ac_gdz_jpg_libdir_e15)
    AC_SUBST(ac_gdz_jpg_lib_e15)
    AC_SUBST(ac_gdz_have_jpg_e15)

    AC_SUBST(ac_gdz_png_incdir_e15)
    AC_SUBST(ac_gdz_png_libdir_e15)
    AC_SUBST(ac_gdz_png_lib_e15)
    AC_SUBST(ac_gdz_have_png_e15)

    AC_SUBST(ac_gdz_gif_incdir_e15)
    AC_SUBST(ac_gdz_gif_libdir_e15)
    AC_SUBST(ac_gdz_gif_lib_e15)
    AC_SUBST(ac_gdz_have_gif_e15)

    AC_SUBST(ac_gdz_freetype1_incdir_e15)
    AC_SUBST(ac_gdz_freetype1_libdir_e15)
    AC_SUBST(ac_gdz_freetype1_lib_e15)
    AC_SUBST(ac_gdz_have_freetype1_e15)

    AC_SUBST(ac_gdz_system_win_e15)
    AC_SUBST(ac_gdz_system_unix_e15)

    AC_SUBST(ac_gdz_base_system_libs_e15)

    AC_SUBST(ac_gdz_compiler_lib_e15)
    AC_SUBST(ac_gdz_system_lib_dir_e15)
    AC_SUBST(ac_gdz_system_incl_dir_e15)

    AC_OUTPUT($ac_gdz_osg_config_e15:$ac_gdz_osg_config_in_e15)
])

AC_DEFUN(AC_GDZ_FIX_SGI_INCLUDES,
[
dnl e16

    ac_gdz_fstream_fix_in_e16=$ac_gdz_commonconf_dir/fstream.in
    ac_gdz_fstream_fix_out_e16=$ac_gdz_package_sub_dir_out/Base/fstream

    ac_gdz_iostream_fix_in_e16=$ac_gdz_commonconf_dir/iostream.in
    ac_gdz_iostream_fix_out_e16=$ac_gdz_package_sub_dir_out/Base/iostream

    ac_gdz_list_fix_in_e16=$ac_gdz_commonconf_dir/list.in
    ac_gdz_list_fix_out_e16=$ac_gdz_package_sub_dir_out/Base/list

    ac_gdz_map_fix_in_e16=$ac_gdz_commonconf_dir/map.in
    ac_gdz_map_fix_out_e16=$ac_gdz_package_sub_dir_out/Base/map

    ac_gdz_string_fix_in_e16=$ac_gdz_commonconf_dir/string.in
    ac_gdz_string_fix_out_e16=$ac_gdz_package_sub_dir_out/Base/string

    ac_gdz_strstream_fix_in_e16=$ac_gdz_commonconf_dir/strstream.in
    ac_gdz_strstream_fix_out_e16=$ac_gdz_package_sub_dir_out/Base/strstream

    ac_gdz_vector_fix_in_e16=$ac_gdz_commonconf_dir/vector.in
    ac_gdz_vector_fix_out_e16=$ac_gdz_package_sub_dir_out/Base/vector

    ac_gdz_algo_fix_in_e16=$ac_gdz_commonconf_dir/algorithm.in
    ac_gdz_algo_fix_out_e16=$ac_gdz_package_sub_dir_out/Base/algorithm

dnl    ac_gdz__fix_in_e16=$ac_gdz_commonconf_dir/.in
dnl    ac_gdz__fix_out_e16=$ac_gdz_package_sub_dir_out/Base/

    touch confdefs.h

    AC_OUTPUT($ac_gdz_fstream_fix_out_e16:$ac_gdz_fstream_fix_in_e16
              $ac_gdz_iostream_fix_out_e16:$ac_gdz_iostream_fix_in_e16
              $ac_gdz_list_fix_out_e16:$ac_gdz_list_fix_in_e16
              $ac_gdz_map_fix_out_e16:$ac_gdz_map_fix_in_e16
              $ac_gdz_string_fix_out_e16:$ac_gdz_string_fix_in_e16
              $ac_gdz_strstream_fix_out_e16:$ac_gdz_strstream_fix_in_e16
              $ac_gdz_algo_fix_out_e16:$ac_gdz_algo_fix_in_e16
              $ac_gdz_vector_fix_out_e16:$ac_gdz_vector_fix_in_e16)
])

AC_DEFUN(AC_GDZ_FIX_MS_INCLUDES,
[
dnl e17

    ac_gdz_xtree_fix_in_e17=$ac_gdz_commonconf_dir/xtree.in
    ac_gdz_xtree_fix_out_e17=$ac_gdz_package_sub_dir_out/Base/xtree

    touch confdefs.h

    if test -e $ac_gdz_xtree_fix_out_e17; then
        rm -f $ac_gdz_xtree_fix_out_e17
    fi

    if test "$enable_fixed_xtree" = yes; then
        AC_OUTPUT($ac_gdz_xtree_fix_out_e17:$ac_gdz_xtree_fix_in_e17)
    fi
])





