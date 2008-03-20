#! /bin/sh

#
# This is a simple script to copy component dirs into correct locations
# (and do some very simple build stuff)
#
echo "INSTALLING WXWIDGETS COMPONENT: propgrid"
if test ! -r "readme-propgrid.txt"; then
  echo "readme-propgrid.txt does not exist, so this script is being executed in wrong directory!"
  echo "Install aborted"
  exit 0;
fi

# try auto-running the script
if test ! "$1" = ""; then
  declare basewxdir="$1"
fi

# query base dir
if test "$1" = ""; then
  echo "Enter your wxWidgets base directory"
  read basewxdir
  if test "$basewxdir" = ""; then
    echo "Directory $basewxdir does not exist!"
    echo "Install aborted"
    exit 0;
  fi
fi
if test ! -r "$basewxdir/configure"; then
  echo "Directory does not exist!"
  echo "Install aborted"
  exit 0;
fi

if test ! "$1" = ""; then
  declare subwxdir="contrib"
fi

# query sub dir
if test "$1" = ""; then
  echo "Enter sub-directory to install [contrib]"
  read subwxdir
  if test "$subwxdir" = ""; then
    declare subwxdir="contrib"
  fi
fi

# test sub dir
if test ! -r $basewxdir/$subwxdir; then
  echo "$basewxdir/$subwxdir does not exist. Create? [Y]"
  read msgres
  if test "$msgres" = "n"; then
    echo "Install aborted"
    exit 0;
  fi
  if test "$msgres" = "N"; then
    echo "Install aborted"
    exit 0;
  fi
  mkdir $basewxdir/$subwxdir
  mkdir $basewxdir/$subwxdir/build
  mkdir $basewxdir/$subwxdir/docs
  mkdir $basewxdir/$subwxdir/docs/html
  mkdir $basewxdir/$subwxdir/include
  mkdir $basewxdir/$subwxdir/include/wx
  mkdir $basewxdir/$subwxdir/lib
  mkdir $basewxdir/$subwxdir/samples
  mkdir $basewxdir/$subwxdir/src
fi

# test existing installation
if test "$1" = ""; then
  if test -r "$basewxdir/$subwxdir/build/propgrid"; then
    echo "Installation already exists. Overwrite [Y]?"
    read msgres
    if test "$msgres" = "n"; then
      echo "Install aborted"
      exit 0;
    fi
    if test "$msgres" = "N"; then
      echo "Install aborted"
      exit 0;
    fi
  fi
fi

# copy files
cp -f -R contrib/build/propgrid $basewxdir/$subwxdir/build
cp -f -R contrib/docs/html/propgrid $basewxdir/$subwxdir/docs/html
cp -f -R contrib/include/wx/propgrid $basewxdir/$subwxdir/include/wx
cp -f -R contrib/samples/propgrid $basewxdir/$subwxdir/samples
cp -f -R contrib/src/propgrid $basewxdir/$subwxdir/src

# copy appropriate makefiles
if test "$1" = ""; then
  echo "Which version of wxWidgets do you use (supported:2.5.2,2.5.3) [2.5.3]?"
  read libver
  if test "$libver" = ""; then
    declare libver="2.5.3"
  fi
else
  declare libver="2.5.3"
fi
if test ! "$libver" = "2.5.3"; then
  if test ! "$libver" = "2.5.2"; then
    echo "WARNING: Unsupported library version - you will need to regenerate/modify the makefile."
    declare libver="2.5.3"
  fi
fi
cp -f contrib/src/propgrid/Makefile.$libver.in $basewxdir/$subwxdir/src/propgrid/Makefile.in
cp -f contrib/samples/propgrid/Makefile.$libver.in $basewxdir/$subwxdir/samples/propgrid/Makefile.in

echo "Install complete!"

# exit now if this was auto-run
if test ! "$1" = ""; then
  exit 0;
fi

# show extra information
if test ! "$subwxdir" = "contrib"; then
    echo ""
    echo "Since you installed in another directory than contrib, running the"
    echo "configure script will not generate makefiles unless you change line"
    echo ""
    echo "    SUBDIRS=\"samples demos utils contrib\""
    echo ""
    echo "into"
    echo ""
    echo "    SUBDIRS=\"samples demos utils contrib $subwxdir\""
    echo ""
    echo "Inorder to build the library and sample, and then run the sample, do the following:"
    echo ""
    echo "    cd <your_wxwidgets_library_dir>"
    echo "    ./configure or ../configure depending are you in wxWidgets base dir or a subdir"
    echo "    cd $subwxdir/src/propgrid"
    echo "    make"
    echo "    cd ../../samples/propgrid"
    echo "    make"
    echo "    ./propgridsample"
    echo ""
    exit 0;
fi

# ask if user wants to build the lib
echo "Do want to build the library and sample, and then run the sample [Y]?"
echo "Note that this will re-run configure to generate makefiles, so it may take some time."
read msgres
if test "$msgres" = "n"; then
exit 0;
fi
if test "$msgres" = "N"; then
exit 0;
fi

echo "Enter library directory (For example, 'buildgtk2' or 'build-debug'."
echo "If you simply have run configure in base wxWidgets dir, just enter an empty string )"
read subbuilddir

# test sub dir
if test ! -r $basewxdir/$subbuilddir; then
  echo "Directory doesn't exist, so nothing can be done!"
  exit 0;
fi

# run configure
if test "$subbuilddir" = ""; then
cd $basewxdir
./configure
else
cd $basewxdir/$subbuilddir
../configure
fi

# make
cd $subwxdir/src/propgrid
make
cd ../../samples/propgrid
make
./propgridsample
