
  ************************************************

    wxPropertyGrid

      By Jaakko Salli (jmsalli79@hotmail.com)
                      (jaakko.salli@pp.inet.fi)

  ************************************************

    Version:   1.0.0 BETA 1
    Requires:  wxWidgets 2.5.2 or later ( supplied makefiles are for 2.5.3 )
    Platforms: Windows - OK; Linux GTK 2.0 - OK; Linux GTK 1.2 - 90% OK;
               other platforms remain untested.
    License:   wxWidgets license

    Homepage:  http://www.geocities.com/jmsalli/propertygrid/index.html

    wxPropertyGrid is a specialized two-column grid for editing properties
    such as strings, numbers, flagsets, fonts, and colours. wxPropertySheet
    used to do the very same thing, but it hasn't been updated for a while
    and it is currently deprecated.

    wxPropertyGrid is modeled after .NET propertygrid (hence the name),
    and thus features are similar.

    IMPORTANT: When upgrading to a new version, always carefully read
               CHANGES-propgrid.txt.

  DOCUMENTATION
  -------------

  See /contrib/docs/html/propgrid/index.htm

  for documentation generated with doxygen.


  HOW TO INSTALL AND BUILD
  ------------------------

  wxPropertyGrid archive has base 'contrib' directory which layout matches wxWidgets 2.5.x
  'contrib'. It also has bakefiles and generated makefiles+project files in matching
  locations.

  WINDOWS

  The simplest way to install is to extract the archive into your wxWidgets directory,
  and then you'll have a new 'contrib' that you can build and compile like any other. This
  also works on other OS'es which allow filling directories without overwriting them
  (atleast my Linux overwrites, so be warned).

  LINUX AND OTHER *NIX OS'es

  Since direct copying to wxWidgets dir may not be an option, you should run
  install-propgrid.sh to copy files to contrib or other sub-directory.

  LIBRARY VERSION NOTE

  Makefiles assume you have wxWidgets 2.5.3. They may not work correctly with
  any other version. In case of an incompatibility, get your hands on bakefile and run
  bakefile_gen in %WXDIR/build/bakefiles/.

  INSTALLING TO A DIFFERENT SUB-DIRECTORY

  If you do not wish to install into 'contrib' directory, then you can uncompress the
  archive into a temporary directory first, rename the base dir, and copy it into
  wxWidgets root (or enter something else than 'contrib' when running install script).
  Supplied (Windows) makefiles and project files should still work, but if you wish
  for configure script to generate a corresponding makefile, then you need to add your
  dir to the following line in configure (install script also tells you to do this):

  SUBDIRS="samples demos utils contrib"

  (it is line 41397 in 2.5.3).

  If you need to (re)generate makefiles to a non-contrib directory, run bakefile_gen,
  as instructed above, but be sure to have modified 'Bakefiles.bkgen' so that it contains
  a version of every directive intended for 'contrib' for your dir as well (to make it
  simple, replace all instances of 'contrib' with name of your dir - but since this
  disables makefile generation for actual contrib dir, you might want to backup the file
  first).
