# This is to define virtual target out of some of the Third Party Softwares
# that provide clumps of variables but not nice little virtual target 
# keywords, like 'fftw3'

# The below text is copied directly from Bryan Hughes' tps.cmake file on another project. It's art.
# ---
# include third party info

# ---------------------------------------------------------------------
# ***** READ THE TEXT BELOW BEFORE YOU CHANGE ANYTHING IN THIS FILE *****

# You are modifying a file that affects the ENTIRE BUILD. Before making changes, understand what you are doing!

# DO NOT add "include_directories" or "link_directories" statements to this file. Yes, I am talking to YOU.

# Try the following instead:
# * If there is a Find<yourlibrary>.cmake file, add find_package(yourlibrary REQUIRED) to this file.
# * If there is NOT a Find<yourlibrary>.cmake file, and you are are unable to find one on the internet,
#   then "set" the following variables:
#     a variable named <yourlibrary>_INCLUDE_DIR (or something similiar)
#     a variable named <yourlibrary>_LIBRARIES (or something similar)

# After you have done one of the things above, change the leaf CMakeLists.txt file inside your target directory to
# include the directories or libraries, local to your target.

#   THE WHOLE WORLD DOES NOT NEED YOUR THIRD PARTY LIBRARY'S INCLUDE AND LINKER PATHS.

# If you do not understand these instructions or are otherwise unable to follow them, ASK FOR HELP.

# If you disregard this message, I reserve the right to spray you with a hose, because you are a bad person.

# ***** READ THE TEXT ABOVE BEFORE YOU CHANGE ANYTHING IN THIS FILE *****
# ---------------------------------------------------------------------

###################
## Find libraries.
###################

# A starting reference for the Trilinos CMake package is:
#    `Trilinos/commonTools/importing/README.Finding_Trilinos`
# Another starting point is:
#    `Trilinos/cmake/doc/export_system/Finding_Trilinos.txt`

# Trilinos recommends it be found BEFORE project() is called.
# We *may* be able to put the search here, BUT some important (and subtle?)
# aspects setup by project() may be missing. 

# MPI check
message("-- Checking if MPI is enabled in Trilinos")
list(FIND Trilinos_TPL_LIST MPI MPI_Enabled)
if (MPI_Enabled GREATER -1)
     message("-- Checking if MPI is enabled in Trilinos - MPI enabled")
     set(Xyce_PARALLEL_MPI TRUE)

     # For MPI builds, Isorropia and Zoltan are REQUIRED.
     message("-- Looking for Isorropia in Trilinos")
     list(FIND Trilinos_PACKAGE_LIST Isorropia Isorropia_FOUND)
     if ( Isorropia_FOUND GREATER -1)
          set(Xyce_USE_ISORROPIA TRUE)
          message("-- Looking for Isorropia in Trilinos - found")
     else ()
          message("-- Looking for Isorropia in Trilinos - not found\n")
          message(FATAL_ERROR "-- FATAL ERROR: Isorropia is required for MPI parallel builds.\n"
                              "                Fix the Trilinos build, and try again.")
     endif()
     message("-- Looking for Zoltan in Trilinos")
     list(FIND Trilinos_PACKAGE_LIST Zoltan Zoltan_FOUND)
     if (NOT (Zoltan_FOUND GREATER -1))
          message("-- Looking for Zoltan in Trilinos - not found\n")
          message(FATAL_ERROR "-- FATAL ERROR: Zoltan is required for MPI parallel builds.\n"
                              "                Fix the Trilinos build, and try again.")
     endif()
     message("-- Looking for Isorropia in Trilinos - found")
else()
     message("-- Checking if MPI is enabled in Trilinos - MPI not enabled")
     set(Xyce_PARALLEL_MPI FALSE)
     set(Xyce_USE_ISORROPIA FALSE)
endif()

LIST(REVERSE Trilinos_LIBRARIES)
LIST(REMOVE_DUPLICATES Trilinos_LIBRARIES)
LIST(REVERSE Trilinos_LIBRARIES)

LIST(REVERSE Trilinos_TPL_LIBRARIES)
LIST(REMOVE_DUPLICATES Trilinos_TPL_LIBRARIES)
LIST(REVERSE Trilinos_TPL_LIBRARIES)

add_library(trilinos INTERFACE IMPORTED GLOBAL)

# ??? IS AMD REQUIRED, OR NOT ???
# If it is, then the "ifdef" needs to be fixed!!!
list(FIND Trilinos_TPL_LIST AMD AMD_IN_Trilinos)
if (AMD_IN_Trilinos GREATER -1)
     message("AMD found in Trilinos")
     add_library(AMD INTERFACE IMPORTED GLOBAL)
     set(Xyce_AMD TRUE)
else()
     message(FATAL_ERROR "-- FATAL ERROR: AMD not available via Trilinos.\n"
                         "--              Fix the Trilinos build, and try again.\n")
endif()

list(FIND Trilinos_TPL_LIST BLAS BLAS_IN_Trilinos)
list(FIND Trilinos_TPL_LIST LAPACK LAPACK_IN_Trilinos)
message("-- Looking for BLAS and LAPACK in Trilinos")
if ((BLAS_IN_Trilinos GREATER -1) AND (LAPACK_IN_Trilinos GREATER -1))
     message("-- Looking for BLAS and LAPACK in Trilinos - found")
else ()
     message("-- Looking for BLAS and LAPACK in Trilinos - not found\n")
     message(FATAL_ERROR "-- FATAL ERROR: BLAS and LAPACK not available via Trilinos.\n"
                         "--              Fix the Trilinos build, and try again.")
endif ()

# Follow something like this pattern for the optional Trilinos packages
# Set optional dependency in MyApp on Epetra package:
#   this toggles code within  #ifdef MYAPP_EPETRA
#         MESSAGE("-- Looking for Epetra:")
#         LIST(FIND Trilinos_PACKAGE_LIST Epetra Epetra_List_ID)
#         IF (Epetra_List_ID GREATER -1)
#           ADD_DEFINITIONS(-DMYAPP_EPETRA)
#           MESSAGE("-- Looking for Epetra: -- found, compiling with -DMYAPP_EPETRA")
#           SET(MYAPP_EPETRA TRUE)
#         ELSE()
#           MESSAGE("-- Looking for Epetra: -- not found.")
#           SET(MYAPP_EPETRA FALSE)
#         ENDIF()


# message("\nTrilinos target = ${Trilinos_LIBRARIES} \n")
# message("Trilinos TPL target = ${Trilinos_TPL_LIBRARIES} \n")


# find the right fftw library.
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
   message("Using Intel provided MKL, the Math Kernel Library")
   find_package(MKL)
else ()
   message("Using FFTW3")
   find_package(FFTW REQUIRED)
   add_library(FFTW::FFTW INTERFACE IMPORTED GLOBAL)
   set_target_properties(FFTW::FFTW PROPERTIES
   	INTERFACE_INCLUDE_DIRECTORIES "${FFTW_INCLUDE_DIRS}" 
	INTERFACE_LINK_LIBRARIES "${FFTW_DOUBLE_LIB}")

endif ()

if (Xyce_REACTION_PARSER)
   find_package(BISON 2.3 REQUIRED)
   # The 2.3 specifies the minimum version.  That is ok at the moment, as Bison
   # has been functional for many versions (through 3.4 at the time of this
   # writing).  Historically, though, new versions have had backward
   # incompatibility issues.  If that occurs again, the BISON_VERSION variable
   # will have to be probed for a certain range.
   find_package(FLEX REQUIRED)
   #[[ "Define YYTEXT_POINTER if yytext defaults to 'char *' instead of 'char'"]]
endif ()
