## Xapl makefile
## For Unix and Unix-like systems
## (C) 2009 Xive Software

## Builds two static libraries xpl and xai first
## Then builds main Xapl binary

# make the printenv command
#
OWNER=bin
GROUP=bin
LDFLAGS= -c -Wall
CC=g++ -g
SRCS=Main.cpp
OBJS=Main.o
DEBUG=0
OUTDIR=../../bin/unix/
XAISRCDIR=../../src/common/xAudioInterface/
XPLSRCDIR=../../src/common/xPlaylistLib/
SRCDIR=../../src/wx/
BINDIR=/usr/local/bin
DEPEND= makedepend $(CFLAGS)
all:    xapl

# Make XAI
libxai.a: MakeDirs xAudioInterface.o
	-ar cq $(OUTDIR)xai/libxai.a $(OUTDIR)xai/xAudioInterface.o

xAudioInterface.o: $(XAISRCDIR)xAudioInterface.cpp
	$(CC) -c $(XAISRCDIR)xAudioInterface.cpp -o $(OUTDIR)xai/xAudioInterface.o

# Make XPL
libxpl.a: MakeDirs xplM3UPlaylist.o
	-ar cq $(OUTDIR)xpl/libxpl.a $(OUTDIR)xpl/xplM3UPlaylist.o

xplM3UPlaylist.o: $(XPLSRCDIR)xplM3UPlaylist.cpp
	$(CC) -c $(XPLSRCDIR)xplM3UPlaylist.cpp -o $(OUTDIR)xpl/xplM3UPlaylist.o

# Make XAPL
xapl: MakeDirs libxai.a libxpl.a Main.o xaplAboutDialog.o xaplApplication.o xaplControlUI.o xaplFavoritesManager.o xaplFileDropTarget.o xaplFileLoader.o xaplMainWindow.o xaplMenuBar.o xaplPlaybackTimer.o xaplPlaylistCtrl.o xaplPositionSubject.o xaplHistoryDialog.o xaplPropertiesDialog.o xaplSearchUI.o xaplSettingsUI.o xaplSleepTimer.o xaplTaskBarIcon.o xaplUtilities.o xaplVolumeSubject.o	
	$(CC) $(OUTDIR)xapl/*.o -o $(OUTDIR)xapl/Xapl -L$(OUTDIR)xai -L$(OUTDIR)xpl -L../../lib/unix `wx-config --libs` -lxai -lxpl -lbass -lbassmix -ltag

Main.o: $(SRCDIR)Main.cpp
	$(CC) -c `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0` $(SRCDIR)Main.cpp -o $(OUTDIR)xapl/Main.o

xaplAboutDialog.o: $(SRCDIR)xaplAboutDialog.cpp
	$(CC) -c `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0` $(SRCDIR)xaplAboutDialog.cpp -o $(OUTDIR)xapl/xaplAboutDialog.o

xaplApplication.o: $(SRCDIR)xaplApplication.cpp
	$(CC) -c `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0` $(SRCDIR)xaplApplication.cpp -o $(OUTDIR)xapl/xaplApplication.o

xaplControlUI.o: $(SRCDIR)xaplControlUI.cpp
	$(CC) -c `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0` $(SRCDIR)xaplControlUI.cpp -o $(OUTDIR)xapl/xaplControlUI.o

xaplFavoritesManager.o: $(SRCDIR)xaplFavoritesManager.cpp
	$(CC) -c `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0` $(SRCDIR)xaplFavoritesManager.cpp -o $(OUTDIR)xapl/xaplFavoritesManager.o

xaplFileDropTarget.o: $(SRCDIR)xaplFileDropTarget.cpp
	$(CC) -c `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0` $(SRCDIR)xaplFileDropTarget.cpp -o $(OUTDIR)xapl/xaplFileDropTarget.o

xaplFileLoader.o: $(SRCDIR)xaplFileLoader.cpp
	$(CC) -c `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0` $(SRCDIR)xaplFileLoader.cpp -o $(OUTDIR)xapl/xaplFileLoader.o

xaplMainWindow.o: $(SRCDIR)xaplMainWindow.cpp
	$(CC) -c `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0` $(SRCDIR)xaplMainWindow.cpp -o $(OUTDIR)xapl/xaplMainWindow.o

xaplMenuBar.o: $(SRCDIR)xaplMenuBar.cpp
	$(CC) -c `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0` $(SRCDIR)xaplMenuBar.cpp -o $(OUTDIR)xapl/xaplMenuBar.o

xaplPlaybackTimer.o: $(SRCDIR)xaplPlaybackTimer.cpp
	$(CC) -c `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0` $(SRCDIR)xaplPlaybackTimer.cpp -o $(OUTDIR)xapl/xaplPlaybackTimer.o

xaplPlaylistCtrl.o: $(SRCDIR)xaplPlaylistCtrl.cpp
	$(CC) -c `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0` $(SRCDIR)xaplPlaylistCtrl.cpp -o $(OUTDIR)xapl/xaplPlaylistCtrl.o

xaplPositionSubject.o: $(SRCDIR)xaplPositionSubject.cpp
	$(CC) -c `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0` $(SRCDIR)xaplPositionSubject.cpp -o $(OUTDIR)xapl/xaplPositionSubject.o

#xaplFuzzySearch.o: $(SRCDIR)xaplFuzzySearch.cpp
#	$(CC) -c `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0` $(SRCDIR)xaplFuzzySearch.cpp -o $(OUTDIR)xapl/xaplFuzzySearch.o

xaplHistoryDialog.o: $(SRCDIR)xaplHistoryDialog.cpp
	$(CC) -c `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0` $(SRCDIR)xaplHistoryDialog.cpp -o $(OUTDIR)xapl/xaplHistoryDialog.o

xaplPropertiesDialog.o: $(SRCDIR)xaplPropertiesDialog.cpp
	$(CC) -c `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0` $(SRCDIR)xaplPropertiesDialog.cpp -o $(OUTDIR)xapl/xaplPropertiesDialog.o

xaplSearchUI.o:	$(SRCDIR)xaplSearchUI.cpp
	$(CC) -c `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0` $(SRCDIR)xaplSearchUI.cpp -o $(OUTDIR)xapl/xaplSearchUI.o

xaplSettingsUI.o: $(SRCDIR)xaplSettingsUI.cpp
	$(CC) -c `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0` $(SRCDIR)xaplSettingsUI.cpp -o $(OUTDIR)xapl/xaplSettingsUI.o

xaplSleepTimer.o: $(SRCDIR)xaplSleepTimer.cpp
	$(CC) -c `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0` $(SRCDIR)xaplSleepTimer.cpp -o $(OUTDIR)xapl/xaplSleepTimer.o

xaplTaskBarIcon.o: $(SRCDIR)xaplTaskBarIcon.cpp
	$(CC) -c `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0` $(SRCDIR)xaplTaskBarIcon.cpp -o $(OUTDIR)xapl/xaplTaskBarIcon.o

xaplUtilities.o: $(SRCDIR)xaplUtilities.cpp
	$(CC) -c `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0` $(SRCDIR)xaplUtilities.cpp -o $(OUTDIR)xapl/xaplUtilities.o

xaplVolumeSubject.o: $(SRCDIR)xaplVolumeSubject.cpp
	$(CC) -c `wx-config --cxxflags` `pkg-config --cflags gtk+-2.0` $(SRCDIR)xaplVolumeSubject.cpp -o $(OUTDIR)xapl/xaplVolumeSubject.o

MakeDirs:
	@echo If you don''t have libbass.so, libbassmix.so, and libtag.so installed in the library search path, you will need to run ''sudo make install_libs'' before the build will work.
	-mkdir $(OUTDIR)
	-mkdir $(OUTDIR)xai
	-mkdir $(OUTDIR)xpl
	-mkdir $(OUTDIR)xapl

install_libs:
	@echo You cannot install the required libraries unless you are root.
	-cp ../../lib/unix/libbass.so /usr/lib
	-cp ../../lib/unix/libbassmix.so /usr/lib
	-cp ../../lib/unix/libtag.so /usr/lib

# To install things in the right place
install: xapl install_libs
	@echo You must be root to install.
	-mkdir ~/.xapl
	-mkdir ~/.xapl/Favorites
	-install -c -o $(OWNER) -g $(GROUP) -m 755 $(OUTDIR)xapl/Xapl $(BINDIR)
	-cp ../../art/xapl.svg /usr/share/icons/hicolor/scalable/apps/

uninstall:
	-rm $(BINDIR)Xapl
	-rm -d -r -f ~/.xapl
	-rm /usr/lib/libbass.so
	-rm /usr/lib/libbassmix.so
	-rm /usr/lib/libtag.so

# what are the source dependencies
depend:
	@echo "  The following are required in your environment:\n   wxWidgets 2.8.0\n   gtk2 dev libs and headers\n    std::string\n   std::deque\n   std::stringstream"

# clean out the dross
clean:
	-rm -d -r -f ../../bin/unix/xapl
	-rm -d -r -f ../../bin/unix/xai
	-rm -d -r -f ../../bin/unix/xpl
