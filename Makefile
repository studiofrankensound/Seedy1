# Project Name
TARGET = MiniMultiV1

# Sources
CPP_SOURCES = Seedy1.cpp DelayEngine.cpp ToneStack.cpp Controls.cpp

# Library Locations
LIBDAISY_DIR = D:/OneDrive/Frankencloud/Installable/AudioInterfaces/DaisySeed/DaisyExamples/libDaisy
DAISYSP_DIR = D:/OneDrive/Frankencloud/Installable/AudioInterfaces/DaisySeed/DaisyExamples/DaisySP

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

