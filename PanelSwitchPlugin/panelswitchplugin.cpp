#include "panelswitch.h"
#include "panelswitchplugin.h"

#include <QtPlugin>

PanelSwitchPlugin::PanelSwitchPlugin(QObject *parent)
    : QObject(parent)
    , initialized(false)
{
}

void PanelSwitchPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (initialized)
        return;

    initialized = true;
}

bool PanelSwitchPlugin::isInitialized() const
{
    return initialized;
}

QWidget *PanelSwitchPlugin::createWidget(QWidget *parent)
{
    return new PanelSwitch(parent);
}

QString PanelSwitchPlugin::name() const
{
    return "PanelSwitch";
}

QString PanelSwitchPlugin::group() const
{
    return "PDP8 Widgets";
}

QIcon PanelSwitchPlugin::icon() const
{
    return QIcon();
}

QString PanelSwitchPlugin::toolTip() const
{
    return "";
}

QString PanelSwitchPlugin::whatsThis() const
{
    return "";
}

bool PanelSwitchPlugin::isContainer() const
{
    return false;
}

QString PanelSwitchPlugin::domXml() const
{
    return "<ui language=\"c++\">\n"
           " <widget class=\"PanelSwitch\" name=\"panelSwitch\">\n"
           "  <property name=\"geometry\">\n"
           "   <rect>\n"
           "    <x>0</x>\n"
           "    <y>0</y>\n"
           "    <width>40</width>\n"
           "    <height>80</height>\n"
           "   </rect>\n"
           "  </property>\n"
           "  <property name=\"toolTip\" >\n"
           "   <string>The current time</string>\n"
           "  </property>\n"
           "  <property name=\"whatsThis\" >\n"
           "   <string>PDP8i Panel Switch</string>\n"
           "  </property>\n"
           " </widget>\n"
           "</ui>\n";
}

QString PanelSwitchPlugin::includeFile() const
{
    return "panelswitch.h";
}
