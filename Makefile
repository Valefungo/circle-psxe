#
# Makefile
#

include ../../Config.mk

CIRCLEHOME = ../../libs/circle
NEWLIBDIR = ../../install/$(NEWLIB_ARCH)

PNG = \
  extra/minipng/pngread.o extra/minipng/pngmem.o extra/minipng/pngpread.o extra/minipng/pngrutil.o \
  extra/minipng/pngwio.o extra/minipng/pngrio.o extra/minipng/pngtrans.o extra/minipng/pngget.o \
  extra/minipng/png.o extra/minipng/pngwtran.o extra/minipng/pngrtran.o extra/minipng/pngwrite.o \
  extra/minipng/pngwutil.o extra/minipng/pngerror.o extra/minipng/pngset.o

ZIP = \
  extra/minizip/gzlib.o extra/minizip/crc32.o extra/minizip/gzread.o extra/minizip/adler32.o \
  extra/minizip/inffast.o extra/minizip/inftrees.o extra/minizip/trees.o extra/minizip/uncompr.o \
  extra/minizip/deflate.o extra/minizip/gzwrite.o extra/minizip/gzclose.o extra/minizip/zutil.o \
  extra/minizip/compress.o extra/minizip/inflate.o extra/minizip/infback.o

VERSION_TAG := "Circle0.0.1"
COMMIT_HASH := "none"
OS_INFO := "NoOS"

#

O = frontend/toml.o frontend/config.o frontend/argparse.o frontend/main.o frontend/screen.o \
    psx/cpu.o psx/exe.o psx/log.o \
    psx/config.o psx/psx.o psx/bus.o \
    psx/input/guncon.o psx/input/sda.o \
    psx/dev/input.o psx/dev/scratchpad.o psx/dev/pad.o psx/dev/xa.o psx/dev/mc1.o psx/dev/gpu.o psx/dev/exp2.o psx/dev/exp1.o psx/dev/mc3.o psx/dev/dma.o \
    psx/dev/cdrom/audio.o psx/dev/cdrom/impl.o psx/dev/cdrom/disc.o psx/dev/cdrom/cue.o psx/dev/cdrom/queue.o psx/dev/cdrom/list.o psx/dev/cdrom/cdrom.o \
    psx/dev/bios.o psx/dev/ic.o psx/dev/timer.o psx/dev/mcd.o psx/dev/spu.o psx/dev/mdec.o psx/dev/ram.o psx/dev/mc2.o


OBJS    = main.o kernel.o noSDL.o $(O) $(ZIP) $(PNG)

include $(CIRCLEHOME)/Rules.mk

CFLAGS += -I"." -I"psx" -DOS_INFO="$(OS_INFO)" -DREP_VERSION="$(VERSION_TAG)" -DREP_COMMIT_HASH="$(COMMIT_HASH)"

# CFLAGS += -I "$(NEWLIBDIR)/include" -I $(STDDEF_INCPATH) -Iinclude -Iextra/minizip -Iextra/minipng -I ../../include -DLOGGING_DISABLED -DNATIVE_BUILD -DPNG_ARM_NEON_OPT=0 -O3
CFLAGS += -I "$(NEWLIBDIR)/include" -I $(STDDEF_INCPATH) -Iinclude -Iextra/minizip -Iextra/minipng -I ../../include -DLOGGING_DISABLED -DNATIVE_BUILD -DPNG_ARM_NEON_OPT=0 -ggdb -O0

LIBS := "$(NEWLIBDIR)/lib/libm.a" "$(NEWLIBDIR)/lib/libc.a" "$(NEWLIBDIR)/lib/libcirclenewlib.a" \
 	$(CIRCLEHOME)/addon/SDCard/libsdcard.a \
  	$(CIRCLEHOME)/lib/usb/libusb.a \
 	$(CIRCLEHOME)/lib/input/libinput.a \
 	$(CIRCLEHOME)/addon/fatfs/libfatfs.a \
 	$(CIRCLEHOME)/lib/fs/libfs.a \
  	$(CIRCLEHOME)/lib/net/libnet.a \
  	$(CIRCLEHOME)/lib/sched/libsched.a \
	$(CIRCLEHOME)/lib/sound/libsound.a \
  	$(CIRCLEHOME)/lib/libcircle.a

-include $(DEPS)

depclean:
	find . -name "*.d" -exec rm {} \;

mrproper: clean depclean
	rm -f $(OBJS)


