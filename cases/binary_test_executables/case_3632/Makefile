LDFLAGS += -L${KAYA_VISION_POINT_LIB_PATH}
LDFLAGS += -Wl,-rpath,${KAYA_VISION_POINT_LIB_PATH}
LDFLAGS += -Wl,-rpath,'$$ORIGIN'
#KYFLAGS+=-lz #only needed when KYFGLib is built with $KAYA_ATTR_OPENCV_LINK_MODE == "local"

all: case_3632

case_3632: case_3632.cpp
	gcc -o case_3632 case_3632.cpp -I$(KAYA_VISION_POINT_INCLUDE_PATH) $(LDFLAGS) -lKYFGLib $(KYFLAGS) -lstdc++

