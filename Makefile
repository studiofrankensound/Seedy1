# Project Name
TARGET = MiniMultiV1

# Sources
CPP_SOURCES = Seedy1.cpp DelayEngine.cpp ToneStack.cpp Controls.cpp Tempo.cpp SimpleCompressor.cpp PitchShiftEngine.cpp LfoEngine.cpp ReverbEngine.cpp FilterEngine.cpp SimpleChorusEngine.cpp

# Library Locations
LIBDAISY_DIR = D:/OneDrive/Frankencloud/Installable/AudioInterfaces/DaisySeed/DaisyExamples/libDaisy
DAISYSP_DIR = D:/OneDrive/Frankencloud/Installable/AudioInterfaces/DaisySeed/DaisyExamples/DaisySP
USE_DAISYSP_LGPL = 1

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile
