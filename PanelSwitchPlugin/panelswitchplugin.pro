TEMPLATE = lib
QT          += widgets uiplugin
CONFIG +=  plugin release
HEADERS = panelswitch.h \
    panelswitchplugin.h
SOURCES = panelswitch.cpp \
    panelswitchplugin.cpp
OTHER_FILES += panelswitch.json

DESTDIR = $$[QT_INSTALL_PLUGINS]/designer
target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS += target
